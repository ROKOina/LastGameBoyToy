#include "RayCollisionCom.h"
#include "Component/System/RayCastManager.h"
#include "SystemStruct\Logger.h"
#include <cereal/cereal.hpp>
#include <cereal/archives/binary.hpp>
#include <cereal/types/vector.hpp>
#include <fstream>

#pragma region シリアライズ

namespace DirectX
{
    template<class Archive>
    void serialize(Archive& archive, XMUINT4& v)
    {
        archive(
            cereal::make_nvp("x", v.x),
            cereal::make_nvp("y", v.y),
            cereal::make_nvp("z", v.z),
            cereal::make_nvp("w", v.w)
        );
    }

    template<class Archive>
    void serialize(Archive& archive, XMFLOAT2& v)
    {
        archive(
            cereal::make_nvp("x", v.x),
            cereal::make_nvp("y", v.y)
        );
    }

    template<class Archive>
    void serialize(Archive& archive, XMFLOAT3& v)
    {
        archive(
            cereal::make_nvp("x", v.x),
            cereal::make_nvp("y", v.y),
            cereal::make_nvp("z", v.z)
        );
    }

    template<class Archive>
    void serialize(Archive& archive, XMFLOAT4& v)
    {
        archive(
            cereal::make_nvp("x", v.x),
            cereal::make_nvp("y", v.y),
            cereal::make_nvp("z", v.z),
            cereal::make_nvp("w", v.w)
        );
    }

    template<class Archive>
    void serialize(Archive& archive, XMFLOAT4X4& m)
    {
        archive(
            cereal::make_nvp("_11", m._11), cereal::make_nvp("_12", m._12), cereal::make_nvp("_13", m._13), cereal::make_nvp("_14", m._14),
            cereal::make_nvp("_21", m._21), cereal::make_nvp("_22", m._22), cereal::make_nvp("_23", m._23), cereal::make_nvp("_24", m._24),
            cereal::make_nvp("_31", m._31), cereal::make_nvp("_32", m._32), cereal::make_nvp("_33", m._33), cereal::make_nvp("_34", m._34),
            cereal::make_nvp("_41", m._41), cereal::make_nvp("_42", m._42), cereal::make_nvp("_43", m._43), cereal::make_nvp("_44", m._44)
        );
    }
}

template<class Archive>
void serialize(Archive& archive, RayCollisionCom::Collision::VertexData& v)
{
    archive(
        cereal::make_nvp("keyID", v.keyID),
        cereal::make_nvp("vertexA", v.vertexA)
    );
}

template<class Archive>
void serialize(Archive& archive, RayCollisionCom::Collision::PolygonGroupe& g)
{
    archive(
        cereal::make_nvp("aabbCenter", g.aabbCenter),
        cereal::make_nvp("aabbRadius", g.aabbRadius),
        cereal::make_nvp("vertexDatas", g.vertexDatas)
    );
}

template<class Archive>
void RayCollisionCom::CollisionVertex::serialize(Archive& archive, int version)
{
    archive(
        CEREAL_NVP(position),
        CEREAL_NVP(normal)
    );
}

template<class Archive>
void RayCollisionCom::Collision::serialize(Archive& archive, int version)
{
    archive(
        CEREAL_NVP(polygonGroupes),
        CEREAL_NVP(vertices)
    );
}

#pragma endregion

RayCollisionCom::RayCollisionCom(const char* filePath)
{
    // ディレクトリパス取得
    char drive[32], dir[256], dirname[256];
    ::_splitpath_s(filePath, drive, sizeof(drive), dir, sizeof(dir), nullptr, 0, nullptr, 0);
    ::_makepath_s(dirname, sizeof(dirname), drive, dir, nullptr, nullptr);

    // デシリアライズ
    Deserialize(filePath);
}

void RayCollisionCom::Start()
{
    RayCastManager::Instance().Register(shared_from_this());
}

void RayCollisionCom::OnDestroy()
{
    RayCastManager::Instance().Remove(shared_from_this());
}

void RayCollisionCom::Deserialize(const char* filePath)
{
    std::ifstream istream(filePath, std::ios::binary);
    if (istream.is_open())
    {
        cereal::BinaryInputArchive archive(istream);

        try
        {
            archive(
                CEREAL_NVP(collision)
            );
        }
        catch (...)
        {
            LOG("collision deserialize failed.\n%s\n", filePath);
            return;
        }
    }
}
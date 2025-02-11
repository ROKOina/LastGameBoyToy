#include "RayCollisionCom.h"
#include "Component/System/RayCastManager.h"
#include "SystemStruct\Logger.h"

#pragma region シリアライズ

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
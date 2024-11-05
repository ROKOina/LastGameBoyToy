#include "SpawnCom.h"
#include "Component/System/TransformCom.h"
#include "Component/Animation/AnimationCom.h"
#include <cstdlib>
#include <cmath>
#include <fstream>
#include "Component/Enemy/NoobEnemyCom.h"
#include "Component/Collsion/FrustumCom.h"
#include "Component/Animation/AimIKCom.h"
#include "Component/Collsion/NodeCollsionCom.h"
#include "Component\Character\CharaStatusCom.h"
#include "Component\MoveSystem\EasingMoveCom.h"
#include "SystemStruct\Dialog.h"
#include "SystemStruct\Logger.h"
#include <cereal/cereal.hpp>
#include <cereal/archives/binary.hpp>
#include "Component/Collsion/PushBackCom.h"
#include "Component\Particle\GPUParticle.h"
#include <random>
#include "Component\Particle\CPUParticle.h"

CEREAL_CLASS_VERSION(SpawnCom::SpawnParameter, 1)

// シリアライズ
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
void SpawnCom::SpawnParameter::serialize(Archive& archive, int version)
{
    archive
    (
        CEREAL_NVP(spawnInterval),
        CEREAL_NVP(spawnRadius),
        CEREAL_NVP(spawnCount),
        CEREAL_NVP(objecttype),
        CEREAL_NVP(Yoffset),
        CEREAL_NVP(distanceXY)
    );
}

// 初期化
SpawnCom::SpawnCom(const char* filename) : currentSpawnedCount(0)
{
    //読み込み
    if (filename)
    {
        Deserialize(filename);
    }
}

// 更新処理
void SpawnCom::Update(float elapsedTime)
{
    if (spwntrigger)
    {
        // 時間更新
        lastSpawnTime += elapsedTime;
    }
    else
    {
        lastSpawnTime = 0.0f;
    }

    // 生成間隔を超えた場合にオブジェクトを生成
    if (lastSpawnTime >= sp.spawnInterval)
    {
        //複数オブジェクトを生成
        for (int i = 0; i < sp.spawnCount; ++i)
        {
            SpawnGameObject();
        }

        // タイマーをリセット
        lastSpawnTime = 0.0f;
    }
}

// imgui
void SpawnCom::OnGUI()
{
    ImGui::SameLine();
    if (ImGui::Button((char*)u8"保存"))
    {
        Serialize();
    }
    ImGui::SameLine();
    if (ImGui::Button((char*)u8"読み込み"))
    {
        LoadDeserialize();
    }
    ImGui::SameLine();
    ImGui::Checkbox((char*)u8"生成フラグ", &spwntrigger);

    if (ImGui::TreeNode((char*)u8"生成時のパラメータ"))
    {
        constexpr const char* objectTypeItems[] = { "ENEMY", "MISSILE","EXPLOSION","BEEM" };
        static_assert(ARRAYSIZE(objectTypeItems) == static_cast<int>(ObjectType::MAX), "objectTypeItems Size Error!");
        ImGui::Combo((char*)u8"オブジェクトタイプ", &sp.objecttype, objectTypeItems, static_cast<int>(ObjectType::MAX));
        objtype = static_cast<ObjectType>(sp.objecttype);
        sp.objecttype = static_cast<int>(objtype);
        ImGui::DragFloat((char*)u8"合計時間", &lastSpawnTime);
        ImGui::DragFloat((char*)u8"生成間隔", &sp.spawnInterval, 0.1f, 0.0f, 10.0f);
        ImGui::DragFloat((char*)u8"生成半径", &sp.spawnRadius, 0.0f, 0.0f, 50.0f);
        ImGui::DragInt((char*)u8"生成数", &sp.spawnCount, 1, 0, 100);
        ImGui::DragFloat((char*)u8"Y軸オフセット", &sp.Yoffset, 0.0f, 0.0f, 20.0f);
        ImGui::DragFloat2((char*)u8"最小最大", &sp.distanceXY.x, 0.1f, 0.0f, 20.0f);
        ImGui::TreePop();
    }
}

//ゲームオブジェクトを複製する処理
void SpawnCom::SpawnGameObject()
{
    std::shared_ptr<GameObject> obj = GameObjectManager::Instance().Create();
    std::shared_ptr<SphereColliderCom> collider;
    std::shared_ptr<RendererCom> renderer;
    std::shared_ptr<CPUParticle>cpuparticle;
    std::shared_ptr<GPUParticle>gpuparticle;
    std::shared_ptr<PushBackCom>pushback;

    // 親オブジェクトの位置を取得
    DirectX::XMFLOAT3 originalPosition = GetGameObject()->transform_->GetWorldPosition();

    // ランダムな位置を生成
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<float> angleDist(0.0f, 2.0f * DirectX::XM_PI);
    std::uniform_real_distribution<float> distanceDist(sp.distanceXY.x, sp.distanceXY.y);

    float randomAngle = angleDist(gen);
    float randomDistance = std::sqrt(distanceDist(gen)) * sp.spawnRadius; // 平方根で距離分布を調整

    float offsetX = std::cos(randomAngle) * randomDistance;
    float offsetZ = std::sin(randomAngle) * randomDistance;

    // 新しい位置を元の位置にオフセットを加えて設定
    DirectX::XMFLOAT3 newPosition =
    {
        originalPosition.x + offsetX,
        originalPosition.y + sp.Yoffset,
        originalPosition.z + offsetZ
    };
    obj->transform_->SetWorldPosition(newPosition);

    //どのオブジェクトを生成するか決定する
    objtype = static_cast<ObjectType>(sp.objecttype);
    sp.objecttype = static_cast<int>(objtype);
    switch (objtype)
    {
    case ObjectType::ENEMY:

        obj->SetName("NoobEnemy");
        obj->transform_->SetScale({ 0.02f, 0.02f, 0.02f });
        renderer = obj->AddComponent<RendererCom>(SHADER_ID_MODEL::DEFERRED, BLENDSTATE::MULTIPLERENDERTARGETS, DEPTHSTATE::ZT_ON_ZW_ON, RASTERIZERSTATE::SOLID_CULL_BACK, true, false);
        renderer->LoadModel("Data/Model/Jammo/jammo.mdl");
        obj->AddComponent<MovementCom>();
        obj->AddComponent<NodeCollsionCom>("Data/Model/Jammo/jammocollsion.nodecollsion");
        obj->AddComponent<AnimationCom>();
        obj->AddComponent<AimIKCom>(nullptr, "mixamorig:Neck");
        obj->AddComponent<NoobEnemyCom>();
        obj->AddComponent<CharaStatusCom>();
        collider = obj->AddComponent<SphereColliderCom>();
        collider->SetMyTag(COLLIDER_TAG::Enemy);
        pushback = obj->AddComponent<PushBackCom>();
        pushback->SetRadius(0.5f);
        pushback->SetWeight(0.5f);

        break;

    case ObjectType::MISSILE:

        obj->SetName("fireball");
        cpuparticle = obj->AddComponent<CPUParticle>("Data/SerializeData/CPUEffect/fireball.cpuparticle", 600);
        cpuparticle->SetActive(true);
        obj->AddComponent<GPUParticle>("Data/SerializeData/GPUEffect/fireball.gpuparticle", 5000);
        collider = obj->AddComponent<SphereColliderCom>();
        collider->SetEnabled(true);
        collider->SetMyTag(COLLIDER_TAG::Enemy);
        collider->SetJudgeTag(COLLIDER_TAG::Player);
        collider->SetRadius(0.8f);
        obj->AddComponent<EasingMoveCom>("Data/SerializeData/3DEasingData/missile.easingmove");

        break;

    case ObjectType::EXPLOSION:

        obj->SetName("explosion");
        obj->AddComponent<CPUParticle>("Data/SerializeData/CPUEffect/explosionsmoke.cpuparticle", 1000);
        obj->AddComponent<CPUParticle>("Data/SerializeData/CPUEffect/explosionfire.cpuparticle", 1000);
        gpuparticle = obj->AddComponent<GPUParticle>("Data/SerializeData/GPUEffect/explosion.gpuparticle", 10000);
        gpuparticle->Play();
        break;

    case ObjectType::BEEM:

        obj->SetName("beem");
        obj->AddComponent<GPUParticle>("Data/SerializeData/GPUEffect/beem.gpuparticle", 10000);
        obj->AddComponent<EasingMoveCom>("Data/SerializeData/3DEasingData/missile.easingmove");

        break;

    default:

        collider = nullptr;
        renderer = nullptr;
        cpuparticle = nullptr;
        gpuparticle = nullptr;
        pushback = nullptr;

        break;
    }

    // オブジェクトに番号を付ける
    std::string objectName = std::string(obj->GetName()) + "_" + std::to_string(currentSpawnedCount);
    obj->SetName(objectName.c_str());

    // 現在の生成数をインクリメント
    currentSpawnedCount++;
}

//シリアライズ
void SpawnCom::Serialize()
{
    static const char* filter = "Spawn Files(*.spawn)\0*.spawn;\0All Files(*.*)\0*.*;\0\0";

    char filename[256] = { 0 };
    DialogResult result = Dialog::SaveFileName(filename, sizeof(filename), filter, nullptr, "spawn", Graphics::Instance().GetHwnd());
    if (result == DialogResult::OK)
    {
        std::ofstream ostream(filename, std::ios::binary);
        if (ostream.is_open())
        {
            cereal::BinaryOutputArchive archive(ostream);

            try
            {
                archive
                (
                    CEREAL_NVP(sp)
                );
            }
            catch (...)
            {
                LOG("spawn deserialize failed.\n%s\n", filename);
                return;
            }
        }
    }
}

// デシリアライズ
void SpawnCom::Deserialize(const char* filename)
{
    std::ifstream istream(filename, std::ios::binary);
    if (istream.is_open())
    {
        cereal::BinaryInputArchive archive(istream);

        try
        {
            archive
            (
                CEREAL_NVP(sp)
            );
        }
        catch (...)
        {
            LOG("spawn deserialize failed.\n%s\n", filename);
            return;
        }
    }
}

// デシリアライズの読み込み
void SpawnCom::LoadDeserialize()
{
    static const char* filter = "Spawn Files(*.spawn)\0*.spawn;\0All Files(*.*)\0*.*;\0\0";

    char filename[256] = { 0 };
    DialogResult result = Dialog::OpenFileName(filename, sizeof(filename), filter, nullptr, Graphics::Instance().GetHwnd());
    if (result == DialogResult::OK)
    {
        Deserialize(filename);
    }
}
#include "SpawnCom.h"
#include "Components/TransformCom.h"
#include "Components/AnimationCom.h"
#include <cstdlib>
#include <cmath>
#include <fstream>
#include "Components/Enemy/NoobEnemy/NoobEnemyCom.h"
#include "Components/FrustumCom.h"
#include "Components/AimIKCom.h"
#include "Components/NodeCollsionCom.h"
#include "Components\Character\CharaStatusCom.h"
#include "Components\EasingMoveCom.h"
#include "Dialog.h"
#include "Logger.h"
#include <cereal/cereal.hpp>
#include <cereal/archives/binary.hpp>
#include "Components/PushBackCom.h"

CEREAL_CLASS_VERSION(SpawnCom::SpawnParameter, 1)

template<class Archive>
void SpawnCom::SpawnParameter::serialize(Archive& archive, int version)
{
    archive
    (
        CEREAL_NVP(spawnInterval),
        CEREAL_NVP(spawnRadius),
        CEREAL_NVP(spawnCount),
        CEREAL_NVP(objecttype),
        CEREAL_NVP(Yoffset)
    );
}

// 初期化
SpawnCom::SpawnCom(const char* filename) : currentSpawnedCount(0)
{
    collider = nullptr;
    renderer = nullptr;
    cpuparticle = nullptr;

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
        ImGui::DragFloat((char*)u8"合計時間", &lastSpawnTime);
        ImGui::DragFloat((char*)u8"生成間隔", &sp.spawnInterval, 0.1f, 0.0f, 10.0f);
        ImGui::DragFloat((char*)u8"生成半径", &sp.spawnRadius, 0.0f, 0.0f, 50.0f);
        ImGui::DragInt((char*)u8"生成数", &sp.spawnCount, 1, 0, 100);
        ImGui::DragFloat((char*)u8"Y軸オフセット", &sp.Yoffset, 0.0f, 0.0f, 20.0f);
        ImGui::TreePop();
    }
}

//ゲームオブジェクトを複製する処理
void SpawnCom::SpawnGameObject()
{
    std::shared_ptr<GameObject> obj = GameObjectManager::Instance().Create();

    //どのオブジェクトを生成するか決定する
    switch (objtype)
    {
    case ObjectType::ENEMY:

        obj->SetName("NoobEnemy");
        obj->transform_->SetScale({ 0.02f, 0.02f, 0.02f });
        renderer = obj->AddComponent<RendererCom>(SHADER_ID_MODEL::DEFERRED, BLENDSTATE::MULTIPLERENDERTARGETS, DEPTHSTATE::ZT_ON_ZW_ON, RASTERIZERSTATE::SOLID_CULL_BACK, true, false);
        renderer->LoadModel("Data/Jammo/jammo.mdl");
        obj->AddComponent<MovementCom>();
        obj->AddComponent<NodeCollsionCom>("Data/Jammo/jammocollsion.nodecollsion");
        obj->AddComponent<AnimationCom>();
        obj->AddComponent<AimIKCom>(nullptr, "mixamorig:Neck");
        obj->AddComponent<NoobEnemyCom>();
        obj->AddComponent<CharaStatusCom>();
        collider = obj->AddComponent<SphereColliderCom>();
        collider->SetMyTag(COLLIDER_TAG::Enemy);

        //auto& pushBack = obj->AddComponent<PushBackCom>();
        //pushBack->SetRadius(1);
        //pushBack->SetWeight(0.5f);

        break;

    case ObjectType::MISSILE:

        obj->SetName("fireball");
        cpuparticle = obj->AddComponent<CPUParticle>("Data/Effect/fireball.cpuparticle", 1000);
        cpuparticle->SetActive(true);
        obj->AddComponent<EasingMoveCom>(nullptr);

        break;

    default:

        collider = nullptr;
        renderer = nullptr;
        cpuparticle = nullptr;

        break;
    }

    // オブジェクトに番号を付ける
    std::string objectName = std::string(obj->GetName()) + "_" + std::to_string(currentSpawnedCount);
    obj->SetName(objectName.c_str());

    // 半径spawnRadiusメートル以内のランダムな位置を計算
    float randomAngle = static_cast<float>(rand()) / RAND_MAX * 2.0f * DirectX::XM_PI; // 0から2πまでのランダム角度
    float randomDistance = static_cast<float>(rand()) / RAND_MAX * sp.spawnRadius;     // 0からspawnRadiusまでのランダム距離

    // 2D平面上でランダム位置を計算
    float offsetX = cosf(randomAngle) * randomDistance;
    float offsetZ = sinf(randomAngle) * randomDistance;

    // 新しい位置を元の位置にオフセットを加えて設定
    DirectX::XMFLOAT3 newPosition =
    {
        GetGameObject()->transform_->GetWorldPosition().x + offsetX,
        GetGameObject()->transform_->GetWorldPosition().y + sp.Yoffset, //ここだけオフセット値でいくぜ
        GetGameObject()->transform_->GetWorldPosition().z + offsetZ
    };
    obj->transform_->SetWorldPosition(newPosition);

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
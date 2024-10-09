#include "SpawnCom.h"
#include "Components/TransformCom.h"
#include "Components/RendererCom.h"
#include "Components/AnimationCom.h"
#include <cstdlib>
#include <cmath>
#include "Components/Enemy/NoobEnemy/NoobEnemyCom.h"
#include "Components/AimIKCom.h"
#include "Components/NodeCollsionCom.h"

// 初期化
SpawnCom::SpawnCom() : currentSpawnedCount(0)
{
}

// 初期設定
void SpawnCom::Start()
{
}

// 更新処理
void SpawnCom::Update(float elapsedTime)
{
    // 時間更新
    lastSpawnTime += elapsedTime;

    // 生成間隔を超えた場合にオブジェクトを生成
    if (lastSpawnTime >= spawnInterval)
    {
        //複数オブジェクトを生成
        for (int i = 0; i < spawnCount; ++i)
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
    ImGui::Text("Spawn Settings");
    ImGui::SliderFloat("Spawn Interval", &spawnInterval, 1.0f, 10.0f);
    ImGui::SliderFloat("Spawn Radius", &spawnRadius, 1.0f, 10.0f);
    ImGui::SliderInt("Spawn Count", &spawnCount, 1, 100);
}

//ゲームオブジェクトを複製する処理
void SpawnCom::SpawnGameObject()
{
    std::shared_ptr<GameObject> obj = GameObjectManager::Instance().Create();
    obj->SetName("NoobEnemy");

    //スケール設定
    obj->transform_->SetScale({ 0.02f, 0.02f, 0.02f });

    // オブジェクトに番号を付ける
    std::string objectName = std::string(obj->GetName()) + "_" + std::to_string(currentSpawnedCount);
    obj->SetName(objectName.c_str());

    //親オブジェクトの位置
    DirectX::XMFLOAT3 originalPosition = GetGameObject()->transform_->GetWorldPosition();

    // 半径spawnRadiusメートル以内のランダムな位置を計算
    float randomAngle = static_cast<float>(rand()) / RAND_MAX * 2.0f * DirectX::XM_PI; // 0から2πまでのランダム角度
    float randomDistance = static_cast<float>(rand()) / RAND_MAX * spawnRadius;        // 0からspawnRadiusまでのランダム距離

    // 2D平面上でランダム位置を計算
    float offsetX = cosf(randomAngle) * randomDistance;
    float offsetZ = sinf(randomAngle) * randomDistance;

    // 新しい位置を設定（高さは元の位置のY座標を維持）
    DirectX::XMFLOAT3 newPosition = {
        originalPosition.x + offsetX,
        originalPosition.y + 0.2f,  // 高さを維持
        originalPosition.z + offsetZ
    };
    obj->transform_->SetWorldPosition(newPosition);

    // 他のコンポーネントも同様に追加
    std::shared_ptr<RendererCom> renderer = obj->AddComponent<RendererCom>(SHADER_ID_MODEL::DEFERRED, BLENDSTATE::MULTIPLERENDERTARGETS, DEPTHSTATE::ZT_ON_ZW_ON, RASTERIZERSTATE::SOLID_CULL_BACK, true, false);
    renderer->LoadModel("Data/Jammo/jammo.mdl");
    obj->AddComponent<MovementCom>();
    obj->AddComponent<NodeCollsionCom>("Data/Jammo/jammocollsion.nodecollsion");
    obj->AddComponent<AnimationCom>();
    obj->AddComponent<AimIKCom>(nullptr, "mixamorig:Neck");
    obj->AddComponent<NoobEnemyCom>();

    // 現在の生成数をインクリメント
    currentSpawnedCount++;
}
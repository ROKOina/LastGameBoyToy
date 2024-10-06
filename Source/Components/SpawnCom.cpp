#include "SpawnCom.h"
#include "Components/TransformCom.h"
#include "Components/RendererCom.h"
#include "Components/AnimationCom.h"
#include <cstdlib>
#include <cmath>

// 初期化
SpawnCom::SpawnCom(const char* filename) : currentSpawnedCount(0)
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
    if (!prototype)
    {
        // プロトタイプが設定されていない場合は何もしない
        return;
    }

    // GameObjectManagerを使って新しいゲームオブジェクトを作成
    std::shared_ptr<GameObject> newObj = GameObjectManager::Instance().Create();

    // 複製元の情報をコピー
    newObj->transform_->SetScale(prototype->transform_->GetScale());

    // オブジェクトに番号を付ける
    std::string objectName = std::string(prototype->GetName()) + "_" + std::to_string(currentSpawnedCount);
    newObj->SetName(objectName.c_str());

    // 元オブジェクトの位置
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
        originalPosition.y,  // 高さを維持
        originalPosition.z + offsetZ
    };
    newObj->transform_->SetWorldPosition(newPosition);

    // RendererComや他のコンポーネントを複製
    std::shared_ptr<RendererCom> renderer = newObj->AddComponent<RendererCom>(SHADER_ID_MODEL::DEFERRED, BLENDSTATE::MULTIPLERENDERTARGETS, DEPTHSTATE::ZT_ON_ZW_ON, RASTERIZERSTATE::SOLID_CULL_BACK, true, false);
    renderer->LoadModel("Data/OneCoin/robot.mdl");

    // 他のコンポーネントも同様に追加
    std::shared_ptr<AnimationCom> anim = newObj->AddComponent<AnimationCom>();
    anim->PlayAnimation(0, true, false, 0.001f);

    // 現在の生成数をインクリメント
    currentSpawnedCount++;
}
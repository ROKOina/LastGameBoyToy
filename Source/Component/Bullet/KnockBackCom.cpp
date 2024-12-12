#include "KnockBackCom.h"
#include "Component\Collsion\ColliderCom.h"
#include "Component\System\HitProcessCom.h"
#include "Component\System\TransformCom.h"
#include "Component\MoveSystem\MovementCom.h"
#include "Math\Mathf.h"

//更新処理
void KnockBackCom::Update(float elapsedTime)
{
    // Colliderコンポーネントの取得
    const auto& collider = GetGameObject()->GetComponent<Collider>();
    if (!collider) return;

    for (auto& obj : collider->OnHitGameObject())
    {
        auto gameObject = obj.gameObject.lock();
        if (!gameObject) continue;

        auto movement = gameObject->GetComponent<MovementCom>();
        if (!movement) continue; // MovementComが無ければ処理をスキップ

        // 自分と衝突対象の位置を取得
        const DirectX::XMFLOAT3 pos = GetGameObject()->transform_->GetWorldPosition();
        const DirectX::XMFLOAT3 enemy = gameObject->transform_->GetWorldPosition();

        // ノックバック力の分離
        const DirectX::XMFLOAT3 forceXZ = { knockbackforce.x, 0, knockbackforce.z };
        const DirectX::XMFLOAT3 forceY = { 0, knockbackforce.y, 0 };

        // XZ平面でのノックバック計算
        const DirectX::XMFLOAT3 normalizedDir = (Mathf::Normalize(enemy - pos) * forceXZ);

        // MovementComの操作
        movement->AddNonMaxSpeedVelocity(normalizedDir);
        movement->AddForce(forceY);
        movement->SetOnGround(false);
        movement->SetAirForce(1.0f);
    }
}

//gui
void KnockBackCom::OnGUI()
{
    ImGui::DragFloat3("knockbackforce", &knockbackforce.x);
}
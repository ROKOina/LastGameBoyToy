#include "KnockBackCom.h"
#include "Component\Collsion\ColliderCom.h"
#include "Component\System\HitProcessCom.h"
#include "Component\System\TransformCom.h"
#include "Component\MoveSystem\MovementCom.h"
#include "Math\Mathf.h"

//XVˆ—
void KnockBackCom::Update(float elapsedTime)
{
    const auto& collider = GetGameObject()->GetComponent<Collider>();
    if (!collider) return;

    for (auto& obj : collider->OnHitGameObject())
    {
        auto gameObject = obj.gameObject.lock();
        if (!gameObject) continue;

        auto movement = gameObject->GetComponent<MovementCom>();
        if (!movement) continue;

        const DirectX::XMFLOAT3 pos = GetGameObject()->transform_->GetWorldPosition();
        const DirectX::XMFLOAT3 enemy = gameObject->transform_->GetWorldPosition();

        const DirectX::XMVECTOR posVec = DirectX::XMLoadFloat3(&pos);
        const DirectX::XMVECTOR enemyVec = DirectX::XMLoadFloat3(&enemy);

        DirectX::XMVECTOR direction = DirectX::XMVectorSubtract(enemyVec, posVec);
        direction = DirectX::XMVector3Normalize(direction);

        DirectX::XMVECTOR forceXZVec = DirectX::XMVectorSet(knockbackforce.x, 0, knockbackforce.z, 0);
        DirectX::XMVECTOR forceYVec = DirectX::XMVectorSet(0, knockbackforce.y, 0, 0);

        DirectX::XMVECTOR resultXZ = DirectX::XMVectorMultiply(direction, forceXZVec);

        DirectX::XMFLOAT3 normalizedDir;
        DirectX::XMStoreFloat3(&normalizedDir, resultXZ);

        movement->ZeroVelocity();
        movement->AddNonMaxSpeedVelocity(normalizedDir);
        movement->AddForce({ 0.0f, knockbackforce.y, 0.0f });
        movement->SetOnGround(false);
        movement->SetAirForce(1.0f);
        movement->SetMoveAcceleration(3.0f);
    }
}

//gui
void KnockBackCom::OnGUI()
{
    ImGui::DragFloat3("knockbackforce", &knockbackforce.x);
}
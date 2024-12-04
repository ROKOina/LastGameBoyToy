#include "KnockBackCom.h"
#include "Component\Collsion\ColliderCom.h"
#include "Component\System\HitProcessCom.h"
#include "Component\System\TransformCom.h"
#include "Math\Mathf.h"

//XVˆ—
void KnockBackCom::Update(float elapsedTime)
{
    const auto& collider = GetGameObject()->GetComponent<Collider>();

    for (auto& obj : collider->OnHitGameObject())
    {
        const auto& hitProcess = GetGameObject()->GetComponent<HitProcessCom>();

        DirectX::XMFLOAT3 pos = GetGameObject()->transform_->GetWorldPosition();
        DirectX::XMFLOAT3 enemy = obj.gameObject.lock()->transform_->GetWorldPosition();

        hitProcess->SetValue3(Mathf::Normalize(enemy - pos) * 3.0f);
    }
}
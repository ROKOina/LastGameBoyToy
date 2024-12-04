#include "KnockBackCom.h"
#include "Component\Collsion\ColliderCom.h"
#include "Component\System\HitProcessCom.h"
#include "Math\Mathf.h"

//XVˆ—
void KnockBackCom::Update(float elapsedTime)
{
    const auto& collider = GetGameObject()->GetComponent<Collider>();

    for (auto& obj : collider->OnHitGameObject())
    {
        const auto& hitProcess = GetGameObject()->GetComponent<HitProcessCom>();
        hitProcess->SetValue3(obj.hitNormal * 3.0f);
    }
}
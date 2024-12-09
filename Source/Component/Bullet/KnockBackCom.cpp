#include "KnockBackCom.h"
#include "Component\Collsion\ColliderCom.h"
#include "Component\System\HitProcessCom.h"
#include "Component\System\TransformCom.h"
#include "Component\MoveSystem\MovementCom.h"
#include "Math\Mathf.h"

//更新処理
void KnockBackCom::Update(float elapsedTime)
{
    const auto& collider = GetGameObject()->GetComponent<Collider>();

    for (auto& obj : collider->OnHitGameObject())
    {
        DirectX::XMFLOAT3 pos = GetGameObject()->transform_->GetWorldPosition();
        DirectX::XMFLOAT3 enemy = obj.gameObject.lock()->transform_->GetWorldPosition();

        if (useTestCoad)
        {
            DirectX::XMFLOAT3 forceXZ = { knockbackforce.x,0,knockbackforce.z };
            DirectX::XMFLOAT3 forceY = { 0,knockbackforce.y,0 };
            DirectX::XMFLOAT3 force = (Mathf::Normalize(enemy - pos) * forceXZ);

            //※※※   ムーブメント例外処理しましょう     ※※※
            //ゲットは一回で良いかと思う
            obj.gameObject.lock()->GetComponent<MovementCom>()->AddNonMaxSpeedVelocity(force);
            obj.gameObject.lock()->GetComponent<MovementCom>()->AddForce(forceY);
            obj.gameObject.lock()->GetComponent<MovementCom>()->SetOnGround(false);
            obj.gameObject.lock()->GetComponent<MovementCom>()->SetAirForce(1.0f);
        }
        else
        {
            const auto& hitProcess = GetGameObject()->GetComponent<HitProcessCom>();
            hitProcess->SetValue3(Mathf::Normalize(enemy - pos) * knockbackforce / elapsedTime);
        }
    }
}

//gui
void KnockBackCom::OnGUI()
{
    ImGui::DragFloat3("knockbackforce", &knockbackforce.x);
}
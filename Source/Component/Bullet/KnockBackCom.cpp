#include "KnockBackCom.h"
#include "Component\Collsion\ColliderCom.h"
#include "Component\System\HitProcessCom.h"
#include "Component\System\TransformCom.h"
#include "Component\MoveSystem\MovementCom.h"
#include "Math\Mathf.h"

//�X�V����
void KnockBackCom::Update(float elapsedTime)
{
    // Collider�R���|�[�l���g�̎擾
    const auto& collider = GetGameObject()->GetComponent<Collider>();
    if (!collider) return;

    for (auto& obj : collider->OnHitGameObject())
    {
        auto gameObject = obj.gameObject.lock();
        if (!gameObject) continue;

        auto movement = gameObject->GetComponent<MovementCom>();
        if (!movement) continue; // MovementCom��������Ώ������X�L�b�v

        // �����ƏՓˑΏۂ̈ʒu���擾
        const DirectX::XMFLOAT3 pos = GetGameObject()->transform_->GetWorldPosition();
        const DirectX::XMFLOAT3 enemy = gameObject->transform_->GetWorldPosition();

        // �m�b�N�o�b�N�͂̕���
        const DirectX::XMFLOAT3 forceXZ = { knockbackforce.x, 0, knockbackforce.z };
        const DirectX::XMFLOAT3 forceY = { 0, knockbackforce.y, 0 };

        // XZ���ʂł̃m�b�N�o�b�N�v�Z
        const DirectX::XMFLOAT3 normalizedDir = (Mathf::Normalize(enemy - pos) * forceXZ);

        // MovementCom�̑���
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
#include "JankratCharacterState.h"
#include "Component\Phsix\RigidBodyCom.h"
#include "Component\Collsion\ColliderCom.h"
#include "Component\System\HitProcessCom.h"
#include "Component\Bullet\BulletCom.h"

JankratCharacter_BaseState::JankratCharacter_BaseState(CharacterCom* owner) : State(owner)
{
    //�����ݒ�
    charaCom = GetComp(JankratCharacterCom);
    moveCom = GetComp(MovementCom);
    transCom = GetComp(TransformCom);
    animationCom = GetComp(AnimationCom);
}

void JankratCharacter_MainAtkState::Enter()
{
    charaCom.lock()->SetHaveBullet(BulletCreate::JankratBombFire(owner->GetGameObject(), transCom.lock()->GetWorldPosition(), charaCom.lock()->GetCharaID()));
}

void JankratCharacter_MainAtkState::Execute(const float& elapsedTime)
{
    if (charaCom.lock()->GetHaveBullet())
    {
        RigidBodyCom* rigid = charaCom.lock()->GetHaveBullet()->GetComponent<RigidBodyCom>().get();

        //�e���Z�b�g����Ă����甭��
        rigid->SetMass(mass);           //����
        rigid->SetRestitution(restitution);    //�����W��
        rigid->SetRigidFlag(physx::PxRigidBodyFlag::eENABLE_CCD, true); //�����Ă��ђʂ��Ȃ��悤�Ȍv�Z�ɂ���t���O
        charaCom.lock()->haveBulletRelease();

        DirectX::XMFLOAT3 vec = owner->GetGameObject()->transform_->GetWorldFront();
        rigid->AddForce(Mathf::Normalize({vec.x, vec.y + 0.2f, vec. z}) * force);
    }

    ChangeAttackState(CharacterCom::CHARACTER_ATTACK_ACTIONS::NONE);
}

void JankratCharacter_MainAtkState::ImGui()
{
    ImGui::DragFloat("Mass", &mass);
    ImGui::DragFloat("Restitution", &restitution);
    ImGui::DragFloat("Force", &force);
}

void JankratCharacter_SubSkillState::Enter()
{
}

void JankratCharacter_SubSkillState::Execute(const float& elapsedTime)
{

}

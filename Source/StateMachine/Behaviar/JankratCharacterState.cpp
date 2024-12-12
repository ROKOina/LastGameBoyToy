#include "JankratCharacterState.h"
#include "Component\Phsix\RigidBodyCom.h"
#include "Component\Collsion\ColliderCom.h"
#include "Component\System\HitProcessCom.h"
#include "Component\Bullet\BulletCom.h"
#include "Component\SkillObj\JankratMineCom.h"
#include "Component\Bullet\JankratBulletCom.h"

//���N���X�N
JankratCharacter_BaseState::JankratCharacter_BaseState(CharacterCom* owner) : State(owner)
{
    //�����ݒ�
    charaCom = GetComp(JankratCharacterCom);
    moveCom = GetComp(MovementCom);
    transCom = GetComp(TransformCom);
    animationCom = GetComp(AnimationCom);
}

// �e�̐�[�ʒu���擾
bool JankratCharacter_BaseState::GetGunTipPosition(DirectX::XMFLOAT3& outGunPos) const
{
    if (std::string(owner->GetGameObject()->GetName()) == "player")
    {
        const auto& cameraObj = owner->GetGameObject()->GetChildFind("cameraPostPlayer");
        const auto& arm = cameraObj->GetChildFind("armChild");

        const auto& model = arm->GetComponent<RendererCom>()->GetModel();
        const auto& gunNode = model->FindNode("gun2"); // �e�̐�[�{�[�����i�����j
        if (!gunNode) return false;

        outGunPos =
        {
            gunNode->worldTransform._41,
            gunNode->worldTransform._42,
            gunNode->worldTransform._43
        };
    }

    return true;
}

// �r�A�j���[�V��������
void JankratCharacter_BaseState::HandleArmAnimation() const
{
    if (std::string(owner->GetGameObject()->GetName()) == "player")
    {
        const auto& arm = owner->GetGameObject()->GetChildFind("cameraPostPlayer")->GetChildFind("armChild");
        const auto& armAnim = arm->GetComponent<AnimationCom>();
        armAnim->PlayAnimation(armAnim->FindAnimation("FPS_shoot"), false);
        armAnim->SetAnimationSeconds(0.3f);
    }
}

// �e�ۂ𔭎˂��鏈��
void JankratCharacter_BaseState::FireBullet(const GameObj& bullet)
{
    const auto& rigid = bullet->GetComponent<RigidBodyCom>().get();
    const auto& jankratBullet = bullet->GetComponent<JankratBulletCom>().get();
    if (!rigid || !jankratBullet) return;

    // �e�ۂ̕����v���p�e�B�ݒ�
    rigid->SetMass(mass);
    rigid->SetRestitution(restitution);
    rigid->SetRigidFlag(physx::PxRigidBodyFlag::eENABLE_CCD, true);

    // �e�ۂ̎����Əd�͐ݒ�
    jankratBullet->SetLifeTime(bulletLifeTimer);
    jankratBullet->SetAddGravity(addGravity);
    jankratBullet->SetExplosionTime(explosiontime);

    // �e�̐�[�ʒu�ƃJ�����������g�p���Ēe�ۂ𔭎�
    DirectX::XMFLOAT3 gunPos = {}, fireDir;
    if (GetGunTipPosition(gunPos))
    {
        // ���˕������v�Z
        fireDir = Mathf::Normalize({
            owner->GetFpsCameraDir().x,
            owner->GetFpsCameraDir().y + fireVecY,
            owner->GetFpsCameraDir().z
            });

        // �e�ۂ̏����ʒu�Ə����x��ݒ�
        bullet->transform_->SetWorldPosition(gunPos);
        rigid->AddForce(fireDir * force);
    }
}

#pragma region �ʏ�e
void JankratCharacter_MainAtkState::Enter()
{
    const auto& charaComponent = charaCom.lock();

    if (!charaComponent)
    {
        return;
    }

    // �e�̐�[�ʒu���擾
    DirectX::XMFLOAT3 gunPos;
    if (GetGunTipPosition(gunPos))
    {
        // �e�ۂ��쐬���Z�b�g
        const auto& bullet = BulletCreate::JankratBulletFire(owner->GetGameObject(), gunPos, charaCom.lock()->GetNetCharaData().GetCharaID());
        charaComponent->SetHaveBullet(bullet);
    }
}
void JankratCharacter_MainAtkState::Execute(const float& elapsedTime)
{
    const auto& charaComponent = charaCom.lock();
    if (!charaComponent)
    {
        return;
    }

    //�r�A�j���[�V�����Đ�
    HandleArmAnimation();

    if (const auto& bullet = charaComponent->GetHaveBullet())
    {
        FireBullet(bullet);
        charaComponent->ReleaseHaveBullet();

        //�e���炳�Ȃ��ƃ����[�h���Ȃ�
        charaComponent->AddCurrentBulletNum(-1);

        //������
        charaComponent->ResetShootTimer();

        // ��ԑJ��
        ChangeAttackState(CharacterCom::CHARACTER_ATTACK_ACTIONS::NONE);
    }
}
void JankratCharacter_MainAtkState::ImGui()
{
    ImGui::DragFloat("Mass", &mass);
    ImGui::DragFloat("Restitution", &restitution);
    ImGui::DragFloat("Force", &force);
    ImGui::DragFloat("BulletLifeTimer", &bulletLifeTimer);
    ImGui::DragFloat("AddGravity", &addGravity);
    ImGui::DragFloat("VecY", &fireVecY);
}
#pragma endregion

#pragma region �n���ݒu
void JankratCharacter_MainSkillState::Execute(const float& elapsedTime)
{
    //�r�A�j���[�V����������
    HandleArmAnimation();

    //�e�̈ʒu���甭��
    DirectX::XMFLOAT3 gunPos = {};
    if (GetGunTipPosition(gunPos))
    {
        // �e�ۂ��쐬���Z�b�g
        charaCom.lock()->AddHaveMine(BulletCreate::JankratMineFire(owner->GetGameObject(), gunPos, 100.0f, 20, charaCom.lock()->GetNetCharaData().GetCharaID()));
        ChangeAttackState(CharacterCom::CHARACTER_ATTACK_ACTIONS::NONE);
    }
}
#pragma endregion

#pragma region �n���N��
void JankratCharacter_SubAttackState::Execute(const float& elapsedTime)
{
    for (auto& mine : charaCom.lock()->GetHaveMine())
    {
        //�S�Ă̐ݒu���̒n�����N��
        mine->GetComponent<JankratMineCom>()->Fire();
    }

    //�J��
    ChangeAttackState(CharacterCom::CHARACTER_ATTACK_ACTIONS::NONE);
}
#pragma endregion

#pragma region �E���g
void JankratCharacter_UltState::Enter()
{
    const auto& charaComponent = charaCom.lock();

    if (!charaComponent)
    {
        return;
    }

    //�����ݒ�
    force = 60.0f;
    fireVecY = 1.350f;
    bulletLifeTimer = 1.0f;
    explosiontime = 0.1f;

    // �e�̐�[�ʒu���擾
    DirectX::XMFLOAT3 gunPos;
    if (GetGunTipPosition(gunPos))
    {
        // �e�ۂ��쐬���Z�b�g
        const auto& bullet = BulletCreate::JankratUlt(owner->GetGameObject(), gunPos, 5.0f);
        charaComponent->SetHaveBullet(bullet);
    }
}
void JankratCharacter_UltState::Execute(const float& elapsedTime)
{
    if (!charaCom.lock()->UseUlt()) return;

    const auto& charaComponent = charaCom.lock();
    if (!charaComponent)
    {
        return;
    }

    //�r�A�j���[�V�����Đ�
    HandleArmAnimation();

    if (const auto& bullet = charaComponent->GetHaveBullet())
    {
        FireBullet(bullet);

        charaComponent->ReleaseHaveBullet();

        //������
        charaComponent->ResetShootTimer();

        // ��ԑJ��
        ChangeAttackState(CharacterCom::CHARACTER_ATTACK_ACTIONS::NONE);
    }
}
void JankratCharacter_UltState::Exit()
{
    //�ݒ��߂��Ă���
    force = 50.0f;
    fireVecY = 0.350f;
    bulletLifeTimer = 3.0f;
    explosiontime = 1.5f;

    //ult�I���
    charaCom.lock()->FinishUlt();
}
void JankratCharacter_UltState::ImGui()
{
    ImGui::DragFloat("Mass", &mass);
    ImGui::DragFloat("Restitution", &restitution);
    ImGui::DragFloat("Force", &force);
    ImGui::DragFloat("BulletLifeTimer", &bulletLifeTimer);
    ImGui::DragFloat("AddGravity", &addGravity);
    ImGui::DragFloat("VecY", &fireVecY);
}
#pragma endregion
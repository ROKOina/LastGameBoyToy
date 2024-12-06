#include "JankratCharacterState.h"
#include "Component\Phsix\RigidBodyCom.h"
#include "Component\Collsion\ColliderCom.h"
#include "Component\System\HitProcessCom.h"
#include "Component\Bullet\BulletCom.h"
#include "Component\SkillObj\JankratMineCom.h"
#include "Component\Bullet\JankratBulletCom.h"

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
    auto& cameraObj = owner->GetGameObject()->GetChildFind("cameraPostPlayer");
    auto& arm = cameraObj->GetChildFind("armChild");
    if (arm)
    {
        const auto& model = arm->GetComponent<RendererCom>()->GetModel();
        const auto& gunNode = model->FindNode("gun2"); // �e�̐�[�{�[�����i�����j

        if (gunNode)
        {
            // �e��[�{�[���̃��[���h�ʒu���擾
            DirectX::XMFLOAT3 gunPos =
            {
                gunNode->worldTransform._41,
                gunNode->worldTransform._42,
                gunNode->worldTransform._43
            };

            charaCom.lock()->SetHaveBullet(BulletCreate::JankratBulletFire(owner->GetGameObject(), gunPos, charaCom.lock()->GetCharaID()));
        }
    }
}

void JankratCharacter_MainAtkState::Execute(const float& elapsedTime)
{
    if (charaCom.lock()->GetHaveBullet())
    {
        GameObj bullet = charaCom.lock()->GetHaveBullet();
        RigidBodyCom* rigid = bullet->GetComponent<RigidBodyCom>().get();
        JankratBulletCom* jankratBullet = bullet->GetComponent<JankratBulletCom>().get();

        // �e���Z�b�g����Ă����甭��
        rigid->SetMass(mass);           // ����
        rigid->SetRestitution(restitution);    // �����W��
        rigid->SetRigidFlag(physx::PxRigidBodyFlag::eENABLE_CCD, true); // �ђʖh�~�t���O
        charaCom.lock()->ReleaseHaveBullet();

        jankratBullet->SetLifeTime(bulletLifeTimer); // �e�̎���
        jankratBullet->SetAddGravity(addGravity);

        auto& cameraObj = owner->GetGameObject()->GetChildFind("cameraPostPlayer");
        auto& arm = cameraObj->GetChildFind("armChild");
        if (arm)
        {
            const auto& model = arm->GetComponent<RendererCom>()->GetModel();
            const auto& gunNode = model->FindNode("gun2"); // �e�̐�[�{�[�����i�����j

            if (gunNode)
            {
                // �e��[�{�[���̃��[���h�ʒu���擾
                DirectX::XMFLOAT3 gunPos =
                {
                    gunNode->worldTransform._41,
                    gunNode->worldTransform._42,
                    gunNode->worldTransform._43
                };

                // �J�����̕����x�N�g�����擾
                DirectX::XMFLOAT3 cameraDir = owner->GetFpsCameraDir();

                // ���˕����x�N�g���̌v�Z (���K�� + Y���␳)
                DirectX::XMFLOAT3 fireDir = Mathf::Normalize({
                    cameraDir.x,
                    cameraDir.y + fireVecY,
                    cameraDir.z
                    });

                // �e�̏����ʒu��ݒ�
                bullet->transform_->SetWorldPosition(gunPos);

                // �e�̏����x��ݒ� (�����x�N�g���ɑ��x����Z)
                rigid->AddForce(fireDir * force);
            }
        }
    }

    ChangeAttackState(CharacterCom::CHARACTER_ATTACK_ACTIONS::NONE);
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

void JankratCharacter_MainSkillState::Enter()
{
}

void JankratCharacter_MainSkillState::Execute(const float& elapsedTime)
{
    //TODO ���˒n�_���e�̈ʒu�ɕύX
    DirectX::XMFLOAT3 firePos = owner->GetGameObject()->transform_->GetWorldPosition();
    firePos.y += 2;

    charaCom.lock()->AddHaveMine(BulletCreate::JankratMineFire(owner->GetGameObject(), firePos, 100.0f, 20, owner->GetCharaID()));
    ChangeAttackState(CharacterCom::CHARACTER_ATTACK_ACTIONS::NONE);
}

void JankratCharacter_SubAttackState::Execute(const float& elapsedTime)
{
    for (auto& mine : charaCom.lock()->GetHaveMine())
    {
        //�S�Ă̐ݒu���̒n�����N��
        mine->GetComponent<JankratMineCom>()->Fire();
    }

    //TODO �A�j���[�V�����I����Ă���J��
    ChangeAttackState(CharacterCom::CHARACTER_ATTACK_ACTIONS::NONE);
}
#include "SolderState.h"
#include <Component\Collsion\ColliderCom.h>
#include "Component\Renderer\RendererCom.h"
#include "Component\Particle\GPUParticle.h"
#include "Component\Bullet\BulletCom.h"

//���N���X
Solder_BaseState::Solder_BaseState(CharacterCom* owner) : State(owner)
{
    //�����ݒ�
    charaCom = GetComp(SoldierCom);
    moveCom = GetComp(MovementCom);
    transCom = GetComp(TransformCom);
    animationCom = GetComp(AnimationCom);
}

#pragma region ���C���U��
void Solder_MainAttackState::Enter()
{
    rayobj = owner->GetGameObject()->GetChildFind("mainattack");
    if (!rayobj)return;

    //���C�ݒ�
    auto& ray = rayobj->GetComponent<RayColliderCom>();
    DirectX::XMFLOAT3 start = rayobj->transform_->GetWorldPosition();

    //�J�����擾
    auto& camera = GameObjectManager::Instance().Find("cameraPostPlayer");
    DirectX::XMFLOAT3 front = camera->transform_->GetWorldFront();
    DirectX::XMFLOAT3 end = start + front * 100;

    //�G�t�F�N�g�Đ�
    owner->GetGameObject()->GetChildFind("beem_fire")->GetComponent<GPUParticle>()->Play();

    //���C�L���X�g�L����
    ray->SetStart(start);
    ray->SetEnd(end);
    ray->SetEnabled(true);
}
void Solder_MainAttackState::Execute(const float& elapsedTime)
{
    auto& ray = rayobj->GetComponent<RayColliderCom>();

    //���C�L���X�gOFF
    if (CharacterInput::MainAttackButton & owner->GetButton())
    {
        ray->SetEnabled(false);
    }

    //�U���I���������U������
    if (CharacterInput::MainAttackButton & owner->GetButtonUp())
    {
        //�r�A�j���[�V����������
        if (std::string(owner->GetGameObject()->GetName()) == "player")
        {
            auto& arm = owner->GetGameObject()->GetChildFind("cameraPostPlayer")->GetChildFind("armChild");
            auto& armAnim = arm->GetComponent<AnimationCom>();
            armAnim->PlayAnimation(armAnim->FindAnimation("FPS_shoot"), false);
            armAnim->SetAnimationSeconds(0.5f);
        }

        //�e���炳�Ȃ��ƃ����[�h���Ȃ�
        charaCom.lock()->AddCurrentBulletNum(-1);

        //�ˌ��Ԋu�^�C�}�[�N��
        charaCom.lock()->ResetShootTimer();

        //���C�L���X�gOFF
        ray->SetEnabled(false);

        //�X�e�[�g�ύX
        ChangeAttackState(CharacterCom::CHARACTER_ATTACK_ACTIONS::NONE);
    }
}
void Solder_MainAttackState::Exit()
{
    rayobj.reset();
}
#pragma endregion

#pragma region ult�U��
void Solder_UltState::Enter()
{
}
void Solder_UltState::Execute(const float& elapsedTime)
{
}
void Solder_UltState::Exit()
{
}
void Solder_UltState::ImGui()
{
}
#pragma endregion

#pragma region Eskill
void Solder_ESkillState::Enter()
{
    //�G�t�F�N�g�Đ�
    owner->GetGameObject()->GetChildFind("beem_fire")->GetComponent<GPUParticle>()->Play();
}
void Solder_ESkillState::Execute(const float& elapsedTime)
{
    //�U���I���������U������
    if (std::string(owner->GetGameObject()->GetName()) == "player")
    {
        auto& arm = owner->GetGameObject()->GetChildFind("cameraPostPlayer")->GetChildFind("armChild");
        auto& armAnim = arm->GetComponent<AnimationCom>();
        armAnim->PlayAnimation(armAnim->FindAnimation("FPS_shoot"), false);
        armAnim->SetAnimationSeconds(0.3f);
    }

    //�U������(�X�^���{�[��)
    BulletCreate::SoldierStanBall(owner->GetGameObject(), 50.0f, 10.0f, 2.0f);

    //�X�e�[�g�ύX
    ChangeAttackState(CharacterCom::CHARACTER_ATTACK_ACTIONS::NONE);
}
#pragma endregion

#pragma region �E�N���b�N�X�L��
void Solder_RightClickSkillState::Enter()
{
    //�G�t�F�N�g�Đ�
    owner->GetGameObject()->GetChildFind("beem_fire")->GetComponent<GPUParticle>()->Play();
}
void Solder_RightClickSkillState::Execute(const float& elapsedTime)
{
    //�U���I���������U������
    if (std::string(owner->GetGameObject()->GetName()) == "player")
    {
        auto& arm = owner->GetGameObject()->GetChildFind("cameraPostPlayer")->GetChildFind("armChild");
        auto& armAnim = arm->GetComponent<AnimationCom>();
        armAnim->PlayAnimation(armAnim->FindAnimation("FPS_shoot"), false);
        armAnim->SetAnimationSeconds(0.3f);
    }

    //�U������
    BulletCreate::SoldierEskillBullet(owner->GetGameObject(), 50.0f, 20.0f);

    //�X�e�[�g�ύX
    ChangeAttackState(CharacterCom::CHARACTER_ATTACK_ACTIONS::NONE);
}
#pragma endregion
#include "SolderState.h"

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
}
void Solder_MainAttackState::Execute(const float& elapsedTime)
{
    //�U���I���������U������
    if (CharacterInput::MainAttackButton & owner->GetButtonUp())
    {
        //�r�A�j���[�V����������
        if (std::string(owner->GetGameObject()->GetName()) == "player")
        {
            auto& arm = owner->GetGameObject()->GetChildFind("cameraPostPlayer")->GetChildFind("armChild");
            auto& armAnim = arm->GetComponent<AnimationCom>();
            armAnim->PlayAnimation(armAnim->FindAnimation("FPS_shoot"), false);
            armAnim->SetAnimationSeconds(0.3f);
        }

        //�e���炳�Ȃ��ƃ����[�h���Ȃ�
        charaCom.lock()->AddCurrentBulletNum(-1);

        //�ˌ��Ԋu�^�C�}�[�N��
        charaCom.lock()->ResetShootTimer();

        //�X�e�[�g�ύX
        ChangeAttackState(CharacterCom::CHARACTER_ATTACK_ACTIONS::NONE);
    }
}
void Solder_MainAttackState::Exit()
{
}
void Solder_MainAttackState::ImGui()
{
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

    //�X�e�[�g�ύX
    ChangeAttackState(CharacterCom::CHARACTER_ATTACK_ACTIONS::NONE);
}
void Solder_ESkillState::Exit()
{
}
void Solder_ESkillState::ImGui()
{
}
#pragma endregion
#include "FarahState.h"
#include "Component\Bullet\BulletCom.h"

//���N���X�ł�
Farah_BaseState::Farah_BaseState(CharacterCom* owner) : State(owner)
{
    //�����ݒ�
    charaCom = GetComp(FarahCom);
    moveCom = GetComp(MovementCom);
    transCom = GetComp(TransformCom);
    animationCom = GetComp(AnimationCom);
}

#pragma region ���C���U��
void Farah_MainAttackState::Enter()
{
}
void Farah_MainAttackState::Execute(const float& elapsedTime)
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

        //�㔼�g�A�j���[�V����?
        owner->GetGameObject()->GetComponent<AnimationCom>()->SetUpAnimationUpdate(AnimationCom::AnimationType::NormalAnimation);

        //�U������
        BulletCreate::FarahDamageFire(owner->GetGameObject(), 65.0f);

        //�X�e�[�g�ύX
        ChangeAttackState(CharacterCom::CHARACTER_ATTACK_ACTIONS::NONE);
    }
}
#pragma endregion
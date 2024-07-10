#include "UenoCharacterState.h"
#include "Input\Input.h"
#include "Components\RendererCom.h"
#include "Components\CameraCom.h"
#include "Components\Character\BulletCom.h"
#include "BaseCharacterState.h"

UenoCharacterState_BaseState::UenoCharacterState_BaseState(CharacterCom* owner) : State(owner)
{
    //�����ݒ�
    charaCom = GetComp(UenoCharacterCom);
    moveCom = GetComp(MovementCom);
    transCom = GetComp(TransformCom);
    animationCom = GetComp(AnimationCom);
}

void UenoCharacterState_AttackState::Enter()
{
}

//�X�e�[�g�X�V����
void UenoCharacterState_AttackState::Execute(const float& elapsedTime)
{
    MoveInputVec(owner->GetGameObject(), 0.5f);

    if (moveCom.lock()->OnGround())
        JumpInput(owner->GetGameObject());

    //�U���I���������U������
    if (CharacterInput::MainAttackButton & owner->GetButtonUp())
    {
        charaCom.lock()->SetLazerFlag(false);
        ChangeAttackState(CharacterCom::CHARACTER_ATTACK_ACTIONS::NONE);
        t = true;
    }
    else
    {
        if (t)
        {
            charaCom.lock()->gpulazerparticle->Reset();
            t = false;
        }
        charaCom.lock()->SetLazerFlag(true);
    }
}

//imgui
void UenoCharacterState_AttackState::ImGui()
{
}
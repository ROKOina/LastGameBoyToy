#include "UenoCharacterState.h"
#include "Input\Input.h"
#include "Components\RendererCom.h"
#include "Components\CameraCom.h"
#include "Components\Character\BulletCom.h"
#include "BaseCharacterState.h"

// �}�N��
#define GetComp(Component) owner->GetGameObject()->GetComponent<Component>();
#define ChangeState(State) testCharaCom.lock()->GetStateMachine().ChangeState(State);

UenoCharacterState_BaseState::UenoCharacterState_BaseState(CharacterCom* owner) : State(owner)
{
    //�����ݒ�
    testCharaCom = GetComp(UenoCharacterCom);
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
        testCharaCom.lock()->SetLazerFlag(false);
        ChangeState(CharacterCom::CHARACTER_ACTIONS::IDLE);
        t = true;
    }
    else
    {
        if (t)
        {
            testCharaCom.lock()->gpulazerparticle->Reset();
            t = false;
        }
        testCharaCom.lock()->SetLazerFlag(true);
    }
}

//imgui
void UenoCharacterState_AttackState::ImGui()
{
}
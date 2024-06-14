#include "BaseCharacterState.h"
#include "Input\Input.h"
#include "BaseCharacterState.h"

// �}�N��
#define GetComp(Component) owner->GetGameObject()->GetComponent<Component>();
#define ChangeState(State) charaCom.lock()->GetStateMachine().ChangeState(State);

BaseCharacter_BaseState::BaseCharacter_BaseState(CharacterCom* owner) : State(owner)
{
    //�����ݒ�
    charaCom = GetComp(CharacterCom);
    moveCom = GetComp(MovementCom);
    transCom = GetComp(TransformCom);
    animationCom = GetComp(AnimationCom);
}

void BaseCharacter_IdleState::Enter()
{
    //�����A�j���[�V�����Đ��J�n
    animationCom.lock()->PlayAnimation(animationCom.lock()->FindAnimation("Idle"), true, false, 0.1f);
}

void BaseCharacter_IdleState::Execute(const float& elapsedTime)
{
    //���͒l�擾
    DirectX::XMFLOAT3 moveVec = SceneManager::Instance().InputVec();

    if (moveVec != 0)
    {
        ChangeState(CharacterCom::CHARACTER_ACTIONS::MOVE);
    }
}

void BaseCharacter_MoveState::Enter()
{
    //�����A�j���[�V�����Đ��J�n
    animationCom.lock()->PlayAnimation(animationCom.lock()->FindAnimation("Walk_Forward"), true);
}

void BaseCharacter_MoveState::Execute(const float& elapsedTime)
{
    GamePad gamePad = Input::Instance().GetGamePad();

    //���͒l�擾
    DirectX::XMFLOAT3 moveVec = SceneManager::Instance().InputVec();

    //����
    DirectX::XMFLOAT3 v = moveVec * moveCom.lock()->GetMoveAcceleration();
    moveCom.lock()->AddForce(v);
    QuaternionStruct q = transCom.lock()->GetRotation();

    //���񏈗�
    transCom.lock()->Turn(moveVec, 0.1f);

    //�W�����v
    if (GamePad::BTN_A & gamePad.GetButtonDown())
    {
        ChangeState(CharacterCom::CHARACTER_ACTIONS::JUMP);
    }
}

void BaseCharacter_JumpState::Enter()
{
    //�W�����v
    moveCom.lock()->AddForce({ 0,charaCom.lock()->GetJumpPower(),0 });
}

void BaseCharacter_JumpState::Execute(const float& elapsedTime)
{
    GamePad gamePad = Input::Instance().GetGamePad();

    //���͒l�擾
    DirectX::XMFLOAT3 moveVec = SceneManager::Instance().InputVec();

    //�󒆐���
    DirectX::XMFLOAT3 v = moveVec * moveCom.lock()->GetMoveAcceleration();
    moveCom.lock()->AddForce(v);
    transCom.lock()->Turn(moveVec, 0.1f);

    if (moveCom.lock()->OnGround())
    {
        ChangeState(CharacterCom::CHARACTER_ACTIONS::IDLE);
    }
}


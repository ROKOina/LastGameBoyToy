#include "TestCharacterState.h"
#include "Input\Input.h"

// �}�N��
#define GetComp(Component) owner->GetGameObject()->GetComponent<Component>();
#define ChangeState(State) testCharaCom.lock()->GetStateMachine().ChangeState(State);

TestCharacter_BaseState::TestCharacter_BaseState(CharacterCom* owner) : State(owner)
{
    //�����ݒ�
    testCharaCom = GetComp(TestCharacterCom);
    moveCom = GetComp(MovementCom);
    transCom = GetComp(TransformCom);
}


void TestCharacter_MoveState::Enter()
{
    //�����A�j���[�V�����Đ��J�n
}

void TestCharacter_MoveState::Execute(const float& elapsedTime)
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

void TestCharacter_JumpState::Enter()
{
    //�W�����v
    moveCom.lock()->AddForce(jumpPower);
}

void TestCharacter_JumpState::Execute(const float& elapsedTime)
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
        ChangeState(CharacterCom::CHARACTER_ACTIONS::MOVE);
    }
}
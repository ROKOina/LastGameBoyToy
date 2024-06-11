#include "TestCharacterState.h"
#include "Input\Input.h"

// �}�N��
#define GetComp(Component) owner->GetGameObject()->GetComponent<Component>();
#define ChangeState(State) testCharaCom.lock()->GetStateMachine().ChangeState(State);

TestCharacter_BaseState::TestCharacter_BaseState(CharacterCom* owner) : State(owner)
{
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
    QuaternionStruct q = transCom.lock()->GetRotation(); //transCom.lock()->Turn(elapsedTime, moveVec,10.0f);
    
    DirectX::XMVECTOR currentRot = DirectX::XMLoadFloat4(&q.dxFloat4);
    DirectX::XMVECTOR goalRot = DirectX::XMLoadFloat4(&q.LookRotation(moveVec).dxFloat4);

    DirectX::XMFLOAT4 rot = {};
    DirectX::XMStoreFloat4(&rot, DirectX::XMQuaternionNormalize(DirectX::XMQuaternionSlerp(currentRot, goalRot, 0.1f)));

    transCom.lock()->SetRotation(rot);

    if (GamePad::BTN_A & gamePad.GetButtonDown())
    {
        ChangeState(CharacterCom::CHARACTER_ACTIONS::JUMP);
    }
}

void TestCharacter_JumpState::Enter()
{
    //�W�����v
    moveCom.lock()->AddForce({0,10.5f,0});
}

void TestCharacter_JumpState::Execute(const float& elapsedTime)
{
    GamePad gamePad = Input::Instance().GetGamePad();

    //���͒l�擾
    DirectX::XMFLOAT3 moveVec = SceneManager::Instance().InputVec();

    //�󒆐���
    DirectX::XMFLOAT3 v = moveVec * moveCom.lock()->GetMoveAcceleration();
    moveCom.lock()->AddForce(v);

    if (moveCom.lock()->OnGround())
    {
        ChangeState(CharacterCom::CHARACTER_ACTIONS::MOVE);
    }
}
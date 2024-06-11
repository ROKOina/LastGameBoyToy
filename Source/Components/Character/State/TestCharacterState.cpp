#include "TestCharacterState.h"
#include "Input\Input.h"

// マクロ
#define GetComp(Component) owner->GetGameObject()->GetComponent<Component>();
#define ChangeState(State) testCharaCom.lock()->GetStateMachine().ChangeState(State);

TestCharacter_BaseState::TestCharacter_BaseState(CharacterCom* owner) : State(owner)
{
    //初期設定
    testCharaCom = GetComp(TestCharacterCom);
    moveCom = GetComp(MovementCom);
    transCom = GetComp(TransformCom);
}


void TestCharacter_MoveState::Enter()
{
    //歩きアニメーション再生開始
}

void TestCharacter_MoveState::Execute(const float& elapsedTime)
{
    GamePad gamePad = Input::Instance().GetGamePad();

    //入力値取得
    DirectX::XMFLOAT3 moveVec = SceneManager::Instance().InputVec();

    //歩く
    DirectX::XMFLOAT3 v = moveVec * moveCom.lock()->GetMoveAcceleration();
    moveCom.lock()->AddForce(v);
    QuaternionStruct q = transCom.lock()->GetRotation();

    //旋回処理
    transCom.lock()->Turn(moveVec, 0.1f);

    //ジャンプ
    if (GamePad::BTN_A & gamePad.GetButtonDown())
    {
        ChangeState(CharacterCom::CHARACTER_ACTIONS::JUMP);
    }
}

void TestCharacter_JumpState::Enter()
{
    //ジャンプ
    moveCom.lock()->AddForce(jumpPower);
}

void TestCharacter_JumpState::Execute(const float& elapsedTime)
{
    GamePad gamePad = Input::Instance().GetGamePad();

    //入力値取得
    DirectX::XMFLOAT3 moveVec = SceneManager::Instance().InputVec();

    //空中制御
    DirectX::XMFLOAT3 v = moveVec * moveCom.lock()->GetMoveAcceleration();
    moveCom.lock()->AddForce(v);
    transCom.lock()->Turn(moveVec, 0.1f);

    if (moveCom.lock()->OnGround())
    {
        ChangeState(CharacterCom::CHARACTER_ACTIONS::MOVE);
    }
}
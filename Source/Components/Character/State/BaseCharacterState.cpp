#include "BaseCharacterState.h"
#include "Input\Input.h"
#include "BaseCharacterState.h"

// マクロ
#define GetComp(Component) owner->GetGameObject()->GetComponent<Component>();
#define ChangeState(State) charaCom.lock()->GetStateMachine().ChangeState(State);

BaseCharacter_BaseState::BaseCharacter_BaseState(CharacterCom* owner) : State(owner)
{
    //初期設定
    charaCom = GetComp(CharacterCom);
    moveCom = GetComp(MovementCom);
    transCom = GetComp(TransformCom);
    animationCom = GetComp(AnimationCom);
}

void BaseCharacter_IdleState::Enter()
{
    //歩きアニメーション再生開始
    animationCom.lock()->PlayAnimation(animationCom.lock()->FindAnimation("Idle"), true, false, 0.1f);
}

void BaseCharacter_IdleState::Execute(const float& elapsedTime)
{
    //入力値取得
    DirectX::XMFLOAT3 moveVec = SceneManager::Instance().InputVec();

    if (moveVec != 0)
    {
        ChangeState(CharacterCom::CHARACTER_ACTIONS::MOVE);
    }
}

void BaseCharacter_MoveState::Enter()
{
    //歩きアニメーション再生開始
    animationCom.lock()->PlayAnimation(animationCom.lock()->FindAnimation("Walk_Forward"), true);
}

void BaseCharacter_MoveState::Execute(const float& elapsedTime)
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

void BaseCharacter_JumpState::Enter()
{
    //ジャンプ
    moveCom.lock()->AddForce({ 0,charaCom.lock()->GetJumpPower(),0 });
}

void BaseCharacter_JumpState::Execute(const float& elapsedTime)
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
        ChangeState(CharacterCom::CHARACTER_ACTIONS::IDLE);
    }
}


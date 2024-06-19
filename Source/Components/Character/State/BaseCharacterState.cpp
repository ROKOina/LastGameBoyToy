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
    GamePad& gamePad = Input::Instance().GetGamePad();

    //入力値取得
    DirectX::XMFLOAT3 moveVec = SceneManager::Instance().InputVec();

    //移動
    if (moveVec != 0)
    {
        ChangeState(CharacterCom::CHARACTER_ACTIONS::MOVE);
    }
    //ジャンプ
    if (GamePad::BTN_A & gamePad.GetButtonDown())
    {
        ChangeState(CharacterCom::CHARACTER_ACTIONS::JUMP);
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

    MoveInputVec(owner->GetGameObject());

    //入力値取得
    DirectX::XMFLOAT3 moveVec = SceneManager::Instance().InputVec();

    //待機
    if (moveVec == 0)
    {
        ChangeState(CharacterCom::CHARACTER_ACTIONS::IDLE);
    }
    //ジャンプ
    if (GamePad::BTN_A & gamePad.GetButtonDown())
    {
        ChangeState(CharacterCom::CHARACTER_ACTIONS::JUMP);
    }
}

void BaseCharacter_JumpState::Enter()
{
    //ジャンプ
    if (!moveCom.lock()->OnGround())
        return;

    JumpInput(owner->GetGameObject());
    moveCom.lock()->SetOnGround(false);

    //moveCom.lock()->AddForce(jumpPower);
    //moveCom.lock()->SetOnGround(false);
}

void BaseCharacter_JumpState::Execute(const float& elapsedTime)
{
    GamePad gamePad = Input::Instance().GetGamePad();

    MoveInputVec(owner->GetGameObject());

    if (moveCom.lock()->OnGround())
    {
        ChangeState(CharacterCom::CHARACTER_ACTIONS::IDLE);
    }
}


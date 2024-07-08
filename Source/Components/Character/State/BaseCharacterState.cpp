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
    animationCom.lock()->SetUpAnimationUpdate(AnimationCom::AnimationType::NormalAnimation);
    animationCom.lock()->PlayAnimation(animationCom.lock()->FindAnimation("Idle"), true, false, 0.1f);
}

void BaseCharacter_IdleState::Execute(const float& elapsedTime)
{
    //入力値取得
    DirectX::XMFLOAT3 moveVec = SceneManager::Instance().InputVec();

    //移動
    if (moveVec != 0)
    {
        ChangeState(CharacterCom::CHARACTER_ACTIONS::MOVE);
    }
    //ジャンプ
    if (GamePad::BTN_A & owner->GetButtonDown())
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
    MoveInputVec(owner->GetGameObject());

    //入力値取得
    DirectX::XMFLOAT3 moveVec = SceneManager::Instance().InputVec();

    //待機
    if (moveVec == 0)
    {
        ChangeState(CharacterCom::CHARACTER_ACTIONS::IDLE);
    }
    //ジャンプ
    if (GamePad::BTN_A & owner->GetButtonDown())
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
    moveVec = SceneManager::Instance().InputVec();
    moveCom.lock()->SetOnGround(false);
}

void BaseCharacter_JumpState::Execute(const float& elapsedTime)
{
    //空中制御
    DirectX::XMFLOAT3 inputVec = SceneManager::Instance().InputVec();
    moveVec = Mathf::Lerp(moveVec, inputVec, 0.1f);

    if (moveCom.lock()->GetVelocity().y < 0.05f && HoveringTimer < HoveringTime)
    {
        DirectX::XMFLOAT3 verocity = moveCom.lock()->GetVelocity();
        verocity.y = -GRAVITY_NORMAL * elapsedTime;
        moveCom.lock()->SetVelocity(verocity);

        HoveringTimer += elapsedTime;
    }

    DirectX::XMFLOAT3 v = moveVec * moveCom.lock()->GetMoveAcceleration();
    moveCom.lock()->AddForce(v);


    if (moveCom.lock()->OnGround())
    {
        ChangeState(CharacterCom::CHARACTER_ACTIONS::IDLE);
    }
}

void BaseCharacter_JumpState::Exit()
{
    HoveringTimer = 0.0f;
}


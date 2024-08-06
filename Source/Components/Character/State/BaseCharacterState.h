#pragma once
#include "../../System/State.h"
#include "../CharacterCom.h"
#include "../../MovementCom.h"
#include "../../TransformCom.h"
#include "../../AnimationCom.h"

//入力値方向に移動と旋回
static void MoveInputVec(std::shared_ptr<GameObject> obj, float speed = 1)
{
    auto& moveCom = obj->GetComponent<MovementCom>();

    GamePad gamePad = Input::Instance().GetGamePad();

    //入力値取得
    DirectX::XMFLOAT3 moveVec = SceneManager::Instance().InputVec();

    //歩く
    DirectX::XMFLOAT3 v = moveVec * speed;
    moveCom->AddForce(v);

    //旋回処理
    //obj->transform_->Turn(moveVec, 0.1f);
}

//ジャンプ
static void JumpInput(std::shared_ptr<GameObject> obj, float speed = 1)
{
    auto& moveCom = obj->GetComponent<MovementCom>();

    if (CharacterInput::JumpButton_SPACE & obj->GetComponent<CharacterCom>()->GetButtonDown())
    {
        DirectX::XMFLOAT3 power = { 0.0f,obj->GetComponent<CharacterCom>()->GetJumpPower() * speed,0.0f };
        moveCom->AddForce(power);
    }
}

class BaseCharacter_BaseState : public State<CharacterCom> {
public:
    BaseCharacter_BaseState(CharacterCom* owner);

protected:
    std::weak_ptr<CharacterCom> charaCom;
    std::weak_ptr<MovementCom> moveCom;
    std::weak_ptr<TransformCom> transCom;
    std::weak_ptr<AnimationCom> animationCom;
};

class BaseCharacter_IdleState : public BaseCharacter_BaseState
{
public:
    BaseCharacter_IdleState(CharacterCom* owner) : BaseCharacter_BaseState(owner) {}

    void Enter() override;
    void Execute(const float& elapsedTime) override;
    virtual const char* GetName() const override { return "Idle"; }
};

class BaseCharacter_MoveState : public BaseCharacter_BaseState
{
public:
    BaseCharacter_MoveState(CharacterCom* owner) : BaseCharacter_BaseState(owner) {}

    void Enter() override;
    void Execute(const float& elapsedTime) override;
    virtual const char* GetName() const override { return "Move"; }
};

class BaseCharacter_JumpState : public BaseCharacter_BaseState
{
public:
    BaseCharacter_JumpState(CharacterCom* owner) : BaseCharacter_BaseState(owner) {}

    void Enter() override;
    void Execute(const float& elapsedTime) override;
    void Exit() override;
    virtual const char* GetName() const override { return "Jump"; }

private:

    DirectX::XMFLOAT3 moveVec = {};
    DirectX::XMFLOAT3 jumpPower = { 0, 12.0f, 0 };
    float HoveringTimer = 0.0f;
    float HoveringTime = 0.05f;
};

class BaseCharacter_NoneAttack : public BaseCharacter_BaseState
{
public:
    BaseCharacter_NoneAttack(CharacterCom* owner) : BaseCharacter_BaseState(owner) {}

    void Enter() override;
    virtual const char* GetName() const override { return "NoneAttack"; }
};
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
        DirectX::XMFLOAT3 power = { 0,obj->GetComponent<CharacterCom>()->GetJumpPower() * speed,0 };
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
};

class BaseCharacter_MoveState : public BaseCharacter_BaseState
{
public:
    BaseCharacter_MoveState(CharacterCom* owner) : BaseCharacter_BaseState(owner) {}

    void Enter() override;
    void Execute(const float& elapsedTime) override;
};

class BaseCharacter_JumpState : public BaseCharacter_BaseState
{
public:
    BaseCharacter_JumpState(CharacterCom* owner) : BaseCharacter_BaseState(owner) {}

    void Enter() override;
    void Execute(const float& elapsedTime) override;
    void Exit() override;

private:

    DirectX::XMFLOAT3 moveVec = {};
    DirectX::XMFLOAT3 jumpPower = { 0, 12.0f, 0 };
    float HoveringTimer = 0.0f;
    float HoveringTime = 0.05f;
};

class BaseCharacter_HitscanState : public BaseCharacter_BaseState
{
    //　※　ヒットスキャンOBJを起動するだけのステート　※
    //  ownerのObjの子に"rayObj"という名前のObjを作り
    //  <RayColliderCom>を追加する

public:
    BaseCharacter_HitscanState(CharacterCom* owner) : BaseCharacter_BaseState(owner)
    {
        name = "Hitscan";
    }

    void Enter() override;
    void Execute(const float& elapsedTime) override;
    void Exit() override;
    void ImGui() override;

private:
    float rayLength = 100;
};

class BaseCharacter_CapsuleState : public BaseCharacter_BaseState
{
    //　※　カプセルOBJを起動するだけのステート　※
    //  ownerのObjの子に"capsuleObj"という名前のObjを作る
    //  NodeCollsionCom以外の形状と判定する

public:
    BaseCharacter_CapsuleState(CharacterCom* owner) : BaseCharacter_BaseState(owner) 
    {
        name = "Capsule";
    }

    void Enter() override;
    void Execute(const float& elapsedTime) override;
    void Exit() override;
    void ImGui() override;

private:
    float capsuleLength = 5;
};

class BaseCharacter_StanBallState : public BaseCharacter_BaseState
{
public:
    BaseCharacter_StanBallState(CharacterCom* owner) : BaseCharacter_BaseState(owner)
    {
        name = "StanBall";
    }

    void Enter() override;
    void Execute(const float& elapsedTime) override;
    void Exit() override;
    void ImGui() override;

private:
    float speed=40;
    float power=1;
};

class BaseCharacter_NoneAttack : public BaseCharacter_BaseState
{
public:
    BaseCharacter_NoneAttack(CharacterCom* owner) : BaseCharacter_BaseState(owner) {}

    void Enter() override;
};
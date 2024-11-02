#pragma once
#include "StateMachine\State.h"
#include "Component\Character\CharacterCom.h"
#include "Component/MoveSystem/MovementCom.h"
#include "Component/System/TransformCom.h"
#include "Component/Animation/AnimationCom.h"

//���͒l�����Ɉړ��Ɛ���
static void MoveInputVec(std::shared_ptr<GameObject> obj, float speed = 100)
{
    auto& moveCom = obj->GetComponent<MovementCom>();

    //GamePad gamePad = Input::Instance().GetGamePad();

    //���͒l�擾
    //obj->GetComponent<CharacterCom>()->GetLeftStick()
    DirectX::XMFLOAT3 moveVec = SceneManager::Instance().InputVec(obj);

    //����
    DirectX::XMFLOAT3 v = moveVec * speed;
    moveCom->AddForce(v);

    //���񏈗�
    //obj->transform_->Turn(moveVec, 0.1f);
}

//�W�����v
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
    virtual const char* GetName() const override { return "Idle"; }
};

class BaseCharacter_MoveState : public BaseCharacter_BaseState
{
public:
    BaseCharacter_MoveState(CharacterCom* owner) : BaseCharacter_BaseState(owner) {}

    void Enter() override;
    void Execute(const float& elapsedTime) override;
    void Exit() override;
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
    float HoveringTimer = 0.0f;
    float HoveringTime = 0.05f;
};

class BaseCharacter_HitscanState : public BaseCharacter_BaseState
{
    //�@���@�q�b�g�X�L����OBJ���N�����邾���̃X�e�[�g�@��
    //  owner��Obj�̎q��"rayObj"�Ƃ������O��Obj�����
    //  <RayColliderCom>��ǉ�����

public:
    BaseCharacter_HitscanState(CharacterCom* owner) : BaseCharacter_BaseState(owner) {}

    void Enter() override;
    void Execute(const float& elapsedTime) override;
    void Exit() override;
    void ImGui() override;
    virtual const char* GetName() const override { return "Hitscan"; }

private:
    float rayLength = 100;
};

class BaseCharacter_CapsuleState : public BaseCharacter_BaseState
{
    //�@���@�J�v�Z��OBJ���N�����邾���̃X�e�[�g�@��
    //  owner��Obj�̎q��"capsuleObj"�Ƃ������O��Obj�����
    //  NodeCollsionCom�ȊO�̌`��Ɣ��肷��

public:
    BaseCharacter_CapsuleState(CharacterCom* owner) : BaseCharacter_BaseState(owner) {}

    void Enter() override;
    void Execute(const float& elapsedTime) override;
    void Exit() override;
    void ImGui() override;
    virtual const char* GetName() const override { return "Capsule"; }

private:
    float capsuleLength = 5;
};

class BaseCharacter_StanBallState : public BaseCharacter_BaseState
{
public:
    BaseCharacter_StanBallState(CharacterCom* owner) : BaseCharacter_BaseState(owner) {}

    void Enter() override;
    void Execute(const float& elapsedTime) override;
    void Exit() override;
    void ImGui() override;
    virtual const char* GetName() const override { return "StanBall"; }

private:
    float speed = 40;
    float power = 1;
};

class BaseCharacter_KnockbackBallState : public BaseCharacter_BaseState
{
public:
    BaseCharacter_KnockbackBallState(CharacterCom* owner) : BaseCharacter_BaseState(owner) {}

    void Enter() override;
    void Execute(const float& elapsedTime) override;
    void Exit() override;
    void ImGui() override;
    virtual const char* GetName() const override { return "KnockbackBall"; }

private:
    float speed = 40;
    float power = 1;
};

class Ult_Attack_State : public BaseCharacter_BaseState
{
public:
    Ult_Attack_State(CharacterCom* owner) : BaseCharacter_BaseState(owner) {}

    void Enter() override;
    void Execute(const float& elapsedTime) override;
    void Exit() override;
    void ImGui() override;
    virtual const char* GetName() const override { return "Ult_Attack"; }

private:
    std::shared_ptr<GameObject> obj;    //�E���g�p�I�u�W�F�N�g��ۑ�
};

class BaseCharacter_NoneAttack : public BaseCharacter_BaseState
{
public:
    BaseCharacter_NoneAttack(CharacterCom* owner) : BaseCharacter_BaseState(owner) {}

    void Enter() override;
    virtual const char* GetName() const override { return "NoneAttack"; }
};
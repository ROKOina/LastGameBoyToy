#pragma once
#include "../../System/State.h"
#include "../../MovementCom.h"
#include "../../TransformCom.h"
#include "../../AnimationCom.h"

class BossCom;

//��b
class Boss_BaseState : public State<BossCom>
{
public:
    Boss_BaseState(BossCom* owner);

protected:
    std::weak_ptr<BossCom> bossCom;
    std::weak_ptr<MovementCom> moveCom;
    std::weak_ptr<TransformCom> transCom;
    std::weak_ptr<AnimationCom> animationCom;
};

//�ҋ@
class Boss_IdleState : public Boss_BaseState
{
public:
    Boss_IdleState(BossCom* owner) :Boss_BaseState(owner) {}

    void Enter() override;
    void Execute(const float& elapsedTime) override;
    void ImGui() override {};
    virtual const char* GetName() const override { return "Idle"; }
};

//�ړ�
class Boss_MoveState : public Boss_BaseState
{
public:
    Boss_MoveState(BossCom* owner) :Boss_BaseState(owner) {}

    void Enter() override;
    void Execute(const float& elapsedTime) override;
    void ImGui() override {};
    virtual const char* GetName() const override { return "Move"; }
};

//�W�����v
class Boss_JumpState : public Boss_BaseState
{
public:
    Boss_JumpState(BossCom* owner) :Boss_BaseState(owner) {}

    void Enter() override;
    void Execute(const float& elapsedTime) override;
    void ImGui() override {};
    virtual const char* GetName() const override { return "Jump"; }
};

//�W�����v���[�v
class Boss_JumpLoopState : public Boss_BaseState
{
public:
    Boss_JumpLoopState(BossCom* owner) :Boss_BaseState(owner) {}

    void Enter() override;
    void Execute(const float& elapsedTime) override;
    void ImGui() override {};
    virtual const char* GetName() const override { return "JumpLoop"; }
};

//���n
class Boss_LandingState : public Boss_BaseState
{
public:
    Boss_LandingState(BossCom* owner) :Boss_BaseState(owner) {}

    void Enter() override;
    void Execute(const float& elapsedTime) override;
    void ImGui() override {};
    virtual const char* GetName() const override { return "Landing"; }
};

//�U��
class Boss_AttackState : public Boss_BaseState
{
public:
    Boss_AttackState(BossCom* owner) :Boss_BaseState(owner) {}

    void Enter() override;
    void Execute(const float& elapsedTime) override;
    void ImGui() override {};
    virtual const char* GetName() const override { return "Attack"; }
};

//�͈͍U��
class Boss_RangeAttackState : public Boss_BaseState
{
public:
    Boss_RangeAttackState(BossCom* owner) :Boss_BaseState(owner) {}

    void Enter() override;
    void Execute(const float& elapsedTime) override;
    void ImGui() override {};
    virtual const char* GetName() const override { return "RangeAttack"; }
};

//�{���v�U��
class Boss_BompAttackState : public Boss_BaseState
{
public:
    Boss_BompAttackState(BossCom* owner) :Boss_BaseState(owner) {}

    void Enter() override;
    void Execute(const float& elapsedTime) override;
    void ImGui() override {};
    virtual const char* GetName() const override { return "BompAttack"; }
};

//�_���[�W
class Boss_DamageState : public Boss_BaseState
{
public:
    Boss_DamageState(BossCom* owner) :Boss_BaseState(owner) {}

    void Enter() override;
    void Execute(const float& elapsedTime) override;
    void ImGui() override {};
    virtual const char* GetName() const override { return "Damage"; }
};

//���S
class Boss_DeathState : public Boss_BaseState
{
public:
    Boss_DeathState(BossCom* owner) :Boss_BaseState(owner) {}

    void Enter() override;
    void Execute(const float& elapsedTime) override;
    void ImGui() override {};
    virtual const char* GetName() const override { return "Death"; }
};
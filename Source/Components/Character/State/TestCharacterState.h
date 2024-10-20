#pragma once
#include "../../System/State.h"
#include "../TestCharacterCom.h"
#include "../../MovementCom.h"
#include "../../TransformCom.h"
#include "../../AnimationCom.h"
#include "../../RendererCom.h"

class TestCharacter_BaseState : public State<CharacterCom> {
public:
    TestCharacter_BaseState(CharacterCom* owner);

protected:
    std::weak_ptr<TestCharacterCom> charaCom;
    std::weak_ptr<MovementCom> moveCom;
    std::weak_ptr<TransformCom> transCom;
    std::weak_ptr<AnimationCom> animationCom;
    std::weak_ptr<RendererCom> renderCom;
};

class TestCharacter_MoveState : public TestCharacter_BaseState
{
public:
    TestCharacter_MoveState(CharacterCom* owner) :TestCharacter_BaseState(owner) {}

    void Enter() override;
    void Execute(const float& elapsedTime) override;
    virtual const char* GetName() const override { return "Move"; }
};

class TestCharacter_AttackState : public TestCharacter_BaseState
{
public:
    TestCharacter_AttackState(CharacterCom* owner) : TestCharacter_BaseState(owner) {}

    void Enter() override;
    void Execute(const float& elapsedTime) override;
    void Exit() override;
    virtual const char* GetName() const override { return "Attack"; }

    void ImGui() override;

private:
    void Fire();

private:
    float friction1 = 1.0f;
    float friction2 = 1.0f;

    float firePower = 30.0f;
    float fireTimer = 0.0f;
    float fireTime = 0.05f;

    float attackPower = 20.0f;
};

class TestCharacter_DashState : public TestCharacter_BaseState
{
public:
    TestCharacter_DashState(CharacterCom* owner) : TestCharacter_BaseState(owner) {}

    void Enter() override;
    void Execute(const float& elapsedTime) override;
    void Exit() override;
    virtual const char* GetName() const override { return "Dash"; }

private:
    float dashAcceleration = 3.0f;
    float maxDashAcceleration = 20.0f;
};
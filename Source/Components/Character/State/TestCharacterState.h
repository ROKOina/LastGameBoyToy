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

};

class TestCharacter_AttackState : public TestCharacter_BaseState
{
public:
    TestCharacter_AttackState(CharacterCom* owner) : TestCharacter_BaseState(owner) {}

    void Enter() override;
    void Execute(const float& elapsedTime) override;
    void Exit() override;

private:
    void Fire();

    float fireTimer = 0.0f;
    float fireTime = 0.05f;

};

class TestCharacter_DashState : public TestCharacter_BaseState
{
public:
    TestCharacter_DashState(CharacterCom* owner) : TestCharacter_BaseState(owner) {}

    void Enter() override;
    void Execute(const float& elapsedTime) override;
    void Exit() override;

private:
    float dashAcceleration = 3.0f;
    float maxDashAcceleration = 20.0f;
};
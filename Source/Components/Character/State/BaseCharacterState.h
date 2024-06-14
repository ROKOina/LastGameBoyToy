#pragma once
#include "../../System/State.h"
#include "../CharacterCom.h"
#include "../CharacterCom.h"
#include "../../MovementCom.h"
#include "../../TransformCom.h"
#include "../../AnimationCom.h"

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
};
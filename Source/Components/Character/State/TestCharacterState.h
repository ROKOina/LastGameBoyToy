#pragma once
#include "../../System/State.h"
#include "../CharacterCom.h"
#include "../TestCharacterCom.h"
#include "../../MovementCom.h"
#include "../../TransformCom.h"

class TestCharacter_BaseState : public State<CharacterCom> {
public:
    TestCharacter_BaseState(CharacterCom* owner);

protected:
    std::weak_ptr<CharacterCom> testCharaCom;
    std::weak_ptr<MovementCom> moveCom;
    std::weak_ptr<TransformCom> transCom;
};


class TestCharacter_MoveState : public TestCharacter_BaseState
{
public:
    TestCharacter_MoveState(CharacterCom* owner) :TestCharacter_BaseState(owner) {}

    void Enter() override;
    void Execute(const float& elapsedTime) override;

};

class TestCharacter_JumpState : public TestCharacter_BaseState
{
public:
    TestCharacter_JumpState(CharacterCom* owner) :TestCharacter_BaseState(owner) {}

    void Enter() override;
    void Execute(const float& elapsedTime) override;

private:
    DirectX::XMFLOAT3 jumpPower = { 0, 30.0f, 0 };
    bool jumpFlag = false;
};
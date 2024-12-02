#pragma once

#include "StateMachine\State.h"
#include "Component\Character\CharacterCom.h"
#include "Component/MoveSystem/MovementCom.h"
#include "Component/System/TransformCom.h"
#include "Component/Animation/AnimationCom.h"
#include "Component\Character\FarahCom.h"

//ƒtƒ@ƒ‰
class Farah_BaseState : public State<CharacterCom>
{
public:
    Farah_BaseState(CharacterCom* owner);

protected:
    std::weak_ptr<FarahCom> charaCom;
    std::weak_ptr<MovementCom> moveCom;
    std::weak_ptr<TransformCom> transCom;
    std::weak_ptr<AnimationCom> animationCom;
};

//ƒƒCƒ“UŒ‚
class Farah_MainAttackState : public Farah_BaseState
{
public:
    Farah_MainAttackState(CharacterCom* owner) :Farah_BaseState(owner) {}

    void Enter() override;
    void Execute(const float& elapsedTime) override;
    void Exit() override {};
    void ImGui() override {};
    virtual const char* GetName() const override { return "MainAttack"; }
};

//ultUŒ‚
class Farah_UltState : public Farah_BaseState
{
public:
    Farah_UltState(CharacterCom* owner) :Farah_BaseState(owner) {}

    void Enter() override;
    void Execute(const float& elapsedTime) override;
    void Exit() override {};
    void ImGui() override {};
    virtual const char* GetName() const override { return "UltAttack"; }
};
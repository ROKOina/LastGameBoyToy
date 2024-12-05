#pragma once

#include "StateMachine\State.h"
#include "Component\Character\CharacterCom.h"
#include "Component/MoveSystem/MovementCom.h"
#include "Component/System/TransformCom.h"
#include "Component/Animation/AnimationCom.h"
#include "Component\Character\JankratCharacterCom.h"

//�n���]�[

class JankratCharacter_BaseState : public State<CharacterCom>
{
public:
    JankratCharacter_BaseState(CharacterCom* owner);

protected:
    std::weak_ptr<JankratCharacterCom> charaCom;
    std::weak_ptr<MovementCom> moveCom;
    std::weak_ptr<TransformCom> transCom;
    std::weak_ptr<AnimationCom> animationCom;
};

class JankratCharacter_MainAtkState : public JankratCharacter_BaseState
{
public:
    JankratCharacter_MainAtkState(CharacterCom* owner) : JankratCharacter_BaseState(owner) {};

    void Enter() override;
    void Execute(const float& elapsedTime) override;
    void Exit() override {};
    void ImGui() override;
    virtual const char* GetName() const override { return "MainAttack"; }

private:
    float mass = 1.0f;
    float restitution = 0.0f;
    float force = 50.0f;

    float bulletLifeTimer = 5.0f;
    float addGravity = -0.8f;
    float fireVecY = 0.35f;
};

class JankratCharacter_SubAttackState : public JankratCharacter_BaseState
{
public:
    JankratCharacter_SubAttackState(CharacterCom* owner) : JankratCharacter_BaseState(owner) {};

    void Enter() override {};
    void Execute(const float& elapsedTime) override;
    void Exit() override {};
    void ImGui() override {};
    virtual const char* GetName() const override { return "SubAttack"; }
};

class JankratCharacter_MainSkillState : public JankratCharacter_BaseState
{
public:
    JankratCharacter_MainSkillState(CharacterCom* owner) : JankratCharacter_BaseState(owner) {};

    void Enter() override;
    void Execute(const float& elapsedTime) override;
    void Exit() override {};
    void ImGui() override {};
    virtual const char* GetName() const override { return "SubSkill"; }
};
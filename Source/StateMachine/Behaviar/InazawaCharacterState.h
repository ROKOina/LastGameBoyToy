#pragma once

#include "StateMachine\State.h"
#include "Component\Character\CharacterCom.h"
#include "Component/MoveSystem/MovementCom.h"
#include "Component/System/TransformCom.h"
#include "Component/Animation/AnimationCom.h"
#include "Component\Character\InazawaCharacterCom.h"

//ÉnÉìÉ]Å[

class InazawaCharacter_BaseState : public State<CharacterCom>
{
public:
    InazawaCharacter_BaseState(CharacterCom* owner);

protected:
    std::weak_ptr<InazawaCharacterCom> charaCom;
    std::weak_ptr<MovementCom> moveCom;
    std::weak_ptr<TransformCom> transCom;
    std::weak_ptr<AnimationCom> animationCom;
};

class InazawaCharacter_AttackState : public InazawaCharacter_BaseState
{
public:
    InazawaCharacter_AttackState(CharacterCom* owner) :InazawaCharacter_BaseState(owner)
    {
        auto& chara = GetComp(CharacterCom);
    }

    void Enter() override;
    void Execute(const float& elapsedTime) override;
    void ImGui() override;
    virtual const char* GetName() const override { return "MainAttack"; }

    float attackMaxMoveSpeed = 3.0f;
    float attackPower = 0;
    float maxAttackPower = 1;
    float arrowSpeed = 40;
};

class InazawaCharacter_ESkillState : public InazawaCharacter_BaseState
{
public:
    InazawaCharacter_ESkillState(CharacterCom* owner) :InazawaCharacter_BaseState(owner) {}

    void Enter() override;
    void Execute(const float& elapsedTime) override;
    void ImGui() override;
    virtual const char* GetName() const override { return "ESkill"; }

    float arrowSpeed = 25.0f;
    int arrowCount;
    float intervalTimer;
    float interval = 0.25f;
    float skillTimer = 3.0f;
};

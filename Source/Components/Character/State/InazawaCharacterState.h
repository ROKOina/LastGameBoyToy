#pragma once
#include "../../System/State.h"
#include "../CharacterCom.h"
#include "../InazawaCharacterCom.h"
#include "../../MovementCom.h"
#include "../../TransformCom.h"
#include "../../AnimationCom.h"

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
        name = "MainAttack";
        auto& chara = GetComp(CharacterCom);
        saveMaxSpeed = chara->GetMoveMaxSpeed();
    }

    void Enter() override;
    void Execute(const float& elapsedTime) override;
    void ImGui() override;

    float saveMaxSpeed;
    float attackMaxMoveSpeed = 5.0f;
    float attackPower = 0;
    float maxAttackPower = 1;
    float arrowSpeed = 40;
};

class InazawaCharacter_ESkillState : public InazawaCharacter_BaseState
{
public:
    InazawaCharacter_ESkillState(CharacterCom* owner) :InazawaCharacter_BaseState(owner)
    {
        name = "ESkill";
    }

    void Enter() override;
    void Execute(const float& elapsedTime) override;
    void ImGui() override;

    float arrowSpeed = 25.0f;
    int arrowCount;
    float intervalTimer;
    float interval = 0.25f;
    float skillTimer = 5.0f;
};


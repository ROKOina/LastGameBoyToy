#pragma once

#include "StateMachine\State.h"
#include "Component\Character\CharacterCom.h"
#include "Component/MoveSystem/MovementCom.h"
#include "Component/System/TransformCom.h"
#include "Component/Animation/AnimationCom.h"
#include "Component\Character\InazawaCharacterCom.h"

//ハンゾー

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
    void Exit() override;
    void ImGui() override;
    virtual const char* GetName() const override { return "MainAttack"; }

    float attackMaxMoveSpeed = 3.0f;
    float attackPower = 0;
    float maxAttackPower = 0.7f;    //ため秒
    float arrowSpeed = 120;

    int maxDamage = 50;
};

class InazawaCharacter_ESkillState : public InazawaCharacter_BaseState
{
public:
    InazawaCharacter_ESkillState(CharacterCom* owner) :InazawaCharacter_BaseState(owner) {}

    void Enter() override;
    void Execute(const float& elapsedTime) override;
    void Exit() override;
    void ImGui() override;
    virtual const char* GetName() const override { return "ESkill"; }

    float arrowSpeed = 90;
    int arrowCount;
    const int maxArrowCount = 8;
    float intervalTimer;
    float interval = 0.25f;
    float skillTimer = 3.0f;
    const float skillTime = 3.0f;
    bool isShot = false; //撃ったタイミング 
    int damage = 30;
};

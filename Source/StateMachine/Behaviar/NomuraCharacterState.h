#pragma once
#include "StateMachine\State.h"
#include "Component\Character\CharacterCom.h"
#include "Component/MoveSystem/MovementCom.h"
#include "Component/System/TransformCom.h"
#include "Component/Animation/AnimationCom.h"
#include "Component\Character\NomuraCharacterCom.h"

//Ç´Ç·Ç∑Ç≈Ç°ÇÃÇ¬Ç‡ÇË
class NomuraCharacter_BaseState :public State<CharacterCom>
{
public:
    NomuraCharacter_BaseState(CharacterCom* owner);

protected:
    std::weak_ptr<NomuraCharacterCom> charaCom;
    std::weak_ptr<MovementCom>        moveCom;
    std::weak_ptr<TransformCom>       transCom;
    std::weak_ptr<AnimationCom>       animationCom;
};

//èeî≠éÀ
class NomuraCharacter_AttackState :public NomuraCharacter_BaseState
{
public:
    NomuraCharacter_AttackState(CharacterCom* owner) :NomuraCharacter_BaseState(owner) {}

    void Enter() override;
    void Execute(const float& elapasedTime) override;
    void ImGui()override;
    virtual const char* GetName() const override { return "Attack"; }

    float attackPower = 0;
    float maxAttackPower = 1;
    float bulletSpeed = 50;
};

//èeòAéÀ
class NomuraCharacter_SubAttackState :public NomuraCharacter_BaseState
{
public:
    NomuraCharacter_SubAttackState(CharacterCom* owner) :NomuraCharacter_BaseState(owner) {}

    void Enter() override;
    void Execute(const float& elapasedTime) override;
    void ImGui()override;
    virtual const char* GetName() const override { return "SubAttack"; }

private:

    float attackPower = 0;
    float maxAttackPower = 1;

    float fireTimer = 0.0f;
    float fireTime = 0.1f;
};

//àÍíUÉçÅ[ÉäÉìÉO
class NomuraCharacter_ESkillState :public NomuraCharacter_BaseState
{
public:
    NomuraCharacter_ESkillState(CharacterCom* owner) :NomuraCharacter_BaseState(owner) {}

    void Enter()override;
    void Execute(const float& elapsedTime)override;
    void ImGui()override;
    virtual const char* GetName() const override { return "ESkill"; }
};

class NomuraCharacter_ReloadState : public NomuraCharacter_BaseState
{
public:
    NomuraCharacter_ReloadState(CharacterCom* owner) :NomuraCharacter_BaseState(owner) {}

    void Enter()override;
    void Execute(const float& elapsedTime)override;
    void ImGui()override;
    virtual const char* GetName() const override { return "Reload"; }
};
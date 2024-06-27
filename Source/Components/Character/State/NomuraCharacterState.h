#pragma once
#include "../../System/State.h"
#include "../CharacterCom.h"
#include "../NomuraCharacterCom.h"
#include "../../MovementCom.h"
#include "../../TransformCom.h"
#include "../../AnimationCom.h"

//‚«‚á‚·‚Å‚¡‚Ì‚Â‚à‚è

class NomuraCharacter_BaseState :public State<CharacterCom>
{
public:
    NomuraCharacter_BaseState(CharacterCom* owner);

protected:
    std::weak_ptr<NomuraCharacterCom> testCharaCom;
    std::weak_ptr<MovementCom>        moveCom;
    std::weak_ptr<TransformCom>       transCom;
    std::weak_ptr<AnimationCom>       animationCom;

};

class NomuraCharacter_AttackState :public NomuraCharacter_BaseState
{
public:
    NomuraCharacter_AttackState(CharacterCom*owner):NomuraCharacter_BaseState(owner){}

    void Enter() override;
    void Execute(const float& elapasedTime) override;
    void ImGui()override;


    int   nowMagazine = 0;
    int   maxBullets = 6;
    float attackPower = 0;
    float maxAttackPower = 1;
    float bulletSpeed = 50;
    
};

class NomuraCharacter_ESkillState :public NomuraCharacter_BaseState
{
public:
    NomuraCharacter_ESkillState(CharacterCom*owner):NomuraCharacter_BaseState(owner){}

    void Enter()override;
    void Execute(const float& elapsedTime)override;
    void ImGui()override;

    float moveDebuff = 0.12f;
    float explosionDamage = 45.0f;
    float skillTimer = 12.0f;
    bool  obstructionSkill = false;
    
};
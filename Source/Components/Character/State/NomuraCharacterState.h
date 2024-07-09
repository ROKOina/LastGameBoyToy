#pragma once
#include "../../System/State.h"
#include "../CharacterCom.h"
#include "../NomuraCharacterCom.h"
#include "../../MovementCom.h"
#include "../../TransformCom.h"
#include "../../AnimationCom.h"

//きゃすでぃのつもり

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


//銃発射
class NomuraCharacter_AttackState :public NomuraCharacter_BaseState
{
public:
    NomuraCharacter_AttackState(CharacterCom*owner):NomuraCharacter_BaseState(owner){}

    void Enter() override;
    void Execute(const float& elapasedTime) override;
    void ImGui()override;


 
    float attackPower = 0;
    float maxAttackPower = 1;
    float bulletSpeed = 50;
  
    
};

//一旦ローリング
class NomuraCharacter_ESkillState :public NomuraCharacter_BaseState
{
public:
    NomuraCharacter_ESkillState(CharacterCom*owner):NomuraCharacter_BaseState(owner){}

    void Enter()override;
    void Execute(const float& elapsedTime)override;
    void ImGui()override;
    
};

class NomuraCharacter_ReloadState : public NomuraCharacter_BaseState
{
public:
    NomuraCharacter_ReloadState(CharacterCom*owner):NomuraCharacter_BaseState(owner){}

    void Enter()override;
    void Execute(const float& elapsedTime)override;
    void ImGui()override;
};
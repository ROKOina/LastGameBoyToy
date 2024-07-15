#pragma once
#include "../../System/State.h"
#include "../CharacterCom.h"
#include "../NomuraCharacterCom.h"
#include "../../MovementCom.h"
#include "../../TransformCom.h"
#include "../../AnimationCom.h"

//���Ⴗ�ł��̂���

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


//�e����
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

//�e�A��
class NomuraCharacter_SubAttackState :public NomuraCharacter_BaseState
{
public:
    NomuraCharacter_SubAttackState(CharacterCom* owner) :NomuraCharacter_BaseState(owner) {}

    void Enter() override;
    void Execute(const float& elapasedTime) override;
    void ImGui()override;


private:

    float attackPower = 0;
    float maxAttackPower = 1;

    float fireTimer = 0.0f;
    float fireTime = 0.1f;
   
};

//��U���[�����O
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
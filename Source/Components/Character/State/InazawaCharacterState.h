#pragma once
#include "../../System/State.h"
#include "../CharacterCom.h"
#include "../TestCharacterCom.h"
#include "../../MovementCom.h"
#include "../../TransformCom.h"
#include "../../AnimationCom.h"

class InazawaCharacter_BaseState : public State<CharacterCom> 
{
public:
    InazawaCharacter_BaseState(CharacterCom* owner);

protected:
    std::weak_ptr<CharacterCom> testCharaCom;
    std::weak_ptr<MovementCom> moveCom;
    std::weak_ptr<TransformCom> transCom;
    std::weak_ptr<AnimationCom> animationCom;
};

class InazawaCharacter_AttackState : public InazawaCharacter_BaseState
{
public:
    InazawaCharacter_AttackState(CharacterCom* owner) :InazawaCharacter_BaseState(owner) {}

    void Enter() override;
    void Execute(const float& elapsedTime) override;

    void Fire();
};

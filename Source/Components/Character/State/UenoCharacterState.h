#pragma once
#include "../../System/State.h"
#include "../CharacterCom.h"
#include "../UenoCharacterCom.h"
#include "../../MovementCom.h"
#include "../../TransformCom.h"
#include "../../AnimationCom.h"
#include "Components/CPUParticle.h"

//èóÇÃÉ^ÉìÉN
class UenoCharacterState_BaseState : public State<CharacterCom>
{
public:
    UenoCharacterState_BaseState(CharacterCom* owner);

protected:
    std::weak_ptr<UenoCharacterCom> charaCom;
    std::weak_ptr<MovementCom> moveCom;
    std::weak_ptr<TransformCom> transCom;
    std::weak_ptr<AnimationCom> animationCom;
};

class UenoCharacterState_AttackState : public UenoCharacterState_BaseState
{
public:
    UenoCharacterState_AttackState(CharacterCom* owner) :UenoCharacterState_BaseState(owner) {}

    void Enter() override;
    void Execute(const float& elapsedTime) override;
    void ImGui() override;

private:

    bool t = false;
};
#pragma once

#include "StateMachine\State.h"
#include "Component\Character\CharacterCom.h"
#include "Component/MoveSystem/MovementCom.h"
#include "Component/System/TransformCom.h"
#include "Component/Animation/AnimationCom.h"
#include "Component\Character\FarahCom.h"

//ƒtƒ@ƒ‰
class Farah_BaseState : public State<CharacterCom>
{
public:
    Farah_BaseState(CharacterCom* owner);

protected:
    std::weak_ptr<FarahCom> charaCom;
    std::weak_ptr<MovementCom> moveCom;
    std::weak_ptr<TransformCom> transCom;
    std::weak_ptr<AnimationCom> animationCom;
};
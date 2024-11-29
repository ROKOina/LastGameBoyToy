#include "FarahState.h"

//Šî’êƒNƒ‰ƒX‚Å‚·
Farah_BaseState::Farah_BaseState(CharacterCom* owner) : State(owner)
{
    //‰Šúİ’è
    charaCom = GetComp(FarahCom);
    moveCom = GetComp(MovementCom);
    transCom = GetComp(TransformCom);
    animationCom = GetComp(AnimationCom);
}
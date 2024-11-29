#include "FarahState.h"

//基底クラスです
Farah_BaseState::Farah_BaseState(CharacterCom* owner) : State(owner)
{
    //初期設定
    charaCom = GetComp(FarahCom);
    moveCom = GetComp(MovementCom);
    transCom = GetComp(TransformCom);
    animationCom = GetComp(AnimationCom);
}
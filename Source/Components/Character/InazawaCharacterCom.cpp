#include "InazawaCharacterCom.h"
#include "State\TestCharacterState.h"
#include "State\BaseCharacterState.h"

void InazawaCharacterCom::Start()
{
    //ÉXÉeÅ[Égìoò^
    stateMachine.AddState(CHARACTER_ACTIONS::MOVE, std::make_shared<TestCharacter_MoveState>(this));
    stateMachine.AddState(CHARACTER_ACTIONS::JUMP, std::make_shared<BaseCharacter_JumpState>(this));

    stateMachine.ChangeState(CHARACTER_ACTIONS::MOVE);
}

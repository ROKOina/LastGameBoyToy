#include "TestCharacterCom.h"
#include "../MovementCom.h"
#include "../TransformCom.h"
#include "Input\Input.h"
#include "State\TestCharacterState.h"

void TestCharacterCom::Start()
{
    //ÉXÉeÅ[Égìoò^
    stateMachine.AddState(CHARACTER_ACTIONS::MOVE, std::make_shared<TestCharacter_MoveState>(this));
    stateMachine.AddState(CHARACTER_ACTIONS::JUMP, std::make_shared<TestCharacter_JumpState>(this));

    stateMachine.ChangeState(CHARACTER_ACTIONS::MOVE);
}



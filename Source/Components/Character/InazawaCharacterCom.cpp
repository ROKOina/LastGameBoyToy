#include "InazawaCharacterCom.h"
#include "State\InazawaCharacterState.h"
#include "State\BaseCharacterState.h"

void InazawaCharacterCom::Start()
{
    //ÉXÉeÅ[Égìoò^
    stateMachine.AddState(CHARACTER_ACTIONS::IDLE, std::make_shared<BaseCharacter_IdleState>(this));
    stateMachine.AddState(CHARACTER_ACTIONS::MOVE, std::make_shared<BaseCharacter_MoveState>(this));
    stateMachine.AddState(CHARACTER_ACTIONS::JUMP, std::make_shared<BaseCharacter_JumpState>(this));

    stateMachine.ChangeState(CHARACTER_ACTIONS::MOVE);
}

void InazawaCharacterCom::MainAttack()
{
    int i = 0;
}

void InazawaCharacterCom::MainSkill()
{
    int i = 0;
}

void InazawaCharacterCom::SubSkill()
{
    int i = 0;
}

#include "JankratCharacterCom.h"
#include "StateMachine\Behaviar\BaseCharacterState.h"
#include "StateMachine\Behaviar\JankratCharacterState.h"

void JankratCharacterCom::Start()
{
    //ÉXÉeÅ[Égìoò^
    moveStateMachine.AddState(CHARACTER_MOVE_ACTIONS::IDLE, std::make_shared<BaseCharacter_IdleState>(this));
    moveStateMachine.AddState(CHARACTER_MOVE_ACTIONS::MOVE, std::make_shared<BaseCharacter_MoveState>(this));
    moveStateMachine.AddState(CHARACTER_MOVE_ACTIONS::JUMP, std::make_shared<BaseCharacter_JumpState>(this));
    moveStateMachine.AddState(CHARACTER_MOVE_ACTIONS::DEATH, std::make_shared<BaseCharacter_DeathState>(this));

    attackStateMachine.AddState(CHARACTER_ATTACK_ACTIONS::MAIN_ATTACK, std::make_shared<JankratCharacter_MainAtkState>(this));
    attackStateMachine.AddState(CHARACTER_ATTACK_ACTIONS::NONE, std::make_shared<BaseCharacter_NoneAttack>(this));

    //SEìoò^

    moveStateMachine.ChangeState(CHARACTER_MOVE_ACTIONS::IDLE);
    attackStateMachine.ChangeState(CHARACTER_ATTACK_ACTIONS::NONE);
}

void JankratCharacterCom::Update(float elapsedTime)
{
    CharacterCom::Update(elapsedTime);
}

void JankratCharacterCom::MainAttackDown()
{
    //ÉAÉ^ÉbÉN
    attackStateMachine.ChangeState(CHARACTER_ATTACK_ACTIONS::MAIN_ATTACK);
}
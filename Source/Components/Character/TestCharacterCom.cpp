#include "TestCharacterCom.h"
#include "../MovementCom.h"
#include "../TransformCom.h"
#include "../RendererCom.h"
#include "Input\Input.h"
#include "State\TestCharacterState.h"
#include "State\BaseCharacterState.h"

void TestCharacterCom::Start()
{
    //ÉXÉeÅ[Égìoò^
    stateMachine.AddState(CHARACTER_ACTIONS::MOVE, std::make_shared<TestCharacter_MoveState>(this));
    stateMachine.AddState(CHARACTER_ACTIONS::JUMP, std::make_shared<BaseCharacter_JumpState>(this));
    stateMachine.AddState(CHARACTER_ACTIONS::IDLE, std::make_shared<BaseCharacter_IdleState>(this));
    stateMachine.AddState(CHARACTER_ACTIONS::ATTACK, std::make_shared<TestCharacter_AttackState>(this));
    stateMachine.AddState(CHARACTER_ACTIONS::DASH, std::make_shared<TestCharacter_DashState>(this));

    stateMachine.ChangeState(CHARACTER_ACTIONS::IDLE);
}

void TestCharacterCom::MainAttack()
{
    stateMachine.ChangeState(CHARACTER_ACTIONS::ATTACK);
}

void TestCharacterCom::SubSkill()
{
    if (Input::Instance().GetGamePad().GetAxisLY() > 0.0f)
    {
        dashFlag = !dashFlag;
        dashFlag ? stateMachine.ChangeState(CHARACTER_ACTIONS::DASH) : stateMachine.ChangeState(CHARACTER_ACTIONS::IDLE);
    }
}



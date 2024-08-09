#include "TestCharacterCom.h"
#include "../MovementCom.h"
#include "../TransformCom.h"
#include "../RendererCom.h"
#include "Input\Input.h"
#include "State\TestCharacterState.h"
#include "State\BaseCharacterState.h"

void TestCharacterCom::Start()
{
    //ステート登録
    moveStateMachine.AddState(CHARACTER_MOVE_ACTIONS::MOVE, std::make_shared<TestCharacter_MoveState>(this));
    moveStateMachine.AddState(CHARACTER_MOVE_ACTIONS::JUMP, std::make_shared<BaseCharacter_JumpState>(this));
    moveStateMachine.AddState(CHARACTER_MOVE_ACTIONS::IDLE, std::make_shared<BaseCharacter_IdleState>(this));

    attackStateMachine.AddState(CHARACTER_ATTACK_ACTIONS::MAIN_ATTACK, std::make_shared<TestCharacter_AttackState>(this));
    attackStateMachine.AddState(CHARACTER_ATTACK_ACTIONS::SUB_SKILL, std::make_shared<TestCharacter_DashState>(this));
    attackStateMachine.AddState(CHARACTER_ATTACK_ACTIONS::NONE, std::make_shared<BaseCharacter_NoneAttack>(this));

    moveStateMachine.ChangeState(CHARACTER_MOVE_ACTIONS::IDLE);
    attackStateMachine.ChangeState(CHARACTER_ATTACK_ACTIONS::NONE);
}

void TestCharacterCom::MainAttackDown()
{
    attackStateMachine.ChangeState(CHARACTER_ATTACK_ACTIONS::MAIN_ATTACK); 
}

void TestCharacterCom::SubSkill()
{
    //ダッシュ
    dashFlag = !dashFlag;
    dashFlag ? attackStateMachine.ChangeState(CHARACTER_ATTACK_ACTIONS::SUB_SKILL)
             : attackStateMachine.ChangeState(CHARACTER_ATTACK_ACTIONS::NONE);
}



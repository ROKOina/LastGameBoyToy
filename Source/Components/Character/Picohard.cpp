#include "Picohard.h"
#include "State\BaseCharacterState.h"
#include "State\PicohardState.h"

void PicohardCharaCom::Start()
{
    //ÉXÉeÅ[Égìoò^
    moveStateMachine.AddState(CHARACTER_MOVE_ACTIONS::IDLE, std::make_shared<BaseCharacter_IdleState>(this));
    moveStateMachine.AddState(CHARACTER_MOVE_ACTIONS::MOVE, std::make_shared<BaseCharacter_MoveState>(this));
    moveStateMachine.AddState(CHARACTER_MOVE_ACTIONS::JUMP, std::make_shared<BaseCharacter_JumpState>(this));

    //auto& a = moveStateMachine.GetState(CHARACTER_MOVE_ACTIONS::JUMP);
    //auto& ba = std::dynamic_pointer_cast<BaseCharacter_JumpState>(a);

    attackStateMachine.AddState(CHARACTER_ATTACK_ACTIONS::MAIN_ATTACK, std::make_shared<Picohard_LeftClick>(this), AttackType::MELEE);
    attackStateMachine.AddState(CHARACTER_ATTACK_ACTIONS::SUB_ATTACK, std::make_shared<Picohard_RightClick>(this), AttackType::NON);
    //attackStateMachine.AddState(CHARACTER_ATTACK_ACTIONS::SUB_SKILL, std::make_shared<BaseCharacter_StanBallState>(this), AttackType::HEAL);
    attackStateMachine.AddState(CHARACTER_ATTACK_ACTIONS::SUB_SKILL, std::make_shared<Picohard_LeftShift>(this), AttackType::NON);
    attackStateMachine.AddState(CHARACTER_ATTACK_ACTIONS::NONE, std::make_shared<BaseCharacter_NoneAttack>(this), AttackType::NON);


    moveStateMachine.ChangeState(CHARACTER_MOVE_ACTIONS::IDLE);
    attackStateMachine.ChangeState(CHARACTER_ATTACK_ACTIONS::NONE);
}

void PicohardCharaCom::Update(float elapsedTime)
{
    CharacterCom::Update(elapsedTime);
}

void PicohardCharaCom::OnGUI()
{
    CharacterCom::OnGUI();
}

void PicohardCharaCom::MainAttackPushing()
{
    if (attackStateMachine.GetCurrentState() != CHARACTER_ATTACK_ACTIONS::MAIN_ATTACK
        && attackStateMachine.GetNextState() != CHARACTER_ATTACK_ACTIONS::MAIN_ATTACK
        && attackStateMachine.GetOldState() != CHARACTER_ATTACK_ACTIONS::MAIN_ATTACK
        && attackStateMachine.GetCurrentState() != CHARACTER_ATTACK_ACTIONS::SUB_ATTACK)
        attackStateMachine.ChangeState(CHARACTER_ATTACK_ACTIONS::MAIN_ATTACK);
}

void PicohardCharaCom::SubAttackPushing()
{
    if (attackStateMachine.GetCurrentState() != CHARACTER_ATTACK_ACTIONS::SUB_ATTACK
        && attackStateMachine.GetNextState() != CHARACTER_ATTACK_ACTIONS::SUB_ATTACK
        && attackStateMachine.GetOldState() != CHARACTER_ATTACK_ACTIONS::SUB_ATTACK)
        attackStateMachine.ChangeState(CHARACTER_ATTACK_ACTIONS::SUB_ATTACK);
}

void PicohardCharaCom::SubSkill()
{
}


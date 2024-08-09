#include "HaveAllAttackCharacter.h"
#include "State\BaseCharacterState.h"
#include "GameSource/Scene/SceneManager.h"
#include "../CameraCom.h"

void HaveAllAttackCharaCom::Start()
{
    //ÉXÉeÅ[Égìoò^
    moveStateMachine.AddState(CHARACTER_MOVE_ACTIONS::IDLE, std::make_shared<BaseCharacter_IdleState>(this));
    moveStateMachine.AddState(CHARACTER_MOVE_ACTIONS::MOVE, std::make_shared<BaseCharacter_MoveState>(this));
    moveStateMachine.AddState(CHARACTER_MOVE_ACTIONS::JUMP, std::make_shared<BaseCharacter_JumpState>(this));

    //auto& a = moveStateMachine.GetState(CHARACTER_MOVE_ACTIONS::JUMP);
    //auto& ba = std::dynamic_pointer_cast<BaseCharacter_JumpState>(a);

    attackStateMachine.AddState(CHARACTER_ATTACK_ACTIONS::MAIN_ATTACK, std::make_shared<BaseCharacter_HitscanState>(this));
    attackStateMachine.AddState(CHARACTER_ATTACK_ACTIONS::SUB_ATTACK, std::make_shared<BaseCharacter_CapsuleState>(this));
    attackStateMachine.AddState(CHARACTER_ATTACK_ACTIONS::SUB_SKILL, std::make_shared<BaseCharacter_StanBallState>(this));
    attackStateMachine.AddState(CHARACTER_ATTACK_ACTIONS::NONE, std::make_shared<BaseCharacter_NoneAttack>(this));


    moveStateMachine.ChangeState(CHARACTER_MOVE_ACTIONS::IDLE);
    attackStateMachine.ChangeState(CHARACTER_ATTACK_ACTIONS::NONE);
}

void HaveAllAttackCharaCom::Update(float elapsedTime)
{
    CharacterCom::Update(elapsedTime);
}

void HaveAllAttackCharaCom::OnGUI()
{
    CharacterCom::OnGUI();
}

void HaveAllAttackCharaCom::MainAttackPushing()
{
    if (attackStateMachine.GetCurrentState() != CHARACTER_ATTACK_ACTIONS::MAIN_ATTACK
        && attackStateMachine.GetNextState() != CHARACTER_ATTACK_ACTIONS::MAIN_ATTACK
        && attackStateMachine.GetOldState() != CHARACTER_ATTACK_ACTIONS::MAIN_ATTACK)
        attackStateMachine.ChangeState(CHARACTER_ATTACK_ACTIONS::MAIN_ATTACK);
}

void HaveAllAttackCharaCom::SubAttackPushing()
{
    if (attackStateMachine.GetCurrentState() != CHARACTER_ATTACK_ACTIONS::SUB_ATTACK
        && attackStateMachine.GetNextState() != CHARACTER_ATTACK_ACTIONS::SUB_ATTACK
        && attackStateMachine.GetOldState() != CHARACTER_ATTACK_ACTIONS::SUB_ATTACK
        && attackStateMachine.GetCurrentState() != CHARACTER_ATTACK_ACTIONS::MAIN_ATTACK)
        attackStateMachine.ChangeState(CHARACTER_ATTACK_ACTIONS::SUB_ATTACK);
}

void HaveAllAttackCharaCom::SubSkill()
{
    attackStateMachine.ChangeState(CHARACTER_ATTACK_ACTIONS::SUB_SKILL);
}

void HaveAllAttackCharaCom::SpaceSkill()
{
}

void HaveAllAttackCharaCom::LeftShiftSkill()
{
}

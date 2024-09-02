#include "UenoCharacterCom.h"
#include "State\BaseCharacterState.h"
#include "GameSource/Scene/SceneManager.h"
#include "Components/Character/State/UenoCharacterState.h"
#include "../CameraCom.h"
#include "Components/CPUParticle.h"
#include "Components\RendererCom.h"

//初期化
void UenoCharacterCom::Start()
{
    //ステート登録
    moveStateMachine.AddState(CHARACTER_MOVE_ACTIONS::IDLE, std::make_shared<UenoCharacterState_IdleState>(this));
    moveStateMachine.AddState(CHARACTER_MOVE_ACTIONS::MOVE, std::make_shared<UenoCharacterState_MoveState>(this));
    moveStateMachine.AddState(CHARACTER_MOVE_ACTIONS::JUMP, std::make_shared<UenoCharacterState_JumpState>(this));
    //moveStateMachine.AddState(CHARACTER_MOVE_ACTIONS::JUMPLOOP, std::make_shared<UenoCharacterState_JumpLoopState>(this));

    //攻撃ステート
    attackStateMachine.AddState(CHARACTER_ATTACK_ACTIONS::MAIN_ATTACK, std::make_shared<BaseCharacter_HitscanState>(this));
    attackStateMachine.AddState(CHARACTER_ATTACK_ACTIONS::SUB_ATTACK, std::make_shared<BaseCharacter_CapsuleState>(this));
    attackStateMachine.AddState(CHARACTER_ATTACK_ACTIONS::SUB_SKILL, std::make_shared<BaseCharacter_StanBallState>(this));
    attackStateMachine.AddState(CHARACTER_ATTACK_ACTIONS::NONE, std::make_shared<BaseCharacter_NoneAttack>(this));

    //初期登録
    moveStateMachine.ChangeState(CHARACTER_MOVE_ACTIONS::IDLE);
    attackStateMachine.ChangeState(CHARACTER_ATTACK_ACTIONS::NONE);
}

//更新処理
void UenoCharacterCom::Update(float elapsedTime)
{
    CharacterCom::Update(elapsedTime);
}

//imgui
void UenoCharacterCom::OnGUI()
{
    CharacterCom::OnGUI();
}

void UenoCharacterCom::MainAttackPushing()
{
    if (attackStateMachine.GetCurrentState() != CHARACTER_ATTACK_ACTIONS::MAIN_ATTACK
        && attackStateMachine.GetNextState() != CHARACTER_ATTACK_ACTIONS::MAIN_ATTACK
        && attackStateMachine.GetOldState() != CHARACTER_ATTACK_ACTIONS::MAIN_ATTACK)
        attackStateMachine.ChangeState(CHARACTER_ATTACK_ACTIONS::MAIN_ATTACK);
}

void UenoCharacterCom::SubAttackPushing()
{
    if (attackStateMachine.GetCurrentState() != CHARACTER_ATTACK_ACTIONS::SUB_ATTACK
        && attackStateMachine.GetNextState() != CHARACTER_ATTACK_ACTIONS::SUB_ATTACK
        && attackStateMachine.GetOldState() != CHARACTER_ATTACK_ACTIONS::SUB_ATTACK
        && attackStateMachine.GetCurrentState() != CHARACTER_ATTACK_ACTIONS::MAIN_ATTACK)
        attackStateMachine.ChangeState(CHARACTER_ATTACK_ACTIONS::SUB_ATTACK);
}

void UenoCharacterCom::SubSkill()
{
    attackStateMachine.ChangeState(CHARACTER_ATTACK_ACTIONS::SUB_SKILL);
}

void UenoCharacterCom::SpaceSkill()
{
}

void UenoCharacterCom::LeftShiftSkill()
{
}
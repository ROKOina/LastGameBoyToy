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
    moveStateMachine.AddState(CHARACTER_MOVE_ACTIONS::JUMPLOOP, std::make_shared<UenoCharacterState_JumpLoopState>(this));

    //初期登録
    moveStateMachine.ChangeState(CHARACTER_MOVE_ACTIONS::IDLE);
    //attackStateMachine.ChangeState(CHARACTER_ATTACK_ACTIONS::NONE);
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

//メインアタック
void UenoCharacterCom::MainAttackDown()
{
    attackStateMachine.ChangeState(CHARACTER_ATTACK_ACTIONS::MAIN_ATTACK);
}
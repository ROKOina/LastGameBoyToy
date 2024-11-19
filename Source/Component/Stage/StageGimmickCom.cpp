#include "StageGimmickCom.h"
#include "StateMachine\Behaviar\StageGimmickState.h"

//初期設定
void StageGimmick::Start()
{
    //ステート登録
    state.AddState(GimmickState::IDLE, std::make_shared<StageGimmick_IdleState>(this));
    state.AddState(GimmickState::ENEMYSPAWN, std::make_shared<StageGimmick_EnemySpawnState>(this));
    state.AddState(GimmickState::BIGATTACK, std::make_shared<StageGimmick_BigAttackState>(this));
    state.AddState(GimmickState::BREAK, std::make_shared<StageGimmick_BreakState>(this));

    //初期ステート登録
    state.ChangeState(GimmickState::IDLE);
}

//更新処理
void StageGimmick::Update(float elapsedTime)
{
    //ステート更新処理
    state.Update(elapsedTime);
}

//imgui
void StageGimmick::OnGUI()
{
    state.ImGui();
}
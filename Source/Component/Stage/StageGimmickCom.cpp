#include "StageGimmickCom.h"
#include "StateMachine\Behaviar\StageGimmickState.h"

//�����ݒ�
void StageGimmick::Start()
{
    //�X�e�[�g�o�^
    state.AddState(GimmickState::IDLE, std::make_shared<StageGimmick_IdleState>(this));
    state.AddState(GimmickState::ENEMYSPAWN, std::make_shared<StageGimmick_EnemySpawnState>(this));
    state.AddState(GimmickState::BIGATTACK, std::make_shared<StageGimmick_BigAttackState>(this));
    state.AddState(GimmickState::BREAK, std::make_shared<StageGimmick_BreakState>(this));

    //�����X�e�[�g�o�^
    state.ChangeState(GimmickState::IDLE);
}

//�X�V����
void StageGimmick::Update(float elapsedTime)
{
    //�X�e�[�g�X�V����
    state.Update(elapsedTime);
}

//imgui
void StageGimmick::OnGUI()
{
    state.ImGui();
}
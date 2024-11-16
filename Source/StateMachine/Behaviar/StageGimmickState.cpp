#include "StageGimmickState.h"
#include "Component\Character\CharaStatusCom.h"

//���R���X�g���N�^
StageGimmick_BaseState::StageGimmick_BaseState(StageGimmick* owner) : State(owner)
{
    spawn = owner->GetGameObject()->GetComponent<SpawnCom>();
    status = owner->GetGameObject()->GetComponent<CharaStatusCom>();
    gpuparticle = owner->GetGameObject()->GetComponent<GPUParticle>();
}

#pragma region �ҋ@
void StageGimmick_IdleState::Enter()
{
}
void StageGimmick_IdleState::Execute(const float& elapsedTime)
{
    const auto& boss = GameObjectManager::Instance().Find("BOSS");

    if (boss)
    {
        //�O��̗̑͂�ێ�����ϐ�
        float damagepreview = boss->GetComponent<CharaStatusCom>()->GetMaxHitpoint();

        //���݂̗̑͂��擾
        float currentHitPoint = *boss->GetComponent<CharaStatusCom>()->GetHitPoint();

        int changeNum = static_cast<int>(damagepreview - currentHitPoint);

        //3���������Ƃ��ɑJ��
        if (static_cast<int>(damagepreview - currentHitPoint) % 11 == 10 && changeNum != spawnChangeNum)
        {
            owner->GetStateMachine().ChangeState(StageGimmick::GimmickState::ENEMYSPAWN);
            spawnChangeNum = changeNum;
            return;
        }
    }

    //��Z�I�Ȃ��
    if (*status.lock()->GetHitPoint() <= 5.0f)
    {
        owner->GetStateMachine().ChangeState(StageGimmick::GimmickState::BIGATTACK);
        return;
    }

    //�M�~�b�N�̑ϋv�l�������Ȃ�Δj��X�e�[�g�ɑJ��
    if (status.lock()->IsDeath())
    {
        owner->GetStateMachine().ChangeState(StageGimmick::GimmickState::BREAK);
        return;
    }
}
#pragma endregion

#pragma region �G�l�~�[����
void StageGimmick_EnemySpawnState::Enter()
{
}
void StageGimmick_EnemySpawnState::Execute(const float& elapsedTime)
{
    //���Ԍo�߂Ő���
    time += elapsedTime;
    if (time > 2.0f)
    {
        spawn.lock()->SetOnTrigger(false);
        owner->GetStateMachine().ChangeState(StageGimmick::GimmickState::IDLE);
        time = 0.0f;
        return;
    }
    else
    {
        spawn.lock()->SetOnTrigger(true);
    }

    //��Z�I�Ȃ��
    if (*status.lock()->GetHitPoint() <= 5.0f)
    {
        owner->GetStateMachine().ChangeState(StageGimmick::GimmickState::BIGATTACK);
        return;
    }

    //�M�~�b�N�̑ϋv�l�������Ȃ�Δj��X�e�[�g�ɑJ��
    if (status.lock()->IsDeath())
    {
        owner->GetStateMachine().ChangeState(StageGimmick::GimmickState::BREAK);
        return;
    }
}
#pragma endregion

#pragma region �����U��
void StageGimmick_BigAttackState::Enter()
{
    gpuparticle.lock()->SetLoop(true);
}
void StageGimmick_BigAttackState::Execute(const float& elapsedTime)
{
    //��Z��łĂΑ����ɔj�󂵂ėǂ�
    {
        //owner->GetStateMachine().ChangeState(StageGimmick::GimmickState::BREAK);
        //return;
    }
}
#pragma endregion

#pragma region �ׂ��N
void StageGimmick_BreakState::Enter()
{
    //�ׂꂽ�牌���o�����o������
}
void StageGimmick_BreakState::Execute(const float& elapsedTime)
{
}
#pragma endregion
#include "StageGimmickState.h"
#include "Component\Character\CharaStatusCom.h"

//���R���X�g���N�^
StageGimmick_BaseState::StageGimmick_BaseState(StageGimmick* owner) : State(owner)
{
    spawn = owner->GetGameObject()->GetComponent<SpawnCom>();
    status = owner->GetGameObject()->GetComponent<CharaStatusCom>();
    gpuparticle = owner->GetGameObject()->GetComponent<GPUParticle>();
    cpuparticle = owner->GetGameObject()->GetComponent<CPUParticle>();
}

#pragma region �ҋ@
void StageGimmick_IdleState::Execute(const float& elapsedTime)
{
    const auto& boss = GameObjectManager::Instance().Find("BOSS");

    if (boss)
    {
        spawntime += elapsedTime;

        //�p�[�e�B�N������
        if (spawntime > 25.0f)
        {
            owner->GetGameObject()->GetChildFind("accumulateparticle")->GetComponent<GPUParticle>()->SetLoop(true);
        }

        //15���������Ƃ��ɑJ��
        if (spawntime > 30.0f)
        {
            owner->GetStateMachine().ChangeState(StageGimmick::GimmickState::ENEMYSPAWN);
            return;
        }

        //��Z�I�Ȃ��
        if (*boss->GetComponent<CharaStatusCom>()->GetHitPoint() <= 100.0f)
        {
            owner->GetStateMachine().ChangeState(StageGimmick::GimmickState::BIGATTACK);
            return;
        }
    }
}
void StageGimmick_IdleState::Exit()
{
    spawntime = 0.0f;
}
#pragma endregion

#pragma region �G�l�~�[����
void StageGimmick_EnemySpawnState::Execute(const float& elapsedTime)
{
    const auto& boss = GameObjectManager::Instance().Find("BOSS");

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
    if (*boss->GetComponent<CharaStatusCom>()->GetHitPoint() <= 100.0f)
    {
        owner->GetStateMachine().ChangeState(StageGimmick::GimmickState::BIGATTACK);
        return;
    }
}
void StageGimmick_EnemySpawnState::Exit()
{
    owner->GetGameObject()->GetChildFind("accumulateparticle")->GetComponent<GPUParticle>()->SetLoop(false);
    spawn.lock()->SetOnTrigger(false);
}
#pragma endregion

#pragma region �����U��
void StageGimmick_BigAttackState::Enter()
{
    const auto& bomber = GameObjectManager::Instance().Find("bomberexplosion");
    gpuparticle.lock()->SetLoop(true);
    bomber->GetComponent<GPUParticle>()->SetLoop(true);
    bomber->GetComponent<SpawnCom>()->SetOnTrigger(true);
}
void StageGimmick_BigAttackState::Execute(const float& elapsedTime)
{
    const auto& boss = GameObjectManager::Instance().Find("BOSS");

    if (*status.lock()->GetHitPoint() <= 0.0f || *boss->GetComponent<CharaStatusCom>()->GetHitPoint() <= 0.0f)
    {
        gpuparticle.lock()->SetLoop(false);
        owner->GetStateMachine().ChangeState(StageGimmick::GimmickState::BREAK);
        return;
    }
}
#pragma endregion

#pragma region �ׂ��N
void StageGimmick_BreakState::Enter()
{
    //�ׂꂽ�牌���o�����o������
    cpuparticle.lock()->SetActive(true);
}
void StageGimmick_BreakState::Execute(const float& elapsedTime)
{
    const auto& zero = GameObjectManager::Instance().Find("Reactar0");
    const auto& one = GameObjectManager::Instance().Find("Reactar1");
    const auto& two = GameObjectManager::Instance().Find("Reactar2");
    const auto& three = GameObjectManager::Instance().Find("Reactar3");
    const auto& bomber = GameObjectManager::Instance().Find("bomberexplosion");
    const auto& boss = GameObjectManager::Instance().Find("BOSS");

    //�j��
    if (*zero->GetComponent<CharaStatusCom>()->GetHitPoint() <= 0.0f &&
        *one->GetComponent<CharaStatusCom>()->GetHitPoint() <= 0.0f &&
        *two->GetComponent<CharaStatusCom>()->GetHitPoint() <= 0.0f &&
        *three->GetComponent<CharaStatusCom>()->GetHitPoint() <= 0.0f ||
        *boss->GetComponent<CharaStatusCom>()->GetHitPoint() <= 0.0f)
    {
        bomber->GetComponent<GPUParticle>()->SetLoop(false);
        bomber->GetComponent<SpawnCom>()->SetOnTrigger(false);
    }
}
#pragma endregion
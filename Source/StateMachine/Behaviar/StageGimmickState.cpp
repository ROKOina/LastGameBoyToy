#include "StageGimmickState.h"
#include "Component\Character\CharaStatusCom.h"

//基底コンストラクタ
StageGimmick_BaseState::StageGimmick_BaseState(StageGimmick* owner) : State(owner)
{
    spawn = owner->GetGameObject()->GetComponent<SpawnCom>();
    status = owner->GetGameObject()->GetComponent<CharaStatusCom>();
    gpuparticle = owner->GetGameObject()->GetComponent<GPUParticle>();
    cpuparticle = owner->GetGameObject()->GetComponent<CPUParticle>();
}

#pragma region 待機
void StageGimmick_IdleState::Execute(const float& elapsedTime)
{
    const auto& boss = GameObjectManager::Instance().Find("BOSS");

    if (boss)
    {
        //前回の体力を保持する変数
        float damagepreview = boss->GetComponent<CharaStatusCom>()->GetMaxHitpoint();

        //現在の体力を取得
        float currentHitPoint = *boss->GetComponent<CharaStatusCom>()->GetHitPoint();

        int changeNum = static_cast<int>(damagepreview - currentHitPoint);

        //3ずつ減ったときに遷移
        if (static_cast<int>(damagepreview - currentHitPoint) % 11 == 10 && changeNum != spawnChangeNum)
        {
            owner->GetStateMachine().ChangeState(StageGimmick::GimmickState::ENEMYSPAWN);
            spawnChangeNum = changeNum;
            return;
        }

        //大技的なやつ
        if (*boss->GetComponent<CharaStatusCom>()->GetHitPoint() <= 20.0f)
        {
            owner->GetStateMachine().ChangeState(StageGimmick::GimmickState::BIGATTACK);
            return;
        }
    }
}
#pragma endregion

#pragma region エネミー生成
void StageGimmick_EnemySpawnState::Execute(const float& elapsedTime)
{
    const auto& boss = GameObjectManager::Instance().Find("BOSS");

    //時間経過で生成
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

    //大技的なやつ
    if (*boss->GetComponent<CharaStatusCom>()->GetHitPoint() <= 20.0f)
    {
        owner->GetStateMachine().ChangeState(StageGimmick::GimmickState::BIGATTACK);
        return;
    }
}
void StageGimmick_EnemySpawnState::Exit()
{
    spawn.lock()->SetOnTrigger(false);
}
#pragma endregion

#pragma region 強い攻撃
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

#pragma region 潰れる君
void StageGimmick_BreakState::Enter()
{
    //潰れたら煙を出す演出をする
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

    //破壊
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
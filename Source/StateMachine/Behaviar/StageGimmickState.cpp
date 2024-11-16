#include "StageGimmickState.h"
#include "Component\Character\CharaStatusCom.h"

//基底コンストラクタ
StageGimmick_BaseState::StageGimmick_BaseState(StageGimmick* owner) : State(owner)
{
    spawn = owner->GetGameObject()->GetComponent<SpawnCom>();
    status = owner->GetGameObject()->GetComponent<CharaStatusCom>();
    gpuparticle = owner->GetGameObject()->GetComponent<GPUParticle>();
}

#pragma region 待機
void StageGimmick_IdleState::Enter()
{
}
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
    }

    //大技的なやつ
    if (*status.lock()->GetHitPoint() <= 5.0f)
    {
        owner->GetStateMachine().ChangeState(StageGimmick::GimmickState::BIGATTACK);
        return;
    }

    //ギミックの耐久値が無くなれば破壊ステートに遷移
    if (status.lock()->IsDeath())
    {
        owner->GetStateMachine().ChangeState(StageGimmick::GimmickState::BREAK);
        return;
    }
}
#pragma endregion

#pragma region エネミー生成
void StageGimmick_EnemySpawnState::Enter()
{
}
void StageGimmick_EnemySpawnState::Execute(const float& elapsedTime)
{
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
    if (*status.lock()->GetHitPoint() <= 5.0f)
    {
        owner->GetStateMachine().ChangeState(StageGimmick::GimmickState::BIGATTACK);
        return;
    }

    //ギミックの耐久値が無くなれば破壊ステートに遷移
    if (status.lock()->IsDeath())
    {
        owner->GetStateMachine().ChangeState(StageGimmick::GimmickState::BREAK);
        return;
    }
}
#pragma endregion

#pragma region 強い攻撃
void StageGimmick_BigAttackState::Enter()
{
    gpuparticle.lock()->SetLoop(true);
}
void StageGimmick_BigAttackState::Execute(const float& elapsedTime)
{
    //大技を打てば即座に破壊して良い
    {
        //owner->GetStateMachine().ChangeState(StageGimmick::GimmickState::BREAK);
        //return;
    }
}
#pragma endregion

#pragma region 潰れる君
void StageGimmick_BreakState::Enter()
{
    //潰れたら煙を出す演出をする
}
void StageGimmick_BreakState::Execute(const float& elapsedTime)
{
}
#pragma endregion
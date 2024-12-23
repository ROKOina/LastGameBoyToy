#include "BossState.h"
#include "Component/Enemy/BossCom.h"
#include "Component\Collsion\ColliderCom.h"
#include "Component\Character\CharaStatusCom.h"
#include "Component\Particle\GPUParticle.h"
#include "Component\System\SpawnCom.h"
#include "Component\PostEffect\PostEffect.h"

Boss_BaseState::Boss_BaseState(BossCom* owner) : State(owner)
{
    //初期設定
    bossCom = owner->GetGameObject()->GetComponent<BossCom>();
    moveCom = owner->GetGameObject()->GetComponent<MovementCom>();
    transCom = owner->GetGameObject()->GetComponent<TransformCom>();
    animationCom = owner->GetGameObject()->GetComponent<AnimationCom>();
    characterstatas = owner->GetGameObject()->GetComponent<CharaStatusCom>();

    // 乱数エンジンのシードを設定
    std::random_device rd;
    gen = std::mt19937(rd());
}

//乱数計算
int Boss_BaseState::ComputeRandom()
{
    //ランダムしたい数を増やす程下記の値が増えていく
    if (availableNumbers.empty())
    {
        availableNumbers = { 1,2,3,4,5,6,7,8 };
    }

    // 乱数生成エンジンを使ってランダムにインデックスを生成
    std::uniform_int_distribution<int> dis(0, availableNumbers.size() - 1);
    int index = dis(gen);
    int randomValue = availableNumbers[index];

    availableNumbers.erase(availableNumbers.begin() + index);

    return randomValue;
}

//アニメーションイベント制御
void Boss_BaseState::AnimtionEventControl(const std::string& eventname, const std::string& nodename, const char* objectname, int eventFlags)
{
    // アニメーションコンポーネントのロックを一度だけ行う
    const auto& animationComPtr = animationCom.lock();
    if (!animationComPtr) return;

    // オブジェクトの取得とnullチェック
    auto object = GameObjectManager::Instance().Find(objectname);
    if (!object) return;

    // 各コンポーネントを取得しておく
    const auto& cpuparticle = object->GetComponent<CPUParticle>();
    const auto& collision = object->GetComponent<SphereColliderCom>();
    const auto& gpuparticle = object->GetComponent<GPUParticle>();
    const auto& spawn = object->GetComponent<SpawnCom>();

    // イベント発火ノードのポジション取得と処理
    DirectX::XMFLOAT3 pos = {};
    if (animationComPtr->IsEventCallingNodePos(eventname, nodename, pos))
    {
        object->transform_->SetWorldPosition(pos);

        // eventFlags に基づいてコンポーネントを有効化(GPU)
        if (eventFlags & EnableGPUParticle && gpuparticle)
        {
            gpuparticle->SetLoop(true);
            gpuparticle->SetStop(false);
        }

        if (eventFlags & EnableCPUParticle && cpuparticle)
        {
            cpuparticle->SetActive(true);
        }

        if (eventFlags & EnableCollision && collision)
        {
            collision->SetEnabled(true);
        }

        if (eventFlags & EnableSpawn && spawn)
        {
            spawn->SetOnTrigger(true);
        }
    }
    else if (!animationComPtr->IsEventCallingNodePos(eventname, nodename, pos))
    {
        // eventFlags に基づいてコンポーネントを無効化
        if (eventFlags & EnableGPUParticle && gpuparticle)
        {
            gpuparticle->SetLoop(false);
        }

        if (eventFlags & EnableCPUParticle && cpuparticle)
        {
            cpuparticle->SetActive(false);
        }

        if (eventFlags & EnableCollision && collision)
        {
            collision->SetEnabled(false);
        }

        if (eventFlags & EnableSpawn && spawn)
        {
            spawn->SetOnTrigger(false);
        }
    }
}

//乱数の行動制御
void Boss_BaseState::RandamBehavior()
{
    // ランダムで行動を切り替える
    int randomAction = ComputeRandom();

    if (randomAction == 1)
    {
        bossCom.lock()->GetStateMachine().ChangeState(BossCom::BossState::IDLESTOP);
    }
    else if (randomAction == 2)
    {
        bossCom.lock()->GetStateMachine().ChangeState(BossCom::BossState::SHORTATTACK1);
    }
    else if (randomAction == 3)
    {
        bossCom.lock()->GetStateMachine().ChangeState(BossCom::BossState::LARIATSTART);
    }
    else if (randomAction == 4)
    {
        bossCom.lock()->GetStateMachine().ChangeState(BossCom::BossState::UPSHOTSTART);
    }
    else if (randomAction == 5)
    {
        bossCom.lock()->GetStateMachine().ChangeState(BossCom::BossState::JUMPATTACKSTART);
    }
    else if (randomAction == 6)
    {
        bossCom.lock()->GetStateMachine().ChangeState(BossCom::BossState::SHOTSTART);
    }
}

#pragma region 待機
void Boss_IdleState::Enter()
{
    animationCom.lock()->PlayAnimation(animationCom.lock()->FindAnimation("Boss_idol"), true, false, 0.1f);
}
void Boss_IdleState::Execute(const float& elapsedTime)
{
    //遠かったら近くにくる
    if (!owner->Search(7.0f))
    {
        bossCom.lock()->GetStateMachine().ChangeState(BossCom::BossState::MOVE);
    }

    if (owner->Search(7.0f))
    {
        RandamBehavior();
    }

    //死亡処理
    if (characterstatas.lock()->GetHitPoint() <= 0)
    {
        bossCom.lock()->GetStateMachine().ChangeState(BossCom::BossState::DEATH);
        return;
    }
}
#pragma endregion

#pragma region 待機行動長時間
void Boss_IdleStopState::Enter()
{
    animationCom.lock()->PlayAnimation(animationCom.lock()->FindAnimation("Boss_idol"), true, false, 0.1f);
}
void Boss_IdleStopState::Execute(const float& elapsedTime)
{
    idletime += elapsedTime;

    //待機時間
    if (idletime >= 2.0f)
    {
        RandamBehavior();
        idletime = 0.0f;
        return;
    }

    //死亡処理
    if (characterstatas.lock()->GetHitPoint() <= 0)
    {
        bossCom.lock()->GetStateMachine().ChangeState(BossCom::BossState::DEATH);
        return;
    }
}
#pragma endregion

#pragma region 移動
void Boss_MoveState::Enter()
{
    animationCom.lock()->PlayAnimation(animationCom.lock()->FindAnimation("Boss_walk_front"), true, false, 0.1f);
}
void Boss_MoveState::Execute(const float& elapsedTime)
{
    owner->MoveToTarget(0.1f, 0.1f);

    //左右の煙
    AnimtionEventControl("FOOTSMOKE", "Boss_R_ancle", "rightfootsmokeeffect", EnableCPUParticle);
    AnimtionEventControl("FOOTSMOKE", "Boss_L_ancle", "leftfootsmokeeffect", EnableCPUParticle);

    //距離判定
    if (owner->Search(7.0f))
    {
        bossCom.lock()->GetStateMachine().ChangeState(BossCom::BossState::IDLE);
    }

    //死亡処理
    if (characterstatas.lock()->GetHitPoint() <= 0)
    {
        bossCom.lock()->GetStateMachine().ChangeState(BossCom::BossState::DEATH);
        return;
    }
}
void Boss_MoveState::Exit()
{
    //エフェクトを切る
    GameObjectManager::Instance().Find("rightfootsmokeeffect")->GetComponent<CPUParticle>()->SetActive(false);
    GameObjectManager::Instance().Find("leftfootsmokeeffect")->GetComponent<CPUParticle>()->SetActive(false);
}
#pragma endregion

#pragma region 近距離攻撃1
void Boss_SA1::Enter()
{
    animationCom.lock()->PlayAnimation(animationCom.lock()->FindAnimation("Boss_short_attack_1"), false, false, 0.1f);
}
void Boss_SA1::Execute(const float& elapsedTime)
{
    AnimtionEventControl("COLLSION", "Boss_R_hand", "righthand", EnableGPUParticle | EnableCPUParticle | EnableCollision);

    //アニメーションが終われば
    if (!animationCom.lock()->IsPlayAnimation())
    {
        bossCom.lock()->GetStateMachine().ChangeState(BossCom::BossState::SHORTATTACK2);
        return;
    }

    //死亡処理
    if (characterstatas.lock()->GetHitPoint() <= 0)
    {
        bossCom.lock()->GetStateMachine().ChangeState(BossCom::BossState::DEATH);
        return;
    }
}
#pragma endregion

#pragma region 近距離攻撃2
void Boss_SA2::Enter()
{
    animationCom.lock()->PlayAnimation(animationCom.lock()->FindAnimation("Boss_short_attack_1_2"), false, false, 0.1f);
}
void Boss_SA2::Execute(const float& elapsedTime)
{
    AnimtionEventControl("COLLSION", "Boss_L_hand", "lefthand", EnableGPUParticle | EnableCPUParticle | EnableCollision);

    //アニメーションが終われば
    if (!animationCom.lock()->IsPlayAnimation())
    {
        //TODOそもそもここランダムでもいいかも
        bossCom.lock()->GetStateMachine().ChangeState(BossCom::BossState::IDLE);
        return;
    }

    //死亡処理
    if (characterstatas.lock()->GetHitPoint() <= 0)
    {
        bossCom.lock()->GetStateMachine().ChangeState(BossCom::BossState::DEATH);
        return;
    }
}
#pragma endregion

#pragma region ラリアット開始
void Boss_LARIATSTART::Enter()
{
    animationCom.lock()->PlayAnimation(animationCom.lock()->FindAnimation("Boss_swing_lariat"), false, false, 0.1f);
}
void Boss_LARIATSTART::Execute(const float& elapsedTime)
{
    //アニメーションが終われば
    if (!animationCom.lock()->IsPlayAnimation())
    {
        bossCom.lock()->GetStateMachine().ChangeState(BossCom::BossState::LARIATLOOP);
        return;
    }

    //死亡処理
    if (characterstatas.lock()->GetHitPoint() <= 0)
    {
        bossCom.lock()->GetStateMachine().ChangeState(BossCom::BossState::DEATH);
        return;
    }
}
#pragma endregion

#pragma region ラリアットループ
void Boss_LARIATLOOP::Enter()
{
    animationCom.lock()->PlayAnimation(animationCom.lock()->FindAnimation("Boss_swing_lariat_loop"), true, false, 0.1f);
}
void Boss_LARIATLOOP::Execute(const float& elapsedTime)
{
    time += elapsedTime;

    //炎を付ける
    AnimtionEventControl("COLLSION", "Boss_R_hand", "righthand", EnableGPUParticle | EnableCPUParticle | EnableCollision);
    AnimtionEventControl("COLLSION", "Boss_L_hand", "lefthand", EnableGPUParticle | EnableCPUParticle | EnableCollision);

    //待機時間
    if (time >= 4.0f)
    {
        bossCom.lock()->GetStateMachine().ChangeState(BossCom::BossState::LARIATEND);
        time = 0.0f;
        return;
    }

    //死亡処理
    if (characterstatas.lock()->GetHitPoint() <= 0)
    {
        bossCom.lock()->GetStateMachine().ChangeState(BossCom::BossState::DEATH);
        return;
    }
}
void Boss_LARIATLOOP::Exit()
{
    //エフェクトを切ると当たり判定を切る
    const auto& righthand = GameObjectManager::Instance().Find("righthand");
    const auto& lefthand = GameObjectManager::Instance().Find("lefthand");
    righthand->GetComponent<CPUParticle>()->SetActive(false);
    righthand->GetComponent<SphereColliderCom>()->SetEnabled(false);
    righthand->GetComponent<GPUParticle>()->SetLoop(false);
    lefthand->GetComponent<CPUParticle>()->SetActive(false);
    lefthand->GetComponent<SphereColliderCom>()->SetEnabled(false);
    lefthand->GetComponent<GPUParticle>()->SetLoop(false);
}
#pragma endregion

#pragma region ラリアット終了
void Boss_LARIATEND::Enter()
{
    animationCom.lock()->PlayAnimation(animationCom.lock()->FindAnimation("Boss_swing_lariat_end"), false, false, 0.1f);
}
void Boss_LARIATEND::Execute(const float& elapsedTime)
{
    //アニメーションが終われば
    if (!animationCom.lock()->IsPlayAnimation())
    {
        //TODOそもそもここランダムでもいいかも
        bossCom.lock()->GetStateMachine().ChangeState(BossCom::BossState::IDLE);
        return;
    }

    //死亡処理
    if (characterstatas.lock()->GetHitPoint() <= 0)
    {
        bossCom.lock()->GetStateMachine().ChangeState(BossCom::BossState::DEATH);
        return;
    }
}
#pragma endregion

#pragma region 打ち上げ始め
void Boss_UpShotStart::Enter()
{
    animationCom.lock()->PlayAnimation(animationCom.lock()->FindAnimation("Boss_up_shot_start"), false, false, 0.1f);
}
void Boss_UpShotStart::Execute(const float& elapsedTime)
{
    //アニメーションが終われば
    if (!animationCom.lock()->IsPlayAnimation())
    {
        bossCom.lock()->GetStateMachine().ChangeState(BossCom::BossState::UPSHOTCHARGE);
        return;
    }

    //死亡処理
    if (characterstatas.lock()->GetHitPoint() <= 0)
    {
        bossCom.lock()->GetStateMachine().ChangeState(BossCom::BossState::DEATH);
        return;
    }
}
#pragma endregion

#pragma region 打ち上げチャージ
void Boss_UpShotCharge::Enter()
{
    animationCom.lock()->PlayAnimation(animationCom.lock()->FindAnimation("Boss_up_shot_charge"), true, false, 0.1f);
}
void Boss_UpShotCharge::Execute(const float& elapsedTime)
{
    AnimtionEventControl("CHARGETIME", "Boss_L_neil2_end", "spawn", EnableGPUParticle);

    time += elapsedTime;
    if (time > 3.7f)
    {
        bossCom.lock()->GetStateMachine().ChangeState(BossCom::BossState::UPSHOTLOOP);
        time = 0.0f;
        return;
    }

    //死亡処理
    if (characterstatas.lock()->GetHitPoint() <= 0)
    {
        bossCom.lock()->GetStateMachine().ChangeState(BossCom::BossState::DEATH);
        return;
    }
}
void Boss_UpShotCharge::Exit()
{
    //エフェクトを切る
    const auto& spaen = GameObjectManager::Instance().Find("spawn");
    spaen->GetComponent<GPUParticle>()->SetLoop(false);
}
#pragma endregion

#pragma region 打ち上げループ
void Boss_UpShotLoop::Enter()
{
    animationCom.lock()->PlayAnimation(animationCom.lock()->FindAnimation("Boss_up_shot_loop"), true, false, 0.1f);
}
void Boss_UpShotLoop::Execute(const float& elapsedTime)
{
    AnimtionEventControl("SPAWN", "Boss_L_neil2_end", "spawn", EnableSpawn | EnableCPUParticle);

    time += elapsedTime;
    if (time > 3.0f)
    {
        bossCom.lock()->GetStateMachine().ChangeState(BossCom::BossState::UPSHOTEND);
        time = 0.0f;
        return;
    }

    //死亡処理
    if (characterstatas.lock()->GetHitPoint() <= 0)
    {
        bossCom.lock()->GetStateMachine().ChangeState(BossCom::BossState::DEATH);
        return;
    }
}
void Boss_UpShotLoop::Exit()
{
    const auto& spawn = GameObjectManager::Instance().Find("spawn");
    spawn->GetComponent<SpawnCom>()->SetOnTrigger(false);
    spawn->GetComponent<CPUParticle>()->SetActive(false);
}
#pragma endregion

#pragma region 打ち上げ終わり
void Boss_UpShotEnd::Enter()
{
    animationCom.lock()->PlayAnimation(animationCom.lock()->FindAnimation("Boss_up_shot_end"), false, false, 0.1f);
}
void Boss_UpShotEnd::Execute(const float& elapsedTime)
{
    //アニメーションが終われば
    if (!animationCom.lock()->IsPlayAnimation())
    {
        //TODOそもそもここランダムでもいいかも
        bossCom.lock()->GetStateMachine().ChangeState(BossCom::BossState::IDLE);
        return;
    }

    //死亡処理
    if (characterstatas.lock()->GetHitPoint() <= 0)
    {
        bossCom.lock()->GetStateMachine().ChangeState(BossCom::BossState::DEATH);
        return;
    }
}
#pragma endregion

#pragma region 打ち始め
void Boss_ShotStart::Enter()
{
    animationCom.lock()->PlayAnimation(animationCom.lock()->FindAnimation("Boss_shot_start"), false, false, 0.1f);
}
void Boss_ShotStart::Execute(const float& elapsedTime)
{
    //アニメーションが終われば
    if (!animationCom.lock()->IsPlayAnimation())
    {
        bossCom.lock()->GetStateMachine().ChangeState(BossCom::BossState::SHOTLOOP);
        return;
    }

    //死亡処理
    if (characterstatas.lock()->GetHitPoint() <= 0)
    {
        bossCom.lock()->GetStateMachine().ChangeState(BossCom::BossState::DEATH);
        return;
    }
}
#pragma endregion

#pragma region チャージ
void Boss_ShotCharge::Enter()
{
    animationCom.lock()->PlayAnimation(animationCom.lock()->FindAnimation("Boss_shot_charge_loop"), true, false, 0.1f);
}
void Boss_ShotCharge::Execute(const float& elapsedTime)
{
    AnimtionEventControl("CHARGETIME", "Boss_R_hand", "charge", EnableGPUParticle);

    time += elapsedTime;
    if (time > 3.0f)
    {
        bossCom.lock()->GetStateMachine().ChangeState(BossCom::BossState::SHOTEND);
        time = 0.0f;
        return;
    }

    //死亡処理
    if (characterstatas.lock()->GetHitPoint() <= 0)
    {
        bossCom.lock()->GetStateMachine().ChangeState(BossCom::BossState::DEATH);
        return;
    }
}
void Boss_ShotCharge::Exit()
{
    const auto& charge = GameObjectManager::Instance().Find("charge");
    charge->GetComponent<GPUParticle>()->SetLoop(false);
}
#pragma endregion

#pragma region 打ちます
void Boss_Shot::Enter()
{
    animationCom.lock()->PlayAnimation(animationCom.lock()->FindAnimation("Boss_shot_shot"), false, false, 0.1f);
}

void Boss_Shot::Execute(const float& elapsedTime)
{
    AnimtionEventControl("BEEM", "Boss_R_hand", "charge", EnableSpawn);

    //アニメーションが終われば
    if (!animationCom.lock()->IsPlayAnimation())
    {
        //TODOそもそもここランダムでもいいかも
        bossCom.lock()->GetStateMachine().ChangeState(BossCom::BossState::IDLE);
        return;
    }

    //死亡処理
    if (characterstatas.lock()->GetHitPoint() <= 0)
    {
        bossCom.lock()->GetStateMachine().ChangeState(BossCom::BossState::DEATH);
        return;
    }
}
#pragma endregion

#pragma region ジャンプ攻撃始め
void Boss_JumpAttackStart::Enter()
{
    animationCom.lock()->PlayAnimation(animationCom.lock()->FindAnimation("Boss_jump_attack_start"), false, false, 0.1f);
}
void Boss_JumpAttackStart::Execute(const float& elapsedTime)
{
    //アニメーションが終われば
    if (!animationCom.lock()->IsPlayAnimation())
    {
        bossCom.lock()->GetStateMachine().ChangeState(BossCom::BossState::JUMPATTACKEND);
        return;
    }

    //死亡処理
    if (characterstatas.lock()->GetHitPoint() <= 0)
    {
        bossCom.lock()->GetStateMachine().ChangeState(BossCom::BossState::DEATH);
        return;
    }
}
#pragma endregion

#pragma region ジャンプ攻撃終わり
void Boss_JumpAttackEnd::Enter()
{
    animationCom.lock()->PlayAnimation(animationCom.lock()->FindAnimation("Boss_swing_attack_end"), false, false, 0.1f);
}
void Boss_JumpAttackEnd::Execute(const float& elapsedTime)
{
    //アニメーションが終われば
    if (!animationCom.lock()->IsPlayAnimation())
    {
        //TODOそもそもここランダムでもいいかも
        bossCom.lock()->GetStateMachine().ChangeState(BossCom::BossState::IDLE);
        return;
    }

    //死亡処理
    if (characterstatas.lock()->GetHitPoint() <= 0)
    {
        bossCom.lock()->GetStateMachine().ChangeState(BossCom::BossState::DEATH);
        return;
    }
}
#pragma endregion

#pragma region 死亡
void Boss_DeathState::Enter()
{
    animationCom.lock()->PlayAnimation(animationCom.lock()->FindAnimation("Boss_dead"), false, false, 0.1f);
}
void Boss_DeathState::Execute(const float& elapsedTime)
{
}
#pragma endregion
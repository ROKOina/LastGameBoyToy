#include "BossState.h"
#include "Component/Enemy/BossCom.h"
#include "Component\Collsion\ColliderCom.h"
#include "Component\Character\CharaStatusCom.h"
#include "Component\Particle\GPUParticle.h"
#include "Component\System\SpawnCom.h"
#include "Component\PostEffect\PostEffect.h"
#include "Component\Camera\CameraCom.h"

//基底コンストラクタ
Boss_BaseState::Boss_BaseState(BossCom* owner) : State(owner)
{
    //初期設定
    bossCom = owner->GetGameObject()->GetComponent<BossCom>();
    moveCom = owner->GetGameObject()->GetComponent<MovementCom>();
    transCom = owner->GetGameObject()->GetComponent<TransformCom>();
    animationCom = owner->GetGameObject()->GetComponent<AnimationCom>();
    audioCom = owner->GetGameObject()->GetComponent<AudioCom>();
    characterstatas = owner->GetGameObject()->GetComponent<CharaStatusCom>();

    // 乱数エンジンのシードを設定
    std::random_device rd;
    gen = std::mt19937(rd());
}

//乱数の近接攻撃制御
void Boss_BaseState::RandamMeleeAttack()
{
    //ランダムしたい数を増やす程下記の値が増えていく
    if (availableNumbers.empty())
    {
        availableNumbers = { 1,2 };
    }

    // 乱数生成エンジンを使ってランダムにインデックスを生成
    std::uniform_int_distribution<int> dis(0, availableNumbers.size() - 1);
    int index = dis(gen);
    int randomValue = availableNumbers[index];

    availableNumbers.erase(availableNumbers.begin() + index);

    //ステート切り替え
    if (randomValue == 1)
    {
        bossCom.lock()->GetStateMachine().ChangeState(BossCom::BossState::IDLESTOP);
    }
    else if (randomValue == 2)
    {
        bossCom.lock()->GetStateMachine().ChangeState(BossCom::BossState::SHORTATTACK1);
    }
}

//乱数の遠距離攻撃制御
void Boss_BaseState::RandamLongRangeAttack()
{
    //ランダムしたい数を増やす程下記の値が増えていく
    if (availableNumbers.empty())
    {
        availableNumbers = { 1,2,3,4,5 };
    }

    // 乱数生成エンジンを使ってランダムにインデックスを生成
    std::uniform_int_distribution<int> dis(0, availableNumbers.size() - 1);
    int index = dis(gen);
    int randomValue = availableNumbers[index];

    availableNumbers.erase(availableNumbers.begin() + index);

    //ステート切り替え
    if (randomValue == 1)
    {
        bossCom.lock()->GetStateMachine().ChangeState(BossCom::BossState::IDLESTOP);
    }
    else if (randomValue == 2)
    {
        bossCom.lock()->GetStateMachine().ChangeState(BossCom::BossState::UPSHOTSTART);
    }
    else if (randomValue == 3)
    {
        bossCom.lock()->GetStateMachine().ChangeState(BossCom::BossState::SHOTSTART);
    }
    else if (randomValue == 4)
    {
        bossCom.lock()->GetStateMachine().ChangeState(BossCom::BossState::JUMPATTACKSTART);
    }
    else if (randomValue == 5)
    {
        bossCom.lock()->GetStateMachine().ChangeState(BossCom::BossState::LARIATSTART);
    }
}

//アニメーションイベント制御
void Boss_BaseState::AnimtionEventControl(const std::string& eventname, const std::string& nodename, const char* objectname, int eventFlags, DirectX::XMFLOAT3 offsetpos)
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
        object->transform_->SetWorldPosition(pos + offsetpos);

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

    //ここで共通ヒット処理を行う
    if (collision)
    {
        for (const auto& hitobject : collision->OnHitGameObject())
        {
            hitobject.gameObject.lock()->GetComponent<CharaStatusCom>()->AddDamagePoint(-1);
        }
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
    if (!owner->Search(owner->walkrange))
    {
        bossCom.lock()->GetStateMachine().ChangeState(BossCom::BossState::MOVE);
    }

    //ここ全体的に修正が必用
    if (owner->Search(owner->meleerange))
    {
        RandamMeleeAttack();
    }
    if (!owner->Search(owner->meleerange) && owner->Search(owner->longrange))
    {
        RandamLongRangeAttack();
    }

    //死亡処理
    if (characterstatas.lock()->IsDeath())
    {
        bossCom.lock()->GetStateMachine().ChangeState(BossCom::BossState::EVENT_DEATH);
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
    if (idletime >= 0.9f)
    {
        //ここ全体的に修正が必用
        if (owner->Search(owner->meleerange))
        {
            RandamMeleeAttack();
        }
        if (!owner->Search(owner->meleerange) && owner->Search(owner->longrange))
        {
            RandamLongRangeAttack();
        }

        idletime = 0.0f;
        return;
    }

    //死亡処理
    if (characterstatas.lock()->IsDeath())
    {
        bossCom.lock()->GetStateMachine().ChangeState(BossCom::BossState::EVENT_DEATH);
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
    if (owner->Search(owner->longrange))
    {
        bossCom.lock()->GetStateMachine().ChangeState(BossCom::BossState::IDLE);
    }

    //死亡処理
    if (characterstatas.lock()->IsDeath())
    {
        bossCom.lock()->GetStateMachine().ChangeState(BossCom::BossState::EVENT_DEATH);
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
    if (characterstatas.lock()->IsDeath())
    {
        bossCom.lock()->GetStateMachine().ChangeState(BossCom::BossState::EVENT_DEATH);
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
        bossCom.lock()->GetStateMachine().ChangeState(BossCom::BossState::IDLE);
        return;
    }

    //死亡処理
    if (characterstatas.lock()->IsDeath())
    {
        bossCom.lock()->GetStateMachine().ChangeState(BossCom::BossState::EVENT_DEATH);
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
    if (characterstatas.lock()->IsDeath())
    {
        bossCom.lock()->GetStateMachine().ChangeState(BossCom::BossState::EVENT_DEATH);
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

    //移動
    owner->MoveToTarget(2.0f, 0.1f);

    //左右の煙
    AnimtionEventControl("COLLSION", "Boss_R_ancle", "rightfootsmokeeffect", EnableCPUParticle);
    AnimtionEventControl("COLLSION", "Boss_L_ancle", "leftfootsmokeeffect", EnableCPUParticle);

    //炎を付ける
    AnimtionEventControl("COLLSION", "Boss_R_hand", "righthand", EnableGPUParticle | EnableCPUParticle | EnableCollision);
    AnimtionEventControl("COLLSION", "Boss_L_hand", "lefthand", EnableGPUParticle | EnableCPUParticle | EnableCollision);

    //ラリアット持続時間
    if (time >= 4.0f)
    {
        bossCom.lock()->GetStateMachine().ChangeState(BossCom::BossState::LARIATEND);
        time = 0.0f;
        return;
    }

    //死亡処理
    if (characterstatas.lock()->IsDeath())
    {
        bossCom.lock()->GetStateMachine().ChangeState(BossCom::BossState::EVENT_DEATH);
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
    GameObjectManager::Instance().Find("rightfootsmokeeffect")->GetComponent<CPUParticle>()->SetActive(false);
    GameObjectManager::Instance().Find("leftfootsmokeeffect")->GetComponent<CPUParticle>()->SetActive(false);
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
        bossCom.lock()->GetStateMachine().ChangeState(BossCom::BossState::IDLE);
        return;
    }

    //死亡処理
    if (characterstatas.lock()->IsDeath())
    {
        bossCom.lock()->GetStateMachine().ChangeState(BossCom::BossState::EVENT_DEATH);
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
    if (characterstatas.lock()->IsDeath())
    {
        bossCom.lock()->GetStateMachine().ChangeState(BossCom::BossState::EVENT_DEATH);
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
    AnimtionEventControl("CHARGETIME", "Boss_L_neil2_end", "spawn", EnableGPUParticle | EnableCPUParticle);

    time += elapsedTime;
    if (time > 3.7f)
    {
        bossCom.lock()->GetStateMachine().ChangeState(BossCom::BossState::UPSHOTLOOP);
        time = 0.0f;
        return;
    }

    //死亡処理
    if (characterstatas.lock()->IsDeath())
    {
        bossCom.lock()->GetStateMachine().ChangeState(BossCom::BossState::EVENT_DEATH);
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
    if (GameObjectManager::Instance().Find("spawn")->GetComponent<SpawnCom>()->GetSpawnFlag())
    {
        AnimtionEventControl("SPAWN", "Boss_L_hand", "muzzleflashleft", EnableCPUParticle, { 0.0f,1.0f,0.0f });
    }

    time += elapsedTime;
    if (time > 3.0f)
    {
        bossCom.lock()->GetStateMachine().ChangeState(BossCom::BossState::UPSHOTEND);
        time = 0.0f;
        return;
    }

    //死亡処理
    if (characterstatas.lock()->IsDeath())
    {
        bossCom.lock()->GetStateMachine().ChangeState(BossCom::BossState::EVENT_DEATH);
        return;
    }
}
void Boss_UpShotLoop::Exit()
{
    const auto& spawn = GameObjectManager::Instance().Find("spawn");
    const auto& muzzleflash = GameObjectManager::Instance().Find("muzzleflashleft");
    spawn->GetComponent<SpawnCom>()->SetOnTrigger(false);
    spawn->GetComponent<CPUParticle>()->SetActive(false);
    muzzleflash->GetComponent<CPUParticle>()->SetActive(false);
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
        bossCom.lock()->GetStateMachine().ChangeState(BossCom::BossState::IDLE);
        return;
    }

    //死亡処理
    if (characterstatas.lock()->IsDeath())
    {
        bossCom.lock()->GetStateMachine().ChangeState(BossCom::BossState::EVENT_DEATH);
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
    if (characterstatas.lock()->IsDeath())
    {
        bossCom.lock()->GetStateMachine().ChangeState(BossCom::BossState::EVENT_DEATH);
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
    AnimtionEventControl("CHARGETIME", "Boss_R_neil2_end", "charge", EnableGPUParticle | EnableCPUParticle);

    time += elapsedTime;
    if (time > 3.0f)
    {
        bossCom.lock()->GetStateMachine().ChangeState(BossCom::BossState::SHOTEND);
        time = 0.0f;
        return;
    }

    //死亡処理
    if (characterstatas.lock()->IsDeath())
    {
        bossCom.lock()->GetStateMachine().ChangeState(BossCom::BossState::EVENT_DEATH);
        return;
    }
}
void Boss_ShotCharge::Exit()
{
    const auto& charge = GameObjectManager::Instance().Find("charge");
    charge->GetComponent<GPUParticle>()->SetLoop(false);
    charge->GetComponent<CPUParticle>()->SetActive(false);
}
#pragma endregion

#pragma region 打ちます
void Boss_Shot::Enter()
{
    animationCom.lock()->PlayAnimation(animationCom.lock()->FindAnimation("Boss_shot_shot"), false, false, 0.1f);
}

void Boss_Shot::Execute(const float& elapsedTime)
{
    AnimtionEventControl("BEEM", "Boss_R_hand", "charge", EnableSpawn | EnableCPUParticle);
    AnimtionEventControl("BEEM", "Boss_R_hand", "muzzleflash", EnableCPUParticle);
    AnimtionEventControl("SMOKETIME", "Boss_R_hand", "charge", EnableCPUParticle);

    //アニメーションが終われば
    if (!animationCom.lock()->IsPlayAnimation())
    {
        bossCom.lock()->GetStateMachine().ChangeState(BossCom::BossState::IDLE);
        return;
    }

    //死亡処理
    if (characterstatas.lock()->IsDeath())
    {
        bossCom.lock()->GetStateMachine().ChangeState(BossCom::BossState::EVENT_DEATH);
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
    //炎を付ける
    AnimtionEventControl("EFFECTTIME", "Boss_R_hand", "righthand", EnableGPUParticle | EnableCPUParticle | EnableCollision);
    AnimtionEventControl("EFFECTTIME", "Boss_L_hand", "lefthand", EnableGPUParticle | EnableCPUParticle | EnableCollision);

    //飛ぶ
    if (animationCom.lock()->IsEventCalling("JUMPINIT"))
    {
        audioCom.lock()->Play("JUMP_ATTACK_START", false, 5.0f);
    }
    if (animationCom.lock()->IsEventCalling("JUMPTIME"))
    {
        moveCom.lock()->AddForce({ owner->GetGameObject()->transform_->GetWorldPosition().x, 2.5f, owner->GetGameObject()->transform_->GetWorldPosition().z });
    }

    //空中だったら移動
    if (!moveCom.lock()->OnGround())
    {
        owner->MoveToTarget(10.0f, 1.0f);
    }

    //アニメーションが終われば重力を強くかける
    if (!animationCom.lock()->IsPlayAnimation())
    {
        moveCom.lock()->SetGravity(2.0f);
    }

    //アニメーションが終われば
    if (!animationCom.lock()->IsPlayAnimation() && moveCom.lock()->GetVelocity().y < -41.0f)
    {
        bossCom.lock()->GetStateMachine().ChangeState(BossCom::BossState::JUMPATTACKEND);
        return;
    }

    //死亡処理
    if (characterstatas.lock()->IsDeath())
    {
        bossCom.lock()->GetStateMachine().ChangeState(BossCom::BossState::EVENT_DEATH);
        return;
    }
}
void Boss_JumpAttackStart::Exit()
{
    //重力を元に戻す
    moveCom.lock()->SetGravity(GRAVITY_NORMAL);
}
#pragma endregion

#pragma region ジャンプ攻撃終わり
void Boss_JumpAttackEnd::Enter()
{
    audioCom.lock()->Play("JUMP_ATTACK_END", false, 10.0f);
    animationCom.lock()->PlayAnimation(animationCom.lock()->FindAnimation("Boss_jump_attack_end"), false, false, 0.1f);

    //カメラシェイク
    GameObjectManager::Instance().Find("cameraPostPlayer")->GetComponent<CameraCom>()->CameraShake(0.02f, 0.5f);
}
void Boss_JumpAttackEnd::Execute(const float& elapsedTime)
{
    //炎を付ける
    AnimtionEventControl("EFFECTTIME", "Boss_R_hand", "righthand", EnableGPUParticle | EnableCPUParticle | EnableCollision);
    AnimtionEventControl("EFFECTTIME", "Boss_L_hand", "lefthand", EnableGPUParticle | EnableCPUParticle | EnableCollision);

    //エフェクト再生
    if (animationCom.lock()->IsEventCalling("GSMOKE"))
    {
        const auto& smoke = GameObjectManager::Instance().Find("groundsmoke");
        smoke->GetComponent<CPUParticle>()->SetActive(true);
    }

    //アニメーションが終われば
    if (moveCom.lock()->JustLanded())
    {
        //カメラシェイク
        audioCom.lock()->Play("JUMP_ATTACK_GROUND", false, 10.0f);
        GameObjectManager::Instance().Find("cameraPostPlayer")->GetComponent<CameraCom>()->CameraShake(0.04f, 0.5f);
    }

    if (!animationCom.lock()->IsPlayAnimation())
    {
        bossCom.lock()->GetStateMachine().ChangeState(BossCom::BossState::IDLE);
        return;
    }

    //死亡処理
    if (characterstatas.lock()->IsDeath())
    {
        bossCom.lock()->GetStateMachine().ChangeState(BossCom::BossState::EVENT_DEATH);
        return;
    }
}
#pragma endregion

void Boss_EventWalk::Enter()
{
    animationCom.lock()->PlayAnimation(animationCom.lock()->FindAnimation("Boss_walk_front"), true, false, 0.1f);
    owner->GetGameObject()->GetComponent<RendererCom>()->GetModel()->GetResource()->GetAnimationsEdit()[animationCom.lock()->FindAnimation("Boss_walk_front")].animationspeed = 2.0f;
}

void Boss_EventWalk::Execute(const float& elapsedTime)
{
    auto& moveCom = owner->GetGameObject()->GetComponent<MovementCom>();
    DirectX::XMFLOAT3 v = owner->GetGameObject()->transform_->GetWorldFront() * 0.1f;
    moveCom->AddForce({ v.x,v.y,v.z });

    //エフェクト出すならここですかね

     //左右の煙
    AnimtionEventControl("FOOTSMOKE", "Boss_R_ancle", "rightfootsmokeeffect", EnableCPUParticle);
    AnimtionEventControl("FOOTSMOKE", "Boss_L_ancle", "leftfootsmokeeffect", EnableCPUParticle);
}

void Boss_EventWalk::Exit()
{
    //エフェクトを切る
    GameObjectManager::Instance().Find("rightfootsmokeeffect")->GetComponent<CPUParticle>()->SetActive(false);
    GameObjectManager::Instance().Find("leftfootsmokeeffect")->GetComponent<CPUParticle>()->SetActive(false);

    owner->GetGameObject()->GetComponent<RendererCom>()->GetModel()->GetResource()->GetAnimationsEdit()[animationCom.lock()->FindAnimation("Boss_walk_front")].animationspeed = 1.0f;
}

void Boss_EventPunch::Enter()
{
    animationCom.lock()->PlayAnimation(animationCom.lock()->FindAnimation("Boss_short_attack_1"), false, false, 0.1f);
}

void Boss_EventPunch::Execute(const float& elapsedTime)
{
    //ここでエフェクト出す
    AnimtionEventControl("COLLSION", "Boss_R_hand", "righthand", EnableGPUParticle | EnableCPUParticle);
}

void Boss_EventPunch::Exit()
{
    //UI表示
    GameObjectManager::Instance().Find("Canvas")->SetEnabled(true);

    //プレイヤー位置設定
    const auto& player = GameObjectManager::Instance().Find("player");
    player->transform_->SetWorldPosition({ -2.471f,-0,-34.219f });
    player->GetChildFind("cameraPostPlayer")->transform_->SetEulerRotation({ -2.550f,0.0f,0.0f });
}

void Boss_EventDeath::Enter()
{
    animationCom.lock()->PlayAnimation(animationCom.lock()->FindAnimation("Boss_dead"), false, false, 0.1f);
}

void Boss_EventDeath::Execute(const float& elapsedTime)
{
}
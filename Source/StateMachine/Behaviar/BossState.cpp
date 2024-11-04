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
void Boss_BaseState::AnimtionEventControl(std::string eventname, std::string nodename, const char* objectname)
{
    // アニメーションコンポーネントのロックを一度だけ行う
    const auto& animationComPtr = animationCom.lock();
    if (!animationComPtr) return;

    // オブジェクトの取得とnullチェック
    const auto& object = GameObjectManager::Instance().Find(objectname);
    if (!object) return;

    // 必要なコンポーネントを一度取得しておく
    const auto& particleComponent = object->GetComponent<CPUParticle>();

    // コリジョンコンポーネントの取得
    const auto& collisionComponent = object->GetComponent<SphereColliderCom>();

    //gpuparticleを取得
    const auto& gpuparticle = object->GetComponent<GPUParticle>();

    // イベント発火ノードのポジション取得と処理
    DirectX::XMFLOAT3 pos = {};
    if (animationComPtr->IsEventCallingNodePos(eventname, nodename, pos))
    {
        object->transform_->SetWorldPosition(pos);

        //あれば
        if (gpuparticle)
        {
            gpuparticle->SetLoop(true);
            gpuparticle->SetStop(false);
        }

        //あれば
        if (particleComponent)
        {
            particleComponent->SetActive(true);
        }

        //あれば
        if (collisionComponent)
        {
            collisionComponent->SetEnabled(true);
        }
    }
    else
    {
        //あれば
        if (gpuparticle)
        {
            gpuparticle->SetLoop(false);
        }

        //あれば
        if (particleComponent)
        {
            particleComponent->SetActive(false);
        }

        //あれば
        if (collisionComponent)
        {
            collisionComponent->SetEnabled(false);
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
}

#pragma region 待機
void Boss_IdleState::Enter()
{
    animationCom.lock()->PlayAnimation(animationCom.lock()->FindAnimation("Boss_idol"), true, 0.6f);
}
void Boss_IdleState::Execute(const float& elapsedTime)
{
    //遠かったら近くにくる
    if (!owner->Search(7.0f))
    {
        bossCom.lock()->GetStateMachine().ChangeState(BossCom::BossState::MOVE);
        //RandamBehavior(static_cast<int>(BossCom::BossState::STOPTIME), static_cast<int>(BossCom::BossState::MOVE));
    }

    if (owner->Search(7.0f))
    {
        RandamBehavior();
    }

    //else if (owner->Search(FLT_MAX))
    //{
    //    bossCom.lock()->GetStateMachine().ChangeState(BossCom::BossState::MOVE);

    //    //RandamBehavior(static_cast<int>(BossCom::BossState::STOPTIME), static_cast<int>(BossCom::BossState::MOVE));
    //}

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
    animationCom.lock()->PlayAnimation(animationCom.lock()->FindAnimation("Boss_idol"), true, 0.6f);
}

void Boss_IdleStopState::Execute(const float& elapsedTime)
{
    idletime += elapsedTime;

    //待機時間
    if (idletime >= 2.0f)
    {
        RandamBehavior();
    }
}
void Boss_IdleStopState::Exit()
{
    idletime = 0.0f;
}
void Boss_IdleStopState::ImGui()
{
    ImGui::DragFloat("idletime", &idletime);
}
#pragma endregion

#pragma region 移動
void Boss_MoveState::Enter()
{
    animationCom.lock()->PlayAnimation(animationCom.lock()->FindAnimation("Boss_walk_front"), true);
}
void Boss_MoveState::Execute(const float& elapsedTime)
{
    owner->MoveToTarget(0.1f, 0.1f);

    //左右の煙
    AnimtionEventControl("FOOTSMOKE", "Boss_R_ancle", "rightfootsmokeeffect");
    AnimtionEventControl("FOOTSMOKE", "Boss_L_ancle", "leftfootsmokeeffect");

    //距離判定
    if (owner->Search(7.0f))
    {
        bossCom.lock()->GetStateMachine().ChangeState(BossCom::BossState::IDLE);

        //RandamBehavior(static_cast<int>(BossCom::BossState::STOPTIME), static_cast<int>(BossCom::BossState::IDLE));
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

#pragma region ジャンプ
void Boss_JumpState::Enter()
{
    animationCom.lock()->PlayAnimation(animationCom.lock()->FindAnimation("Jump"), false);

    //ジャンプ
    owner->Jump(10.0f);
}
void Boss_JumpState::Execute(const float& elapsedTime)
{
    //死亡処理
    if (characterstatas.lock()->GetHitPoint() <= 0)
    {
        bossCom.lock()->GetStateMachine().ChangeState(BossCom::BossState::DEATH);
        return;
    }
}
#pragma endregion

#pragma region パンチ
void Boss_PunchState::Enter()
{
    animationCom.lock()->PlayAnimation(animationCom.lock()->FindAnimation("Attack"), false);
}
void Boss_PunchState::Execute(const float& elapsedTime)
{
    //GPUエフェクト生成
    DirectX::XMFLOAT3 pos = {};
    if (animationCom.lock()->IsEventCallingNodePos("EFFECT", "mixamorig:LeftHand", pos))
    {
        GameObj sparkobject = GameObjectManager::Instance().Create();
        sparkobject->transform_->SetWorldPosition(pos);
        sparkobject->transform_->SetRotation(owner->GetGameObject()->transform_->GetRotation());
        sparkobject->SetName("sparkeffect");
        std::shared_ptr<GPUParticle>sparkeffect = sparkobject->AddComponent<GPUParticle>("Data/SerializeData/GPUEffect/sparks.gpuparticle", 10000);
        sparkeffect->Play();
    }

    //アニメーションが終われば
    if (!animationCom.lock()->IsPlayAnimation())
    {
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

#pragma region キック
void Boss_KickState::Enter()
{
    animationCom.lock()->PlayAnimation(animationCom.lock()->FindAnimation("Kick"), false);
}
void Boss_KickState::Execute(const float& elapsedTime)
{
    //GPUエフェクト生成
    DirectX::XMFLOAT3 pos = {};
    if (animationCom.lock()->IsEventCallingNodePos("EFFECT", "mixamorig:RightToeBase", pos))
    {
        GameObj sparkobject = GameObjectManager::Instance().Create();
        sparkobject->transform_->SetWorldPosition(pos);
        sparkobject->transform_->SetRotation(owner->GetGameObject()->transform_->GetRotation());
        sparkobject->SetName("sparkeffect");
        std::shared_ptr<GPUParticle>sparkeffect = sparkobject->AddComponent<GPUParticle>("Data/SerializeData/GPUEffect/sparks.gpuparticle", 10000);
        sparkeffect->Play();
    }

    //アニメーションが終われば
    if (!animationCom.lock()->IsPlayAnimation())
    {
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

#pragma region 範囲攻撃
void Boss_RangeAttackState::Enter()
{
    animationCom.lock()->PlayAnimation(animationCom.lock()->FindAnimation("TatumakiSenpuken"), false);
}
void Boss_RangeAttackState::Execute(const float& elapsedTime)
{
    //GPUエフェクト再生
    if (animationCom.lock()->IsEventCalling("EFFECT"))
    {
        //竜巻のエフェクト
        GameObj toru = GameObjectManager::Instance().Create();
        toru->transform_->SetWorldPosition(owner->GetGameObject()->transform_->GetWorldPosition());
        toru->SetName("torunedeffect");
        std::shared_ptr<GPUParticle>torunedeffect = toru->AddComponent<GPUParticle>("Data/SerializeData/GPUEffect/tornado.gpuparticle", 10000);
        torunedeffect->Play();
    }

    //アニメーションが終われば
    if (!animationCom.lock()->IsPlayAnimation())
    {
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

#pragma region ボンプ攻撃
void Boss_BompAttackState::Enter()
{
    animationCom.lock()->PlayAnimation(animationCom.lock()->FindAnimation("Stand"), false);
}
void Boss_BompAttackState::Execute(const float& elapsedTime)
{
    //アニメーションイベント中にボンプを生成
    if (animationCom.lock()->IsEventCalling("SPAWNENEMY"))
    {
        GameObjectManager::Instance().Find("bomp")->GetComponent<SpawnCom>()->SetOnTrigger(true);
        GameObjectManager::Instance().Find("player")->GetChildFind("spawnbomber")->GetComponent<SpawnCom>()->SetOnTrigger(true);
    }
    else
    {
        GameObjectManager::Instance().Find("bomp")->GetComponent<SpawnCom>()->SetOnTrigger(false);
        GameObjectManager::Instance().Find("player")->GetChildFind("spawnbomber")->GetComponent<SpawnCom>()->SetOnTrigger(false);
    }

    //アニメーションが終われば
    if (!animationCom.lock()->IsPlayAnimation())
    {
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

#pragma region ファイヤーボール
void Boss_FireBallState::Enter()
{
    animationCom.lock()->PlayAnimation(animationCom.lock()->FindAnimation("FireBall"), false);

    //プレイヤーを見ながら放って欲しいので旋回だけ適用
    owner->MoveToTarget(0.0f, 1.0f);
}
void Boss_FireBallState::Execute(const float& elapsedTime)
{
    //アニメーションイベント中にノードの位置を取得
    DirectX::XMFLOAT3 pos = {};
    if (animationCom.lock()->IsEventCallingNodePos("EFFECT", "mixamorig:RightHand", pos))
    {
        //ビーム作成
        {
            //ビームの手からの放射
            GameObj beemhand = GameObjectManager::Instance().Create();
            beemhand->transform_->SetWorldPosition(pos);
            beemhand->SetName("beemhandeffct");
            std::shared_ptr<GPUParticle>beemhandeffct = beemhand->AddComponent<GPUParticle>("Data/SerializeData/GPUEffect/beemhand.gpuparticle", 10000);
            beemhandeffct->Play();

            //ビームの真ん中
            GameObj beem = GameObjectManager::Instance().Create();
            beem->transform_->SetWorldPosition(pos);
            beem->transform_->SetRotation(owner->GetGameObject()->transform_->GetRotation());
            beem->SetName("beemeffct");
            std::shared_ptr<GPUParticle>beemeffct = beem->AddComponent<GPUParticle>("Data/SerializeData/GPUEffect/beem.gpuparticle", 10000);
            beemeffct->Play();

            //ビームの回り
            GameObj beemaround = GameObjectManager::Instance().Create();
            beemaround->transform_->SetWorldPosition(pos);
            beemaround->transform_->SetRotation(owner->GetGameObject()->transform_->GetRotation());
            beemaround->SetName("beemaroundeffct");
            std::shared_ptr<GPUParticle>beemaroundeffct = beemaround->AddComponent<GPUParticle>("Data/SerializeData/GPUEffect/beemaround.gpuparticle", 10000);
            beemaroundeffct->Play();
        }
    }

    //アニメーションが終われば
    if (!animationCom.lock()->IsPlayAnimation())
    {
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

#pragma region ミサイル攻撃
void Boss_MissileAttackState::Enter()
{
    animationCom.lock()->PlayAnimation(animationCom.lock()->FindAnimation("Stand"), false);
}
void Boss_MissileAttackState::Execute(const float& elapsedTime)
{
    //アニメーションが終われば
    if (!animationCom.lock()->IsPlayAnimation())
    {
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
    animationCom.lock()->PlayAnimation(animationCom.lock()->FindAnimation("Death"), false);
}
void Boss_DeathState::Execute(const float& elapsedTime)
{
}
#pragma endregion

#pragma region 近距離攻撃1
void Boss_SA1::Enter()
{
    animationCom.lock()->PlayAnimation(animationCom.lock()->FindAnimation("Boss_short_attack_1"), false);
}
void Boss_SA1::Execute(const float& elapsedTime)
{
    AnimtionEventControl("COLLSION", "Boss_R_hand", "righthand");

    //アニメーションが終われば
    if (!animationCom.lock()->IsPlayAnimation())
    {
        bossCom.lock()->GetStateMachine().ChangeState(BossCom::BossState::SHORTATTACK2);
        return;
    }
}
#pragma endregion

#pragma region 近距離攻撃2
void Boss_SA2::Enter()
{
    animationCom.lock()->PlayAnimation(animationCom.lock()->FindAnimation("Boss_short_attack_1_2"), false, 0.6f);
}
void Boss_SA2::Execute(const float& elapsedTime)
{
    AnimtionEventControl("COLLSION", "Boss_L_hand", "lefthand");

    //アニメーションが終われば
    if (!animationCom.lock()->IsPlayAnimation())
    {
        //TODOそもそもここランダムでもいいかも
        bossCom.lock()->GetStateMachine().ChangeState(BossCom::BossState::IDLE);
        return;
    }
}
#pragma endregion

#pragma region ラリアット開始
void Boss_LARIATSTART::Enter()
{
    animationCom.lock()->PlayAnimation(animationCom.lock()->FindAnimation("Boss_swing_lariat"), false, 0.6f);
}
void Boss_LARIATSTART::Execute(const float& elapsedTime)
{
    //アニメーションが終われば
    if (!animationCom.lock()->IsPlayAnimation())
    {
        bossCom.lock()->GetStateMachine().ChangeState(BossCom::BossState::LARIATLOOP);
        return;
    }
}
#pragma endregion

#pragma region ラリアットループ
void Boss_LARIATLOOP::Enter()
{
    animationCom.lock()->PlayAnimation(animationCom.lock()->FindAnimation("Boss_swing_lariat_loop"), true, 0.6f);
}
void Boss_LARIATLOOP::Execute(const float& elapsedTime)
{
    time += elapsedTime;

    //炎を付ける
    AnimtionEventControl("COLLSION", "Boss_R_hand", "righthand");
    AnimtionEventControl("COLLSION", "Boss_L_hand", "lefthand");

    //待機時間
    if (time >= 4.0f)
    {
        bossCom.lock()->GetStateMachine().ChangeState(BossCom::BossState::LARIATEND);
        time = 0.0f;
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
    animationCom.lock()->PlayAnimation(animationCom.lock()->FindAnimation("Boss_swing_lariat_end"), false, 0.6f);
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
}
#pragma endregion
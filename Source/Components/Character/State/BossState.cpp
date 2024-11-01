#include "BossState.h"
#include "Components/Enemy/Boss/BossCom.h"
#include "Components\ColliderCom.h"
#include "Components\Character\CharaStatusCom.h"
#include "Components\GPUParticle.h"
#include "Components\SpawnCom.h"
#include "Graphics\Shaders\PostEffect.h"

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

//アニメーション中の当たり判定
bool Boss_BaseState::AnimNodeCollsion(std::string eventname, std::string nodename, const char* objectname)
{
    // 初回のみFindしてキャッシュ
    if (!cachedobject || cachedobject->GetName() != objectname)
    {
        cachedobject = GameObjectManager::Instance().Find(objectname);
        if (!cachedobject)
        {
            // オブジェクトが見つからない場合はfalseを返す
            return false;
        }
    }

    // イベントが呼び出されているか確認
    if (animationCom.lock()->IsEventCallingNodePos(eventname, nodename, nodepos))
    {
        auto collider = cachedobject->GetComponent<SphereColliderCom>();
        if (collider)
        {
            collider->SetEnabled(true);

            //ヒット時体力を減らす
            for (auto& hitgameobject : collider->OnHitGameObject())
            {
                hitgameobject.gameObject.lock()->GetComponent<CharaStatusCom>()->AddDamagePoint(-1);
            }
        }

        cachedobject->transform_->SetWorldPosition(nodepos);

        return true;
    }
    else
    {
        //エフェクトのONOFFをここでしても良いと思うけどもしかしたら変更するかもね！
        if (cpuparticle != nullptr)
        {
            cpuparticle->GetComponent<CPUParticle>()->SetActive(false);
        }

        auto collider = cachedobject->GetComponent<SphereColliderCom>();
        if (collider)
        {
            collider->SetEnabled(false);
        }
        return false;
    }

    if (!cachedobject)
    {
        cachedobject.reset(); // キャッシュをリセット
    }
}

//CPUエフェクトの検索
void Boss_BaseState::CPUEffect(const char* objectname, bool posflag)
{
    // 初回のみFindしてキャッシュ
    if (!cpuparticle || cpuparticle->GetName() != objectname)
    {
        cpuparticle = GameObjectManager::Instance().Find(objectname);
        if (posflag)
        {
            cpuparticle->transform_->SetWorldPosition(nodepos);
        }
        cpuparticle->GetComponent<CPUParticle>()->SetActive(true);
    }

    if (!cpuparticle)
    {
        cpuparticle.reset(); // キャッシュをリセット
    }
}

//乱数で選択された行動を選択する関数
void Boss_BaseState::RandamBehavior(int one, int two)
{
    // ランダムで行動を切り替える
    int randomAction = ComputeRandom();

    if (randomAction == 1)
    {
        bossCom.lock()->GetStateMachine().ChangeState(static_cast<BossCom::BossState>(one));
    }
    else if (randomAction == 2)
    {
        bossCom.lock()->GetStateMachine().ChangeState(static_cast<BossCom::BossState>(two));
    }
    else if (randomAction == 3)
    {
        bossCom.lock()->GetStateMachine().ChangeState(BossCom::BossState::PUNCH);
    }
    else if (randomAction == 4)
    {
        bossCom.lock()->GetStateMachine().ChangeState(BossCom::BossState::KICK);
    }
    else if (randomAction == 5)
    {
        bossCom.lock()->GetStateMachine().ChangeState(BossCom::BossState::BOMPATTTACK);
    }
    else if (randomAction == 6)
    {
        bossCom.lock()->GetStateMachine().ChangeState(BossCom::BossState::RANGEATTACK);
    }
    else if (randomAction == 7)
    {
        bossCom.lock()->GetStateMachine().ChangeState(BossCom::BossState::JUMP);
    }
    else if (randomAction == 8)
    {
        bossCom.lock()->GetStateMachine().ChangeState(BossCom::BossState::FIREBALL);
    }
}

#pragma region 待機
void Boss_IdleState::Enter()
{
    animationCom.lock()->PlayAnimation(animationCom.lock()->FindAnimation("Idle"), true);
}
void Boss_IdleState::Execute(const float& elapsedTime)
{
    //距離判定
    if (owner->Search(5.0f))
    {
        RandamBehavior(static_cast<int>(BossCom::BossState::STOPTIME), static_cast<int>(BossCom::BossState::MOVE));
    }
    else if (owner->Search(FLT_MAX))
    {
        RandamBehavior(static_cast<int>(BossCom::BossState::STOPTIME), static_cast<int>(BossCom::BossState::MOVE));
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
    animationCom.lock()->PlayAnimation(animationCom.lock()->FindAnimation("Idle"), true);
}

void Boss_IdleStopState::Execute(const float& elapsedTime)
{
    idletime += elapsedTime;

    //待機時間
    if (idletime >= 2.0f)
    {
        RandamBehavior(static_cast<int>(BossCom::BossState::IDLE), static_cast<int>(BossCom::BossState::MOVE));
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
    animationCom.lock()->PlayAnimation(animationCom.lock()->FindAnimation("Running"), true);
}
void Boss_MoveState::Execute(const float& elapsedTime)
{
    owner->MoveToTarget(2.0f, 0.4f);

    //距離判定
    if (owner->Search(5.0f))
    {
        RandamBehavior(static_cast<int>(BossCom::BossState::STOPTIME), static_cast<int>(BossCom::BossState::IDLE));
    }

    //死亡処理
    if (characterstatas.lock()->GetHitPoint() <= 0)
    {
        bossCom.lock()->GetStateMachine().ChangeState(BossCom::BossState::DEATH);
        return;
    }
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
    //アニメーションが終われば
    if (!animationCom.lock()->IsPlayAnimation())
    {
        bossCom.lock()->GetStateMachine().ChangeState(BossCom::BossState::JUMPLOOP);
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

#pragma region ジャンプループ
void Boss_JumpLoopState::Enter()
{
    animationCom.lock()->PlayAnimation(animationCom.lock()->FindAnimation("Falling"), true);
}
void Boss_JumpLoopState::Execute(const float& elapsedTime)
{
    //着地すれば
    if (moveCom.lock()->OnGround())
    {
        bossCom.lock()->GetStateMachine().ChangeState(BossCom::BossState::LANDINGATTACK);
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

#pragma region 着地
void Boss_LandingState::Enter()
{
    animationCom.lock()->PlayAnimation(animationCom.lock()->FindAnimation("Landing"), false);
}
void Boss_LandingState::Execute(const float& elapsedTime)
{
    //煙のエフェクト生成
    CPUEffect("cpulandsmokeeffect", false);

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

#pragma region パンチ
void Boss_PunchState::Enter()
{
    animationCom.lock()->PlayAnimation(animationCom.lock()->FindAnimation("Attack"), false);
}
void Boss_PunchState::Execute(const float& elapsedTime)
{
    //アニメーションイベント時の当たり判定
    if (AnimNodeCollsion("ATTACK", "mixamorig:LeftHand", "lefthandcollsion"))
    {
        CPUEffect("cpufireeffect", true);
    }

    //GPUエフェクト生成
    DirectX::XMFLOAT3 pos = {};
    if (animationCom.lock()->IsEventCallingNodePos("EFFECT", "mixamorig:LeftHand", pos))
    {
        GameObj sparkobject = GameObjectManager::Instance().Create();
        sparkobject->transform_->SetWorldPosition(pos);
        sparkobject->transform_->SetRotation(owner->GetGameObject()->transform_->GetRotation());
        sparkobject->SetName("sparkeffect");
        std::shared_ptr<GPUParticle>sparkeffect = sparkobject->AddComponent<GPUParticle>("Data/Effect/sparks.gpuparticle", 10000);
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
    //アニメーションイベント時の当たり判定
    if (AnimNodeCollsion("KICKATTACK", "mixamorig:RightToeBase", "rightlegscollsion"))
    {
        CPUEffect("cpufireeffect", true);
    }

    //GPUエフェクト生成
    DirectX::XMFLOAT3 pos = {};
    if (animationCom.lock()->IsEventCallingNodePos("EFFECT", "mixamorig:RightToeBase", pos))
    {
        GameObj sparkobject = GameObjectManager::Instance().Create();
        sparkobject->transform_->SetWorldPosition(pos);
        sparkobject->transform_->SetRotation(owner->GetGameObject()->transform_->GetRotation());
        sparkobject->SetName("sparkeffect");
        std::shared_ptr<GPUParticle>sparkeffect = sparkobject->AddComponent<GPUParticle>("Data/Effect/sparks.gpuparticle", 10000);
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
    //アニメーションイベント時の当たり判定
    if (AnimNodeCollsion("TATUMAKIATTACK", "mixamorig:LeftToeBase", "rightlegscollsion"))
    {
        CPUEffect("cycloncpueffect", false);
    }

    //GPUエフェクト再生
    if (animationCom.lock()->IsEventCalling("EFFECT"))
    {
        //竜巻のエフェクト
        GameObj toru = GameObjectManager::Instance().Create();
        toru->transform_->SetWorldPosition(owner->GetGameObject()->transform_->GetWorldPosition());
        toru->SetName("torunedeffect");
        std::shared_ptr<GPUParticle>torunedeffect = toru->AddComponent<GPUParticle>("Data/Effect/tornado.gpuparticle", 10000);
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
            std::shared_ptr<GPUParticle>beemhandeffct = beemhand->AddComponent<GPUParticle>("Data/Effect/beemhand.gpuparticle", 10000);
            beemhandeffct->Play();

            //ビームの真ん中
            GameObj beem = GameObjectManager::Instance().Create();
            beem->transform_->SetWorldPosition(pos);
            beem->transform_->SetRotation(owner->GetGameObject()->transform_->GetRotation());
            beem->SetName("beemeffct");
            std::shared_ptr<GPUParticle>beemeffct = beem->AddComponent<GPUParticle>("Data/Effect/beem.gpuparticle", 10000);
            beemeffct->Play();

            //ビームの回り
            GameObj beemaround = GameObjectManager::Instance().Create();
            beemaround->transform_->SetWorldPosition(pos);
            beemaround->transform_->SetRotation(owner->GetGameObject()->transform_->GetRotation());
            beemaround->SetName("beemaroundeffct");
            std::shared_ptr<GPUParticle>beemaroundeffct = beemaround->AddComponent<GPUParticle>("Data/Effect/beemaround.gpuparticle", 10000);
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

#pragma region ダメージ
void Boss_DamageState::Enter()
{
    animationCom.lock()->PlayAnimation(animationCom.lock()->FindAnimation("GetHit1"), false);
}
void Boss_DamageState::Execute(const float& elapsedTime)
{
    //死亡処理
    if (characterstatas.lock()->GetHitPoint() <= 0)
    {
        bossCom.lock()->GetStateMachine().ChangeState(BossCom::BossState::DEATH);
        return;
    }
}
#pragma endregion
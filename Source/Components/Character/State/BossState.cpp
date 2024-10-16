#include "BossState.h"
#include "Components/Enemy/Boss/BossCom.h"
#include "Components\ColliderCom.h"

Boss_BaseState::Boss_BaseState(BossCom* owner) : State(owner)
{
    //初期設定
    bossCom = owner->GetGameObject()->GetComponent<BossCom>();
    moveCom = owner->GetGameObject()->GetComponent<MovementCom>();
    transCom = owner->GetGameObject()->GetComponent<TransformCom>();
    animationCom = owner->GetGameObject()->GetComponent<AnimationCom>();
    characterstatas = owner->GetGameObject()->GetComponent<CharaStatusCom>();
}

//アニメーション中の当たり判定
bool Boss_BaseState::AnimNodeCollsion(std::string eventname, std::string nodename, const char* objectname)
{
    // アニメーションイベント時の当たり判定
    DirectX::XMFLOAT3 nodepos = {};

    auto animCom = animationCom.lock();
    if (!animCom)
    {
        return false;
    }

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
    if (animCom->IsEventCallingNodePos(eventname, nodename, nodepos))
    {
        cachedobject->transform_->SetWorldPosition(nodepos);

        auto collider = cachedobject->GetComponent<SphereColliderCom>();
        if (collider)
        {
            collider->SetEnabled(true);
        }
        return true;
    }
    else
    {
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
        return false;
    }
}

#pragma region 待機
void Boss_IdleState::Enter()
{
    animationCom.lock()->PlayAnimation(animationCom.lock()->FindAnimation("Idle"), true);
}
void Boss_IdleState::Execute(const float& elapsedTime)
{
    // ランダムで行動を切り替える
    int randomAction = owner->ComputeRandom();

    //距離判定
    if (owner->Search(5.0f))
    {
        if (randomAction == 1)
        {
            bossCom.lock()->GetStateMachine().ChangeState(BossCom::BossState::ATTACK);
        }
        else if (randomAction == 2)
        {
            bossCom.lock()->GetStateMachine().ChangeState(BossCom::BossState::STOPTIME);
        }
        else if (randomAction == 3)
        {
            bossCom.lock()->GetStateMachine().ChangeState(BossCom::BossState::JUMP);
        }
    }
    else if (owner->Search(FLT_MAX))
    {
        if (randomAction == 1)
        {
            bossCom.lock()->GetStateMachine().ChangeState(BossCom::BossState::MOVE);
        }
        else if (randomAction == 2)
        {
            bossCom.lock()->GetStateMachine().ChangeState(BossCom::BossState::STOPTIME);
        }
        else if (randomAction == 3)
        {
            bossCom.lock()->GetStateMachine().ChangeState(BossCom::BossState::JUMP);
        }
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
        // ランダムで行動を切り替える
        int randomAction = owner->ComputeRandom();

        if (randomAction == 1)
        {
            bossCom.lock()->GetStateMachine().ChangeState(BossCom::BossState::ATTACK);
        }
        else if (randomAction == 2)
        {
            bossCom.lock()->GetStateMachine().ChangeState(BossCom::BossState::IDLE);
        }
        else if (randomAction == 3)
        {
            bossCom.lock()->GetStateMachine().ChangeState(BossCom::BossState::JUMP);
        }
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
    owner->MoveToTarget(2.0f, 2.0f);

    //距離判定
    if (owner->Search(5.0f))
    {
        // ランダムで行動を切り替える
        int randomAction = owner->ComputeRandom();

        if (randomAction == 1)
        {
            bossCom.lock()->GetStateMachine().ChangeState(BossCom::BossState::ATTACK);
        }
        else if (randomAction == 2)
        {
            bossCom.lock()->GetStateMachine().ChangeState(BossCom::BossState::STOPTIME);
        }
        else if (randomAction == 3)
        {
            bossCom.lock()->GetStateMachine().ChangeState(BossCom::BossState::JUMP);
        }
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

#pragma region 攻撃
void Boss_AttackState::Enter()
{
    animationCom.lock()->PlayAnimation(animationCom.lock()->FindAnimation("Attack"), false);
}
void Boss_AttackState::Execute(const float& elapsedTime)
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

    //アニメーションイベント時の当たり判定
    AnimNodeCollsion("ATTACK", "mixamorig:LeftHand", "lefthandcollsion");
}
void Boss_AttackState::ImGui()
{
}
#pragma endregion

#pragma region 範囲攻撃
void Boss_RangeAttackState::Enter()
{
    animationCom.lock()->PlayAnimation(animationCom.lock()->FindAnimation("Attack"), false);
}
void Boss_RangeAttackState::Execute(const float& elapsedTime)
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

#pragma region ボンプ攻撃
void Boss_BompAttackState::Enter()
{
    animationCom.lock()->PlayAnimation(animationCom.lock()->FindAnimation("Attack"), false);
}
void Boss_BompAttackState::Execute(const float& elapsedTime)
{
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
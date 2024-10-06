#include "BossState.h"
#include "Components/Character/BossCom.h"

Boss_BaseState::Boss_BaseState(BossCom* owner) : State(owner)
{
    //初期設定
    bossCom = owner->GetGameObject()->GetComponent<BossCom>();
    moveCom = owner->GetGameObject()->GetComponent<MovementCom>();
    transCom = owner->GetGameObject()->GetComponent<TransformCom>();
    animationCom = owner->GetGameObject()->GetComponent<AnimationCom>();
}

#pragma region 待機
void Boss_IdleState::Enter()
{
    animationCom.lock()->PlayAnimation(animationCom.lock()->FindAnimation("Idle"), true);
}

void Boss_IdleState::Execute(const float& elapsedTime)
{
    //見つけてなかったら歩きに変更
    if (!owner->Search(7.0f) && owner->ComputeRandom() == 1)
    {
        bossCom.lock()->GetStateMachine().ChangeState(BossCom::BossState::MOVE);
        return;
    }
    if (owner->Search(7.0f) && owner->ComputeRandom() == 2)
    {
        bossCom.lock()->GetStateMachine().ChangeState(BossCom::BossState::JUMP);
        return;
    }
    if (owner->Search(7.0f) && owner->ComputeRandom() == 3)
    {
        bossCom.lock()->GetStateMachine().ChangeState(BossCom::BossState::ATTACK);
        return;
    }
}

#pragma endregion

#pragma region 移動
void Boss_MoveState::Enter()
{
    animationCom.lock()->PlayAnimation(animationCom.lock()->FindAnimation("Running"), true);
}

void Boss_MoveState::Execute(const float& elapsedTime)
{
    //移動(ここキモイ)
    owner->MoveToTarget(1.0f, 2.0f);

    //見つかったら待機に変更
    if (owner->Search(4.0f) && owner->ComputeRandom() == 1)
    {
        bossCom.lock()->GetStateMachine().ChangeState(BossCom::BossState::IDLE);
        return;
    }
    if (owner->Search(4.0f) && owner->ComputeRandom() == 2)
    {
        bossCom.lock()->GetStateMachine().ChangeState(BossCom::BossState::JUMP);
        return;
    }
}
#pragma endregion

#pragma region ジャンプ
void Boss_JumpState::Enter()
{
    animationCom.lock()->PlayAnimation(animationCom.lock()->FindAnimation("Jump"), false);

    //ジャンプ
    owner->Jump(20.0f);
}
void Boss_JumpState::Execute(const float& elapsedTime)
{
    //アニメーションが終われば
    if (!animationCom.lock()->IsPlayAnimation())
    {
        bossCom.lock()->GetStateMachine().ChangeState(BossCom::BossState::JUMPLOOP);
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
}
#pragma endregion
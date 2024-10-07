#include "BossState.h"
#include "Components/Enemy/Boss/BossCom.h"

Boss_BaseState::Boss_BaseState(BossCom* owner) : State(owner)
{
    //�����ݒ�
    bossCom = owner->GetGameObject()->GetComponent<BossCom>();
    moveCom = owner->GetGameObject()->GetComponent<MovementCom>();
    transCom = owner->GetGameObject()->GetComponent<TransformCom>();
    animationCom = owner->GetGameObject()->GetComponent<AnimationCom>();
}

#pragma region �ҋ@
void Boss_IdleState::Enter()
{
    animationCom.lock()->PlayAnimation(animationCom.lock()->FindAnimation("Idle"), true);
}

void Boss_IdleState::Execute(const float& elapsedTime)
{
    // �����_���ōs����؂�ւ���
    int randomAction = owner->ComputeRandom();

    if (!owner->Search(FLT_MAX) || owner->Search(FLT_MAX))
    {
        // �������ꍇ�ł��A�����_���ōs����ύX
        if (randomAction == 1)
        {
            bossCom.lock()->GetStateMachine().ChangeState(BossCom::BossState::MOVE);
        }
        else if (randomAction == 2)
        {
            bossCom.lock()->GetStateMachine().ChangeState(BossCom::BossState::JUMP);
        }
        else if (randomAction == 3)
        {
            bossCom.lock()->GetStateMachine().ChangeState(BossCom::BossState::ATTACK);
        }
    }
}

#pragma endregion

#pragma region �ړ�
void Boss_MoveState::Enter()
{
    animationCom.lock()->PlayAnimation(animationCom.lock()->FindAnimation("Running"), true);
}

void Boss_MoveState::Execute(const float& elapsedTime)
{
    // �����_���ōs����؂�ւ���
    int randomAction = owner->ComputeRandom();

    //�ړ�
    float speed = static_cast<float>(randomAction) * 0.5f + 1.0f;
    owner->MoveToTarget(speed, 2.0f);

    if (owner->Search(5.0f))
    {
        // �������ꍇ�ł��A�����_���ōs����ύX
        if (randomAction == 1)
        {
            bossCom.lock()->GetStateMachine().ChangeState(BossCom::BossState::IDLE);
        }
        else if (randomAction == 2)
        {
            bossCom.lock()->GetStateMachine().ChangeState(BossCom::BossState::JUMP);
        }
        else if (randomAction == 3)
        {
            bossCom.lock()->GetStateMachine().ChangeState(BossCom::BossState::ATTACK);
        }
    }
}
#pragma endregion

#pragma region �W�����v
void Boss_JumpState::Enter()
{
    animationCom.lock()->PlayAnimation(animationCom.lock()->FindAnimation("Jump"), false);

    //�W�����v
    owner->Jump(20.0f);
}
void Boss_JumpState::Execute(const float& elapsedTime)
{
    //�A�j���[�V�������I����
    if (!animationCom.lock()->IsPlayAnimation())
    {
        bossCom.lock()->GetStateMachine().ChangeState(BossCom::BossState::JUMPLOOP);
        return;
    }
}
#pragma endregion

#pragma region �W�����v���[�v
void Boss_JumpLoopState::Enter()
{
    animationCom.lock()->PlayAnimation(animationCom.lock()->FindAnimation("Falling"), true);
}
void Boss_JumpLoopState::Execute(const float& elapsedTime)
{
    //���n�����
    if (moveCom.lock()->OnGround())
    {
        bossCom.lock()->GetStateMachine().ChangeState(BossCom::BossState::LANDINGATTACK);
        return;
    }
}
#pragma endregion

#pragma region ���n
void Boss_LandingState::Enter()
{
    animationCom.lock()->PlayAnimation(animationCom.lock()->FindAnimation("Landing"), false);
}

void Boss_LandingState::Execute(const float& elapsedTime)
{
    //�A�j���[�V�������I����
    if (!animationCom.lock()->IsPlayAnimation())
    {
        bossCom.lock()->GetStateMachine().ChangeState(BossCom::BossState::IDLE);
        return;
    }
}
#pragma endregion

#pragma region �U��
void Boss_AttackState::Enter()
{
    animationCom.lock()->PlayAnimation(animationCom.lock()->FindAnimation("Attack"), false);
}

void Boss_AttackState::Execute(const float& elapsedTime)
{
    //�A�j���[�V�������I����
    if (!animationCom.lock()->IsPlayAnimation())
    {
        bossCom.lock()->GetStateMachine().ChangeState(BossCom::BossState::IDLE);
        return;
    }
}
#pragma endregion

#pragma region �͈͍U��
void Boss_RangeAttackState::Enter()
{
}
void Boss_RangeAttackState::Execute(const float& elapsedTime)
{
}
#pragma endregion

#pragma region �{���v�U��
void Boss_BompAttackState::Enter()
{
}
void Boss_BompAttackState::Execute(const float& elapsedTime)
{
}
#pragma endregion

#pragma region ���S
void Boss_DeathState::Enter()
{
    animationCom.lock()->PlayAnimation(animationCom.lock()->FindAnimation("Death"), false);
}
void Boss_DeathState::Execute(const float& elapsedTime)
{
}
#pragma endregion

#pragma region �_���[�W
void Boss_DamageState::Enter()
{
    animationCom.lock()->PlayAnimation(animationCom.lock()->FindAnimation("GetHit1"), false);
}
void Boss_DamageState::Execute(const float& elapsedTime)
{
}
#pragma endregion
#include "BossState.h"
#include "Components/Character/BossCom.h"

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
    //�����ĂȂ�����������ɕύX
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

#pragma region �ړ�
void Boss_MoveState::Enter()
{
    animationCom.lock()->PlayAnimation(animationCom.lock()->FindAnimation("Running"), true);
}

void Boss_MoveState::Execute(const float& elapsedTime)
{
    //�ړ�(�����L���C)
    owner->MoveToTarget(1.0f, 2.0f);

    //����������ҋ@�ɕύX
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
#include "BossState.h"
#include "Components/Enemy/Boss/BossCom.h"
#include "Components\ColliderCom.h"

Boss_BaseState::Boss_BaseState(BossCom* owner) : State(owner)
{
    //�����ݒ�
    bossCom = owner->GetGameObject()->GetComponent<BossCom>();
    moveCom = owner->GetGameObject()->GetComponent<MovementCom>();
    transCom = owner->GetGameObject()->GetComponent<TransformCom>();
    animationCom = owner->GetGameObject()->GetComponent<AnimationCom>();
    characterstatas = owner->GetGameObject()->GetComponent<CharaStatusCom>();
}

//�A�j���[�V�������̓����蔻��
bool Boss_BaseState::AnimNodeCollsion(std::string eventname, std::string nodename, const char* objectname)
{
    // �A�j���[�V�����C�x���g���̓����蔻��
    DirectX::XMFLOAT3 nodepos = {};

    auto animCom = animationCom.lock();
    if (!animCom)
    {
        return false;
    }

    // ����̂�Find���ăL���b�V��
    if (!cachedobject || cachedobject->GetName() != objectname)
    {
        cachedobject = GameObjectManager::Instance().Find(objectname);
        if (!cachedobject)
        {
            // �I�u�W�F�N�g��������Ȃ��ꍇ��false��Ԃ�
            return false;
        }
    }

    // �C�x���g���Ăяo����Ă��邩�m�F
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
        cachedobject.reset(); // �L���b�V�������Z�b�g
        return false;
    }
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

    //��������
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

    //���S����
    if (characterstatas.lock()->GetHitPoint() <= 0)
    {
        bossCom.lock()->GetStateMachine().ChangeState(BossCom::BossState::DEATH);
        return;
    }
}
#pragma endregion

#pragma region �ҋ@�s��������
void Boss_IdleStopState::Enter()
{
    animationCom.lock()->PlayAnimation(animationCom.lock()->FindAnimation("Idle"), true);
}

void Boss_IdleStopState::Execute(const float& elapsedTime)
{
    idletime += elapsedTime;

    //�ҋ@����
    if (idletime >= 2.0f)
    {
        // �����_���ōs����؂�ւ���
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

#pragma region �ړ�
void Boss_MoveState::Enter()
{
    animationCom.lock()->PlayAnimation(animationCom.lock()->FindAnimation("Running"), true);
}
void Boss_MoveState::Execute(const float& elapsedTime)
{
    owner->MoveToTarget(2.0f, 2.0f);

    //��������
    if (owner->Search(5.0f))
    {
        // �����_���ōs����؂�ւ���
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

    //���S����
    if (characterstatas.lock()->GetHitPoint() <= 0)
    {
        bossCom.lock()->GetStateMachine().ChangeState(BossCom::BossState::DEATH);
        return;
    }
}
#pragma endregion

#pragma region �W�����v
void Boss_JumpState::Enter()
{
    animationCom.lock()->PlayAnimation(animationCom.lock()->FindAnimation("Jump"), false);

    //�W�����v
    owner->Jump(10.0f);
}
void Boss_JumpState::Execute(const float& elapsedTime)
{
    //�A�j���[�V�������I����
    if (!animationCom.lock()->IsPlayAnimation())
    {
        bossCom.lock()->GetStateMachine().ChangeState(BossCom::BossState::JUMPLOOP);
        return;
    }

    //���S����
    if (characterstatas.lock()->GetHitPoint() <= 0)
    {
        bossCom.lock()->GetStateMachine().ChangeState(BossCom::BossState::DEATH);
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

    //���S����
    if (characterstatas.lock()->GetHitPoint() <= 0)
    {
        bossCom.lock()->GetStateMachine().ChangeState(BossCom::BossState::DEATH);
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

    //���S����
    if (characterstatas.lock()->GetHitPoint() <= 0)
    {
        bossCom.lock()->GetStateMachine().ChangeState(BossCom::BossState::DEATH);
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

    //���S����
    if (characterstatas.lock()->GetHitPoint() <= 0)
    {
        bossCom.lock()->GetStateMachine().ChangeState(BossCom::BossState::DEATH);
        return;
    }

    //�A�j���[�V�����C�x���g���̓����蔻��
    AnimNodeCollsion("ATTACK", "mixamorig:LeftHand", "lefthandcollsion");
}
void Boss_AttackState::ImGui()
{
}
#pragma endregion

#pragma region �͈͍U��
void Boss_RangeAttackState::Enter()
{
    animationCom.lock()->PlayAnimation(animationCom.lock()->FindAnimation("Attack"), false);
}
void Boss_RangeAttackState::Execute(const float& elapsedTime)
{
    //�A�j���[�V�������I����
    if (!animationCom.lock()->IsPlayAnimation())
    {
        bossCom.lock()->GetStateMachine().ChangeState(BossCom::BossState::IDLE);
        return;
    }

    //���S����
    if (characterstatas.lock()->GetHitPoint() <= 0)
    {
        bossCom.lock()->GetStateMachine().ChangeState(BossCom::BossState::DEATH);
        return;
    }
}
#pragma endregion

#pragma region �{���v�U��
void Boss_BompAttackState::Enter()
{
    animationCom.lock()->PlayAnimation(animationCom.lock()->FindAnimation("Attack"), false);
}
void Boss_BompAttackState::Execute(const float& elapsedTime)
{
    //���S����
    if (characterstatas.lock()->GetHitPoint() <= 0)
    {
        bossCom.lock()->GetStateMachine().ChangeState(BossCom::BossState::DEATH);
        return;
    }
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
    //���S����
    if (characterstatas.lock()->GetHitPoint() <= 0)
    {
        bossCom.lock()->GetStateMachine().ChangeState(BossCom::BossState::DEATH);
        return;
    }
}
#pragma endregion
#include "BossState.h"
#include "Component/Enemy/BossCom.h"
#include "Component\Collsion\ColliderCom.h"
#include "Component\Character\CharaStatusCom.h"
#include "Component\Particle\GPUParticle.h"
#include "Component\System\SpawnCom.h"
#include "Component\PostEffect\PostEffect.h"

Boss_BaseState::Boss_BaseState(BossCom* owner) : State(owner)
{
    //�����ݒ�
    bossCom = owner->GetGameObject()->GetComponent<BossCom>();
    moveCom = owner->GetGameObject()->GetComponent<MovementCom>();
    transCom = owner->GetGameObject()->GetComponent<TransformCom>();
    animationCom = owner->GetGameObject()->GetComponent<AnimationCom>();
    characterstatas = owner->GetGameObject()->GetComponent<CharaStatusCom>();

    // �����G���W���̃V�[�h��ݒ�
    std::random_device rd;
    gen = std::mt19937(rd());
}

//�����v�Z
int Boss_BaseState::ComputeRandom()
{
    //�����_�����������𑝂₷�����L�̒l�������Ă���
    if (availableNumbers.empty())
    {
        availableNumbers = { 1,2,3,4,5,6,7,8 };
    }

    // ���������G���W�����g���ă����_���ɃC���f�b�N�X�𐶐�
    std::uniform_int_distribution<int> dis(0, availableNumbers.size() - 1);
    int index = dis(gen);
    int randomValue = availableNumbers[index];

    availableNumbers.erase(availableNumbers.begin() + index);

    return randomValue;
}

//�A�j���[�V�����C�x���g����
void Boss_BaseState::AnimtionEventControl(std::string eventname, std::string nodename, const char* objectname)
{
    // �A�j���[�V�����R���|�[�l���g�̃��b�N����x�����s��
    const auto& animationComPtr = animationCom.lock();
    if (!animationComPtr) return;

    // �I�u�W�F�N�g�̎擾��null�`�F�b�N
    const auto& object = GameObjectManager::Instance().Find(objectname);
    if (!object) return;

    // �K�v�ȃR���|�[�l���g����x�擾���Ă���
    const auto& particleComponent = object->GetComponent<CPUParticle>();

    // �R���W�����R���|�[�l���g�̎擾
    const auto& collisionComponent = object->GetComponent<SphereColliderCom>();

    //gpuparticle���擾
    const auto& gpuparticle = object->GetComponent<GPUParticle>();

    // �C�x���g���΃m�[�h�̃|�W�V�����擾�Ə���
    DirectX::XMFLOAT3 pos = {};
    if (animationComPtr->IsEventCallingNodePos(eventname, nodename, pos))
    {
        object->transform_->SetWorldPosition(pos);

        //�����
        if (gpuparticle)
        {
            gpuparticle->SetLoop(true);
            gpuparticle->SetStop(false);
        }

        //�����
        if (particleComponent)
        {
            particleComponent->SetActive(true);
        }

        //�����
        if (collisionComponent)
        {
            collisionComponent->SetEnabled(true);
        }
    }
    else
    {
        //�����
        if (gpuparticle)
        {
            gpuparticle->SetLoop(false);
        }

        //�����
        if (particleComponent)
        {
            particleComponent->SetActive(false);
        }

        //�����
        if (collisionComponent)
        {
            collisionComponent->SetEnabled(false);
        }
    }
}

//�����̍s������
void Boss_BaseState::RandamBehavior()
{
    // �����_���ōs����؂�ւ���
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

#pragma region �ҋ@
void Boss_IdleState::Enter()
{
    animationCom.lock()->PlayAnimation(animationCom.lock()->FindAnimation("Boss_idol"), true, 0.6f);
}
void Boss_IdleState::Execute(const float& elapsedTime)
{
    //����������߂��ɂ���
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
    animationCom.lock()->PlayAnimation(animationCom.lock()->FindAnimation("Boss_idol"), true, 0.6f);
}

void Boss_IdleStopState::Execute(const float& elapsedTime)
{
    idletime += elapsedTime;

    //�ҋ@����
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

#pragma region �ړ�
void Boss_MoveState::Enter()
{
    animationCom.lock()->PlayAnimation(animationCom.lock()->FindAnimation("Boss_walk_front"), true);
}
void Boss_MoveState::Execute(const float& elapsedTime)
{
    owner->MoveToTarget(0.1f, 0.1f);

    //���E�̉�
    AnimtionEventControl("FOOTSMOKE", "Boss_R_ancle", "rightfootsmokeeffect");
    AnimtionEventControl("FOOTSMOKE", "Boss_L_ancle", "leftfootsmokeeffect");

    //��������
    if (owner->Search(7.0f))
    {
        bossCom.lock()->GetStateMachine().ChangeState(BossCom::BossState::IDLE);

        //RandamBehavior(static_cast<int>(BossCom::BossState::STOPTIME), static_cast<int>(BossCom::BossState::IDLE));
    }

    //���S����
    if (characterstatas.lock()->GetHitPoint() <= 0)
    {
        bossCom.lock()->GetStateMachine().ChangeState(BossCom::BossState::DEATH);
        return;
    }
}
void Boss_MoveState::Exit()
{
    //�G�t�F�N�g��؂�
    GameObjectManager::Instance().Find("rightfootsmokeeffect")->GetComponent<CPUParticle>()->SetActive(false);
    GameObjectManager::Instance().Find("leftfootsmokeeffect")->GetComponent<CPUParticle>()->SetActive(false);
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
    //���S����
    if (characterstatas.lock()->GetHitPoint() <= 0)
    {
        bossCom.lock()->GetStateMachine().ChangeState(BossCom::BossState::DEATH);
        return;
    }
}
#pragma endregion

#pragma region �p���`
void Boss_PunchState::Enter()
{
    animationCom.lock()->PlayAnimation(animationCom.lock()->FindAnimation("Attack"), false);
}
void Boss_PunchState::Execute(const float& elapsedTime)
{
    //GPU�G�t�F�N�g����
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

#pragma region �L�b�N
void Boss_KickState::Enter()
{
    animationCom.lock()->PlayAnimation(animationCom.lock()->FindAnimation("Kick"), false);
}
void Boss_KickState::Execute(const float& elapsedTime)
{
    //GPU�G�t�F�N�g����
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

#pragma region �͈͍U��
void Boss_RangeAttackState::Enter()
{
    animationCom.lock()->PlayAnimation(animationCom.lock()->FindAnimation("TatumakiSenpuken"), false);
}
void Boss_RangeAttackState::Execute(const float& elapsedTime)
{
    //GPU�G�t�F�N�g�Đ�
    if (animationCom.lock()->IsEventCalling("EFFECT"))
    {
        //�����̃G�t�F�N�g
        GameObj toru = GameObjectManager::Instance().Create();
        toru->transform_->SetWorldPosition(owner->GetGameObject()->transform_->GetWorldPosition());
        toru->SetName("torunedeffect");
        std::shared_ptr<GPUParticle>torunedeffect = toru->AddComponent<GPUParticle>("Data/SerializeData/GPUEffect/tornado.gpuparticle", 10000);
        torunedeffect->Play();
    }

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
    animationCom.lock()->PlayAnimation(animationCom.lock()->FindAnimation("Stand"), false);
}
void Boss_BompAttackState::Execute(const float& elapsedTime)
{
    //�A�j���[�V�����C�x���g���Ƀ{���v�𐶐�
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

#pragma region �t�@�C���[�{�[��
void Boss_FireBallState::Enter()
{
    animationCom.lock()->PlayAnimation(animationCom.lock()->FindAnimation("FireBall"), false);

    //�v���C���[�����Ȃ�������ė~�����̂Ő��񂾂��K�p
    owner->MoveToTarget(0.0f, 1.0f);
}
void Boss_FireBallState::Execute(const float& elapsedTime)
{
    //�A�j���[�V�����C�x���g���Ƀm�[�h�̈ʒu���擾
    DirectX::XMFLOAT3 pos = {};
    if (animationCom.lock()->IsEventCallingNodePos("EFFECT", "mixamorig:RightHand", pos))
    {
        //�r�[���쐬
        {
            //�r�[���̎肩��̕���
            GameObj beemhand = GameObjectManager::Instance().Create();
            beemhand->transform_->SetWorldPosition(pos);
            beemhand->SetName("beemhandeffct");
            std::shared_ptr<GPUParticle>beemhandeffct = beemhand->AddComponent<GPUParticle>("Data/SerializeData/GPUEffect/beemhand.gpuparticle", 10000);
            beemhandeffct->Play();

            //�r�[���̐^��
            GameObj beem = GameObjectManager::Instance().Create();
            beem->transform_->SetWorldPosition(pos);
            beem->transform_->SetRotation(owner->GetGameObject()->transform_->GetRotation());
            beem->SetName("beemeffct");
            std::shared_ptr<GPUParticle>beemeffct = beem->AddComponent<GPUParticle>("Data/SerializeData/GPUEffect/beem.gpuparticle", 10000);
            beemeffct->Play();

            //�r�[���̉��
            GameObj beemaround = GameObjectManager::Instance().Create();
            beemaround->transform_->SetWorldPosition(pos);
            beemaround->transform_->SetRotation(owner->GetGameObject()->transform_->GetRotation());
            beemaround->SetName("beemaroundeffct");
            std::shared_ptr<GPUParticle>beemaroundeffct = beemaround->AddComponent<GPUParticle>("Data/SerializeData/GPUEffect/beemaround.gpuparticle", 10000);
            beemaroundeffct->Play();
        }
    }

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

#pragma region �~�T�C���U��
void Boss_MissileAttackState::Enter()
{
    animationCom.lock()->PlayAnimation(animationCom.lock()->FindAnimation("Stand"), false);
}
void Boss_MissileAttackState::Execute(const float& elapsedTime)
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

#pragma region ���S
void Boss_DeathState::Enter()
{
    animationCom.lock()->PlayAnimation(animationCom.lock()->FindAnimation("Death"), false);
}
void Boss_DeathState::Execute(const float& elapsedTime)
{
}
#pragma endregion

#pragma region �ߋ����U��1
void Boss_SA1::Enter()
{
    animationCom.lock()->PlayAnimation(animationCom.lock()->FindAnimation("Boss_short_attack_1"), false);
}
void Boss_SA1::Execute(const float& elapsedTime)
{
    AnimtionEventControl("COLLSION", "Boss_R_hand", "righthand");

    //�A�j���[�V�������I����
    if (!animationCom.lock()->IsPlayAnimation())
    {
        bossCom.lock()->GetStateMachine().ChangeState(BossCom::BossState::SHORTATTACK2);
        return;
    }
}
#pragma endregion

#pragma region �ߋ����U��2
void Boss_SA2::Enter()
{
    animationCom.lock()->PlayAnimation(animationCom.lock()->FindAnimation("Boss_short_attack_1_2"), false, 0.6f);
}
void Boss_SA2::Execute(const float& elapsedTime)
{
    AnimtionEventControl("COLLSION", "Boss_L_hand", "lefthand");

    //�A�j���[�V�������I����
    if (!animationCom.lock()->IsPlayAnimation())
    {
        //TODO�����������������_���ł���������
        bossCom.lock()->GetStateMachine().ChangeState(BossCom::BossState::IDLE);
        return;
    }
}
#pragma endregion

#pragma region �����A�b�g�J�n
void Boss_LARIATSTART::Enter()
{
    animationCom.lock()->PlayAnimation(animationCom.lock()->FindAnimation("Boss_swing_lariat"), false, 0.6f);
}
void Boss_LARIATSTART::Execute(const float& elapsedTime)
{
    //�A�j���[�V�������I����
    if (!animationCom.lock()->IsPlayAnimation())
    {
        bossCom.lock()->GetStateMachine().ChangeState(BossCom::BossState::LARIATLOOP);
        return;
    }
}
#pragma endregion

#pragma region �����A�b�g���[�v
void Boss_LARIATLOOP::Enter()
{
    animationCom.lock()->PlayAnimation(animationCom.lock()->FindAnimation("Boss_swing_lariat_loop"), true, 0.6f);
}
void Boss_LARIATLOOP::Execute(const float& elapsedTime)
{
    time += elapsedTime;

    //����t����
    AnimtionEventControl("COLLSION", "Boss_R_hand", "righthand");
    AnimtionEventControl("COLLSION", "Boss_L_hand", "lefthand");

    //�ҋ@����
    if (time >= 4.0f)
    {
        bossCom.lock()->GetStateMachine().ChangeState(BossCom::BossState::LARIATEND);
        time = 0.0f;
        return;
    }
}
void Boss_LARIATLOOP::Exit()
{
    //�G�t�F�N�g��؂�Ɠ����蔻���؂�
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

#pragma region �����A�b�g�I��
void Boss_LARIATEND::Enter()
{
    animationCom.lock()->PlayAnimation(animationCom.lock()->FindAnimation("Boss_swing_lariat_end"), false, 0.6f);
}
void Boss_LARIATEND::Execute(const float& elapsedTime)
{
    //�A�j���[�V�������I����
    if (!animationCom.lock()->IsPlayAnimation())
    {
        //TODO�����������������_���ł���������
        bossCom.lock()->GetStateMachine().ChangeState(BossCom::BossState::IDLE);
        return;
    }
}
#pragma endregion
#include "BossState.h"
#include "Components/Enemy/Boss/BossCom.h"
#include "Components\ColliderCom.h"
#include "Components\Character\CharaStatusCom.h"
#include "Components\GPUParticle.h"
#include "Components\SpawnCom.h"
#include "Graphics\Shaders\PostEffect.h"

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

//�A�j���[�V�������̓����蔻��
bool Boss_BaseState::AnimNodeCollsion(std::string eventname, std::string nodename, const char* objectname)
{
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
    if (animationCom.lock()->IsEventCallingNodePos(eventname, nodename, nodepos))
    {
        auto collider = cachedobject->GetComponent<SphereColliderCom>();
        if (collider)
        {
            collider->SetEnabled(true);

            //�q�b�g���̗͂����炷
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
        //�G�t�F�N�g��ONOFF�������ł��Ă��ǂ��Ǝv�����ǂ�����������ύX���邩���ˁI
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
        cachedobject.reset(); // �L���b�V�������Z�b�g
    }
}

//CPU�G�t�F�N�g�̌���
void Boss_BaseState::CPUEffect(const char* objectname, bool posflag)
{
    // ����̂�Find���ăL���b�V��
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
        cpuparticle.reset(); // �L���b�V�������Z�b�g
    }
}

//�����őI�����ꂽ�s����I������֐�
void Boss_BaseState::RandamBehavior(int one, int two)
{
    // �����_���ōs����؂�ւ���
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

#pragma region �ҋ@
void Boss_IdleState::Enter()
{
    animationCom.lock()->PlayAnimation(animationCom.lock()->FindAnimation("Idle"), true);
}
void Boss_IdleState::Execute(const float& elapsedTime)
{
    //��������
    if (owner->Search(5.0f))
    {
        RandamBehavior(static_cast<int>(BossCom::BossState::STOPTIME), static_cast<int>(BossCom::BossState::MOVE));
    }
    else if (owner->Search(FLT_MAX))
    {
        RandamBehavior(static_cast<int>(BossCom::BossState::STOPTIME), static_cast<int>(BossCom::BossState::MOVE));
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

#pragma region �ړ�
void Boss_MoveState::Enter()
{
    animationCom.lock()->PlayAnimation(animationCom.lock()->FindAnimation("Running"), true);
}
void Boss_MoveState::Execute(const float& elapsedTime)
{
    owner->MoveToTarget(2.0f, 0.4f);

    //��������
    if (owner->Search(5.0f))
    {
        RandamBehavior(static_cast<int>(BossCom::BossState::STOPTIME), static_cast<int>(BossCom::BossState::IDLE));
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
    //���̃G�t�F�N�g����
    CPUEffect("cpulandsmokeeffect", false);

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

#pragma region �p���`
void Boss_PunchState::Enter()
{
    animationCom.lock()->PlayAnimation(animationCom.lock()->FindAnimation("Attack"), false);
}
void Boss_PunchState::Execute(const float& elapsedTime)
{
    //�A�j���[�V�����C�x���g���̓����蔻��
    if (AnimNodeCollsion("ATTACK", "mixamorig:LeftHand", "lefthandcollsion"))
    {
        CPUEffect("cpufireeffect", true);
    }

    //GPU�G�t�F�N�g����
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
    //�A�j���[�V�����C�x���g���̓����蔻��
    if (AnimNodeCollsion("KICKATTACK", "mixamorig:RightToeBase", "rightlegscollsion"))
    {
        CPUEffect("cpufireeffect", true);
    }

    //GPU�G�t�F�N�g����
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
    //�A�j���[�V�����C�x���g���̓����蔻��
    if (AnimNodeCollsion("TATUMAKIATTACK", "mixamorig:LeftToeBase", "rightlegscollsion"))
    {
        CPUEffect("cycloncpueffect", false);
    }

    //GPU�G�t�F�N�g�Đ�
    if (animationCom.lock()->IsEventCalling("EFFECT"))
    {
        //�����̃G�t�F�N�g
        GameObj toru = GameObjectManager::Instance().Create();
        toru->transform_->SetWorldPosition(owner->GetGameObject()->transform_->GetWorldPosition());
        toru->SetName("torunedeffect");
        std::shared_ptr<GPUParticle>torunedeffect = toru->AddComponent<GPUParticle>("Data/Effect/tornado.gpuparticle", 10000);
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
            std::shared_ptr<GPUParticle>beemhandeffct = beemhand->AddComponent<GPUParticle>("Data/Effect/beemhand.gpuparticle", 10000);
            beemhandeffct->Play();

            //�r�[���̐^��
            GameObj beem = GameObjectManager::Instance().Create();
            beem->transform_->SetWorldPosition(pos);
            beem->transform_->SetRotation(owner->GetGameObject()->transform_->GetRotation());
            beem->SetName("beemeffct");
            std::shared_ptr<GPUParticle>beemeffct = beem->AddComponent<GPUParticle>("Data/Effect/beem.gpuparticle", 10000);
            beemeffct->Play();

            //�r�[���̉��
            GameObj beemaround = GameObjectManager::Instance().Create();
            beemaround->transform_->SetWorldPosition(pos);
            beemaround->transform_->SetRotation(owner->GetGameObject()->transform_->GetRotation());
            beemaround->SetName("beemaroundeffct");
            std::shared_ptr<GPUParticle>beemaroundeffct = beemaround->AddComponent<GPUParticle>("Data/Effect/beemaround.gpuparticle", 10000);
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
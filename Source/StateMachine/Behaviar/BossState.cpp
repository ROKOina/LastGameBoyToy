#include "BossState.h"
#include "Component/Enemy/BossCom.h"
#include "Component\Collsion\ColliderCom.h"
#include "Component\Character\CharaStatusCom.h"
#include "Component\Particle\GPUParticle.h"
#include "Component\System\SpawnCom.h"
#include "Component\PostEffect\PostEffect.h"
#include "Component\Camera\CameraCom.h"

//���R���X�g���N�^
Boss_BaseState::Boss_BaseState(BossCom* owner) : State(owner)
{
    //�����ݒ�
    bossCom = owner->GetGameObject()->GetComponent<BossCom>();
    moveCom = owner->GetGameObject()->GetComponent<MovementCom>();
    transCom = owner->GetGameObject()->GetComponent<TransformCom>();
    animationCom = owner->GetGameObject()->GetComponent<AnimationCom>();
    audioCom = owner->GetGameObject()->GetComponent<AudioCom>();
    characterstatas = owner->GetGameObject()->GetComponent<CharaStatusCom>();

    // �����G���W���̃V�[�h��ݒ�
    std::random_device rd;
    gen = std::mt19937(rd());
}

//�����̋ߐڍU������
void Boss_BaseState::RandamMeleeAttack()
{
    //�����_�����������𑝂₷�����L�̒l�������Ă���
    if (availableNumbers.empty())
    {
        availableNumbers = { 1,2 };
    }

    // ���������G���W�����g���ă����_���ɃC���f�b�N�X�𐶐�
    std::uniform_int_distribution<int> dis(0, availableNumbers.size() - 1);
    int index = dis(gen);
    int randomValue = availableNumbers[index];

    availableNumbers.erase(availableNumbers.begin() + index);

    //�X�e�[�g�؂�ւ�
    if (randomValue == 1)
    {
        bossCom.lock()->GetStateMachine().ChangeState(BossCom::BossState::IDLESTOP);
    }
    else if (randomValue == 2)
    {
        bossCom.lock()->GetStateMachine().ChangeState(BossCom::BossState::SHORTATTACK1);
    }
}

//�����̉������U������
void Boss_BaseState::RandamLongRangeAttack()
{
    //�����_�����������𑝂₷�����L�̒l�������Ă���
    if (availableNumbers.empty())
    {
        availableNumbers = { 1,2,3,4,5 };
    }

    // ���������G���W�����g���ă����_���ɃC���f�b�N�X�𐶐�
    std::uniform_int_distribution<int> dis(0, availableNumbers.size() - 1);
    int index = dis(gen);
    int randomValue = availableNumbers[index];

    availableNumbers.erase(availableNumbers.begin() + index);

    //�X�e�[�g�؂�ւ�
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

//�A�j���[�V�����C�x���g����
void Boss_BaseState::AnimtionEventControl(const std::string& eventname, const std::string& nodename, const char* objectname, int eventFlags, DirectX::XMFLOAT3 offsetpos)
{
    // �A�j���[�V�����R���|�[�l���g�̃��b�N����x�����s��
    const auto& animationComPtr = animationCom.lock();
    if (!animationComPtr) return;

    // �I�u�W�F�N�g�̎擾��null�`�F�b�N
    auto object = GameObjectManager::Instance().Find(objectname);
    if (!object) return;

    // �e�R���|�[�l���g���擾���Ă���
    const auto& cpuparticle = object->GetComponent<CPUParticle>();
    const auto& collision = object->GetComponent<SphereColliderCom>();
    const auto& gpuparticle = object->GetComponent<GPUParticle>();
    const auto& spawn = object->GetComponent<SpawnCom>();

    // �C�x���g���΃m�[�h�̃|�W�V�����擾�Ə���
    DirectX::XMFLOAT3 pos = {};
    if (animationComPtr->IsEventCallingNodePos(eventname, nodename, pos))
    {
        object->transform_->SetWorldPosition(pos + offsetpos);

        // eventFlags �Ɋ�Â��ăR���|�[�l���g��L����(GPU)
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
        // eventFlags �Ɋ�Â��ăR���|�[�l���g�𖳌���
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

    //�����ŋ��ʃq�b�g�������s��
    if (collision)
    {
        for (const auto& hitobject : collision->OnHitGameObject())
        {
            hitobject.gameObject.lock()->GetComponent<CharaStatusCom>()->AddDamagePoint(-1);
        }
    }
}

#pragma region �ҋ@
void Boss_IdleState::Enter()
{
    animationCom.lock()->PlayAnimation(animationCom.lock()->FindAnimation("Boss_idol"), true, false, 0.1f);
}
void Boss_IdleState::Execute(const float& elapsedTime)
{
    //����������߂��ɂ���
    if (!owner->Search(owner->walkrange))
    {
        bossCom.lock()->GetStateMachine().ChangeState(BossCom::BossState::MOVE);
    }

    //�����S�̓I�ɏC�����K�p
    if (owner->Search(owner->meleerange))
    {
        RandamMeleeAttack();
    }
    if (!owner->Search(owner->meleerange) && owner->Search(owner->longrange))
    {
        RandamLongRangeAttack();
    }

    //���S����
    if (characterstatas.lock()->IsDeath())
    {
        bossCom.lock()->GetStateMachine().ChangeState(BossCom::BossState::EVENT_DEATH);
        return;
    }
}
#pragma endregion

#pragma region �ҋ@�s��������
void Boss_IdleStopState::Enter()
{
    animationCom.lock()->PlayAnimation(animationCom.lock()->FindAnimation("Boss_idol"), true, false, 0.1f);
}
void Boss_IdleStopState::Execute(const float& elapsedTime)
{
    idletime += elapsedTime;

    //�ҋ@����
    if (idletime >= 0.9f)
    {
        //�����S�̓I�ɏC�����K�p
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

    //���S����
    if (characterstatas.lock()->IsDeath())
    {
        bossCom.lock()->GetStateMachine().ChangeState(BossCom::BossState::EVENT_DEATH);
        return;
    }
}
#pragma endregion

#pragma region �ړ�
void Boss_MoveState::Enter()
{
    animationCom.lock()->PlayAnimation(animationCom.lock()->FindAnimation("Boss_walk_front"), true, false, 0.1f);
}
void Boss_MoveState::Execute(const float& elapsedTime)
{
    owner->MoveToTarget(0.1f, 0.1f);

    //���E�̉�
    AnimtionEventControl("FOOTSMOKE", "Boss_R_ancle", "rightfootsmokeeffect", EnableCPUParticle);
    AnimtionEventControl("FOOTSMOKE", "Boss_L_ancle", "leftfootsmokeeffect", EnableCPUParticle);

    //��������
    if (owner->Search(owner->longrange))
    {
        bossCom.lock()->GetStateMachine().ChangeState(BossCom::BossState::IDLE);
    }

    //���S����
    if (characterstatas.lock()->IsDeath())
    {
        bossCom.lock()->GetStateMachine().ChangeState(BossCom::BossState::EVENT_DEATH);
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

#pragma region �ߋ����U��1
void Boss_SA1::Enter()
{
    animationCom.lock()->PlayAnimation(animationCom.lock()->FindAnimation("Boss_short_attack_1"), false, false, 0.1f);
}
void Boss_SA1::Execute(const float& elapsedTime)
{
    AnimtionEventControl("COLLSION", "Boss_R_hand", "righthand", EnableGPUParticle | EnableCPUParticle | EnableCollision);

    //�A�j���[�V�������I����
    if (!animationCom.lock()->IsPlayAnimation())
    {
        bossCom.lock()->GetStateMachine().ChangeState(BossCom::BossState::SHORTATTACK2);
        return;
    }

    //���S����
    if (characterstatas.lock()->IsDeath())
    {
        bossCom.lock()->GetStateMachine().ChangeState(BossCom::BossState::EVENT_DEATH);
        return;
    }
}
#pragma endregion

#pragma region �ߋ����U��2
void Boss_SA2::Enter()
{
    animationCom.lock()->PlayAnimation(animationCom.lock()->FindAnimation("Boss_short_attack_1_2"), false, false, 0.1f);
}
void Boss_SA2::Execute(const float& elapsedTime)
{
    AnimtionEventControl("COLLSION", "Boss_L_hand", "lefthand", EnableGPUParticle | EnableCPUParticle | EnableCollision);

    //�A�j���[�V�������I����
    if (!animationCom.lock()->IsPlayAnimation())
    {
        bossCom.lock()->GetStateMachine().ChangeState(BossCom::BossState::IDLE);
        return;
    }

    //���S����
    if (characterstatas.lock()->IsDeath())
    {
        bossCom.lock()->GetStateMachine().ChangeState(BossCom::BossState::EVENT_DEATH);
        return;
    }
}
#pragma endregion

#pragma region �����A�b�g�J�n
void Boss_LARIATSTART::Enter()
{
    animationCom.lock()->PlayAnimation(animationCom.lock()->FindAnimation("Boss_swing_lariat"), false, false, 0.1f);
}
void Boss_LARIATSTART::Execute(const float& elapsedTime)
{
    //�A�j���[�V�������I����
    if (!animationCom.lock()->IsPlayAnimation())
    {
        bossCom.lock()->GetStateMachine().ChangeState(BossCom::BossState::LARIATLOOP);
        return;
    }

    //���S����
    if (characterstatas.lock()->IsDeath())
    {
        bossCom.lock()->GetStateMachine().ChangeState(BossCom::BossState::EVENT_DEATH);
        return;
    }
}
#pragma endregion

#pragma region �����A�b�g���[�v
void Boss_LARIATLOOP::Enter()
{
    animationCom.lock()->PlayAnimation(animationCom.lock()->FindAnimation("Boss_swing_lariat_loop"), true, false, 0.1f);
}
void Boss_LARIATLOOP::Execute(const float& elapsedTime)
{
    time += elapsedTime;

    //�ړ�
    owner->MoveToTarget(2.0f, 0.1f);

    //���E�̉�
    AnimtionEventControl("COLLSION", "Boss_R_ancle", "rightfootsmokeeffect", EnableCPUParticle);
    AnimtionEventControl("COLLSION", "Boss_L_ancle", "leftfootsmokeeffect", EnableCPUParticle);

    //����t����
    AnimtionEventControl("COLLSION", "Boss_R_hand", "righthand", EnableGPUParticle | EnableCPUParticle | EnableCollision);
    AnimtionEventControl("COLLSION", "Boss_L_hand", "lefthand", EnableGPUParticle | EnableCPUParticle | EnableCollision);

    //�����A�b�g��������
    if (time >= 4.0f)
    {
        bossCom.lock()->GetStateMachine().ChangeState(BossCom::BossState::LARIATEND);
        time = 0.0f;
        return;
    }

    //���S����
    if (characterstatas.lock()->IsDeath())
    {
        bossCom.lock()->GetStateMachine().ChangeState(BossCom::BossState::EVENT_DEATH);
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
    GameObjectManager::Instance().Find("rightfootsmokeeffect")->GetComponent<CPUParticle>()->SetActive(false);
    GameObjectManager::Instance().Find("leftfootsmokeeffect")->GetComponent<CPUParticle>()->SetActive(false);
}
#pragma endregion

#pragma region �����A�b�g�I��
void Boss_LARIATEND::Enter()
{
    animationCom.lock()->PlayAnimation(animationCom.lock()->FindAnimation("Boss_swing_lariat_end"), false, false, 0.1f);
}
void Boss_LARIATEND::Execute(const float& elapsedTime)
{
    //�A�j���[�V�������I����
    if (!animationCom.lock()->IsPlayAnimation())
    {
        bossCom.lock()->GetStateMachine().ChangeState(BossCom::BossState::IDLE);
        return;
    }

    //���S����
    if (characterstatas.lock()->IsDeath())
    {
        bossCom.lock()->GetStateMachine().ChangeState(BossCom::BossState::EVENT_DEATH);
        return;
    }
}
#pragma endregion

#pragma region �ł��グ�n��
void Boss_UpShotStart::Enter()
{
    animationCom.lock()->PlayAnimation(animationCom.lock()->FindAnimation("Boss_up_shot_start"), false, false, 0.1f);
}
void Boss_UpShotStart::Execute(const float& elapsedTime)
{
    //�A�j���[�V�������I����
    if (!animationCom.lock()->IsPlayAnimation())
    {
        bossCom.lock()->GetStateMachine().ChangeState(BossCom::BossState::UPSHOTCHARGE);
        return;
    }

    //���S����
    if (characterstatas.lock()->IsDeath())
    {
        bossCom.lock()->GetStateMachine().ChangeState(BossCom::BossState::EVENT_DEATH);
        return;
    }
}
#pragma endregion

#pragma region �ł��グ�`���[�W
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

    //���S����
    if (characterstatas.lock()->IsDeath())
    {
        bossCom.lock()->GetStateMachine().ChangeState(BossCom::BossState::EVENT_DEATH);
        return;
    }
}
void Boss_UpShotCharge::Exit()
{
    //�G�t�F�N�g��؂�
    const auto& spaen = GameObjectManager::Instance().Find("spawn");
    spaen->GetComponent<GPUParticle>()->SetLoop(false);
}
#pragma endregion

#pragma region �ł��グ���[�v
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

    //���S����
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

#pragma region �ł��グ�I���
void Boss_UpShotEnd::Enter()
{
    animationCom.lock()->PlayAnimation(animationCom.lock()->FindAnimation("Boss_up_shot_end"), false, false, 0.1f);
}
void Boss_UpShotEnd::Execute(const float& elapsedTime)
{
    //�A�j���[�V�������I����
    if (!animationCom.lock()->IsPlayAnimation())
    {
        bossCom.lock()->GetStateMachine().ChangeState(BossCom::BossState::IDLE);
        return;
    }

    //���S����
    if (characterstatas.lock()->IsDeath())
    {
        bossCom.lock()->GetStateMachine().ChangeState(BossCom::BossState::EVENT_DEATH);
        return;
    }
}
#pragma endregion

#pragma region �ł��n��
void Boss_ShotStart::Enter()
{
    animationCom.lock()->PlayAnimation(animationCom.lock()->FindAnimation("Boss_shot_start"), false, false, 0.1f);
}
void Boss_ShotStart::Execute(const float& elapsedTime)
{
    //�A�j���[�V�������I����
    if (!animationCom.lock()->IsPlayAnimation())
    {
        bossCom.lock()->GetStateMachine().ChangeState(BossCom::BossState::SHOTLOOP);
        return;
    }

    //���S����
    if (characterstatas.lock()->IsDeath())
    {
        bossCom.lock()->GetStateMachine().ChangeState(BossCom::BossState::EVENT_DEATH);
        return;
    }
}
#pragma endregion

#pragma region �`���[�W
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

    //���S����
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

#pragma region �ł��܂�
void Boss_Shot::Enter()
{
    animationCom.lock()->PlayAnimation(animationCom.lock()->FindAnimation("Boss_shot_shot"), false, false, 0.1f);
}

void Boss_Shot::Execute(const float& elapsedTime)
{
    AnimtionEventControl("BEEM", "Boss_R_hand", "charge", EnableSpawn | EnableCPUParticle);
    AnimtionEventControl("BEEM", "Boss_R_hand", "muzzleflash", EnableCPUParticle);
    AnimtionEventControl("SMOKETIME", "Boss_R_hand", "charge", EnableCPUParticle);

    //�A�j���[�V�������I����
    if (!animationCom.lock()->IsPlayAnimation())
    {
        bossCom.lock()->GetStateMachine().ChangeState(BossCom::BossState::IDLE);
        return;
    }

    //���S����
    if (characterstatas.lock()->IsDeath())
    {
        bossCom.lock()->GetStateMachine().ChangeState(BossCom::BossState::EVENT_DEATH);
        return;
    }
}
#pragma endregion

#pragma region �W�����v�U���n��
void Boss_JumpAttackStart::Enter()
{
    animationCom.lock()->PlayAnimation(animationCom.lock()->FindAnimation("Boss_jump_attack_start"), false, false, 0.1f);
}
void Boss_JumpAttackStart::Execute(const float& elapsedTime)
{
    //����t����
    AnimtionEventControl("EFFECTTIME", "Boss_R_hand", "righthand", EnableGPUParticle | EnableCPUParticle | EnableCollision);
    AnimtionEventControl("EFFECTTIME", "Boss_L_hand", "lefthand", EnableGPUParticle | EnableCPUParticle | EnableCollision);

    //���
    if (animationCom.lock()->IsEventCalling("JUMPINIT"))
    {
        audioCom.lock()->Play("JUMP_ATTACK_START", false, 5.0f);
    }
    if (animationCom.lock()->IsEventCalling("JUMPTIME"))
    {
        moveCom.lock()->AddForce({ owner->GetGameObject()->transform_->GetWorldPosition().x, 2.5f, owner->GetGameObject()->transform_->GetWorldPosition().z });
    }

    //�󒆂�������ړ�
    if (!moveCom.lock()->OnGround())
    {
        owner->MoveToTarget(10.0f, 1.0f);
    }

    //�A�j���[�V�������I���Ώd�͂�����������
    if (!animationCom.lock()->IsPlayAnimation())
    {
        moveCom.lock()->SetGravity(2.0f);
    }

    //�A�j���[�V�������I����
    if (!animationCom.lock()->IsPlayAnimation() && moveCom.lock()->GetVelocity().y < -41.0f)
    {
        bossCom.lock()->GetStateMachine().ChangeState(BossCom::BossState::JUMPATTACKEND);
        return;
    }

    //���S����
    if (characterstatas.lock()->IsDeath())
    {
        bossCom.lock()->GetStateMachine().ChangeState(BossCom::BossState::EVENT_DEATH);
        return;
    }
}
void Boss_JumpAttackStart::Exit()
{
    //�d�͂����ɖ߂�
    moveCom.lock()->SetGravity(GRAVITY_NORMAL);
}
#pragma endregion

#pragma region �W�����v�U���I���
void Boss_JumpAttackEnd::Enter()
{
    audioCom.lock()->Play("JUMP_ATTACK_END", false, 10.0f);
    animationCom.lock()->PlayAnimation(animationCom.lock()->FindAnimation("Boss_jump_attack_end"), false, false, 0.1f);

    //�J�����V�F�C�N
    GameObjectManager::Instance().Find("cameraPostPlayer")->GetComponent<CameraCom>()->CameraShake(0.02f, 0.5f);
}
void Boss_JumpAttackEnd::Execute(const float& elapsedTime)
{
    //����t����
    AnimtionEventControl("EFFECTTIME", "Boss_R_hand", "righthand", EnableGPUParticle | EnableCPUParticle | EnableCollision);
    AnimtionEventControl("EFFECTTIME", "Boss_L_hand", "lefthand", EnableGPUParticle | EnableCPUParticle | EnableCollision);

    //�G�t�F�N�g�Đ�
    if (animationCom.lock()->IsEventCalling("GSMOKE"))
    {
        const auto& smoke = GameObjectManager::Instance().Find("groundsmoke");
        smoke->GetComponent<CPUParticle>()->SetActive(true);
    }

    //�A�j���[�V�������I����
    if (moveCom.lock()->JustLanded())
    {
        //�J�����V�F�C�N
        audioCom.lock()->Play("JUMP_ATTACK_GROUND", false, 10.0f);
        GameObjectManager::Instance().Find("cameraPostPlayer")->GetComponent<CameraCom>()->CameraShake(0.04f, 0.5f);
    }

    if (!animationCom.lock()->IsPlayAnimation())
    {
        bossCom.lock()->GetStateMachine().ChangeState(BossCom::BossState::IDLE);
        return;
    }

    //���S����
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

    //�G�t�F�N�g�o���Ȃ炱���ł�����

     //���E�̉�
    AnimtionEventControl("FOOTSMOKE", "Boss_R_ancle", "rightfootsmokeeffect", EnableCPUParticle);
    AnimtionEventControl("FOOTSMOKE", "Boss_L_ancle", "leftfootsmokeeffect", EnableCPUParticle);
}

void Boss_EventWalk::Exit()
{
    //�G�t�F�N�g��؂�
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
    //�����ŃG�t�F�N�g�o��
    AnimtionEventControl("COLLSION", "Boss_R_hand", "righthand", EnableGPUParticle | EnableCPUParticle);
}

void Boss_EventPunch::Exit()
{
    //UI�\��
    GameObjectManager::Instance().Find("Canvas")->SetEnabled(true);

    //�v���C���[�ʒu�ݒ�
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
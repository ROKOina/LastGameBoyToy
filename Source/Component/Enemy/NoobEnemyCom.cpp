#include "NoobEnemyCom.h"
#include "Component/System/TransformCom.h"
#include"Component/Character/CharacterCom.h"
#include"Component/Character/CharaStatusCom.h"
#include "Graphics/Graphics.h"
#include "Component/Renderer/RendererCom.h"
#include "Component\Particle\GPUParticle.h"
#include "Component\Particle\CPUParticle.h"
#include "Component\Collsion\ColliderCom.h"
#include <cmath>

//�R���X�g���N�^
NoobEnemyCom::NoobEnemyCom()
{
}

//�f�X�g���N�^
NoobEnemyCom::~NoobEnemyCom()
{
}

//GUI�`��
void NoobEnemyCom::OnGUI()
{
}

//�X�^�[�g
void NoobEnemyCom::Start()
{
    animationCom = GetGameObject()->GetComponent<AnimationCom>();
    TransitionIdleState();
}

//�X�V����
void NoobEnemyCom::Update(float elapsedTime)
{
    StateUpdate(elapsedTime);

    //�����蔻��
    HitPlayer();
}

//�G�l�~�[����v���C���[�ւ̃x�N�g��
DirectX::XMFLOAT3 NoobEnemyCom::GetEnemyToPlayer()
{
    DirectX::XMVECTOR playerVec = DirectX::XMLoadFloat3(&GameObjectManager::Instance().Find("player")->transform_->GetWorldPosition());
    DirectX::XMVECTOR noobVec = DirectX::XMLoadFloat3(&GetGameObject()->transform_->GetWorldPosition());

    DirectX::XMVECTOR planoobVec = DirectX::XMVector3Normalize(DirectX::XMVectorSubtract(playerVec, noobVec));

    DirectX::XMFLOAT3 targetVec;
    DirectX::XMStoreFloat3(&targetVec, planoobVec);

    return targetVec;
}

//�v���C���[�܂ł̋���
float NoobEnemyCom::GetPlayerDist()
{
    DirectX::XMFLOAT3 playerPos = GameObjectManager::Instance().Find("player")->transform_->GetWorldPosition();
    DirectX::XMFLOAT3 noobPos = GetGameObject()->transform_->GetWorldPosition();
    float vx = playerPos.x - noobPos.x;
    float vy = playerPos.y - noobPos.y;
    float vz = playerPos.z - noobPos.z;

    float dist = sqrtf(vx * vx + vy * vy + vz * vz);

    return dist;
}

//�X�e�[�g�̓���
void NoobEnemyCom::StateUpdate(float elapsedTime)
{
    //�X�e�[�g
    switch (state)
    {
    case State::Idle:
        UpdateIdle(elapsedTime);
        break;
    case State::Purstuit:
        UpdatePursuit(elapsedTime);
        break;
    case State::Death:
        UpdateDeath(elapsedTime);
        break;
    case State::Explosion:
        UpdateExplosion(elapsedTime);
        break;
    }
}

//�����蔻�菈��
void NoobEnemyCom::HitPlayer()
{
    for (const auto& hitobject : GetGameObject()->GetComponent<SphereColliderCom>()->OnHitGameObject())
    {
        if (const auto& hitObj = hitobject.gameObject.lock())
        {
            if (const auto& status = hitObj->GetComponent<CharaStatusCom>())
            {
                status->AddDamagePoint(-10);
            }
        }
    }
}

//�ҋ@�X�e�[�g
void NoobEnemyCom::TransitionIdleState()
{
    state = State::Idle;
    animationCom.lock()->PlayAnimation(animationCom.lock()->FindAnimation("Enemy_run"), true);
    firstIdleTime = static_cast<float>(rand()) / RAND_MAX * 1.5f;
}

//�ǐՃX�e�[�g
void NoobEnemyCom::TransitionPursuit()
{
    state = State::Purstuit;
    animationCom.lock()->PlayAnimation(animationCom.lock()->FindAnimation("Enemy_run"), true);
}

//�����X�e�[�g
void NoobEnemyCom::TransitionExplosion()
{
    state = State::Explosion;
    animationCom.lock()->PlayAnimation(animationCom.lock()->FindAnimation("Enemy_dead"), false);
    GetGameObject()->GetChildFind("accumulateexplosion")->GetComponent<GPUParticle>()->SetLoop(false);

    //�R���C�_�[ON
    GetGameObject()->GetComponent<SphereColliderCom>()->SetEnabled(true);

    //�G�t�F�N�g�Đ�
    const auto& bomber = GetGameObject()->GetChildFind("bomber");
    bomber->GetComponent<GPUParticle>()->SetLoop(true);
    bomber->GetComponent<CPUParticle>()->SetActive(true);
}

//���S�X�e�[�g
void NoobEnemyCom::TransiotnDeath()
{
    state = State::Death;
    animationCom.lock()->PlayAnimation(animationCom.lock()->FindAnimation("Enemy_dead"), false);
}

//�ҋ@�X�e�[�g�X�V����
void NoobEnemyCom::UpdateIdle(float elapsedTime)
{
    firstIdleTimer += elapsedTime;
    //�����_���őҋ@
    if (firstIdleTime < firstIdleTimer)
    {
        TransitionPursuit();
        firstIdleTime = 0.0f;
    }

    //���S�t���O�����Ă�
    if (GetGameObject()->GetComponent<CharaStatusCom>()->IsDeath())
    {
        TransiotnDeath();
    }
}

//�ǐՃX�e�[�g�X�V����
void NoobEnemyCom::UpdatePursuit(float elapedTime)
{
    auto& moveCom = GetGameObject()->GetComponent<MovementCom>();
    DirectX::XMFLOAT3 v = GetEnemyToPlayer() * speed;

    moveCom->AddForce({ v.x, v.y, v.z });

    // t��y������0.0f�ȉ��ɂȂ�Ȃ��悤�ɐݒ�
    DirectX::XMFLOAT3 t = { GetEnemyToPlayer().x, 0.0f, GetEnemyToPlayer().z };
    GetGameObject()->transform_->Turn(t, 0.1f);

    //�����̗��߃p�[�e�B�N���Đ�
    const auto& acumu = GetGameObject()->GetChildFind("accumulateexplosion");
    if (8.5f > GetPlayerDist())
    {
        acumu->GetComponent<GPUParticle>()->SetLoop(true);
    }
    else
    {
        acumu->GetComponent<GPUParticle>()->SetLoop(false);
    }

    //�����X�e�[�g�ɑJ��
    if (explosionDist > GetPlayerDist())
    {
        TransitionExplosion();
    }

    //���S�t���O�����Ă�
    if (GetGameObject()->GetComponent<CharaStatusCom>()->IsDeath())
    {
        TransiotnDeath();
    }
}

//�����X�e�[�g�X�V����
void NoobEnemyCom::UpdateExplosion(float elapsedTime)
{
    //�A�j���[�V�������I����
    if (!animationCom.lock()->IsPlayAnimation())
    {
        time += elapsedTime;
        GetGameObject()->GetComponent<RendererCom>()->SetDissolveThreshold(time);

        //�G�t�F�N�g��~
        const auto& bomber = GetGameObject()->GetChildFind("bomber");
        bomber->GetComponent<GPUParticle>()->SetLoop(false);
        bomber->GetComponent<CPUParticle>()->SetActive(false);

        if (time > 1.0f)
        {
            //��莞�Ԃ��̏�Ɏ~�܂��Ă��甚��
            GameObjectManager::Instance().Remove(this->GetGameObject());
        }
    }
}

//���S�X�e�[�g
void NoobEnemyCom::UpdateDeath(float elapsedTime)
{
    //�A�j���[�V�������I���΍Đ�
    if (!animationCom.lock()->IsPlayAnimation())
    {
        time += elapsedTime;
        GetGameObject()->GetComponent<RendererCom>()->SetDissolveThreshold(time);

        if (time > 1.0f)
        {
            //��莞�Ԃ��̏�Ɏ~�܂��Ă��甚��
            GameObjectManager::Instance().Remove(this->GetGameObject());
        }
    }
}
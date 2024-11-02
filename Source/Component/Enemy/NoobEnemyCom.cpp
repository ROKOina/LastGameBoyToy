#include "NoobEnemyCom.h"
#include "Component/System/TransformCom.h"
#include"Component/Character/CharacterCom.h"
#include"Component/Character/CharaStatusCom.h"
#include "Graphics/Graphics.h"
#include "Component/Renderer/RendererCom.h"
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
    case State::Explosion:
        UpdateExplosion(elapsedTime);
        break;
    }
}

//�ҋ@�X�e�[�g
void NoobEnemyCom::TransitionIdleState()
{
    state = State::Idle;
    animationCom.lock()->PlayAnimation(animationCom.lock()->FindAnimation("Idle"), true);
    firstIdleTime = static_cast<float>(rand()) / RAND_MAX * 1.5f;
}

//�ǐՃX�e�[�g
void NoobEnemyCom::TransitionPursuit()
{
    state = State::Purstuit;
    animationCom.lock()->PlayAnimation(animationCom.lock()->FindAnimation("Running"), true);
}

//�����X�e�[�g
void NoobEnemyCom::TransitionExplosion()
{
    state = State::Explosion;
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
}

//�ǐՃX�e�[�g�X�V����
void NoobEnemyCom::UpdatePursuit(float elapedTime)
{
    auto& moveCom = GetGameObject()->GetComponent<MovementCom>();
    DirectX::XMFLOAT3 v = GetEnemyToPlayer() * speed;
    moveCom->AddForce({ v.x,0.0f,v.z });
    DirectX::XMFLOAT3 t = { GetEnemyToPlayer().x,0.0f,GetEnemyToPlayer().z };
    GetGameObject()->transform_->Turn(t, 0.1f);

    if (explosionDist > GetPlayerDist())
    {
        TransitionExplosion();
    }
}

//�����X�e�[�g�X�V����
void NoobEnemyCom::UpdateExplosion(float elapsedTime)
{
    explosionGrace -= elapsedTime;
    if (0 > explosionGrace)
    {
        //��莞�Ԃ��̏�Ɏ~�܂��Ă��甚��
        GameObjectManager::Instance().Remove(this->GetGameObject());
    }
}
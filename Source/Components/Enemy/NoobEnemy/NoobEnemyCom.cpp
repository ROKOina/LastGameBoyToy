#include "NoobEnemyCom.h"
#include "Components/TransformCom.h"
#include"../../Character/CharacterCom.h"
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
    TransitionPursuit();
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
}

//�ǐՃX�e�[�g
void NoobEnemyCom::TransitionPursuit()
{
    state = State::Purstuit;
}

//�����X�e�[�g
void NoobEnemyCom::TransitionExplosion()
{
    state = State::Explosion;
}

//�ҋ@�X�e�[�g�X�V����
void NoobEnemyCom::UpdateIdle(float elapsedTime)
{
    //���͉������Ȃ�
}

//�ǐՃX�e�[�g�X�V����
void NoobEnemyCom::UpdatePursuit(float elapedTime)
{
    auto& moveCom = GetGameObject()->GetComponent<MovementCom>();
    DirectX::XMFLOAT3 v = GetEnemyToPlayer() * speed;
    moveCom->AddForce(v);
    GetGameObject()->transform_->Turn(GetEnemyToPlayer(), 0.1f);

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
        if (explosionRange > GetPlayerDist())
        {
            GameObjectManager::Instance().Find("player")->GetComponent<CharacterCom>()->AddDamagePoint(explosionDamage);
            GameObjectManager::Instance().Remove(this->GetGameObject());
        }
    }
}
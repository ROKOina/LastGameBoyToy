#include "NoobEnemyCom.h"
#include "Component/System/TransformCom.h"
#include"Component/Character/CharacterCom.h"
#include"Component/Character/CharaStatusCom.h"
#include "Graphics/Graphics.h"
#include "Component/Renderer/RendererCom.h"
#include <cmath>

//コンストラクタ
NoobEnemyCom::NoobEnemyCom()
{
}

//デストラクタ
NoobEnemyCom::~NoobEnemyCom()
{
}

//GUI描画
void NoobEnemyCom::OnGUI()
{
}

//スタート
void NoobEnemyCom::Start()
{
    animationCom = GetGameObject()->GetComponent<AnimationCom>();
    TransitionIdleState();
}

//更新処理
void NoobEnemyCom::Update(float elapsedTime)
{
    StateUpdate(elapsedTime);
}

//エネミーからプレイヤーへのベクトル
DirectX::XMFLOAT3 NoobEnemyCom::GetEnemyToPlayer()
{
    DirectX::XMVECTOR playerVec = DirectX::XMLoadFloat3(&GameObjectManager::Instance().Find("player")->transform_->GetWorldPosition());
    DirectX::XMVECTOR noobVec = DirectX::XMLoadFloat3(&GetGameObject()->transform_->GetWorldPosition());

    DirectX::XMVECTOR planoobVec = DirectX::XMVector3Normalize(DirectX::XMVectorSubtract(playerVec, noobVec));

    DirectX::XMFLOAT3 targetVec;
    DirectX::XMStoreFloat3(&targetVec, planoobVec);

    return targetVec;
}

//プレイヤーまでの距離
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

//ステートの統括
void NoobEnemyCom::StateUpdate(float elapsedTime)
{
    //ステート
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

//待機ステート
void NoobEnemyCom::TransitionIdleState()
{
    state = State::Idle;
    animationCom.lock()->PlayAnimation(animationCom.lock()->FindAnimation("Idle"), true);
    firstIdleTime = static_cast<float>(rand()) / RAND_MAX * 1.5f;
}

//追跡ステート
void NoobEnemyCom::TransitionPursuit()
{
    state = State::Purstuit;
    animationCom.lock()->PlayAnimation(animationCom.lock()->FindAnimation("Running"), true);
}

//爆発ステート
void NoobEnemyCom::TransitionExplosion()
{
    state = State::Explosion;
}

//待機ステート更新処理
void NoobEnemyCom::UpdateIdle(float elapsedTime)
{
    firstIdleTimer += elapsedTime;
    //ランダムで待機
    if (firstIdleTime < firstIdleTimer)
    {
        TransitionPursuit();
        firstIdleTime = 0.0f;
    }
}

//追跡ステート更新処理
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

//爆発ステート更新処理
void NoobEnemyCom::UpdateExplosion(float elapsedTime)
{
    explosionGrace -= elapsedTime;
    if (0 > explosionGrace)
    {
        //一定時間その場に止まってから爆発
        GameObjectManager::Instance().Remove(this->GetGameObject());
    }
}
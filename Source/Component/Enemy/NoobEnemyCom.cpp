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

    //当たり判定
    HitPlayer();
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
    case State::Death:
        UpdateDeath(elapsedTime);
        break;
    case State::Explosion:
        UpdateExplosion(elapsedTime);
        break;
    }
}

//当たり判定処理
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

//待機ステート
void NoobEnemyCom::TransitionIdleState()
{
    state = State::Idle;
    animationCom.lock()->PlayAnimation(animationCom.lock()->FindAnimation("Enemy_run"), true);
    firstIdleTime = static_cast<float>(rand()) / RAND_MAX * 1.5f;
}

//追跡ステート
void NoobEnemyCom::TransitionPursuit()
{
    state = State::Purstuit;
    animationCom.lock()->PlayAnimation(animationCom.lock()->FindAnimation("Enemy_run"), true);
}

//爆発ステート
void NoobEnemyCom::TransitionExplosion()
{
    state = State::Explosion;
    animationCom.lock()->PlayAnimation(animationCom.lock()->FindAnimation("Enemy_dead"), false);
    GetGameObject()->GetChildFind("accumulateexplosion")->GetComponent<GPUParticle>()->SetLoop(false);

    //コライダーON
    GetGameObject()->GetComponent<SphereColliderCom>()->SetEnabled(true);

    //エフェクト再生
    const auto& bomber = GetGameObject()->GetChildFind("bomber");
    bomber->GetComponent<GPUParticle>()->SetLoop(true);
    bomber->GetComponent<CPUParticle>()->SetActive(true);
}

//死亡ステート
void NoobEnemyCom::TransiotnDeath()
{
    state = State::Death;
    animationCom.lock()->PlayAnimation(animationCom.lock()->FindAnimation("Enemy_dead"), false);
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

    //死亡フラグが立てば
    if (GetGameObject()->GetComponent<CharaStatusCom>()->IsDeath())
    {
        TransiotnDeath();
    }
}

//追跡ステート更新処理
void NoobEnemyCom::UpdatePursuit(float elapedTime)
{
    auto& moveCom = GetGameObject()->GetComponent<MovementCom>();
    DirectX::XMFLOAT3 v = GetEnemyToPlayer() * speed;

    moveCom->AddForce({ v.x, v.y, v.z });

    // tのy成分も0.0f以下にならないように設定
    DirectX::XMFLOAT3 t = { GetEnemyToPlayer().x, 0.0f, GetEnemyToPlayer().z };
    GetGameObject()->transform_->Turn(t, 0.1f);

    //爆発の溜めパーティクル再生
    const auto& acumu = GetGameObject()->GetChildFind("accumulateexplosion");
    if (8.5f > GetPlayerDist())
    {
        acumu->GetComponent<GPUParticle>()->SetLoop(true);
    }
    else
    {
        acumu->GetComponent<GPUParticle>()->SetLoop(false);
    }

    //爆発ステートに遷移
    if (explosionDist > GetPlayerDist())
    {
        TransitionExplosion();
    }

    //死亡フラグが立てば
    if (GetGameObject()->GetComponent<CharaStatusCom>()->IsDeath())
    {
        TransiotnDeath();
    }
}

//爆発ステート更新処理
void NoobEnemyCom::UpdateExplosion(float elapsedTime)
{
    //アニメーションが終われば
    if (!animationCom.lock()->IsPlayAnimation())
    {
        time += elapsedTime;
        GetGameObject()->GetComponent<RendererCom>()->SetDissolveThreshold(time);

        //エフェクト停止
        const auto& bomber = GetGameObject()->GetChildFind("bomber");
        bomber->GetComponent<GPUParticle>()->SetLoop(false);
        bomber->GetComponent<CPUParticle>()->SetActive(false);

        if (time > 1.0f)
        {
            //一定時間その場に止まってから爆発
            GameObjectManager::Instance().Remove(this->GetGameObject());
        }
    }
}

//死亡ステート
void NoobEnemyCom::UpdateDeath(float elapsedTime)
{
    //アニメーションが終われば再生
    if (!animationCom.lock()->IsPlayAnimation())
    {
        time += elapsedTime;
        GetGameObject()->GetComponent<RendererCom>()->SetDissolveThreshold(time);

        if (time > 1.0f)
        {
            //一定時間その場に止まってから爆発
            GameObjectManager::Instance().Remove(this->GetGameObject());
        }
    }
}
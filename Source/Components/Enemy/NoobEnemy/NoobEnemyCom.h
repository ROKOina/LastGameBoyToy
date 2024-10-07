#pragma once
#include "../../System/Component.h"
#include "../../System\StateMachine.h"
#include "GameSource\Scene\SceneManager.h"
#include "GameSource\Math\Mathf.h"
#include "Input\Input.h"
#include "../../AnimationCom.h"
#include <array>

#include "../../MovementCom.h"

#define GetComp(Component) owner->GetGameObject()->GetComponent<Component>();

class NoobEnemyCom : public Component
{
public:
    NoobEnemyCom();
    ~NoobEnemyCom() override;

    //名前取得
    const char* GetName() const override { return "NoobEnemy"; }

    //GUI描画
    void OnGUI()override;

    //開始処理
    void Start()override;

    //更新処理
    void Update(float elapsedTime)override;

public:

    //エネミーからプレイヤーへのベクトル
    DirectX::XMFLOAT3 GetEnemyToPlayer();

    //プレイヤーまでの距離
    float GetPlayerDist();

private:

    //ステートの統括
    void StateUpdate(float elapsedTime);

private:
    //ステート関連

    //遷移

    //待機ステート
    void TransitionIdleState();
    //追跡ステート
    void TransitionPursuit();
    //爆発ステート
    void TransitionExplosion();

    //更新

    //待機ステート更新処理
    void UpdateIdle(float elapsedTime);
    //追跡ステート更新処理
    void UpdatePursuit(float elpasedTime);
    //爆発ステート更新処理
    void UpdateExplosion(float elapsedTime);

private:

    //雑魚敵のステート
    enum class State
    {
        Idle,
        Purstuit,
        Explosion
    };

    State state = State::Idle;

private:
    //雑魚敵のパラメーター

    //移動スピード
    float speed = 0.2f;
    //爆発するまでの距離
    float explosionDist = 1.0f;
    //爆発範囲
    float explosionRange = 1.5f;
    //爆発で与えるダメージ
    float explosionDamage = 10.0f;
    //爆発猶予
    float explosionGrace = 1.0f;
};

#pragma once
#include "StateMachine\StateMachine.h"
#include "Scene\SceneManager.h"
#include "Component/Animation/AnimationCom.h"
#include "Component\MoveSystem\MovementCom.h"
#include "Audio\Audio3D.h"

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

    //当たり判定処理
    void HitPlayer();

private:
    //ステート関連

    //遷移

    //待機ステート
    void TransitionIdleState();
    //追跡ステート
    void TransitionPursuit();
    //爆発ステート
    void TransitionExplosion();
    //死亡ステート
    void TransiotnDeath();

    //更新

    //待機ステート更新処理
    void UpdateIdle(float elapsedTime);
    //追跡ステート更新処理
    void UpdatePursuit(float elpasedTime);
    //爆発ステート更新処理
    void UpdateExplosion(float elapsedTime);
    //死亡ステート
    void UpdateDeath(float elapsedTime);

private:

    //雑魚敵のステート
    enum class State
    {
        Idle,
        Purstuit,
        Death,
        Explosion
    };

    State state = State::Idle;

private:

    std::weak_ptr<AnimationCom> animationCom;

    //雑魚敵のパラメーター

    //移動スピード
    float speed = 0.4f;
    //爆発するまでの距離
    float explosionDist = 3.5f;
    //最初の待機時間
    float firstIdleTime = 0.0f;
    //待機時間を進める時間
    float firstIdleTimer = 0.0f;
    //削除時間
    float time = {};

private://音
    std::map<AUDIOID3D, GameObj> audios;
    GameObj boss;
public:
    void SetAudios(AUDIOID3D id, GameObj obj) {
        GameObj SeObj = obj->AddChildObject();
        std::shared_ptr<AudioSource3D> Se = SeObj->AddComponent<AudioSource3D>(id);
        Se->SetCurveDistanceScaler(70.0f);
        audios[id] = SeObj;
    }
    AudioSource3D* GetAudios(AUDIOID3D id) { return audios[id]->GetComponent<AudioSource3D>().get(); }
};
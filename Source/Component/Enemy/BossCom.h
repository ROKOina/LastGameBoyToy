#pragma once

#include "Component/System/Component.h"
#include "StateMachine\Behaviar\BossState.h"
#include "StateMachine/StateMachine.h"

class BossCom :public Component
{
public:

    BossCom() {};
    ~BossCom() {};

    //初期設定
    void Start()override;

    //更新処理
    void Update(float elapsedTime)override;

    //imgui
    void OnGUI()override;

    //名前設定
    const char* GetName() const override { return "Boss"; }

public:

    //索敵関数
    bool Search(float range);

    // 目標地点へ移動
    void MoveToTarget(float movespeed, float turnspeed);

    //ジャンプ
    void Jump(float power);

private:

    //後ろにいる場合旋回する
    void BackTurn();

    //ダメージ判定
    void DamegaEvent(float elapsedTime);

private:

    // ターゲット方向への進行ベクトルを算出
    DirectX::XMFLOAT2 TargetVec();

public:

    //ステート
    enum class BossState
    {
        IDLE,
        IDLESTOP,
        MOVE,
        SHORTATTACK1,
        SHORTATTACK2,
        LARIATSTART,
        LARIATLOOP,
        LARIATEND,
        JUMPATTACKSTART,
        JUMPATTACKEND,
        SHOTSTART,
        SHOTLOOP,
        SHOTEND,
        UPSHOTSTART,
        UPSHOTCHARGE,
        UPSHOTLOOP,
        UPSHOTEND,

        EVENT_WALK, //イベント用ステート
        EVENT_PUNCH,
        EVENT_DEATH,

        MAX
    };

public:

    StateMachine<BossCom, BossState>& GetStateMachine() { return state; }

    float meleerange = 9.0f;
    float longrange = 45.0f;
    float walkrange = 45.0f;

private:
    StateMachine<BossCom, BossState> state;
    DirectX::XMFLOAT3 targetposition = {};
};
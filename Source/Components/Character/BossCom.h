#pragma once

#include "Components/System/Component.h"
#include "State/BossState.h"
#include "../System\StateMachine.h"

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

    //乱数計算
    int ComputeRandom();

    //ジャンプ
    void Jump(float power);

private:

    // ターゲット方向への進行ベクトルを算出
    DirectX::XMFLOAT2 TargetVec();

public:

    //ステート
    enum class BossState
    {
        IDLE,
        MOVE,
        JUMP,
        JUMPLOOP,
        LANDINGATTACK,
        ATTACK,
        RANGEATTACK,
        BOMPATTTACK,
        DEATH,
        HIT,
        MAX
    };

public:

    StateMachine<BossCom, BossState>& GetStateMachine() { return state; }

private:
    StateMachine<BossCom, BossState> state;
    DirectX::XMFLOAT3 targetposition = {};
    std::vector<int> availableNumbers = { 1, 2, 3 };
};
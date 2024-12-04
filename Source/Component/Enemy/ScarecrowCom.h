#pragma once
#include "Component/System/Component.h"
#include "StateMachine\Behaviar\TrainingScarecrowState.h"
#include "StateMachine/StateMachine.h"

class ScarecrowCom :public Component
{
public:

    ScarecrowCom() {};
    ~ScarecrowCom() {};

    //初期設定
    void Start()override;

    //更新処理
    void Update(float elapsedTime)override;

    //IMGUI
    void OnGUI()override;

    //名前設定
    const char* GetName() const override { return "Crow"; }

public:
    //モードのゲッター
    int GetCrowMode() { return scareCrowMode; }
    void SetCrowMode(int Index) { scareCrowMode = Index; }
public:

    //ステート
    enum class ScareCrowState
    {
        IDLE,
       
        RANDOMIDLE,

        MOVE,

        DEATH,

        MAX
    };

public:

    StateMachine<ScarecrowCom, ScareCrowState>& GetStateMachine() { return state; }

private:

    StateMachine<ScarecrowCom, ScareCrowState> state;

    //案山子のモード
    //0=棒立ち　1＝ランダム移動　
    int scareCrowMode = 1;

};
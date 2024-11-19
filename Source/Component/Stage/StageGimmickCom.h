#pragma once

#include "Component\System\Component.h"
#include "StateMachine/StateMachine.h"

class StageGimmick : public Component
{
public:

    StageGimmick() {};
    ~StageGimmick() {};

    //初期設定
    void Start()override;

    //更新処理
    void Update(float elapsedTime)override;

    //imgui
    void OnGUI()override;

    //名前設定
    const char* GetName() const override { return "StageGimmick"; }

public:

    //ステート
    enum class GimmickState
    {
        IDLE,
        ENEMYSPAWN,
        BIGATTACK,
        BREAK,
        MAX
    };

    StateMachine<StageGimmick, GimmickState>& GetStateMachine() { return state; }

private:

    StateMachine<StageGimmick, GimmickState> state;
};
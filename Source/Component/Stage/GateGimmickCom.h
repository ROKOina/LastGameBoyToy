#pragma once
#include "Component\System\Component.h"
#include "StateMachine/StateMachine.h"

class GateGimmick : public Component
{
public:

    GateGimmick() {};
    ~GateGimmick() {};

    //初期設定
    void Start()override;

    //更新処理
    void Update(float elapsedTime)override;

    //imgui
    void OnGUI()override {};

    //名前設定
    const char* GetName() const override { return "GateGimmick"; }

public:

    //ステート
    enum class GimmickState
    {
        IDLE,
        DOWN,
        UP,
        MAX
    };
    StateMachine<GateGimmick, GimmickState>& GetStateMachine() { return state; }

    DirectX::XMFLOAT3 GetUpPos() { return upPos; }
    void SetUpPos(DirectX::XMFLOAT3 pos) { upPos = pos; }
    DirectX::XMFLOAT3 GetDownPos() { return downPos; }
    void SetDownPos(DirectX::XMFLOAT3 pos) { downPos = pos; }
    float GetMoveSpeed() { return moveSpeed; }

private:
    StateMachine<GateGimmick, GimmickState> state;
    DirectX::XMFLOAT3 upPos = {};
    DirectX::XMFLOAT3 downPos = {};

    float moveSpeed = 1.0f;
    bool downInit = false;
};

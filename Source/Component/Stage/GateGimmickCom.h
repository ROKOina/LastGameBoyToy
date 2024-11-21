#pragma once
#include "Component\System\Component.h"
#include "StateMachine/StateMachine.h"

class GateGimmick : public Component
{
public:

    GateGimmick() {};
    ~GateGimmick() {};

    //�����ݒ�
    void Start()override;

    //�X�V����
    void Update(float elapsedTime)override;

    //imgui
    void OnGUI()override {};

    //���O�ݒ�
    const char* GetName() const override { return "GateGimmick"; }

public:

    //�X�e�[�g
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

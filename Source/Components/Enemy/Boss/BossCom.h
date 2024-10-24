#pragma once

#include "Components/System/Component.h"
#include "Components/Character/State/BossState.h"
#include "Components/System/StateMachine.h"

class BossCom :public Component
{
public:

    BossCom() {};
    ~BossCom() {};

    //�����ݒ�
    void Start()override;

    //�X�V����
    void Update(float elapsedTime)override;

    //imgui
    void OnGUI()override;

    //���O�ݒ�
    const char* GetName() const override { return "Boss"; }

public:

    //���G�֐�
    bool Search(float range);

    // �ڕW�n�_�ֈړ�
    void MoveToTarget(float movespeed, float turnspeed);

    //�W�����v
    void Jump(float power);

private:

    // �^�[�Q�b�g�����ւ̐i�s�x�N�g�����Z�o
    DirectX::XMFLOAT2 TargetVec();

public:

    //�X�e�[�g
    enum class BossState
    {
        IDLE,
        STOPTIME,
        MOVE,
        JUMP,
        JUMPLOOP,
        LANDINGATTACK,
        PUNCH,
        KICK,
        RANGEATTACK,
        BOMPATTTACK,
        FIREBALL,
        MISSILE,
        HIT,
        DEATH,

        MAX
    };

public:

    StateMachine<BossCom, BossState>& GetStateMachine() { return state; }

private:
    StateMachine<BossCom, BossState> state;
    DirectX::XMFLOAT3 targetposition = {};
    DirectX::XMFLOAT3 righthandnodepos = {};
};
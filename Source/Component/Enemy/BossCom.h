#pragma once

#include "Component/System/Component.h"
#include "StateMachine\Behaviar\BossState.h"
#include "StateMachine/StateMachine.h"

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

    //���ɂ���ꍇ���񂷂�
    void BackTurn();

    //�_���[�W����
    void DamegaEvent(float elapsedTime);

private:

    // �^�[�Q�b�g�����ւ̐i�s�x�N�g�����Z�o
    DirectX::XMFLOAT2 TargetVec();

public:

    //�X�e�[�g
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

        EVENT_WALK, //�C�x���g�p�X�e�[�g
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
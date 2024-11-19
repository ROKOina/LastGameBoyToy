#pragma once

#include "Component\System\Component.h"
#include "StateMachine/StateMachine.h"

class StageGimmick : public Component
{
public:

    StageGimmick() {};
    ~StageGimmick() {};

    //�����ݒ�
    void Start()override;

    //�X�V����
    void Update(float elapsedTime)override;

    //imgui
    void OnGUI()override;

    //���O�ݒ�
    const char* GetName() const override { return "StageGimmick"; }

public:

    //�X�e�[�g
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
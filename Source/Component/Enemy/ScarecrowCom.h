#pragma once
#include "Component/System/Component.h"
#include "StateMachine\Behaviar\TrainingScarecrowState.h"
#include "StateMachine/StateMachine.h"

class ScarecrowCom :public Component
{
public:

    ScarecrowCom() {};
    ~ScarecrowCom() {};

    //�����ݒ�
    void Start()override;

    //�X�V����
    void Update(float elapsedTime)override;

    //IMGUI
    void OnGUI()override;

    //���O�ݒ�
    const char* GetName() const override { return "Crow"; }

public:
    //���[�h�̃Q�b�^�[
    int GetCrowMode() { return scareCrowMode; }
    void SetCrowMode(int Index) { scareCrowMode = Index; }
public:

    //�X�e�[�g
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

    //�ĎR�q�̃��[�h
    //0=�_�����@1�������_���ړ��@
    int scareCrowMode = 1;

};
#pragma once

#include "Components/System/Component.h"

class HitProcessCom : public Component
{
public:
    HitProcessCom() :hitIntervalTimer(0){}
    ~HitProcessCom() override {}

    // ���O�擾
    const char* GetName() const override { return "HitProcess"; }

    // �J�n����
    void Start() override {}

    // �X�V����
    void Update(float elapsedTime) override;

    // GUI�`��
    void OnGUI() override;

    enum class HIT_TYPE
    {
        DAMAGE,
        HEAL,
    };
    void SetHitType(HitProcessCom::HIT_TYPE type) { hitType = type; }

private:
    void HitProcess(int myID, int hitID);

private:
    //���
    HIT_TYPE hitType = HIT_TYPE::DAMAGE;

    //�q�b�g�Ԋu
    float hitIntervalTime = 0.1f;
    float hitIntervalTimer;

    //�^���鐔
    int value = 1;

};
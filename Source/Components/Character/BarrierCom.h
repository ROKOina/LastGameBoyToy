#pragma once

#include "../System\Component.h"

class BarrierCom : public Component
{
public:
    BarrierCom() {};
    ~BarrierCom() override {};

    // ���O�擾
    const char* GetName() const override { return "Barrier"; }

    // �J�n����
    void Start() override {};

    // �X�V����
    void Update(float elapsedTime) override;

    // GUI�`��
    void OnGUI() override {};

private:

    float barriertime = 0.0f;
};
#pragma once

#include "../System\Component.h"

class KnockBackCom : public Component
{
public:
    KnockBackCom() {};
    ~KnockBackCom() override {};

    // ���O�擾
    const char* GetName() const override { return "KnockBack"; }

    // �J�n����
    void Start() override {};

    // �X�V����
    void Update(float elapsedTime) override;

    // GUI�`��
    void OnGUI() override {};
};
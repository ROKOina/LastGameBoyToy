#pragma once

#include "../System\Component.h"
#include "Math\Mathf.h"
#include "Component\System\HitProcessCom.h"

class JankratUltCom : public Component
{
public:
    JankratUltCom() {};
    ~JankratUltCom() override {};

    // ���O�擾
    const char* GetName() const override { return "JankratUlt"; }

    // �J�n����
    void Start() override {};

    // �X�V����
    void Update(float elapsedTime) override;

    // GUI�`��
    void OnGUI() override {};

private:

    //�N��
    void Fire(float elapsedTime);

private:

    float time = 0.0f;
};
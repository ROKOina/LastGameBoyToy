#pragma once
#include "UiSystem.h"

class UiGauge : public UiSystem
{
    //�R���|�[�l���g�I�[�o�[���C�h
public:
    UiGauge(const char* filename, SpriteShader spriteshader, bool collsion);
    ~UiGauge() {}

    // ���O�擾
    const char* GetName() const override { return "UiGauge"; }

    // �J�n����
    void Start() override {};

    // �X�V����
    void Update(float elapsedTime) override;

private:
    DirectX::XMFLOAT2 originalTexSize = {};
};

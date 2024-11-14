#pragma once
#include "Component\UI\UiSystem.h"

class UI_Skill : public UiSystem
{
    //�R���|�[�l���g�I�[�o�[���C�h
public:
    UI_Skill(const char* filename, SpriteShader spriteshader, bool collsion, float min, float max);
    ~UI_Skill() {}

    // ���O�擾
    const char* GetName() const override { return "UiFlag"; }

    // �J�n����
    void Start() override;

    // �X�V����
    void Update(float elapsedTime) override;

private:
    float changePosValue;
    DirectX::XMFLOAT2 originalPos = {};
};

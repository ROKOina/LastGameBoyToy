#pragma once
#include "Components\UI\UiSystem.h"

class UiFlag : public UiSystem
{
    //�R���|�[�l���g�I�[�o�[���C�h
public:
    UiFlag(const char* filename, SpriteShader spriteshader, bool collsion, bool* flag);
    ~UiFlag() {}

    // ���O�擾
    const char* GetName() const override { return "UiFlag"; }

    // �J�n����
    void Start() override {};

    // �X�V����
    void Update(float elapsedTime) override;

    void OnGUI()override;

private:
    bool* flag; //�Q�Ƃ���t���O
};

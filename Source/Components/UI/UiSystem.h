#pragma once
#include "Graphics\Sprite\Sprite.h"
#include "math.h"
class UiSystem : public Sprite
{
    //�R���|�[�l���g�I�[�o�[���C�h
public:
    UiSystem(const char* filename, SpriteShader spriteshader, bool collsion);
    ~UiSystem() {}

    // ���O�擾
    const char* GetName() const override { return "UiSystem"; }

    // �J�n����
    void Start() override {};

    // �X�V����
    void Update(float elapsedTime) override;

    //�`��
    void Render(const DirectX::XMFLOAT4X4& view, const DirectX::XMFLOAT4X4& projection);

    // GUI�`��
    void OnGUI() override;

    //�Q�ƒl,�ő�l�ݒ�
    void SetVariableValue(float* value) { variableValue = value; }
    void SetMaxValue(float value) { maxValue = value; };

protected:
    float* variableValue;     //�Q�Ƃ���l
    float  maxValue;          //�ő�l
    float  valueRate;        //����
};

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


    void FadeIn(float elapsedTime);
    void FadeOut(float elapsedTime);


    //�Z�b�^�[,�Q�b�^�[
    void SetVariableValue(float* value) { variableValue = value; }
    void SetMaxValue(float value) { maxValue = value; }
    void SetTImer(float value) { timer = value; }
    void SetFadeTimer(float value) { fadeTimer = value; originalFadeTime = value; }
    void SetFadeInFlag(bool flag) { fadeInFlag = flag;}
    void SetFadeOutFlag(bool flag) { fadeOutFlag = flag;}


protected:
    float* variableValue;    //�Q�Ƃ���l
    float  maxValue;         //�ő�l
    float  valueRate;        //����

    float timer;            //�ėp�^�C�}�[

    float fadeTimer;        //�t�F�[�h�p�^�C�}�[
    float originalFadeTime; //�ő�l��ێ����Ă����ϐ�
    bool  fadeInFlag;       //�t�F�[�h�C���p�t���O
    bool  fadeOutFlag;       //�t�F�[�h�A�E�g�p�t���O


    DirectX::XMFLOAT2 originalTexSize = {};    //���̃e�N�X�`���T�C�Y
};

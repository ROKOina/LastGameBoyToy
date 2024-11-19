#pragma once
#include "Component\UI\UiSystem.h"
#include "Component\UI\UiGauge.h"
class UI_Skill : public UiSystem
{
    //�R���|�[�l���g�I�[�o�[���C�h
public:
    UI_Skill(const char* filename, SpriteShader spriteshader, bool collsion, float min, float max);
    ~UI_Skill() {}

    // ���O�擾
    const char* GetName() const override { return "Ui_Skill"; }

    // �J�n����
    void Start() override;

    // �X�V����
    void Update(float elapsedTime) override;


private:
    float changePosValue;
    DirectX::XMFLOAT2 originalPos = {};
};

class UI_BoosGauge : public Component
{
    //�R���|�[�l���g�I�[�o�[���C�h
public:
    UI_BoosGauge(int num);
    ~UI_BoosGauge() {}

    // ���O�擾
    const char* GetName() const override { return "UI_BoostGauge"; }

    // �J�n����
    void Start() override ;

    // �X�V����
    void Update(float elapsedTime) override;


private:
    std::vector<std::shared_ptr<GameObject>>   gauges;
    std::vector<std::shared_ptr<GameObject>>   frames;
    int  num;
    float maxDashGauge;                  //�_�b�V���Q�[�W�̑��ʂ�ێ�
    float* value;                       //�_�b�V���Q�[�W�̌��݂̒l
    float separateValue;                //��؂�̒l
    DirectX::XMFLOAT2 originlTexSize;   //���̃e�N�X�`���[�T�C�Y
    std::weak_ptr<GameObject> player;
};
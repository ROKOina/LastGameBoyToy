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
    DirectX::XMFLOAT2 maxPos = {};
};

//class UI_SkillManager : public Component
//{
//    //�R���|�[�l���g�I�[�o�[���C�h
//public:
//    UI_SkillManager(int num);
//    ~UI_SkillManager() {}
//
//    // ���O�擾
//    const char* GetName() const override { return "UI_BoostGauge"; }
//
//    // �J�n����
//    void Start() override;
//
//    // �X�V����
//    void Update(float elapsedTime) override;
//
//private:
//};

class UI_BoosGauge : public Component
{
    //�R���|�[�l���g�I�[�o�[���C�h
public:
    UI_BoosGauge(int num);
    ~UI_BoosGauge() {}

    // ���O�擾
    const char* GetName() const override { return "UI_BoostGauge"; }

    // �J�n����
    void Start() override;

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

class UI_LockOn : public Component
{
    //�R���|�[�l���g�I�[�o�[���C�h
public:
    UI_LockOn(int num, float min, float max);
    ~UI_LockOn() {}

    // ���O�擾
    const char* GetName() const override { return "UI_BoostGauge"; }

    // �J�n����
    void Start() override;

    // �X�V����
    void Update(float elapsedTime) override;

    void OnGUI()override;

    //�����̐�̃I�u�W�F�N�g�����
    std::shared_ptr<GameObject> SearchObjct();

    void UpdateGauge(float elapsedTime, std::shared_ptr<GameObject> obj);

    void LockIn(float elapsedTime);
    void LockOut(float elapsedTime);

private:
    std::vector<std::shared_ptr<GameObject>> reacters;

    std::weak_ptr<GameObject> camera;   //�J����

    std::shared_ptr<GameObject> lockOn;
    std::shared_ptr<GameObject> lockOn2;
    std::shared_ptr<GameObject> gaugeFrame;
    std::shared_ptr<GameObject> gaugeFrame2;
    std::shared_ptr<GameObject> gauge;
    std::shared_ptr<GameObject> gaugeMask;

    std::shared_ptr<UiSystem> lockOnUi;         //lockOn
    std::shared_ptr<UiSystem> lockOn2Ui;         //lockOn
    std::shared_ptr<UiSystem> gaugeFrameUi;    //GaugeFrame
    std::shared_ptr<UiSystem> gaugeUi;        //Gauge
    std::shared_ptr<UiSystem> gaugeMaskUi;    //GaugeMask

    float minAngle = 0.0f;
    float maxAngle = 0.0f;
    std::vector<float> similarity;
};

class UI_E_SkillCount : public Component
{
    //�R���|�[�l���g�I�[�o�[���C�h
public:
    UI_E_SkillCount(int num);
    ~UI_E_SkillCount() {}

    // ���O�擾
    const char* GetName() const override { return "UI_BoostGauge"; }

    // �J�n����
    void Start() override;

    // �X�V����
    void Update(float elapsedTime) override;
    void UpdateGauge(float elapsedTime);
    void UpdateCore(float elapsedTime);

    void OnGUI()override;

private:
    struct SkillCore {
        std::shared_ptr<UiSystem> coreFrameUi;
        std::shared_ptr<UiSystem> coreUi;
    };
    std::vector<SkillCore> coresUi;
    std::shared_ptr<UiSystem> gaugeUi;
    std::shared_ptr<UiSystem> gaugeFrameUi;

    std::vector<std::shared_ptr<GameObject>> cores;
    std::vector<std::shared_ptr<GameObject>> coreFrames;
    std::shared_ptr<GameObject> gauge;
    std::shared_ptr<GameObject> gaugeFrame;

    int num = 0;
    int* arrowCount = 0;
    float* skillTimer = nullptr;
    float skillTime = 3.0f;
    float spacing = 30.0f;
    bool* isShot;
    DirectX::XMFLOAT2 originalTexSize = {};
    std::weak_ptr<GameObject> player;
};

class UI_Ult_Count : public Component
{
    //�R���|�[�l���g�I�[�o�[���C�h
public:
    UI_Ult_Count(int num);
    ~UI_Ult_Count() {}

    // ���O�擾
    const char* GetName() const override { return "UI_BoostGauge"; }

    // �J�n����
    void Start() override;

    // �X�V����
    void Update(float elapsedTime) override;
    void UpdateCore(float elapsedTime);

private:
    struct SkillCore {
        std::shared_ptr<UiSystem> coreFrameUi;
        std::shared_ptr<UiSystem> coreUi;
    };
    std::vector<SkillCore> coresUi;

    std::vector<std::shared_ptr<GameObject>> cores;
    std::vector<std::shared_ptr<GameObject>> coreFrames;

    int num = 0;
    int* ultCount;
    bool* isUlt;
    float spacing = 30.0f;
    std::weak_ptr<GameObject> player;
};

class UI_Reticle : public Component
{
    //�R���|�[�l���g�I�[�o�[���C�h
public:
    UI_Reticle();
    ~UI_Reticle() {}

    // ���O�擾
    const char* GetName() const override { return "UI_BoostGauge"; }

    // �J�n����
    void Start() override;

    // �X�V����
    void Update(float elapsedTime) override;

private:
    std::weak_ptr<GameObject> player;

    std::shared_ptr<GameObject> reticleFrame;
    std::shared_ptr<GameObject> reticleCircle;

    std::shared_ptr<UiSystem> reticleFrameUi;         //�O�g
    std::shared_ptr<UiSystem> reticleCircleUi;        //�^��

    float* attackPower;  //���݂̃`���[�W��
    float maxAttackPower = 0.0f; //�`���[�W�̍ő�
};
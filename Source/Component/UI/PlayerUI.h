#pragma once
#include "Component\UI\UiSystem.h"
#include "Component\UI\UiGauge.h"
class UI_Skill : public UiSystem
{
    //コンポーネントオーバーライド
public:
    UI_Skill(const char* filename, SpriteShader spriteshader, bool collsion, float min, float max);
    ~UI_Skill() {}

    // 名前取得
    const char* GetName() const override { return "Ui_Skill"; }

    // 開始処理
    void Start() override;

    // 更新処理
    void Update(float elapsedTime) override;

private:
    float changePosValue;
    DirectX::XMFLOAT2 originalPos = {};
    DirectX::XMFLOAT2 maxPos = {};
};

//class UI_SkillManager : public Component
//{
//    //コンポーネントオーバーライド
//public:
//    UI_SkillManager(int num);
//    ~UI_SkillManager() {}
//
//    // 名前取得
//    const char* GetName() const override { return "UI_BoostGauge"; }
//
//    // 開始処理
//    void Start() override;
//
//    // 更新処理
//    void Update(float elapsedTime) override;
//
//private:
//};

class UI_BoosGauge : public Component
{
    //コンポーネントオーバーライド
public:
    UI_BoosGauge(int num);
    ~UI_BoosGauge() {}

    // 名前取得
    const char* GetName() const override { return "UI_BoostGauge"; }

    // 開始処理
    void Start() override;

    // 更新処理
    void Update(float elapsedTime) override;

private:
    std::vector<std::shared_ptr<GameObject>>   gauges;
    std::vector<std::shared_ptr<GameObject>>   frames;
    int  num;
    float maxDashGauge;                  //ダッシュゲージの総量を保持
    float* value;                       //ダッシュゲージの現在の値
    float separateValue;                //区切りの値
    DirectX::XMFLOAT2 originlTexSize;   //元のテクスチャーサイズ
    std::weak_ptr<GameObject> player;
};

class UI_LockOn : public Component
{
    //コンポーネントオーバーライド
public:
    UI_LockOn(int num, float min, float max);
    ~UI_LockOn() {}

    // 名前取得
    const char* GetName() const override { return "UI_BoostGauge"; }

    // 開始処理
    void Start() override;

    // 更新処理
    void Update(float elapsedTime) override;

    void OnGUI()override;

    //視線の先のオブジェクトを入手
    std::shared_ptr<GameObject> SearchObjct();

    void UpdateGauge(float elapsedTime, std::shared_ptr<GameObject> obj);

    void LockIn(float elapsedTime);
    void LockOut(float elapsedTime);

private:
    std::vector<std::shared_ptr<GameObject>> reacters;

    std::weak_ptr<GameObject> camera;   //カメラ

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
    //コンポーネントオーバーライド
public:
    UI_E_SkillCount(int num);
    ~UI_E_SkillCount() {}

    // 名前取得
    const char* GetName() const override { return "UI_BoostGauge"; }

    // 開始処理
    void Start() override;

    // 更新処理
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
    //コンポーネントオーバーライド
public:
    UI_Ult_Count(int num);
    ~UI_Ult_Count() {}

    // 名前取得
    const char* GetName() const override { return "UI_BoostGauge"; }

    // 開始処理
    void Start() override;

    // 更新処理
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
    //コンポーネントオーバーライド
public:
    UI_Reticle();
    ~UI_Reticle() {}

    // 名前取得
    const char* GetName() const override { return "UI_BoostGauge"; }

    // 開始処理
    void Start() override;

    // 更新処理
    void Update(float elapsedTime) override;

private:
    std::weak_ptr<GameObject> player;

    std::shared_ptr<GameObject> reticleFrame;
    std::shared_ptr<GameObject> reticleCircle;

    std::shared_ptr<UiSystem> reticleFrameUi;         //外枠
    std::shared_ptr<UiSystem> reticleCircleUi;        //真ん中

    float* attackPower;  //現在のチャージ率
    float maxAttackPower = 0.0f; //チャージの最大
};
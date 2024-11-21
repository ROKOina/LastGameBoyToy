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
};

class UI_BoosGauge : public Component
{
    //コンポーネントオーバーライド
public:
    UI_BoosGauge(int num);
    ~UI_BoosGauge() {}

    // 名前取得
    const char* GetName() const override { return "UI_BoostGauge"; }

    // 開始処理
    void Start() override ;

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
    UI_LockOn(int num);
    ~UI_LockOn() {}

    // 名前取得
    const char* GetName() const override { return "UI_BoostGauge"; }

    // 開始処理
    void Start() override ;

    // 更新処理
    void Update(float elapsedTime) override;

    void OnGUI()override;


    //視線の先のオブジェクトを入手
    std::shared_ptr<GameObject> SearchObjct();
    
    void LockIn(float elapsedTime);
    void LockOut(float elapsedTime);

private:
    std::vector<std::shared_ptr<GameObject>> reacters;

  
    std::weak_ptr<GameObject> camera;   //カメラ

    std::shared_ptr<GameObject> lockOn;
    std::shared_ptr<GameObject> gauge;

    std::shared_ptr<UiSystem> lockOnUi; //lockOn
    std::shared_ptr<UiSystem> gaugeUi;  //Gauge

    std::vector<float> similarity;
};
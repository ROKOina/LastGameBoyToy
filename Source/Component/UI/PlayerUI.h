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
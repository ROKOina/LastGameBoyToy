#pragma once
#include "Component\UI\UiSystem.h"

class UI_Skill : public UiSystem
{
    //コンポーネントオーバーライド
public:
    UI_Skill(const char* filename, SpriteShader spriteshader, bool collsion, float min, float max);
    ~UI_Skill() {}

    // 名前取得
    const char* GetName() const override { return "UiFlag"; }

    // 開始処理
    void Start() override;

    // 更新処理
    void Update(float elapsedTime) override;

private:
    float changePosValue;
    DirectX::XMFLOAT2 originalPos = {};
};

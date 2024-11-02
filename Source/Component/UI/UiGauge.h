#pragma once
#include "UiSystem.h"

class UiGauge : public UiSystem
{
    //コンポーネントオーバーライド
public:
    UiGauge(const char* filename, SpriteShader spriteshader, bool collsion);
    ~UiGauge() {}

    // 名前取得
    const char* GetName() const override { return "UiGauge"; }

    // 開始処理
    void Start() override {};

    // 更新処理
    void Update(float elapsedTime) override;

private:
    DirectX::XMFLOAT2 originalTexSize = {};
};

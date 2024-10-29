#pragma once
#include "Graphics\Sprite\Sprite.h"
#include "math.h"
class UiSystem : public Sprite
{
    //コンポーネントオーバーライド
public:
    UiSystem(const char* filename, SpriteShader spriteshader, bool collsion);
    ~UiSystem() {}

    // 名前取得
    const char* GetName() const override { return "UiSystem"; }

    // 開始処理
    void Start() override {};

    // 更新処理
    void Update(float elapsedTime) override;

    //描画
    void Render(const DirectX::XMFLOAT4X4& view, const DirectX::XMFLOAT4X4& projection);

    // GUI描画
    void OnGUI() override;

    //参照値,最大値設定
    void SetVariableValue(float* value) { variableValue = value; }
    void SetMaxValue(float value) { maxValue = value; };

protected:
    float* variableValue;     //参照する値
    float  maxValue;          //最大値
    float  valueRate;        //割合
};

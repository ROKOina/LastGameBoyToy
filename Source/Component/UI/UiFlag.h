#pragma once
#include "Components\UI\UiSystem.h"

class UiFlag : public UiSystem
{
    //コンポーネントオーバーライド
public:
    UiFlag(const char* filename, SpriteShader spriteshader, bool collsion, bool* flag);
    ~UiFlag() {}

    // 名前取得
    const char* GetName() const override { return "UiFlag"; }

    // 開始処理
    void Start() override {};

    // 更新処理
    void Update(float elapsedTime) override;

    void OnGUI()override;

private:
    bool* flag; //参照するフラグ
};

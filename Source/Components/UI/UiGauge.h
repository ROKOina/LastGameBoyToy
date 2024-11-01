#pragma once
#include "Components\UI\UiSystem.h"

class UiGauge : public UiSystem
{
    //コンポーネントオーバーライド
public:
    UiGauge(const char* filename, SpriteShader spriteshader, bool collsion,int cahngeValue);
    ~UiGauge() {}

    // 名前取得
    const char* GetName() const override { return "UiGauge"; }

    // 開始処理
    void Start() override {};

    // 更新処理
    void Update(float elapsedTime) override;


private:
   
    int changeValue = -1; //変更する値がXかYか両方かを判別
};


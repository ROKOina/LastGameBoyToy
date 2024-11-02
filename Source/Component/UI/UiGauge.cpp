#include "UiGauge.h"
#include "Math\Mathf.h"
UiGauge::UiGauge(const char* filename, SpriteShader spriteshader, bool collsion) :UiSystem(filename, spriteshader, collsion)
{
    //元のtexSizeを保持
    originalTexSize = spc.texSize;
}

void UiGauge::Update(float elapsedTime)
{
    // 変化値がマイナスに行かないように補正
    *variableValue = Mathf::Clamp(*variableValue, 0.01f, maxValue);
    //ゲージの倍率を求める
    valueRate = *variableValue / maxValue;
    spc.texSize = { originalTexSize.x * valueRate,spc.texSize.y };

    //親クラスのUpdateを呼ぶ
    this->UiSystem::Update(elapsedTime);
}
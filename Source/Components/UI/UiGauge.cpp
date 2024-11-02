#include "UiGauge.h"
#include "GameSource\Math\Mathf.h"
UiGauge::UiGauge(const char* filename, SpriteShader spriteshader, bool collsion, int changeValue) :UiSystem(filename, spriteshader, collsion)
{
    //元のtexSizeを保持
    originalTexSize = spc.texSize;
    //変更する値を設定
    this->changeValue = changeValue;
}

void UiGauge::Update(float elapsedTime)
{
    // 変化値がマイナスに行かないように補正
    *variableValue = Mathf::Clamp(*variableValue, 0.01f, maxValue);
    //ゲージの倍率を求める
    valueRate = *variableValue / maxValue;
    //Xのみ
    if (changeValue == UiSystem::ChangeValue::X_ONLY) {
        spc.texSize = { originalTexSize.x * valueRate,spc.texSize.y };
    }
    //Yのみ
    else if (changeValue == UiSystem::ChangeValue::Y_ONLY) {
        spc.texSize = { spc.texSize.y,originalTexSize.y * valueRate };
    }
    //ＸＹ両方
    else if (changeValue == UiSystem::ChangeValue::X_AND_Y) {
        spc.texSize = { originalTexSize.x * valueRate,originalTexSize.y * valueRate };
    }

    //親クラスのUpdateを呼ぶ
    this->UiSystem::Update(elapsedTime);
}
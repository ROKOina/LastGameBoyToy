#include "UiGauge.h"
#include "Math\Mathf.h"

UiGauge::UiGauge(const char* filename, SpriteShader spriteshader, bool collsion, int changeValue) :UiSystem(filename, spriteshader, collsion)
{
    //元のtexSizeを保持
    originalTexSize = spc.texSize;
    //変更する値を設定
    this->changeValue = changeValue;
}

void UiGauge::Start()
{
    this->UiSystem::Start();
}

void UiGauge::Update(float elapsedTime)
{
    //ゲージの倍率を求める
    valueRate = *variableValue / maxValue;
    if (!isDebug) {
        //Xのみ
        if (changeValue == UiSystem::ChangeValue::X_ONLY_ADD) {
            spc.texSize = { originalTexSize.x * valueRate,spc.texSize.y };
        }
        //Xのみ
        else  if (changeValue == UiSystem::ChangeValue::X_ONLY_SUB) {
            spc.texSize = { originalTexSize.x * (1 - valueRate),spc.texSize.y };
        }
        //Yのみ
        else if (changeValue == UiSystem::ChangeValue::Y_ONLY_ADD) {
            spc.texSize = { spc.texSize.x,originalTexSize.y * valueRate };
        }
        else if (changeValue == UiSystem::ChangeValue::Y_ONLY_SUB) {
            spc.texSize = { spc.texSize.x,originalTexSize.y * (1 - valueRate) };
        }
        //ＸＹ両方
        else if (changeValue == UiSystem::ChangeValue::X_AND_Y_ADD) {
            spc.texSize = { originalTexSize.x * valueRate,originalTexSize.y * valueRate };
        }
        else if (changeValue == UiSystem::ChangeValue::X_AND_Y_SUB) {
            spc.texSize = { originalTexSize.x * (1 - valueRate) ,originalTexSize.y * (1 - valueRate) };
        }
    }
    //親クラスのUpdateを呼ぶ
    this->UiSystem::Update(elapsedTime);
}
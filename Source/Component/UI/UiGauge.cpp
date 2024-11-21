#include "UiGauge.h"
#include "Math\Mathf.h"

UiGauge::UiGauge(const char* filename, SpriteShader spriteshader, bool collsion, int changeValue) :UiSystem(filename, spriteshader, collsion)
{
    //Œ³‚ÌtexSize‚ð•ÛŽ
    originalTexSize = spc.texSize;
    //•ÏX‚·‚é’l‚ðÝ’è
    this->changeValue = changeValue;
}

void UiGauge::Start()
{
    this->UiSystem::Start();
}

void UiGauge::Update(float elapsedTime)
{
    //ƒQ[ƒW‚Ì”{—¦‚ð‹‚ß‚é
    valueRate = *variableValue / maxValue;
    if (!isDebug) {
        //X‚Ì‚Ý
        if (changeValue == UiSystem::ChangeValue::X_ONLY_ADD) {
            spc.texSize = { originalTexSize.x * valueRate,spc.texSize.y };
        }
        //X‚Ì‚Ý
        else  if (changeValue == UiSystem::ChangeValue::X_ONLY_SUB) {
            spc.texSize = { originalTexSize.x * (1 - valueRate),spc.texSize.y };
        }
        //Y‚Ì‚Ý
        else if (changeValue == UiSystem::ChangeValue::Y_ONLY_ADD) {
            spc.texSize = { spc.texSize.x,originalTexSize.y * valueRate };
        }
        else if (changeValue == UiSystem::ChangeValue::Y_ONLY_SUB) {
            spc.texSize = { spc.texSize.x,originalTexSize.y * (1 - valueRate) };
        }
        //‚w‚x—¼•û
        else if (changeValue == UiSystem::ChangeValue::X_AND_Y_ADD) {
            spc.texSize = { originalTexSize.x * valueRate,originalTexSize.y * valueRate };
        }
        else if (changeValue == UiSystem::ChangeValue::X_AND_Y_SUB) {
            spc.texSize = { originalTexSize.x * (1 - valueRate) ,originalTexSize.y * (1 - valueRate) };
        }
    }
    //eƒNƒ‰ƒX‚ÌUpdate‚ðŒÄ‚Ô
    this->UiSystem::Update(elapsedTime);
}
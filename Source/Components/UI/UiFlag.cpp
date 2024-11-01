#include "UiFlag.h"

UiFlag::UiFlag(const char* filename, SpriteShader spriteshader, bool collsion, bool* flag):UiSystem(filename,spriteshader,collsion)
{
    this->flag = flag;
    spc.color.w = 0.0f;
}

void UiFlag::Update(float elapsedTime)
{
    if (flag){
        spc.color.w = 1.0f;
    }
    else {
        spc.color.w = 0.0f;
    }

    //親クラスのアップデートをよぶ
    this->UiSystem::Update(elapsedTime);
}



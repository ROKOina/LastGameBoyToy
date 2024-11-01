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

    //�e�N���X�̃A�b�v�f�[�g�����
    this->UiSystem::Update(elapsedTime);
}



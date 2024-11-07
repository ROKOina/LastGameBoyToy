#include "UiGauge.h"
#include "Math\Mathf.h"

UiGauge::UiGauge(const char* filename, SpriteShader spriteshader, bool collsion, int changeValue) :UiSystem(filename, spriteshader, collsion)
{
    //����texSize��ێ�
    originalTexSize = spc.texSize;
    //�ύX����l��ݒ�
    this->changeValue = changeValue;
}

void UiGauge::Update(float elapsedTime)
{
    // �ω��l���}�C�i�X�ɍs���Ȃ��悤�ɕ␳
    *variableValue = Mathf::Clamp(*variableValue, 0.01f, maxValue);
    //�Q�[�W�̔{�������߂�
    valueRate = *variableValue / maxValue;
    //X�̂�
    if (changeValue == UiSystem::ChangeValue::X_ONLY_ADD) {
        spc.texSize = { originalTexSize.x * valueRate,spc.texSize.y };
    }
    //X�̂�
    else  if (changeValue == UiSystem::ChangeValue::X_ONLY_SUB) {
        spc.texSize = { originalTexSize.x *(1- valueRate),spc.texSize.y };
    }
    //Y�̂�
    else if (changeValue == UiSystem::ChangeValue::Y_ONLY_ADD) {
        spc.texSize = { spc.texSize.x,originalTexSize.y * valueRate};
    }
    else if (changeValue == UiSystem::ChangeValue::Y_ONLY_SUB) {
        spc.texSize = { spc.texSize.x,originalTexSize.y * (1 - valueRate) };
    }
    //�w�x����
    else if (changeValue == UiSystem::ChangeValue::X_AND_Y_ADD) {
        spc.texSize = { originalTexSize.x * valueRate,originalTexSize.y * valueRate };
    }
    else if (changeValue == UiSystem::ChangeValue::X_AND_Y_SUB) {
        spc.texSize = { originalTexSize.x * (1 - valueRate) ,originalTexSize.y * (1 - valueRate) };
    }

    //�e�N���X��Update���Ă�
    this->UiSystem::Update(elapsedTime);
}
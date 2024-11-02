#include "UiGauge.h"
#include "Math\Mathf.h"
UiGauge::UiGauge(const char* filename, SpriteShader spriteshader, bool collsion) :UiSystem(filename, spriteshader, collsion)
{
    //����texSize��ێ�
    originalTexSize = spc.texSize;
}

void UiGauge::Update(float elapsedTime)
{
    // �ω��l���}�C�i�X�ɍs���Ȃ��悤�ɕ␳
    *variableValue = Mathf::Clamp(*variableValue, 0.01f, maxValue);
    //�Q�[�W�̔{�������߂�
    valueRate = *variableValue / maxValue;
    spc.texSize = { originalTexSize.x * valueRate,spc.texSize.y };

    //�e�N���X��Update���Ă�
    this->UiSystem::Update(elapsedTime);
}
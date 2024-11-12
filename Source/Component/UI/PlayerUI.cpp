#include "PlayerUI.h"
#include "Math\Mathf.h"
UI_Skill::UI_Skill(const char* filename, SpriteShader spriteshader, bool collsion, float min, float max) :UiSystem(filename, spriteshader, collsion)
{
    changePosValue = min - max;
    //�����}�C�i�X�Ȃ琮���l�ɕς���
    if (changePosValue <= 0.0f) {
        changePosValue *= -1.0f;
    }
    originalPos = spc.position;
}

void UI_Skill::Update(float elapsedTime)
{
    // �ω��l���}�C�i�X�ɍs���Ȃ��悤�ɕ␳
    *variableValue = Mathf::Clamp(*variableValue, 0.01f, maxValue);

    //�Q�[�W�̔{�������߂�
    valueRate = *variableValue / maxValue;
    float addPos = changePosValue * valueRate;
    spc.position = { spc.position.x,originalPos.y - addPos};
    
}


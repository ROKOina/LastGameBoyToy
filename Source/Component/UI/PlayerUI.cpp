#include "PlayerUI.h"
#include "Math\Mathf.h"
UI_Skill::UI_Skill(const char* filename, SpriteShader spriteshader, bool collsion, float min, float max) :UiSystem(filename, spriteshader, collsion)
{
    changePosValue = min - max;
    //もしマイナスなら整数値に変える
    if (changePosValue <= 0.0f) {
        changePosValue *= -1.0f;
    }
    originalPos = spc.position;
}

void UI_Skill::Update(float elapsedTime)
{
    // 変化値がマイナスに行かないように補正
    *variableValue = Mathf::Clamp(*variableValue, 0.01f, maxValue);

    //ゲージの倍率を求める
    valueRate = *variableValue / maxValue;
    float addPos = changePosValue * valueRate;
    spc.position = { spc.position.x,originalPos.y - addPos};
    
}


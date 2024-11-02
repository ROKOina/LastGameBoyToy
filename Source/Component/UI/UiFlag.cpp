#include "UiFlag.h"
#include "Components\Character\CharacterCom.h"
UiFlag::UiFlag(const char* filename, SpriteShader spriteshader, bool collsion, bool* flag) :UiSystem(filename, spriteshader, collsion)
{
    this->flag = flag;
    spc.color.w = 0.0f;
}

void UiFlag::Update(float elapsedTime)
{
    if (*flag) {
        spc.color.w = 1.0f;
        spc.scale.x = 1.0f;
        spc.scale.y = 1.0f;

        *flag = false;
    }
    else {
        spc.color.w -= 6.0 * elapsedTime;
        if (spc.scale.x >= 0.9f && spc.scale.y >= 0.9f)
        {
            spc.scale.x -= 0.05f;
            spc.scale.y -= 0.05f;
        }
    }

    //親クラスのアップデートをよぶ
    this->UiSystem::Update(elapsedTime);
}

void UiFlag::OnGUI()
{
    ImGui::Checkbox("Flag", *&flag);
    this->UiSystem::OnGUI();
}
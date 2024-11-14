#include "Uisystem.h"
#include "Math\Mathf.h"
UiSystem::UiSystem(const char* filename, SpriteShader spriteshader, bool collsion) :Sprite(filename, spriteshader, collsion)
{
    fadeInFlag = false;
    fadeOutFlag = false;
}

//初期設定
void UiSystem::Start()
{
    this->Sprite::Start();
}

void UiSystem::Update(float elapsedTime)
{
    //親クラスのupdateを呼ぶ
    this->Sprite::Update(elapsedTime);

    FadeOut(elapsedTime);
    FadeIn(elapsedTime);
}

void UiSystem::Render(const DirectX::XMFLOAT4X4& view, const DirectX::XMFLOAT4X4& projection)
{
    //親クラスのRender呼ぶ
    this->Sprite::Render(view, projection);
}

void UiSystem::OnGUI()
{
    ImGui::DragFloat("FadeTimer", &fadeTimer);

    ImGui::Checkbox("FadeIn", &fadeInFlag);
    ImGui::SameLine();
    ImGui::Checkbox("FadeOut", &fadeOutFlag);
    ImGui::Separator();

    //親クラスのRender呼ぶ
    this->Sprite::OnGUI();
}

void UiSystem::FadeIn(float elapsedTime)
{
    if (fadeInFlag) {
        fadeTimer -= elapsedTime;
        spc.color.w = 1.0f - fadeTimer / originalFadeTime;
        if (fadeTimer <= 0.0f) {
            fadeInFlag = false;
        }
    }
}

void UiSystem::FadeOut(float elapsedTime)
{
    if (fadeOutFlag) {
        fadeTimer -= elapsedTime;
        spc.color.w = fadeTimer / originalFadeTime;
        if (fadeTimer <= 0.0f) {
            fadeOutFlag = false;
        }
    }
}
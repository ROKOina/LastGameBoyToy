#include "Uisystem.h"

UiSystem::UiSystem(const char* filename, SpriteShader spriteshader, bool collsion) :Sprite(filename, spriteshader, collsion)
{
}

void UiSystem::Update(float elapsedTime)
{
    //親クラスのupdateを呼ぶ
    this->Sprite::Update(elapsedTime);
}

void UiSystem::Render(const DirectX::XMFLOAT4X4& view, const DirectX::XMFLOAT4X4& projection)
{
    //親クラスのRender呼ぶ
    this->Sprite::Render(view, projection);
}

void UiSystem::OnGUI()
{
    //親クラスのRender呼ぶ
    this->Sprite::OnGUI();
}
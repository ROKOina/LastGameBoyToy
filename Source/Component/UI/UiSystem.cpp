#include "Uisystem.h"

UiSystem::UiSystem(const char* filename, SpriteShader spriteshader, bool collsion) :Sprite(filename, spriteshader, collsion)
{
}

void UiSystem::Update(float elapsedTime)
{
    //�e�N���X��update���Ă�
    this->Sprite::Update(elapsedTime);
}

void UiSystem::Render(const DirectX::XMFLOAT4X4& view, const DirectX::XMFLOAT4X4& projection)
{
    //�e�N���X��Render�Ă�
    this->Sprite::Render(view, projection);
}

void UiSystem::OnGUI()
{
    //�e�N���X��Render�Ă�
    this->Sprite::OnGUI();
}
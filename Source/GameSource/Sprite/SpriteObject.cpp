#include "SpriteObject.h"

SpriteObject::SpriteObject(std::string fileName)
{
    sprite = std::make_unique<Sprite>("fileName");

    texSize.x = sprite->GetTextureWidth();
    texSize.y = sprite->GetTextureHeight();
}

void SpriteObject::Render(ID3D11DeviceContext* dc)
{
    sprite.get()->Render(
        dc,
        position.x, position.y,
        size.x, size.y,
        texPos.x, texPos.y,
        texSize.x, texSize.y,
        angle,
        color.x, color.y, color.z, color.w
    );
}

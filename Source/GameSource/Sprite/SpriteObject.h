#pragma once
#include "Graphics\Sprite\Sprite.h"

#include <memory>
#include <string>

class SpriteObject
{
public:
	SpriteObject(std::string fileName);
	 
    virtual void Update(float elapsedTime) {}
    virtual void Render(ID3D11DeviceContext* dc);

private:
    std::unique_ptr<Sprite> sprite;

	DirectX::XMFLOAT2 position;
	DirectX::XMFLOAT2 size;
	DirectX::XMFLOAT2 texPos;
	DirectX::XMFLOAT2 texSize;
	float angle;
	DirectX::XMFLOAT4 color = { 1,1,1,1 };
};

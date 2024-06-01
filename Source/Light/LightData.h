#pragma once

#include <DirectXMath.h>

struct DirectionalLightData
{
    DirectX::XMFLOAT4	direction;
    DirectX::XMFLOAT4	color;
};

// 点光源情報
struct PointLightData
{
    DirectX::XMFLOAT4	position;
    DirectX::XMFLOAT4	color;
    float			range;
    DirectX::XMFLOAT3	dummy;
};
// 点光源の最大数
static	constexpr	int	POINT_LIGHT_MAX = 10;

// スポットライト情報
struct SpotLightData
{
    DirectX::XMFLOAT4	position;
    DirectX::XMFLOAT4	direction;
    DirectX::XMFLOAT4	color;
    float			range;
    float			innerCorn; 	// インナー角度範囲
    float			outerCorn; 	// アウター角度範囲
    float			dummy;
};
// スポットライトの最大数
static	constexpr	int	SPOT_LIGHT_MAX = 10;

// 定数バッファ用構造体
struct LightCB
{
    DirectionalLightData directionalLight;
};
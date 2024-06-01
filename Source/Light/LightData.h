#pragma once

#include <DirectXMath.h>

struct DirectionalLightData
{
    DirectX::XMFLOAT4	direction;
    DirectX::XMFLOAT4	color;
};

// �_�������
struct PointLightData
{
    DirectX::XMFLOAT4	position;
    DirectX::XMFLOAT4	color;
    float			range;
    DirectX::XMFLOAT3	dummy;
};
// �_�����̍ő吔
static	constexpr	int	POINT_LIGHT_MAX = 10;

// �X�|�b�g���C�g���
struct SpotLightData
{
    DirectX::XMFLOAT4	position;
    DirectX::XMFLOAT4	direction;
    DirectX::XMFLOAT4	color;
    float			range;
    float			innerCorn; 	// �C���i�[�p�x�͈�
    float			outerCorn; 	// �A�E�^�[�p�x�͈�
    float			dummy;
};
// �X�|�b�g���C�g�̍ő吔
static	constexpr	int	SPOT_LIGHT_MAX = 10;

// �萔�o�b�t�@�p�\����
struct LightCB
{
    DirectionalLightData directionalLight;
};
#pragma once
#include "LightData.h"

// �����^�C�v
enum class LightType
{
    Directional,	// ���s����
    Point,			// �_����
    Spot,			// �X�|�b�g���C�g
};

// �����N���X
class Light
{
public:
    Light(LightType lightType = LightType::Directional);

    // ���C�g����RenderContext�ɐς�
    void PushRenderContext(LightCB& cb);

    // �f�o�b�O���̕\��
    void DrawDebugGUI();

    // �f�o�b�O�}�`�̕\��
    void DrawDebugPrimitive();

    // ���C�g�^�C�v
    LightType GetLightType() { return lightType; }

    // ���C�g�̍��W
    DirectX::XMFLOAT3 Position() { return position; }
    void SetPosition(DirectX::XMFLOAT3 position) { this->position = position; }

    // ���C�g�̌���
    const DirectX::XMFLOAT3& GetDirection() const { return direction; }
    void SetDirection(DirectX::XMFLOAT3 direction) { this->direction = direction; }

    // �F
    DirectX::XMFLOAT4 GetColor() { return color; }
    void SetColor(DirectX::XMFLOAT4 color) { this->color = color; }

    void SetPower(float power) { this->power = power; }

    // ���C�g�͈̔�
    float GetRange() { return range; }
    void SetRange(float range) { this->range = range; }

    // �C���i�[
    float GetInnerCorn() { return innerCorn; }
    void SetInnerCorn(float innerCorn) { this->innerCorn = innerCorn; }

    // �A�E�^�[
    float GetOuterCorn() { return outerCorn; }
    void SetOuterCorn(float outerCorn) { this->outerCorn = outerCorn; }

private:
    LightType			lightType = LightType::Directional;		// ���C�g�^�C�v
    DirectX::XMFLOAT3	position = DirectX::XMFLOAT3(0, 0, 0);	// ���C�g�̍��W
    DirectX::XMFLOAT3	direction = DirectX::XMFLOAT3(0, -1, -1);	// ���C�g�̌���
    DirectX::XMFLOAT4	color = DirectX::XMFLOAT4(1, 1, 1, 1);	// ���C�g�̐F
    float	range = 20.0f;	// �͈�
    float	power = 1.0f;		// ���C�g�̋���

    float	innerCorn = 0.99f;	// �C���i�[
    float	outerCorn = 0.9f;	// �A�E�^�[
};
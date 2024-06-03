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
    const LightType& GetLightType()const { return m_lightType; }

    // ���C�g�̍��W
    const DirectX::XMFLOAT3& Position() const { return m_position; }
    void SetPosition(DirectX::XMFLOAT3 position) { this->m_position = position; }

    // ���C�g�̌���
    const DirectX::XMFLOAT3& GetDirection() const { return m_direction; }
    void SetDirection(DirectX::XMFLOAT3 direction) { this->m_direction = direction; }

    // �F
    const DirectX::XMFLOAT4& GetColor() const { return m_color; }
    void SetColor(DirectX::XMFLOAT4 color) { this->m_color = color; }

    void SetPower(float power) { this->m_power = power; }

    // ���C�g�͈̔�
    const float& GetRange() const { return m_range; }
    void SetRange(float range) { this->m_range = range; }

    // �C���i�[
    const float& GetInnerCorn()const { return m_innercorn; }
    void SetInnerCorn(float innerCorn) { this->m_innercorn = innerCorn; }

    // �A�E�^�[
    const float& GetOuterCorn()const { return m_outercorn; }
    void SetOuterCorn(float outerCorn) { this->m_outercorn = outerCorn; }

private:
    LightType			m_lightType = LightType::Directional;		 // ���C�g�^�C�v
    DirectX::XMFLOAT3	m_position = DirectX::XMFLOAT3(0, 0, 0);	 // ���C�g�̍��W
    DirectX::XMFLOAT3	m_direction = DirectX::XMFLOAT3(0, -1, -1);// ���C�g�̌���
    DirectX::XMFLOAT4	m_color = DirectX::XMFLOAT4(1, 1, 1, 1);	 // ���C�g�̐F
    float	m_range = 20.0f;	// �͈�
    float	m_power = 1.0f;		// ���C�g�̋���

    float	m_innercorn = 0.99f;	// �C���i�[
    float	m_outercorn = 0.9f;	// �A�E�^�[
};
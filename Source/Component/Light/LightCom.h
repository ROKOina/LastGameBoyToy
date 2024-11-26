#pragma once

#include "Component\System\Component.h"
#include <d3d11.h>
#include <wrl.h>

class Light :public Component
{
public:

    Light(const char* filename);
    ~Light() {};

    //�����ݒ�
    void Start()override {};

    //�X�V����
    void Update(float elapsedTime)override;

    //imgui
    void OnGUI()override;

    //���O�ݒ�
    const char* GetName() const override { return "Light"; }

private:

    //�f�o�b�O�`��
    void DebugPrimitive();

    //�V���A���C�Y
    void Serialize();

    //�f�V���A���C�Y
    void Desirialize(const char* filename);

    //�ǂݍ���
    void LoadDesirialize();

public:

    // ���C�g�̌���
    DirectX::XMFLOAT3 GetDirection() const { return { cb.directionalLight.direction.x, cb.directionalLight.direction.y, cb.directionalLight.direction.z }; }

public:

    //���C�g�̃p�����[�^
    struct LightParameter
    {
        float power = 1.0f;          //���C�g�̌��̋���
        int lighttype = 0;           //���C�g�^�C�v
        DirectX::XMFLOAT4 color;     // �F
        DirectX::XMFLOAT3 position;  // Point/Spot�p�ʒu
        DirectX::XMFLOAT3 direction; // Directional/Spot�p����
        float range;                 // Point/Spot�p�͈�
        float innerCone;             // Spot�p�C���i�[�p
        float outerCone;             // Spot�p�A�E�^�[�p

        template<class Archive>
        void serialize(Archive& archive, int version);
    };
    std::vector<LightParameter> LP = {};

private:

    // �����^�C�v
    enum class LightType
    {
        Directional,	// ���s����
        Point,			// �_����
        Spot,			// �X�|�b�g���C�g
        MAX
    };

    //���s����
    struct DirectionalLightData
    {
        DirectX::XMFLOAT4	direction = { 0.0f,-1.0f,-1.0f,0.0f };
        DirectX::XMFLOAT4	color = { 1,1,1,1 };
    };

    // �_�������
    struct PointLightData
    {
        DirectX::XMFLOAT4	position = {};
        DirectX::XMFLOAT4	color = { 1,1,1,1 };
        float			    range = { 1.0f };
        DirectX::XMFLOAT3	dummy = {};
    };
    // �_�����̍ő吔
    static	constexpr	int	POINT_LIGHT_MAX = 1;

    // �X�|�b�g���C�g���
    struct SpotLightData
    {
        DirectX::XMFLOAT4	position = {};
        DirectX::XMFLOAT4	direction = {};
        DirectX::XMFLOAT4	color = { 1,1,1,1 };
        float			    range = { 1.0f };
        float			    innerCorn = {}; 	// �C���i�[�p�x�͈�
        float			    outerCorn = {}; 	// �A�E�^�[�p�x�͈�
        float			    dummy = {};
    };
    // �X�|�b�g���C�g�̍ő吔
    static	constexpr	int	SPOT_LIGHT_MAX = 1;

    // �萔�o�b�t�@�p�\����
    struct LightCB
    {
        DirectionalLightData directionalLight = {};
        PointLightData       pointLight[POINT_LIGHT_MAX];
        SpotLightData        spotLight[SPOT_LIGHT_MAX];
    };
    LightCB cb = {};

private:
    Microsoft::WRL::ComPtr<ID3D11Buffer>m_lightCb;       // �萔�o�b�t�@
    LightType m_lightType = LightType::Directional;		 // ���C�g�^�C�v
};
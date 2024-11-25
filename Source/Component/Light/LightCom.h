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
    void Start()override;

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

        template<class Archive>
        void serialize(Archive& archive, int version);
    };

    // �_�������
    struct PointLightData
    {
        DirectX::XMFLOAT4	position = {};
        DirectX::XMFLOAT4	color = { 1,1,1,1 };
        float			    range = { 1.0f };
        DirectX::XMFLOAT3	dummy = {};

        template<class Archive>
        void serialize(Archive& archive, int version);
    };
    // �_�����̍ő吔
    static	constexpr	int	POINT_LIGHT_MAX = 10;

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

        template<class Archive>
        void serialize(Archive& archive, int version);
    };
    // �X�|�b�g���C�g�̍ő吔
    static	constexpr	int	SPOT_LIGHT_MAX = 10;

    // �萔�o�b�t�@�p�\����
    struct LightCB
    {
        DirectionalLightData directionalLight = {};
        PointLightData       pointLight[POINT_LIGHT_MAX];
        SpotLightData        spotLight[SPOT_LIGHT_MAX];

        template<class Archive>
        void serialize(Archive& archive, int version);
    };
    LightCB cb = {};

    //���C�g�̃p�����[�^
    struct LightParameter
    {
        float power = 1.0f;        //���C�g�̌��̋���
        int lighttype = 0;         //���C�g�^�C�v

        template<class Archive>
        void serialize(Archive& archive, int version);
    };
    LightParameter LP = {};

private:
    Microsoft::WRL::ComPtr<ID3D11Buffer>m_lightCb;       // �萔�o�b�t�@
    LightType m_lightType = LightType::Directional;		 // ���C�g�^�C�v
    DirectX::XMFLOAT4 directioncolor = { 1,1,1,1 };
    int pointLightCount = 0;
    int spotlightmax = 0;
};
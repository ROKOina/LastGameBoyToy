static const int PointLightMax = 10;
static const int SpotLightMax = 10;

// ���s�������
struct DirectionalLightData
{
    float4 direction;
    float4 color; // w = intensity
};

// �_�������
struct PointLightData
{
    float4 position;
    float4 color;
    float range;
    float3 dummy;
};

// �X�|�b�g���C�g���
struct SpotLightData
{
    float4 position;
    float4 direction;
    float4 color;
    float range;
    float innerCorn;
    float outerCorn;
    float dummy;
};

cbuffer LightBuffer : register(b8)
{
    DirectionalLightData directionalLight;
    PointLightData pointLight;
    SpotLightData spotLight;
};
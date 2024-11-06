#include "GPUparticle.hlsli"
#include "../Constants.hlsli"
#include "..\\3D\\Light.hlsli"
#include "../Common.hlsli"

StructuredBuffer<MainParticle> particlebuffer : register(t0);

[maxvertexcount(4)]
void main(point VS_OUT input[1], inout TriangleStream<GS_OUT> output)
{
// �r���{�[�h�ƃe�N�X�`�����W
    float3 BILLBOARD[] =
    {
        float3(-0.5f, +0.5f, 0.0f), // ����
        float3(+0.5f, +0.5f, 0.0f), // �E��
        float3(-0.5f, -0.5f, 0.0f), // ����
        float3(+0.5f, -0.5f, 0.0f), // �E��
    };
    const float2 TEXCOORD[] =
    {
        float2(0.0f, 0.0f), // ����
        float2(1.0f, 0.0f), // �E��
        float2(0.0f, 1.0f), // ����
        float2(1.0f, 1.0f), // �E��
    };

    // �ȈՓI�ȃ��C�e�B���O�v�Z
    float3 N = normalize(float3(0, 0, 1)); // �p�[�e�B�N���ʖ@�� (�f�t�H���gZ����)
    float3 L = normalize(-directionalLight.direction.xyz);
    float d = dot(L, N);
    float power = max(0, d) * 0.5f + 0.5f;

    //���̂����
    MainParticle p = particlebuffer[input[0].vertex_id];

    //���W�ϊ�(���͂ƈʒu)
    float4 viewpos = mul(float4(p.position, 1.0), view);
    float4 worldviewpos = mul(mul(float4(p.position, 1.0f), world), view);
    float4 viewvelo = mul(float4(p.velocity.xyz, 0.0), view);

    //��]
    p.rotation = rotation;

    //�����t���O��false�Ȃ琶�����Ȃ�
    p.isalive = isalive;
    if (p.isalive == 0)
    {
        return;
    }

    GS_OUT element;

    //���_�ǉ�
    [unroll]
    for (int i = 0; i < 4; ++i)
    {
        float3 cornerPos = BILLBOARD[i];

        // �X�g���b�`�r���{�[�h��K�p
        if (stretchFlag == 1 && length(p.velocity.xyz) > 0.001f)
        {
            float3 stretchDirection = normalize(viewvelo.xyz);
            float stretchAmount = length(viewvelo.xyz) * strechscale;
            cornerPos += stretchDirection * stretchAmount * (BILLBOARD[i].x > 0 ? 1 : -1);
        }

        // ���_�̕ϊ�
        if (worldpos == 1)
        {
            float3 scaledCornerPos = cornerPos * float3(p.scale, 1.0f);
            float3 worldPosition = worldviewpos.xyz + scaledCornerPos;
            element.position = mul(float4(worldPosition, 1.0f), projection);
        }
        else
        {
            float3 scaledCornerPos = cornerPos * float3(p.scale, 1.0f);
            float3 worldPosition = viewpos.xyz + scaledCornerPos;
            element.position = mul(float4(worldPosition, 1.0f), projection);
        }
        element.color.rgb = p.color.rgb * power * baseColor.rgb;
        element.color.a = p.color.a * baseColor.a;
        element.color.rgb *= colorScale;
        element.texcoord = TEXCOORD[i];
        output.Append(element);
    }

    output.RestartStrip();
}
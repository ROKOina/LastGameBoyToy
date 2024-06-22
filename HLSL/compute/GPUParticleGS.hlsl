#include "GPUparticle.hlsli"
#include "../Constants.hlsli"

StructuredBuffer<MainParticle> particlebuffer : register(t0);

// �N�H�[�^�j�I����]�֐�
float3 QuaternionRotate(float3 position, float4 q)
{
    float3 u = q.xyz;
    float s = q.w;

    float3 crossUPos = cross(u, position);
    float dotUPos = dot(u, position);
    float dotUU = dot(u, u);

    return 2.0f * dotUPos * u + (s * s - dotUU) * position + 2.0f * s * crossUPos;
}

[maxvertexcount(4)]
void main(point VS_OUT input[1], inout TriangleStream<GS_OUT> output)
{
    //�r���{�[�h�ƃe�N�X�R�[�h
    float3 BILLBOARD[] =
    {
        float3(-0.5f * scale.x, +0.5f * scale.y, 0.0f), // ����
	    float3(+0.5f * scale.x, +0.5f * scale.y, 0.0f), // �E��
        float3(-0.5f * scale.x, -0.5f * scale.y, 0.0f), // ����
	    float3(+0.5f * scale.x, -0.5f * scale.y, 0.0f), // �E��
    };
    const float2 TEXCOORD[] =
    {
        float2(0.0f, 0.0f), // ����
	    float2(1.0f, 0.0f), // �E��
        float2(0.0f, 1.0f), // ����
	    float2(1.0f, 1.0f), // �E��
    };

    //���̂����
    MainParticle p = particlebuffer[input[0].vertex_id];

    //���W�ϊ�(���͂ƈʒu)
    float4 viewpos = mul(float4(p.position, 1.0), view);
    float4 viewvelo = mul(float4(p.strechvelocity, 0.0), view);

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
        float3 cornerPos = 0;
        //�X�g���b�`�r���{�[�h���g�p���邩�ۂ��̃t���O
        if (strechflag == 0)
        {
            cornerPos = BILLBOARD[i] * float3(p.scale, 1.0f);
        }
        else
        {
            cornerPos = BILLBOARD[i] * viewvelo.xyz * float3(p.scale, 1.0f);
        }
        cornerPos = QuaternionRotate(cornerPos, p.rotation);
        element.position = mul(float4(viewpos.xyz + cornerPos, 1.0f), projection);
        element.color = p.color * color;
        element.color.rgb *= luminance;
        element.texcoord = TEXCOORD[i];
        output.Append(element);
    }

    output.RestartStrip();
}
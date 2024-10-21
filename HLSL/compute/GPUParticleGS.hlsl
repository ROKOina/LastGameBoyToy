#include "GPUparticle.hlsli"
#include "../Constants.hlsli"
#include "..\\3D\\Light.hlsli"

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

    //�ȈՓI�ȃ��C�e�B���O�v�Z
    float3 n = float3(1, 1, 1); // �p�[�e�B�N���̖ʖ@��
    float3 N = normalize(n);
    float3 L = normalize(-directionalLight.direction.xyz);
    float d = dot(L, N);
    float power = max(0, d) * 0.5f + 0.5f;

    //���̂����
    MainParticle p = particlebuffer[input[0].vertex_id];

    //���W�ϊ�(���͂ƈʒu)
    float4 viewpos = mul(float4(p.position.xyz, 1.0), view);
    float4 viewvelo = float4(p.velocity.xyz, 0.0);

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
        if (strechflag == 1 && length(p.velocity.xyz) > 0.001f)
        {
           // �p�[�e�B�N���̑��x�����Ɋ�Â��ĐL�΂�
            float3 stretchDirection = normalize(viewvelo.xyz);
            float stretchAmount = length(viewvelo.xyz) * 0.5f; // �X�g���b�`�̋��x�͑��x�Ɋ�Â�

            // �L�΂����ɉ����ăr���{�[�h�̈ꕔ��L�΂�
            float3 stretchedBillboard = BILLBOARD[i];
            stretchedBillboard.x += stretchDirection.x * stretchAmount * (BILLBOARD[i].x > 0 ? 1 : -1);
            stretchedBillboard.y += stretchDirection.y * stretchAmount * (BILLBOARD[i].y > 0 ? 1 : -1);
            stretchedBillboard.z += stretchDirection.z * stretchAmount * (BILLBOARD[i].z > 0 ? 1 : -1);

            cornerPos = stretchedBillboard * float3(p.scale, 1.0f);
        }
        else
        {
            cornerPos = BILLBOARD[i] * float3(p.scale, 1.0f);
        }
        cornerPos = QuaternionRotate(cornerPos, p.rotation);
        element.position = mul(float4(viewpos.xyz + cornerPos, 1.0f), projection);
        element.color.rgb = p.color.rgb * power * baseColor.rgb;
        element.color.a = p.color.a * baseColor.a;
        element.color.rgb *= colorScale;
        element.texcoord = TEXCOORD[i];
        output.Append(element);
    }

    output.RestartStrip();
}
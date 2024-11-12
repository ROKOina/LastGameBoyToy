#include "CPUParticle.hlsli"

float4 QuaternionRotate(float3 position, float4 q)
{
    float3 u = q.xyz;
    float s = q.w;
    return float4(
        (2.0f * dot(u, position) * u) +
        ((s * s - dot(u, u)) * position) +
        (2.0f * s * cross(u, position)), 1.0f);
}

[maxvertexcount(4)]
void main(point GS_IN In[1], inout TriangleStream<PS_IN> ParticleStream)
{
    // ���W�ϊ� (���[���h���W�n �� �r���[���W�n)
    float4 pos = mul(float4(In[0].Position, 1.0), view);

    // �N�H�[�^�j�I���̒�`
    float4 q = In[0].Rotate;

    // �_��ʂɂ���(�S���_�����)
    float3 rightDir = QuaternionRotate(float3(1.0, 0.0, 0.0), q).xyz;
    float3 upDir = QuaternionRotate(float3(0.0, 1.0, 0.0), q).xyz;

    // �����T�C�Y���v�Z�i�s�v�Ȍv�Z������邽�߁A���O�ɃX�P�[�����|�����x�N�g�����쐬�j
    float3 halfRight = rightDir * (In[0].Size.x * 0.5);
    float3 halfUp = upDir * (In[0].Size.y * 0.5);

    // �e���_�ʒu���v�Z
    float4 pos_left_top = float4(pos.xyz - halfRight + halfUp, 1.0f);
    float4 pos_left_bottom = float4(pos.xyz - halfRight - halfUp, 1.0f);
    float4 pos_right_top = float4(pos.xyz + halfRight + halfUp, 1.0f);
    float4 pos_right_bottom = float4(pos.xyz + halfRight - halfUp, 1.0f);

    // UV�}�b�s���O�ݒ�
    uint type = (int) In[0].Param.y;
    uint komax = (int) In[0].Param.z;
    int komay = (int) In[0].Param.w;
    float w = 1.0 / komax;
    float h = 1.0 / komay;
    float2 uv_base = float2((type % komax) * w, (type / komax) * h);

    // PS_IN�\���̂̋��ʐݒ�
    PS_IN Out = (PS_IN) 0;
    Out.Color = In[0].Color; // ���ʐF��ݒ�

    // ����̒��_
    Out.Position = mul(pos_left_top, projection);
    Out.Size = uv_base; // �e�N�X�`������
    ParticleStream.Append(Out);

    // �E��̒��_
    Out.Position = mul(pos_right_top, projection);
    Out.Size = uv_base + float2(w, 0); // �e�N�X�`���E��
    ParticleStream.Append(Out);

    // �����̒��_
    Out.Position = mul(pos_left_bottom, projection);
    Out.Size = uv_base + float2(0, h); // �e�N�X�`������
    ParticleStream.Append(Out);

    // �E���̒��_
    Out.Position = mul(pos_right_bottom, projection);
    Out.Size = uv_base + float2(w, h); // �e�N�X�`���E��
    ParticleStream.Append(Out);

    ParticleStream.RestartStrip();
}
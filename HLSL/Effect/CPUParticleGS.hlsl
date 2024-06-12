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
    float4 q = float4(In[0].Rotate.x, In[0].Rotate.y, In[0].Rotate.z, In[0].Rotate.w);

    // �_��ʂɂ���(�S���_�����)
    float rot = In[0].Param.x;
    float s = sin(rot);
    float c = cos(rot);
    float3 rightDir = QuaternionRotate(float3(1.0, 0.0, 0.0), q).xyz;
    float3 upDir = QuaternionRotate(float3(0.0, 1.0, 0.0), q).xyz;

    float4 right = float4(rightDir * (In[0].Size.x * 0.5), 0);
    float4 up = float4(upDir * (In[0].Size.y * 0.5), 0);

    float4 pos_left_top = pos - right + up;
    float4 pos_left_bottom = pos - right - up;
    float4 pos_right_top = pos + right + up;
    float4 pos_right_bottom = pos + right - up;

    // �^�C�v�ݒ�
    uint type = (int) In[0].Param.y;
    uint komax = (int) In[0].Param.z;
    int komay = (int) In[0].Param.w;
    float w = 1.0 / komax;
    float h = 1.0 / komay;
    float2 uv = float2((type % komax) * w, (type / komax) * h);
    
    // ����̓_�̈ʒu(�ˉe���W�n)�EUV�E�F���v�Z���ďo��
    PS_IN Out = (PS_IN) 0;
    Out.Color = In[0].Color;
    Out.Position = mul(pos_left_top, projection);
    Out.Size = uv + float2(0, 0); // �e�N�X�`������
    ParticleStream.Append(Out);
    
    // �E��̓_�̈ʒu(�ˉe���W�n) �ƃe�N�X�`�����W�̌v�Z�����ďo��
    Out.Color = In[0].Color;
    Out.Position = mul(pos_right_top, projection);
    Out.Size = uv + float2(w, 0); // �e�N�X�`��
    ParticleStream.Append(Out);

    // �����̓_�̈ʒu(�ˉe���W�n) �ƃe�N�X�`�����W�̌v�Z�����ďo��
    Out.Color = In[0].Color;
    Out.Position = mul(pos_left_bottom, projection);
    Out.Size = uv + float2(0, h); // �e�N�X�`��
    ParticleStream.Append(Out);

    // �E���̓_�̈ʒu(�ˉe���W�n) �ƃe�N�X�`�����W�̌v�Z�����ďo��
    Out.Color = In[0].Color;
    Out.Position = mul(pos_right_bottom, projection);
    Out.Size = uv + float2(w, h); // �e�N�X�`��
    ParticleStream.Append(Out);
    
    ParticleStream.RestartStrip();
}
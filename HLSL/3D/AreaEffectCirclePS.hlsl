#include "Defalt.hlsli"
#include "../Constants.hlsli"
#include "../Common.hlsli"

cbuffer EffectCircleConstants : register(b0)
{
    float simulateTime1;
    float simulateTime2;
    float simulateTime3;
    float waveEffectRange;
    
    float4 waveEffectColor;
    
    float waveEffectIntensity;
    float3 dummy;
}

Texture2D OuterCircle : register(t0); //�O���̃T�[�N��
Texture2D MiddleCircle : register(t1); //���Ԃ̃T�[�N��
Texture2D CenterCircle : register(t2); //���S���̃T�[�N��
Texture2D EffectRamp : register(t3); //�T�[�N�����̌��̔g�p�̃����v�e�N�X�`��

// UV����]������
half2 RotateUV(float timer, float2 texcoord)
{
    half angleCos = cos(timer);
    half angleSin = sin(timer);
    half2x2 rotateMatrix = half2x2(angleCos, -angleSin, angleSin, angleCos);
    half2 uv = texcoord - 0.5;

    return mul(uv, rotateMatrix) + 0.5;
}

float4 main(VS_OUT pin) : SV_TARGET
{
    // �O���̃T�[�N��
    float4 color = OuterCircle.Sample(sampler_states[LINEAR], pin.texcoord);
    
    // ���Ԃ̃T�[�N��
    half2 midUV = RotateUV(simulateTime1, pin.texcoord);
    color += MiddleCircle.Sample(sampler_states[LINEAR], midUV);
    
    // ���S�̃T�[�N��
    half2 centerUV = RotateUV(simulateTime2, pin.texcoord);
    color += CenterCircle.Sample(sampler_states[LINEAR], centerUV);
    
    // �G�~�b�V�u�𔽉f
    color.rgb *= emissivecolor * emissiveintensity;
    
    // �����I�Ɍ����I�[��
    half centerDist = length(pin.texcoord * 2.0 - 1.0);
    float wave = EffectRamp.Sample(sampler_states[LINEAR], float2(abs(centerDist) - simulateTime3, 0));
    color += wave * waveEffectColor * waveEffectIntensity * step(centerDist, waveEffectRange) * step(color.a, 0);
    
    // ����Ȃ������ɋ߂��s�N�Z���͔j������
    clip(color.a - EPSILON);
    
    return color;
}
#include "../Common.hlsli"
#include "CPUParticle.hlsli"

Texture2D texturemap : register(t0); // �J���[�e�N�X�`��
Texture2D DissolveMap : register(t1); //DissolveMap

float4 main(PS_IN input) : SV_TARGET
{
    // �e�N�X�`���J���[���擾
    float4 color = texturemap.Sample(sampler_states[LINEAR], input.Size);
    color.rgb = pow(color.rgb, GAMMA);

    // �f�B�]���u�}�b�v���T���v�����O
    float dissolveValue = DissolveMap.Sample(sampler_states[LINEAR], input.Size).r;
    float dalpha = smoothstep(dissolveThreshold, dissolveThreshold, dissolveValue);

    // �J���[�ƃA���t�@�̒���
    color.a *= input.Color.a * cpuparticlecolor.a;
    color.a *= dalpha;
    color.rgb *= input.Color.rgb * cpuparticlecolor.rgb;
    color.rgb *= cpuparticleluminance;

    // �����j��: �A���t�@���Ⴂ�ꍇ
    if (color.a < EPSILON)
        discard;

    return color;
}
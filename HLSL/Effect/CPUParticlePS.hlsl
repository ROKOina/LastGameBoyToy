#include "../Common.hlsli"
#include "CPUParticle.hlsli"

Texture2D texturemap : register(t0); // �J���[�e�N�X�`��

float4 main(PS_IN input) : SV_TARGET
{
 // �e�N�X�`���J���[���擾
    float4 color = texturemap.Sample(sampler_states[LINEAR], input.Size);
    color.rgb = pow(color.rgb, GAMMA);

    // �J���[�ƃA���t�@�̒���
    color.a *= input.Color.a * cpuparticlecolor.a;
    color.rgb *= input.Color.rgb * cpuparticlecolor.rgb;
    color.rgb *= cpuparticleluminance;

    // �����j��: �A���t�@���Ⴂ�ꍇ
    if (color.a < EPSILON)
        discard;

    return color;
}
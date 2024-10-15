#include "../Common.hlsli"
#include "CPUParticle.hlsli"

Texture2D texturemap : register(t0); //�J���[�e�N�X�`��

float4 main(PS_IN input) : SV_Target
{
    float4 color = texturemap.Sample(sampler_states[LINEAR], input.Size);
    color.rgb = pow(color.rgb, GAMMA);
    color.a *= input.Color.a * cpuparticlecolor.a;
    color.rgb *= input.Color.rgb * cpuparticlecolor.rgb;
    color.rgb *= cpuparticleluminance;

    //�A���t�@���Ⴂ�ꍇ�͔j��
    if (color.a < EPSILON)
        discard;

    color.rgb = pow(color.rgb, 1.0 / GAMMA);

    return color;
}
#include "../Common.hlsli"
#include "CPUParticle.hlsli"

Texture2D texturemap : register(t0); //カラーテクスチャ

float4 main(PS_IN input) : SV_Target
{
    float4 color = texturemap.Sample(sampler_states[LINEAR], input.Size) * input.Color * cpuparticlecolor;
    color.rgb *= cpuparticleluminance;
    return color;
}
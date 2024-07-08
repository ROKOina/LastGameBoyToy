#include "GPUParticle.hlsli"
#include "../Common.hlsli"

Texture2D colortexture : register(t20);
Texture2D depthMap : register(t8);

float4 main(GS_OUT pin) : SV_TARGET
{
    clip(colortexture.Sample(sampler_states[LINEAR], pin.texcoord).a - EPSILON);
    return colortexture.Sample(sampler_states[LINEAR], pin.texcoord) * pin.color;
}
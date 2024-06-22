#include "GPUParticle.hlsli"
#include "../Common.hlsli"

Texture2D colortexture : register(t20);

float4 main(GS_OUT pin) : SV_TARGET
{
    return colortexture.Sample(sampler_states[LINEAR], pin.texcoord) * pin.color;
}
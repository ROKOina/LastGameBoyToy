#include "FullScreenQuad.hlsli"
#include "Common.hlsli"

Texture2D texture_maps : register(t0);

float4 main(VS_OUT pin) : SV_TARGET
{
    return texture_maps.Sample(sampler_states[BORDER_POINT], pin.texcoord);
}
#include "../Common.hlsli"
#include "Font.hlsli"

Texture2D texture0 : register(t0);
SamplerState sampler0 : register(s0);

float4 main(VS_OUT pin) : SV_TARGET
{
    float4 color = texture0.Sample(sampler_states[LINEAR], pin.texcoord) * pin.mask;
    float alpha = color.x + color.y + color.z + color.w;
    return float4(pin.color.rgb, pin.color.a * alpha);

}


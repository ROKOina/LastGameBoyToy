#include "FullScreenQuad.hlsli"
#include "PostEffect.hlsli"
#include "../../Common/Common.hlsli"

Texture2D texture_map : register(t0);

// Tone mapping operators
// https://www.shadertoy.com/view/lslGzl
static const float gamma = 2.3;
float3 uncharted2_tonemapping(float3 color, float exposure)
{
    color = max(0., color);

    float A = 0.15;
    float B = 0.50;
    float C = 0.10;
    float D = 0.20;
    float E = 0.02;
    float F = 0.30;
    float W = 11.2;
    color *= exposure;
    color = ((color * (A * color + C * B) + D * E) / (color * (A * color + B) + D * F)) - E / F;
    float white = ((W * (A * W + C * B) + D * E) / (W * (A * W + B) + D * F)) - E / F;
    color /= white;
    color = pow(color, 1. / gamma);
    return color;
}

float4 main(VS_OUT pin) : SV_TARGET
{
    float4 sampled_color = texture_map.Sample(sampler_states[POINT], pin.texcoord.xy);
    float3 fragment_color = sampled_color.rgb;
    float alpha = sampled_color.a;

    fragment_color = uncharted2_tonemapping(fragment_color, exposure);

    return float4(fragment_color * colorize, alpha);
}
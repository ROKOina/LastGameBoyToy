#include "sprite.hlsli"
#include "../Common.hlsli"
#include "../Constants.hlsli"

Texture2D texturemaps : register(t0);

float random(float2 seeds)
{
    return frac(sin(dot(seeds, float2(12.9898, 78.233))) * 43758.5453);
}

float blockNoise(float2 seeds)
{
    return random(floor(seeds));
}

float noiserandom(float2 seeds)
{
    return -1.0 + 2.0 * blockNoise(seeds);
}

float4 main(VS_OUT pin) : SV_TARGET
{
    float4 color;
    float2 gv = pin.texcoord;
    float noise = blockNoise(pin.texcoord.y * 10.0f);
    noise += random(pin.texcoord.x) * 0.3;
    float2 randomvalue = noiserandom(float2(pin.texcoord.y, time * 10.0f));
    gv.x += randomvalue * sin(sin(0.1f) * .5) * sin(-sin(noise) * .2) * frac(time);
    color.r = texturemaps.Sample(sampler_states[TRANSPARENT_BORDER_LINEAR], gv + float2(0.006, 0)).r;
    color.g = texturemaps.Sample(sampler_states[TRANSPARENT_BORDER_LINEAR], gv).g;
    color.b = texturemaps.Sample(sampler_states[TRANSPARENT_BORDER_LINEAR], gv - float2(0.008, 0)).b;
    color.a = texturemaps.Sample(sampler_states[TRANSPARENT_BORDER_LINEAR], pin.texcoord).a;

    //逆ガンマ補正でリニア空間に変換 (通常のガンマ値は 2.2)
    color.rgb = pow(color.rgb, GAMMA);

    color *= pin.color;

    //最終的な色にガンマ補正を適用 (逆ガンマの 1 / 2.2 = 約 0.4545)
    color.rgb = pow(color.rgb, 1.0 / GAMMA);

    return color;
}
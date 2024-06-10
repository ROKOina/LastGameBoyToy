#include "Defalt.hlsli"
#include "../Common.hlsli"
#include "../Constants.hlsli"

Texture2D DiffuseMap : register(t0); //カラーテクスチャ

float4 main(VS_OUT pin) : SV_TARGET
{
    float2 uv = pin.texcoord.xy;

    float w = 0.01 * (0.5 + 0.5 * sin(3.14 * -0.5 + 1.4 * time));

    float d1 = w * sin(2.1 * time + 17.0 * uv.xy.y);
    float d2 = w * sin(2.4 * time + 21.0 * uv.xy.y + 0.7);

    float r = DiffuseMap.Sample(sampler_states[POINT], float2(uv.xy.x /*+ d2*/, uv.xy.y)).r * pin.color.r;
    float g = DiffuseMap.Sample(sampler_states[POINT], float2(uv.xy.x, uv.xy.y /*+ d1*/)).g * pin.color.g;
    float b = DiffuseMap.Sample(sampler_states[POINT], float2(uv.xy.x, uv.xy.y)).b * pin.color.b;
    float a = DiffuseMap.Sample(sampler_states[POINT], uv).a * pin.color.a;
    float4 color = float4(r * 3.5f, g * 3.5f, b * 3.5f, a);

    return color;
}
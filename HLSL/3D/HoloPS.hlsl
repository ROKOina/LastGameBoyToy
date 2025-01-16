#include "Defalt.hlsli"
#include "../Constants.hlsli"
#include "../Common.hlsli"

float absin(float t)
{
    return 0.5 + sin(t) * 0.5;
}

float ababsin(float t)
{
    return 0.75 + sin(t) * 0.25;
}

float abababsin(float t)
{
    return 0.7 + sin(t) * 0.24;
}

float absincos(float t, float n)
{
    return clamp(abababsin(n * 3.14159 * cos(t)), 0., 1.);
}

Texture2D HoloMap : register(t0);
Texture2D EmissiveHoloMap : register(t5);

float4 main(VS_OUT pin) : SV_TARGET
{
    float2 uv = pin.texcoord.xy / pin.position.xy;
    float4 base = materialcolor;
    float4 game = HoloMap.Sample(sampler_states[LINEAR], uv);
    float3 emissivegame = EmissiveHoloMap.Sample(sampler_states[LINEAR], uv).rgb * emissivecolor * emissiveintensity;

    float L = 2800. + 50.0 * cos(time / 5.);
    float P = 25.;
    float N = 27.0;
    float M = 250.0 + 50.0 * sin(time / 5.);
    float x = pin.position.x / pin.texcoord.x;
    float y = pin.position.y / pin.texcoord.y;
    float a = absincos((x - 2.0 * y) / L + time * .15, P) * absincos(time * .3 + y / M, N);
    float4 color = base * a + game * (1. - a);
    color.rgb = color.rgb * emissivegame;

    return color;
}
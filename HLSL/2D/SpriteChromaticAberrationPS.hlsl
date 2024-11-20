#include "sprite.hlsli"
#include "../Common.hlsli"
#include "../Constants.hlsli"

Texture2D texturemaps : register(t0);

float4 main(VS_OUT pin) : SV_TARGET
{
    float2 uv = pin.texcoord.xy;
    float w = 0.01 * (0.5 + 0.5 * sin(3.14 * -0.5 + 1.4 * time));
    float d1 = w * sin(2.1 * time + 17.0 * uv.xy.y);
    float d2 = w * sin(2.4 * time + 21.0 * uv.xy.y + 0.7);
    float r = texturemaps.Sample(sampler_states[TRANSPARENT_BORDER_LINEAR], float2(uv.xy.x + d2, uv.xy.y)).r;
    float g = texturemaps.Sample(sampler_states[TRANSPARENT_BORDER_LINEAR], float2(uv.xy.x, uv.xy.y + d1)).g;
    float b = texturemaps.Sample(sampler_states[TRANSPARENT_BORDER_LINEAR], float2(uv.xy.x, uv.xy.y)).b;
    float a = texturemaps.Sample(sampler_states[TRANSPARENT_BORDER_LINEAR], uv).a;
    float4 color = float4(r, g, b, a);

    color.rgb = pow(color.rgb, GAMMA);

    color.a *= pin.color.a;
    color.rgb *= pin.color.rgb;

    //ÉAÉãÉtÉ@Ç™í·Ç¢èÍçáÇÕîjä¸
    if (color.a < EPSILON)
        discard;

    color.rgb = pow(color.rgb, 1.0 / GAMMA);

    return color;
}
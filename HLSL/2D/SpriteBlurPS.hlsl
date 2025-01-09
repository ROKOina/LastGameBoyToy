#include "sprite.hlsli"
#include "../Common.hlsli"
#include "../Constants.hlsli"

Texture2D texturemaps : register(t0);

// Ç⁄Ç©Çµópä÷êî
float4 blur(float2 uv, float w, float2 offset)
{
    float2 shiftUv = uv + offset * blurdistance;
    float4 tex = texturemaps.Sample(sampler_states[TRANSPARENT_BORDER_LINEAR], shiftUv);
    tex.a *= w * blurpower * luminance;
    return tex;
}

float4 main(VS_OUT pin) : SV_TARGET
{
    float4 col = 0;

    for (int i = 1; i <= 6; i++)
    {
        float weight = 0.01 * i;
        float2 offsets[4] =
        {
            float2(0, -0.0075 + 0.0015 * i),
        float2(0, +0.0075 - 0.0015 * i),
        float2(-0.0075 + 0.0015 * i, 0),
        float2(+0.0075 - 0.0015 * i, 0),
        };

        for (int j = 0; j < 4; j++)
        {
            col += blur(pin.texcoord, weight, offsets[j]);
        }
    }

    col.rgb = col.rgb * blurcolor;

    return col;
}
#include "sprite.hlsli"
#include "../Common.hlsli"

Texture2D texturemaps : register(t0);

float4 main(VS_OUT pin) : SV_TARGET
{
    float4 color = texturemaps.Sample(sampler_states[POINT], pin.texcoord) * pin.color;
    if (color.a < 0.00001)
        discard;; //アルファ値がないなら書き込まない

    return color;
}

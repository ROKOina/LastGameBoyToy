#include "GPUParticle.hlsli"
#include "../Common.hlsli"

Texture2D colortexture : register(t20);
Texture2D depthMap : register(t8);

float4 main(GS_OUT pin) : SV_TARGET
{
    //テクスチャから色をサンプルし、リニア空間に変換
    float4 color = colortexture.Sample(sampler_states[LINEAR], pin.texcoord);

    //逆ガンマ補正でリニア空間に変換 (通常のガンマ値は 2.2)
    color.rgb = pow(color.rgb, GAMMA);

    //ピンの色とアルファを掛け算
    color.a *= pin.color.a;
    color.rgb *= pin.color.rgb;

    //アルファが低い場合は破棄
    if (color.a < EPSILON)
        discard;

    //最終的な色にガンマ補正を適用 (逆ガンマの 1 / 2.2 = 約 0.4545)
    color.rgb = pow(color.rgb, 1.0 / GAMMA);

    return color;
}
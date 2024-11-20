#include "GPUParticle.hlsli"
#include "../Common.hlsli"

Texture2D colortexture : register(t20);
Texture2D depthmap : register(t8); // 深度テクスチャ（ソフトパーティクル用）

float4 main(GS_OUT pin) : SV_TARGET
{
    //テクスチャから色をサンプルし、リニア空間に変換
    float4 color = colortexture.Sample(sampler_states[LINEAR], pin.texcoord);

    //逆ガンマ補正でリニア空間に変換 (通常のガンマ値は 2.2)
    color.rgb = pow(color.rgb, GAMMA);

    //ピンの色とアルファを掛け算
    color.a *= pin.color.a;
    color.rgb *= pin.color.rgb;

    // ソフトパーティクル処理
    //float particleDepth = pin.position.z / pin.position.w; // パーティクルの深度
    //float sceneDepth = depthmap.Sample(sampler_states[LINEAR], pin.texcoord).r; // シーンの深度

     // パーティクルとシーンの深度差を計算
    //float depthDifference = sceneDepth - particleDepth;

    // フェード範囲の調整（0.1～0.5の範囲でフェードを行う）
    //float fadeFactor = saturate((depthDifference - 0.1) / (0.5 - 0.1));
    //color.a *= fadeFactor;

    //アルファが低い場合は破棄
    if (color.a < EPSILON)
        discard;

    //0以下にしないようにする
    color.rgb = max(color.rgb, 0);

    return color;
}
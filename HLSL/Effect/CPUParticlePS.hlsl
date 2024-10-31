#include "../Common.hlsli"
#include "CPUParticle.hlsli"

Texture2D texturemap : register(t0); // カラーテクスチャ
Texture2D depthmap : register(t8); // 深度テクスチャ（ソフトパーティクル用）

float4 main(PS_IN input) : SV_Target
{
    // テクスチャカラーを取得
    float4 color = texturemap.Sample(sampler_states[LINEAR], input.Size);
    color.rgb = pow(color.rgb, GAMMA);

    // カラーとアルファの調整
    color.a *= input.Color.a * cpuparticlecolor.a;
    color.rgb *= input.Color.rgb * cpuparticlecolor.rgb;
    color.rgb *= cpuparticleluminance;

    // ソフトパーティクル処理
    float particleDepth = input.Position.z / input.Position.w; // パーティクルの深度
    float sceneDepth = depthmap.Sample(sampler_states[LINEAR], input.Size).r; // シーンの深度

    // パーティクルとシーンの深度差を計算
    float depthDifference = sceneDepth - particleDepth;

    // フェード範囲の調整（0.1～0.5の範囲でフェードを行う）
    float fadeFactor = saturate((depthDifference - 0.1) / (0.5 - 0.1));
    color.a *= fadeFactor;

    // アルファが低い場合は破棄
    if (color.a < EPSILON)
        discard;

    // ガンマ補正
    color.rgb = pow(color.rgb, 1.0 / GAMMA);

    return color;
}
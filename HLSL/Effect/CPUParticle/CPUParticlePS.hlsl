#include "../../Common/Common.hlsli"
#include "CPUParticle.hlsli"

Texture2D texturemap : register(t0); // カラーテクスチャ
Texture2D depthmap : register(t8); // 深度テクスチャ（ソフトパーティクル用）

float4 main(PS_IN input) : SV_TARGET
{
    // テクスチャカラーを取得し、リニア空間に変換
    float4 color = texturemap.Sample(sampler_states[LINEAR], input.Size);

    // ガンマ補正の近似 (通常のガンマ値が2.2のため、sqrtで近似)
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

    // カメラの距離を取得（仮にカメラ位置が (0, 0, 0) の場合）
    float cameraDistance = length(cameraposition); // カメラからパーティクルまでの距離

    // フェード範囲を動的に設定
    float minFadeRange = 0.05; // 最小のフェード開始位置
    float maxFadeRange = 0.2; // 最小のフェード終了位置

    if (cameraDistance < 5.0) // カメラが近い場合
    {
        minFadeRange = 0.01; // フェード開始位置を狭める
        maxFadeRange = 0.1; // フェード終了位置も狭める
    }

    // フェード範囲の調整
    float fadeFactor = saturate((depthDifference - minFadeRange) / (maxFadeRange - minFadeRange));
    color.a *= fadeFactor;

    // アルファが極端に低い場合は破棄せず、RGBをゼロに
    if (color.a < EPSILON)
    {
        color.rgb = float3(0, 0, 0);
        color.a = 0;
    }

    return color;
}
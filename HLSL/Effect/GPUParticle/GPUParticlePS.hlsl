#include "GPUParticle.hlsli"
#include "../../Common/Common.hlsli"
#include "../../Common/Constants.hlsli"

Texture2D colortexture : register(t20);
Texture2D depthmap : register(t8); // 深度テクスチャ（ソフトパーティクル用）

float4 main(GS_OUT pin) : SV_TARGET
{
    // テクスチャから色をサンプルし、リニア空間に変換
    float4 color = colortexture.Sample(sampler_states[LINEAR], pin.texcoord);

    // ガンマ補正を近似的に行う (通常のガンマ値が 2.2 の場合、sqrtで近似)
    color.rgb = sqrt(color.rgb);

    // ピンの色とアルファを掛け算
    color.a *= pin.color.a;
    color.rgb *= pin.color.rgb;

    // ソフトパーティクル処理
    float particleDepth = pin.position.z / pin.position.w; // パーティクルの深度
    float sceneDepth = depthmap.Sample(sampler_states[LINEAR], pin.texcoord).r; // シーンの深度

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

    // アルファが非常に低い場合はRGB値をそのままにする（discard の使用を避ける）
    if (color.a < EPSILON)
    {
        color.rgb = float3(0, 0, 0);
        color.a = 0;
    }

    // 0以下にしないようにする
    color.rgb = max(color.rgb, 0);

    return color;
}
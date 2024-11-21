#include "../Common.hlsli"
#include "CPUParticle.hlsli"

Texture2D texturemap : register(t0); // カラーテクスチャ
Texture2D depthmap : register(t8); // 深度テクスチャ（ソフトパーティクル用）

float4 main(PS_IN input) : SV_TARGET
{
 // テクスチャカラーを取得
    float4 color = texturemap.Sample(sampler_states[LINEAR], input.Size);
    color.rgb = pow(color.rgb, GAMMA);

    // カラーとアルファの調整
    color.a *= input.Color.a * cpuparticlecolor.a;
    color.rgb *= input.Color.rgb * cpuparticlecolor.rgb;
    color.rgb *= cpuparticleluminance;

    // 早期破棄: アルファが低い場合
    if (color.a < EPSILON)
        discard;

    // 深度フェード用変数（ソフトパーティクル）
    float fadeFactor = 1.0;

    // LODの導入: カメラ距離に応じて計算の省略
    // パーティクルのスクリーン上の大きさを確認
    float particleSize = abs(input.Size.x * input.Position.w); // 視点空間でのパーティクルサイズ
    if (particleSize > 0.01) // 一定以上のサイズのみ深度計算を適用
    {
        // ソフトパーティクル処理
        float particleDepth = input.Position.z / input.Position.w; // パーティクルの深度
        float sceneDepth = depthmap.Sample(sampler_states[LINEAR], input.Size).r; // シーンの深度

        // パーティクルとシーンの深度差を計算
        float depthDifference = sceneDepth - particleDepth;

        // 深度差が範囲内かチェックし、範囲内の場合のみフェード処理を行う
        fadeFactor = saturate((depthDifference - 0.1) / (0.5 - 0.1));
        fadeFactor = lerp(0.0, 1.0, fadeFactor); // フェードの線形補間
    }

    // アルファ値にフェードファクターを適用
    color.a *= fadeFactor;

    // 再度破棄: アルファが低い場合
    if (color.a < EPSILON)
        discard;

    return color;
}
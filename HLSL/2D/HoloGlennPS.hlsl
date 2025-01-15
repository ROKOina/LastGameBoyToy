#include "sprite.hlsli"
#include "../Common.hlsli"
#include "../Constants.hlsli"

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

Texture2D texturemaps : register(t0);

float3 HueShift(float3 color, float shift)
{
    float angle = shift * 3.14159 * 2.0; // 色相のシフト角度
    float3 k = float3(0.57735, 0.57735, 0.57735); // 標準的なRGB軸の正規化ベクトル
    float3 p = cos(angle) * color + sin(angle) * cross(k, color) + (1.0 - cos(angle)) * dot(k, color) * k;
    return saturate(p);
}

float4 main(VS_OUT pin) : SV_TARGET
{
    // テクスチャから色をサンプルし、リニア空間に変換
    float4 color = texturemaps.Sample(sampler_states[TRANSPARENT_BORDER_LINEAR], pin.texcoord);

    //フラグなら
    if (onflag)
    {
        return color;
    }

    // 逆ガンマ補正でリニア空間に変換 (通常のガンマ値は 2.2)
    color.rgb = pow(color.rgb, GAMMA);

    // ピンの色とアルファを掛け算
    color.a *= pin.color.a;
    color.rgb *= pin.color.rgb;

    // アルファが低い場合は破棄
    if (color.a < EPSILON)
        discard;

    // 背景色 (ベース色) とエフェクトを設定
    float4 base = float4(0.8, 0.8, 0.8, 0.4);
    float glow = absin(time * 0.01) * 0.01 + 0.01; // 点滅速度をさらに遅く調整

    // アニメーション係数の計算
    float L = 2800.0 + 50.0 * cos(time / 5000.0); // 動きをさらにゆっくりに調整
    float P = 25.0;
    float N = 27.0; // should be #defines
    float M = 250.0 + 50.0 * sin(time / 5000.0); // 動きをさらにゆっくりに調整
    float x = pin.texcoord.x;
    float y = pin.texcoord.y;

    float a = absincos((x - 2.0 * y) / L + time * 0.01875, P) * absincos(time * 0.0375 + y / M, N);

    // 色相を時間に応じてシフト
    color.rgb = HueShift(color.rgb, absin(time * 0.8)); // 色の変化速度も大幅に遅く

    // 輝きを追加
    color.rgb += glow * 0.2;

    // テクスチャ色とエフェクトを組み合わせる
    color.rgb = lerp(base.rgb, color.rgb, a);

    // 最終的な色にガンマ補正を適用 (逆ガンマの 1 / 2.2 = 約 0.4545)
    color.rgb = pow(color.rgb, 1.0 / GAMMA);

    return color;
}
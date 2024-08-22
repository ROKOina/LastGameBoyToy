#include "sprite.hlsli"
#include "../Common.hlsli"

Texture2D texturemaps : register(t0);
Texture2D noisemap : register(t1);
Texture2D rampmap : register(t2);

float delayTime = 0.4; // Clip Timeの変換に使う遅延時間

float4 main(VS_OUT pin) : SV_TARGET
{
    // テクスチャから色をサンプルし、ピンのカラーを掛け算
    float4 color = texturemaps.Sample(sampler_states[POINT], pin.texcoord) * pin.color;

    // ノイズマップからノイズ値をサンプル
    float noise = noisemap.Sample(sampler_states[POINT], pin.texcoord).r;

     // ノイズ値にスムージングを適用
    float smoothedNoise = smoothstep(0.0, 1.0, noise);

    // Clip Timeを0~1から0~1.4にマップ
    float clipTime = 1.0 + delayTime; // Clip Timeの範囲上限 (1.0 + 0.4)
    float mappedClipTime = (clipTime - 1.0) / delayTime * smoothedNoise;

    // モデルを塗りつぶすためのノイズ二値化
    float alphaClip = step(cliptime, smoothedNoise);

    // フチを光らせるための処理
    float edgeGlow = smoothstep(edgethreshold - edgeoffset, edgethreshold, smoothedNoise);

    //Clip Threshold が 0 のとき、フチの処理を無視
    if (cliptime <= 0.0)
    {
        // アルファクリップの処理を行わず、そのまま色を返す
        return color;
    }

    // ランプマップからカラーグラデーションをサンプル
    // ノイズ値を0~1の範囲で使ってカラーを補間
    float4 gradientColor = rampmap.Sample(sampler_states[POINT], float2(smoothedNoise, 0.0));

    // ノイズの値が Clip Time を下回る部分を 0 にする処理
    float maskedColor = step(cliptime, smoothedNoise); // クリッピングのためのマスク

    // Clip Threshold が 0 より大きい場合の処理
    // ノイズ値に基づいて色のRGB成分を調整
    color.rgb *= maskedColor; // ノイズの色が ClipTime を下回る部分を 0 にする

    // フチの光り具合を色に追加
    color.rgb += edgeGlow * edgecolor; // 光の色を調整

    // ランプマップで着色
    color.rgb *= gradientColor.rgb;

    // 範囲変換後のClip Timeに基づくアルファテスト
    if (color.a < 0.00001 || alphaClip < mappedClipTime)
        discard;

    return color;
}
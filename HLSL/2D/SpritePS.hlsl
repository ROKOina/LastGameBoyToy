#include "sprite.hlsli"
#include "../Common.hlsli"
#include "../Constants.hlsli"

Texture2D texturemaps : register(t0);
Texture2D noisemap : register(t1);
Texture2D rampmap : register(t2);

// ランダムな値を生成
float random(float2 seeds)
{
    return frac(sin(dot(seeds, float2(12.9898, 78.233))) * 43758.5453);
}

// ブロック状のノイズ生成
float blockNoise(float2 seeds)
{
    return random(floor(seeds));
}

// -1から1までの範囲でランダム値を生成
float noiserandom(float2 seeds)
{
    return -1.0 + 2.0 * blockNoise(seeds);
}

// HSLからRGBへの変換関数
float3 HSLtoRGB(float h, float s, float l)
{
    float3 rgb = l.xxx;
    if (s != 0)
    {
        float q = l < 0.5 ? l * (1.0 + s) : l + s - l * s;
        float p = 2.0 * l - q;
        float3 t = float3(h + 1.0 / 3.0, h, h - 1.0 / 3.0);

        for (int i = 0; i < 3; i++)
        {
            if (t[i] < 0.0)
                t[i] += 1.0;
            if (t[i] > 1.0)
                t[i] -= 1.0;
            if (t[i] < 1.0 / 6.0)
                rgb[i] = p + (q - p) * 6.0 * t[i];
            else if (t[i] < 1.0 / 2.0)
                rgb[i] = q;
            else if (t[i] < 2.0 / 3.0)
                rgb[i] = p + (q - p) * (2.0 / 3.0 - t[i]) * 6.0;
            else
                rgb[i] = p;
        }
    }
    return rgb;
}

float delayTime = 0.4; // Clip Timeの変換に使う遅延時間

float4 main(VS_OUT pin) : SV_TARGET
{
    //// グリッチエフェクトの初期設定
    //float2 gv = pin.texcoord;
    //float glichnoise = blockNoise(pin.texcoord.y * 10);
    //glichnoise += random(pin.texcoord.x) * 0.3;
    //float2 randomvalue = noiserandom(float2(pin.texcoord.y, time * 10));
    //gv.x += randomvalue * sin(sin(0.1) * .5) * sin(-sin(glichnoise) * .2) * frac(time) * 0;

    //// ホログラム風の色変化を計算 (HSLを利用)
    //float glitchTimeFactor = frac(time * 2.0); // 色が変わる頻度を調整
    //float hue = frac(pin.texcoord.y + glitchTimeFactor); // y座標と時間に基づいて色相を変化
    //float saturation = 0.9; // 高い彩度で鮮やかな色
    //float luminance = 0.5 + 0.1 * sin(time * 3.0); // 時間と共に変化する輝度
    //float4 glitchRGB = float4(HSLtoRGB(hue, saturation, luminance), 1.0f);

    // テクスチャから色をサンプルし、ピンのカラーを掛け算
    float4 color = texturemaps.Sample(sampler_states[LINEAR], pin.texcoord) /** glitchRGB*/;
    color.a *= pin.color.a;
    if (!(color.a - EPSILON))
        discard;
    color.rgb = pow(color.rgb, max(GAMMA, 1)) * pin.color.rgb;

    //// ノイズマップからノイズ値をサンプル
    //float noise = noisemap.Sample(sampler_states[POINT], pin.texcoord).r;

    // // ノイズ値にスムージングを適用
    //float smoothedNoise = smoothstep(0.0, 1.0, noise);

    //// Clip Timeを0~1から0~1.4にマップ
    //float clipTime = 1.0 + delayTime; // Clip Timeの範囲上限 (1.0 + 0.4)
    //float mappedClipTime = (clipTime - 1.0) / delayTime * smoothedNoise;

    //// モデルを塗りつぶすためのノイズ二値化
    //float alphaClip = step(cliptime, smoothedNoise);

    //// フチを光らせるための処理
    //float edgeGlow = smoothstep(edgethreshold - edgeoffset, edgethreshold, smoothedNoise);

    //// ランプマップからカラーグラデーションをサンプル
    //// ノイズ値を0~1の範囲で使ってカラーを補間
    //float4 gradientColor = rampmap.Sample(sampler_states[POINT], float2(smoothedNoise, 0.0));

    //// ノイズの値が Clip Time を下回る部分を 0 にする処理
    //float maskedColor = step(cliptime, smoothedNoise); // クリッピングのためのマスク

    ////Clip Threshold が 0 のとき、フチの処理を無視
    //if (cliptime <= EPSILON)
    //{
    //    // アルファクリップの処理を行わず、そのまま色を返す
    //    return color;
    //}

    //// Clip Threshold が 0 より大きい場合の処理
    //// ノイズ値に基づいて色のRGB成分を調整
    //color.rgb *= maskedColor; // ノイズの色が ClipTime を下回る部分を 0 にする

    //// フチの光り具合を色に追加
    //color.rgb += edgeGlow * edgecolor; // 光の色を調整

    //// ランプマップで着色
    //color.rgb *= gradientColor.rgb;

    //// 範囲変換後のClip Timeに基づくアルファテスト
    //if (color.a < EPSILON || alphaClip < mappedClipTime)
    //    discard;

    return color;
}
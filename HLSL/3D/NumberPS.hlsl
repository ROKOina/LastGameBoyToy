#include "Defalt.hlsli"
#include "../Constants.hlsli"
#include "../Common.hlsli"

// UVから六角形タイルを出力
float hex(float2 uv, float scale = 1)
{
    float2 p = uv * scale;
    p.x *= 1.15470053838; // x座標を2/√3倍 (六角形の横方向の大きさが√3/2倍になる)
    float isTwo = frac(floor(p.x) / 2.0) * 2.0; // 偶数列目なら1.0
    p.y += isTwo * 0.5; // 偶数列目を0.5ずらす
    p = frac(p) - 0.5;
    p = abs(p); // 上下左右対称にする
                // 六角形タイルとして出力
    return abs(max(p.x * 1.5 + p.y, p.y * 2.0) - 1.0);
}

// メインシェーダー
float4 main(VS_OUT pin) : SV_TARGET
{
    // 時間に基づいてタイルの変形や動きを変える
    float scale = 50 + 20 * sin(time * 2.0); // タイルのスケールを時間で振動させる

    // 六角形描画のUVを利用して補間値を計算
    float interpolation = hex(pin.texcoord, scale);

    // 色を時間で変化させる
    float3 color = lerp(float3(0.0, 0.5, 1.0), float3(1.0, 0.0, 0.0), sin(time * 0.5) * 0.5 + 0.5); // 青から赤に変化

    // 六角形描画のUVを利用してアルファを塗分け
    float alpha = lerp(1, 0, interpolation);

    // アルファ値が1未満の場合に、視覚的にタイルが消えるように処理
    alpha = max(alpha, 0.2); // 完全に消えないように最低でも0.2のアルファを維持

    clip(alpha); // アルファが0以下ならクリップ

    // 最終的な色とアルファを返す
    return float4(color, alpha); // 時間で変化する色と透明度
}
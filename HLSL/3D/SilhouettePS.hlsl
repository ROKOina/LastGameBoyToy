#include "Defalt.hlsli"
#include "../Common.hlsli"
#include "../Constants.hlsli"

Texture2D DiffuseMap : register(t0);// 色テクスチャ
Texture2D NormalMap : register(t1); // 法線テクスチャ

[earlydepthstencil]
float4 main(VS_OUT pin) : SV_TARGET
{
    return float4(1, 0, 0, 1);

    uint width, height, number_of_levels;
    DiffuseMap.GetDimensions(0, width, height, number_of_levels);
    float2 texelSize = float2(1.0f / width, 1.0f / height);

    // 現在のピクセルの法線を取得
    float3 normal = NormalMap.Sample(sampler_states[LINEAR], pin.texcoord).xyz;

    // 隣接するピクセルの法線を取得
    float3 normalLeft = NormalMap.Sample(sampler_states[LINEAR], pin.texcoord + float2(-1, 0) * texelSize).xyz;
    float3 normalRight = NormalMap.Sample(sampler_states[LINEAR], pin.texcoord + float2(1, 0) * texelSize).xyz;
    float3 normalUp = NormalMap.Sample(sampler_states[LINEAR], pin.texcoord + float2(0, -1) * texelSize).xyz;
    float3 normalDown = NormalMap.Sample(sampler_states[LINEAR], pin.texcoord + float2(0, 1) * texelSize).xyz;

    // 法線の差分を計算
    float diffLeft = length(normal - normalLeft);
    float diffRight = length(normal - normalRight);
    float diffUp = length(normal - normalUp);
    float diffDown = length(normal - normalDown);

    // 法線の差分からSobelフィルタを計算
    float sobelH = diffLeft - diffRight;
    float sobelV = diffUp - diffDown;
    float sobel = sqrt(sobelH * sobelH + sobelV * sobelV);

    // 適応的なしきい値を設定
    float threshold = lerp(0.01, 0.001, saturate(length(pin.texcoord.xy - 0.5f) * 2.0f)); // 距離に応じたしきい値の設定

    // カメラとの距離が非常に近い場合には輪郭線を描画しない
    bool drawOutline = length(pin.texcoord.xy - 0.5f) > 0.1f;

    // Sobel演算に基づくエッジ検出
    float edge = (drawOutline && sobel > threshold) ? 1.0f : 0.0f;

    // アウトラインが検出された場合にのみ色を付ける
    if (edge > 0.0f)
    {
        return float4(1.0, 0.0, 0.0, 1.0f); // アウトライン部分は赤色
    }
    else
    {
        return float4(0.0, 0.0, 0.0, 0.0f); // アウトライン以外は透明
    }
}
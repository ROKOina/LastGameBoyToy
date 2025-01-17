#include "Defalt.hlsli"
#include "../Constants.hlsli"
#include "../Common.hlsli"

// シンプルな補助関数
float absincos(float t, float n)
{
    return 0.7 + 0.24 * sin(n * 3.14159 * cos(t));
}

// テクスチャ
Texture2D HoloMap : register(t0);
Texture2D EmissiveHoloMap : register(t5);

float4 main(VS_OUT pin) : SV_TARGET
{
    // UV座標
    float2 uv = pin.texcoord.xy;

    // 基本色とテクスチャサンプル
    float4 base = materialcolor;
    float4 game = HoloMap.Sample(sampler_states[LINEAR], uv);

    // 発光の計算
    float3 emissivegame = EmissiveHoloMap.Sample(sampler_states[LINEAR], uv).rgb * emissivecolor * emissiveintensity;

    // 動的パラメータ
    float L = 2800.0 + 5.0 * cos(time / 5.0);
    float P = 25.0;
    float N = 27.0;
    float M = 250.0 + 5.0 * sin(time / 5.0);

    // XYの位置
    float x = pin.position.x;
    float y = pin.position.y;

    // 合成率
    float a = absincos((x - 2.0 * y) / L + time * 0.15, P) *
              absincos(time * 0.3 + y / M, N);

    // 色の合成
    float4 color = lerp(game, base, a);

    // 発光の適用
    color.rgb = color.rgb + emissivegame;

    return color;
}
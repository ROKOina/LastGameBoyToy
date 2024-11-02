#include "Defalt.hlsli"
#include "../Common/Common.hlsli"
#include "../Common/Constants.hlsli"
#include "Light.hlsli"

Texture2D NormalMap : register(t1); // 法線テクスチャ

[earlydepthstencil]
float4 main(VS_OUT pin) : SV_TARGET
{
    // 法線マップ
    float3 normal = NormalMap.Sample(sampler_states[ANISOTROPIC], pin.texcoord).xyz;
    normal = normal * 2 - 1;

    // 変換用の3x3行列を用意
    float3x3 CM =
    {
        pin.tangent,
        pin.binormal,
        pin.normal
    };

    // 法線マップで取得した法線情報に変換行列を掛け合わせる
    float3 N = normalize(mul(normal, CM));

    // ワールド空間の法線 (0 ～ 1) の間にスケーリング
    N = (N * 0.5) + 0.5;

    // 法線を使った凹凸感
    float bumpEffect = saturate(N.z); // 法線のZ成分で凹凸感を調整

    // 凹凸感をベースカラーに反映(ここ定数バッファーに変更)
    float4 color = float4(1, 0.5, 0, 1);
    color.rgb *= lerp(1.0, bumpEffect, 0.8); // 凹凸効果を柔らかく色に適用

    // 最終的な色合いを鮮やかにするための調整
    color.rgb = pow(color.rgb, 1.0f / GAMMA); // ガンマ補正で発色を向上

    return color;
}
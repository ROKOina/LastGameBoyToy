#include "FullScreenQuad.hlsli"
#include "../../3D/PBR+IBLFunction.hlsli"
#include "../../3D/Light.hlsli"
#include "../../Common.hlsli"
#include "../../Constants.hlsli"

Texture2D colorMap : register(t0);
Texture2D normalMap : register(t1); // 法線マップ
Texture2D positionMap : register(t2); // 位置マップ
Texture2D MRAO_Map : register(t3); // メタリックラフネス環境遮蔽マップ
Texture2D emissiveMap : register(t4); // 自己発光マップ
TextureCube diffuseIem : register(t11); // プレフィルタリング済スカイボックス(Diffuse)
TextureCube specularPmrem : register(t12); // プレフィルタリング済スカイボックス(Specular)
Texture2D lutGGX : register(t13); // スカイボックスの色対応表

float4 main(VS_OUT pin) : SV_TARGET
{
    // テクスチャからパラメーター取得
    float4 albedoColor = colorMap.Sample(sampler_states[BLACK_BORDER_POINT], pin.texcoord);
    clip(albedoColor.a - EPSILON);

    // ワールド空間の法線
    float3 N = normalMap.Sample(sampler_states[BLACK_BORDER_POINT], pin.texcoord).xyz;
    N = normalize(N * 2 - 1); // -1 ~ 1 にスケール

    // ワールド座標
    float4 wPos = positionMap.Sample(sampler_states[BLACK_BORDER_POINT], pin.texcoord);

    // 金属度・粗さ・環境遮蔽
    float3 MRAO = MRAO_Map.Sample(sampler_states[BLACK_BORDER_POINT], pin.texcoord).rgb;

    // ライトベクトル、カメラベクトルを正規化
    float3 L = normalize(directionalLight.direction.xyz);
    float3 V = normalize(cameraposition.xyz - wPos.xyz);

    // 入射光のうち拡散反射になる割合
    float3 diffuseReflectance = lerp(albedoColor.rgb, 0.02f, MRAO.x);

    // 垂直反射時のフレネル反射率
    float3 F0 = lerp(dot(_DielectricF0, Specular * Specular), albedoColor.rgb, MRAO.x);

    // 間接光による拡散反射
    float3 indirectDiffuse = ShadeSHPerPixel(N, albedoColor, wPos.xyz).xyz;

    // 環境光による拡散反射＋鏡面反射
    float3 envColor = IBL(lutGGX, diffuseIem, specularPmrem, sampler_states[BLACK_BORDER_ANISOTROPIC], diffuseReflectance, F0, MRAO.r, N, V);

    // 最終の色
    float4 color = BRDF(albedoColor, MRAO.x, MRAO.y, N, V, -L, directionalLight.color.rgb, indirectDiffuse, envColor);

    // AOマップ適用
    color.rgb *= MRAO.z;

    // トーンマップ
    color.rgb = saturate(color.rgb); // クランプ

    // エミッション適用
    color.rgb += emissiveMap.Sample(sampler_states[BLACK_BORDER_POINT], pin.texcoord).rgb;

    return color;
}
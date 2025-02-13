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

// ACES Filmic トーンマッピング関数
float3 ACESToneMapping(float3 color)
{
    // ACES Filmic トーンマッピングの定数
    const float A = 2.51;
    const float B = 0.03;
    const float C = 2.43;
    const float D = 0.59;
    const float E = 0.14;

    return (color * (A * color + B)) / (color * (C * color + D) + E);
}

float4 main(VS_OUT pin) : SV_TARGET
{
    // --- Gバッファからパラメータ取得 ---
    float4 albedoColor = colorMap.Sample(sampler_states[BLACK_BORDER_POINT], pin.texcoord.xy);
    float3 N = normalMap.Sample(sampler_states[BLACK_BORDER_POINT], pin.texcoord.xy).xyz;
    N = normalize(N * 2 - 1);
    float4 wPos = positionMap.Sample(sampler_states[BLACK_BORDER_POINT], pin.texcoord.xy);
    float3 MRAO = MRAO_Map.Sample(sampler_states[BLACK_BORDER_POINT], pin.texcoord.xy).rgb;
    float3 emissive = emissiveMap.Sample(sampler_states[BLACK_BORDER_POINT], pin.texcoord.xy).rgb;

     // --- カメラベクトル ---
    float3 V = normalize(cameraposition.xyz - wPos.xyz);

     // --- 方向光の計算 ---
    float3 L = normalize(directionalLight.direction.xyz);

    // --- PBRパラメータ計算 ---
    float3 diffuseReflectance = lerp(albedoColor.rgb, 0.02f, MRAO.r);
    float3 F0 = lerp(dot(Dielectric, Specular * Specular), albedoColor.rgb, MRAO.r);

     // --- 平行光源による拡散反射＋鏡面反射 ---
    float3 directioncolor = BRDF(-L, directionalLight.color.rgb, V, N, diffuseReflectance, MRAO.r, MRAO.y, F0);

    // --- 環境光による拡散反射＋鏡面反射 ---
    float3 envcolor = IBL(lutGGX, diffuseIem, specularPmrem, sampler_states[BLACK_BORDER_ANISOTROPIC], diffuseReflectance, F0, MRAO.y, N, V);;

    //最終の色情報
    float3 color = albedoColor.rgb + directioncolor + envcolor;

    // --- トーンマッピング (白飛び防止) ---
    color = ACESToneMapping(color);

    // --- AO & エミッション適用 ---
    color *= MRAO.z;
    color += emissive;

    // --- 透明ピクセルを削除 ---
    if (albedoColor.a < EPSILON)
        discard;

    return float4(color, albedoColor.a);
}
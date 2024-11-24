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
    float4 albedoColor = colorMap.Sample(sampler_states[BLACK_BORDER_POINT], pin.texcoord.xy);

    // ワールド空間の法線
    float3 N = normalMap.Sample(sampler_states[BLACK_BORDER_POINT], pin.texcoord.xy).xyz;
    N = normalize(N * 2 - 1); // -1 ~ 1 にスケール

    // ワールド座標
    float4 wPos = positionMap.Sample(sampler_states[BLACK_BORDER_POINT], pin.texcoord.xy);

    // 金属度・粗さ・環境遮蔽
    float3 MRAO = MRAO_Map.Sample(sampler_states[BLACK_BORDER_POINT], pin.texcoord.xy).rgb;

    // カメラベクトル
    float3 V = normalize(cameraposition.xyz - wPos.xyz);

    // 入射光のうち拡散反射になる割合
    float3 diffuseReflectance = lerp(albedoColor.rgb, 0.02f, MRAO.x);

    // 垂直反射時のフレネル反射率
    float3 F0 = lerp(float3(0.04f, 0.04f, 0.04f), albedoColor.rgb, MRAO.x);

    // 間接光による拡散反射
    float3 indirectDiffuse = ShadeSHPerPixel(N, albedoColor).xyz;

    // 環境光による拡散反射＋鏡面反射
    float3 envColor = IBL(lutGGX, diffuseIem, specularPmrem, sampler_states[BLACK_BORDER_ANISOTROPIC], diffuseReflectance, F0, MRAO.r, N, V);

    // 方向光の拡散反射と鏡面反射
    float3 color = 0.0f;
    float3 L = normalize(directionalLight.direction.xyz);
    color += BRDF(albedoColor, MRAO.x, MRAO.y, N, V, -L, directionalLight.color.rgb, indirectDiffuse, envColor).rgb * directionalLight.color.rgb;

    // ポイントライトの計算
    for (int i = 0; i < 2; ++i)
    {
        float3 L = pointLight[i].position.xyz - wPos.xyz;
        float distance = length(L);
        L = normalize(L);

        // 距離減衰
        float attenuation = saturate(1.0 - (distance / pointLight[i].range));
        attenuation *= attenuation; // 距離減衰を二乗で適用

        color += BRDF(albedoColor, MRAO.x, MRAO.y, N, V, L, pointLight[i].color.rgb, indirectDiffuse, envColor).rgb * pointLight[i].color.rgb * attenuation;
    }

    // スポットライトの計算
    for (int k = 0; k < 2; ++k)
    {
        float3 L = spotLight[k].position.xyz - wPos.xyz;
        float distance = length(L);
        L = normalize(L);

        // スポットライトの角度減衰
        float spotEffect = dot(-L, normalize(spotLight[k].direction.xyz));
        float innerCos = cos(spotLight[k].innerCorn);
        float outerCos = cos(spotLight[k].outerCorn);
        float spotAttenuation = saturate((spotEffect - outerCos) / (innerCos - outerCos));

        // 距離減衰
        float distanceAttenuation = saturate(1.0 - (distance / spotLight[k].range));
        distanceAttenuation *= distanceAttenuation;

        color += BRDF(albedoColor, MRAO.x, MRAO.y, N, V, L, spotLight[k].color.rgb, indirectDiffuse, envColor).rgb * spotLight[k].color.rgb * distanceAttenuation * spotAttenuation;
    }

    // AOマップ適用
    color *= MRAO.z;

    // エミッション適用
    color += emissiveMap.Sample(sampler_states[BLACK_BORDER_POINT], pin.texcoord.xy).rgb;

    if (albedoColor.a < EPSILON)
        discard;

    // 負の値にならないように制御
    color = max(color, 0);

    return float4(color, 1.0);
}
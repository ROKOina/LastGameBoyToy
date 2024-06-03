#include "Defalt.hlsli"
#include "Light.hlsli"
#include "PBR+IBLFunction.hlsli"
#include "../Constants.hlsli"

Texture2D DiffuseMap : register(t0); //カラーテクスチャ
Texture2D NormalMap : register(t1); //法線テクスチャ
Texture2D MetallicMap : register(t2); //メタリックマップ
Texture2D RoughnessMap : register(t3); //ラフネスマップ
Texture2D AOMap : register(t4); //AOマップ
Texture2D EmissionMap : register(t5); //エミッションマップ
Texture2D SkyBox : register(t6); //skybox
TextureCube diffuseIem : register(t7); // プレフィルタリング済スカイボックス(Diffuse)
TextureCube specularPmrem : register(t8); // プレフィルタリング済スカイボックス(Specular)
Texture2D lutGGX : register(t9); // スカイボックスの色対応表

//MRT対応
struct PS_OUT
{
    float4 diffuse : SV_TARGET0;
    float4 normal : SV_TARGET1;
    float4 position : SV_TARGET2;
    float4 MRO : SV_TARGET3;
    float4 emission : SV_TARGET4;
};

PS_OUT main(VS_OUT pin)
{
    //色テクスチャをサンプリング
    float4 diffusecolor = DiffuseMap.Sample(sampler_states[ANISOTROPIC], pin.texcoord) * pin.color;
    diffusecolor.rgb = pow(diffusecolor.rgb, GAMMA);
    
    //法線テクスチャをサンプリング
    float3 normal = NormalMap.Sample(sampler_states[ANISOTROPIC], pin.texcoord).xyz * 2 - 1;
    float3x3 CM = { normalize(pin.tangent), normalize(pin.binormal), normalize(pin.normal.xyz) };
    float3 N = normalize(mul(normal, CM));

    //金属テクスチャをサンプリング
    float metallic = saturate(MetallicMap.Sample(sampler_states[LINEAR], pin.texcoord).r * Metalness);

    //光沢テクスチャをサンプリング
    float roughness = saturate(RoughnessMap.Sample(sampler_states[LINEAR], pin.texcoord).r * Roughness);

    //AOマップ
    float ao = AOMap.Sample(sampler_states[LINEAR], pin.texcoord).r;

    //エミッション
    float3 emission = EmissionMap.Sample(sampler_states[LINEAR], pin.texcoord).rgb * emissivecolor.rgb * emissiveintensity;

    //視線ベクトルとライトの方向の正規化
    float3 L = normalize(directionalLight.direction.xyz);
    float3 E = normalize(cameraposition.xyz - pin.world_position.xyz);

     //入射光のうち拡散反射になる割合
    float3 diffuseReflectance = lerp(diffusecolor.rgb, 0.02f, metallic);

	//垂直反射時のフレネル反射率
    float3 F0 = lerp(dot(_DielectricF0, Specular * Specular), diffusecolor.rgb, metallic);

    //Indirect Diffuse
    float3 indirectDiffuse = ShadeSHPerPixel(N, diffusecolor, pin.world_position.xyz);

    //環境光による拡散反射＋鏡面反射
    float3 envColor = 0;
    {
        envColor = IBL(lutGGX, diffuseIem, specularPmrem, sampler_states[BLACK_BORDER_ANISOTROPIC], diffuseReflectance, F0, roughness, N, E);
    }

    //最終の色
    float4 color = BRDF(diffusecolor, metallic, roughness, N, E, -L, directionalLight.color.rgb, indirectDiffuse, envColor);

    //AOマップ適用
    color.rgb *= ao;

    //トーンマップ
    color.rgb = saturate(color.rgb); //クランプ

    //エミッション適用
    color.rgb += emission;

    //MRT出力
    PS_OUT ret;
    ret.diffuse = color;
    ret.normal = mul(float4(N, 0.0f), view);
    ret.position = mul(pin.world_position, view);
    ret.MRO = float4(metallic, roughness, ao, 1.0f);
    ret.emission = float4(emission.rgb, 1.0f);
    return ret;
}
#include "../Common.hlsli"
#include "../2D/PostEffect/FilterFunctions.hlsli"

// 誘電率
static const float Dielectric = 0.16f;
static const float Specular = 0.5f;

//------------------------------------------------
//	 拡散反射BRDF
//------------------------------------------------
// 正規化ランバート
float3 DiffuseBRDF(float3 albedo)
{
    return albedo / PI;
}

// 法線分布関数 D項 ( GGX/Trowbridge-Reitz )
#define MEDIUMP_FLT_MAX    65504.0
#define saturateMediump(x) min(x, MEDIUMP_FLT_MAX)
inline float D_GGX(float NdotH, float3 NxH, float roughness)
{
    float a = NdotH * roughness;
    float k = roughness / (dot(NxH, NxH) + a * a);
    float d = k * k * (1.0 / PI);
    return saturateMediump(d);
}

// 幾何減衰項 V項 ( Smith Joint GGX 近似式 ) "鏡面反射BRDFの式が変わるので注意"
inline float V_HeightCorrelatedGGX(float NdotL, float NdotV, float roughness)
{
    float alpha = roughness * roughness;
    float lambdaV = NdotL * (NdotV * (1 - alpha) + alpha);
    float lambdaL = NdotV * (NdotL * (1 - alpha) + alpha);
    return 0.5f / (lambdaV + lambdaL + 0.0001);
}

// フレネル項 F項 ( Schlick )
inline float3 F_Schlick(float3 f0, float EdotH)
{
    return f0 + (1 - f0) * pow(1 - EdotH, 5);
}

// 鏡面反射BRDF (DVF項Ver)
inline float3 SpecularBRDF(float3 harfV, float NdotL, float NdotV, float NdotH, float3 NxH, float rough, float3 f0)
{
    return D_GGX(NdotH, NxH, rough) * V_HeightCorrelatedGGX(NdotL, NdotV, rough) * F_Schlick(f0, NdotH);
}

//------------------------------------------------
//	 PBR_BRDF
//------------------------------------------------
inline float3 BRDF(float3 lightVec, float3 lightColor, float3 view, float3 normal, float3 albedo, float metal, float rough, float3 f0)
{
    float3 harfV = normalize(lightVec + view);
    float NdotL = max(dot(normal, lightVec), 0.0001);
    float NdotV = max(dot(normal, view), 0.0001);
    float NdotH = max(dot(normal, harfV), 0.0001);
    float3 NxH = cross(normal, harfV);

    float3 irradiance = lightColor * NdotL;
    float3 Fr = DiffuseBRDF(albedo) * (1 - metal) + SpecularBRDF(harfV, NdotL, NdotV, NdotH, NxH, rough, f0);

    return Fr * irradiance;
}

float4 sampleLutGGX(Texture2D lutGGX, SamplerState sampleState, float2 brdf_sample_point)
{
    return lutGGX.Sample(sampleState, brdf_sample_point);
}

float4 sampleDiffuseIem(TextureCube diffuseIem, SamplerState sampleState, float3 v)
{
    return diffuseIem.Sample(sampleState, v);
}

float4 sampleSpecularPmrem(TextureCube specularPmrem, SamplerState sampleState, float3 v, float roughness)
{
    uint width, height, numberOfLevels;
    specularPmrem.GetDimensions(0, width, height, numberOfLevels);

    float lod = roughness * float(numberOfLevels - 1);

    return specularPmrem.SampleLevel(sampleState, v, lod);
}

float3 ApproximateSpecularIBL(Texture2D lutGGX, TextureCube specularPmrem, SamplerState sampleState, float3 F0, float Roughness, float3 N, float3 V)
{
    float NdotV = saturate(dot(N, V));
    float3 R = normalize(reflect(-V, N));
    float2 samplePoint = saturate(float2(NdotV, Roughness));

    // プレフィルタリングされた入射光（スペキュラIBLテクスチャ）
    float3 PrefilteredColor = sampleSpecularPmrem(specularPmrem, sampleState, R, Roughness).rgb;

    // Environment BRDF 2D LUT
    float2 EnvBRDF = sampleLutGGX(lutGGX, sampleState, samplePoint).xy;

    // F0 = SpecularColor
    return PrefilteredColor * (F0 * EnvBRDF.x + EnvBRDF.y);
}

float3 RadianceLambertian(Texture2D lutGGX, TextureCube diffuseIem, SamplerState sampleState, float3 N, float3 V, float roughness, float3 diffuse_color, float3 f0)
{
    float NoV = clamp(dot(N, V), 0.0, 1.0);

    float2 samplePoint = clamp(float2(NoV, roughness), 0.0, 1.0);
    float2 f_ab = sampleLutGGX(lutGGX, sampleState, samplePoint).rg;

    float3 irradiance = sampleDiffuseIem(diffuseIem, sampleState, N).rgb;

    float3 fr = max(1.0 - roughness, f0) - f0;
    float3 k_s = f0 + fr * pow(1.0 - NoV, 5.0);
    float3 fss_ess = k_s * f_ab.x + f_ab.y;

    float ems = (1.0 - (f_ab.x + f_ab.y));
    float3 f_avg = (f0 + (1.0 - f0) / 21.0);
    float3 fms_ems = ems * fss_ess * f_avg / (1.0 - f_avg * ems);
    float3 k_d = diffuse_color * (1.0 - fss_ess + fms_ems);

    return (fms_ems + k_d) * irradiance;
}

//IBL
#define MaxLuminance (2)
float3 IBL(Texture2D lutGGX, TextureCube diffuseIem, TextureCube specularPmrem, SamplerState sampleState, float3 DiffuseColor, float3 F0, float Roughness, float3 N, float3 V)
{
    float3 diffuse = RadianceLambertian(lutGGX, diffuseIem, sampleState, N, V, Roughness, DiffuseColor, F0);
    float3 specular = ApproximateSpecularIBL(lutGGX, specularPmrem, sampleState, F0, Roughness, N, V);

    // スペキュラ反射の強さを調整
    float LuminanceRatio = MaxLuminance / RGB2Luminance(specular);
    LuminanceRatio = clamp(LuminanceRatio, 0.0, 1.0);

    return diffuse + specular * LuminanceRatio;
}
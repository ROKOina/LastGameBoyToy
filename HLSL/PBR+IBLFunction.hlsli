#include "Common.hlsli"

// 誘電率
static const float Specular = 0.5f;

#define UNITY_INV_PI             0.31830988618f
#define _DielectricF0            0.04f

#define MEDIUMP_FLT_MAX    65504.0
//法線分布関数（D項）
inline float D_GGX(float perceptualRoughness, float ndoth, float3 normal, float3 halfDir)
{
    float3 ncrossh = cross(normal, halfDir);
    float a = ndoth * perceptualRoughness;
    float k = perceptualRoughness / (dot(ncrossh, ncrossh) + a * a);
    float d = k * k * UNITY_INV_PI;
    return min(d, MEDIUMP_FLT_MAX);
}

//幾何減衰項(V項)
#define G1Func(_dot) (_dot /(_dot * (1 - k)+ k))
inline float V_SmithGGXCorrelated(float NdotL, float NdotV, float roughness)
{
    float k = (roughness + 1) * (roughness + 1) / 8;
    return G1Func(NdotL) * G1Func(NdotV);
}

// フレネル項 F項 ( Schlick )
inline float3 F_Schlick(float3 f0, float ldoth)
{
    return f0 + (1 - f0) * pow(1 - ldoth, 5);
}

float4 sampleLutGGX(Texture2D lutGGX, SamplerState sampleState, float2 brdf_sample_point)
{
    return lutGGX.Sample(sampleState, brdf_sample_point);
}

float4 sampleDiffuseIem(TextureCube diffuseIem, SamplerState sampleState, float3 v)
{
    return diffuseIem.Sample(sampleState, v);
}

float3 RadianceLambertian(Texture2D lutGGX, TextureCube diffuseIem, SamplerState sampleState, float3 N, float3 V, float perceptualRoughness, float3 diffuse_color, float3 f0)
{
    float NoV = clamp(dot(N, V), 0.0, 1.0);
    float2 samplePoint = clamp(float2(NoV, perceptualRoughness), 0.0, 1.0);
    float2 f_ab = sampleLutGGX(lutGGX, sampleState, samplePoint).rg;
    float3 irradiance = sampleDiffuseIem(diffuseIem, sampleState, N).rgb;
    float3 fr = max(1.0 - perceptualRoughness, f0) - f0;
    float3 k_s = f0 + fr * pow(1.0 - NoV, 5.0);
    float3 fss_ess = k_s * f_ab.x + f_ab.y;
    float ems = (1.0 - (f_ab.x + f_ab.y));
    float3 f_avg = (f0 + (1.0 - f0) / 21.0);
    float3 fms_ems = ems * fss_ess * f_avg / (1.0 - f_avg * ems);
    float3 k_d = diffuse_color * (1.0 - fss_ess + fms_ems);
    return (fms_ems + k_d) * irradiance;
}

float4 sampleSpecularPmrem(TextureCube specularPmrem, SamplerState sampleState, float3 v, float perceptualRoughness)
{
    uint width, height, numberOfLevels;
    specularPmrem.GetDimensions(0, width, height, numberOfLevels);
    float lod = perceptualRoughness * float(numberOfLevels - 1);
    return specularPmrem.SampleLevel(sampleState, v, lod);
}

float3 ApproximateSpecularIBL(Texture2D lutGGX, TextureCube specularPmrem, SamplerState sampleState, float3 F0, float perceptualRoughness, float3 N, float3 V)
{
    float NdotV = saturate(dot(N, V));
    float3 R = normalize(reflect(-V, N));
    float2 samplePoint = saturate(float2(NdotV, perceptualRoughness));
    // プレフィルタリングされた入射光（スペキュラIBLテクスチャ）
    float3 PrefilteredColor = sampleSpecularPmrem(specularPmrem, sampleState, R, perceptualRoughness).rgb;
    // Environment BRDF 2D LUT
    float2 EnvBRDF = sampleLutGGX(lutGGX, sampleState, samplePoint).xy;
    // F0 = SpecularColor
    return PrefilteredColor * (F0 * EnvBRDF.x + EnvBRDF.y);
}

//--------------------------------------------
//	RGB色空間の数値から輝度値への変換関数
//--------------------------------------------
//rgb:RGB色空間の数値
float RGB2Luminance(float3 rgb)
{
    static const float3 LUMINANCE = float3(0.299f, 0.587f, 0.114f);
    return dot(LUMINANCE, rgb);
}

#define MaxLuminance (2)
float3 IBL(Texture2D lutGGX, TextureCube diffuseIem, TextureCube specularPmrem, SamplerState sampleState, float3 DiffuseColor, float3 F0, float perceptualRoughness, float3 N, float3 V)
{
    float3 diffuse = RadianceLambertian(lutGGX, diffuseIem, sampleState, N, V, perceptualRoughness, DiffuseColor, F0);
    float3 specular = ApproximateSpecularIBL(lutGGX, specularPmrem, sampleState, F0, perceptualRoughness, N, V);
    // スペキュラ反射の強さを調整
    float LuminanceRatio = MaxLuminance / RGB2Luminance(specular);
    LuminanceRatio = clamp(LuminanceRatio, 0.0, 1.0);
    return diffuse + specular * LuminanceRatio;
}

//拡散反射BRDF
inline float Fd_Burley(float ndotv, float ndotl, float ldoth, float perceptualRoughness)
{
    float fd90 = 0.5 + 2 * ldoth * ldoth * perceptualRoughness;
    float lightScatter = (1 + (fd90 - 1) * pow(1 - ndotl, 5));
    float viewScatter = (1 + (fd90 - 1) * pow(1 - ndotv, 5));

    float diffuse = lightScatter * viewScatter;
    return diffuse;
}

//鏡面反射BRDF
float4 BRDF(float4 albedo, float metallic, float perceptualRoughness, float3 normal, float3 viewDir, float3 lightDir, float3 lightColor, float3 indirectDiffuse, float3 indirectSpecular)
{
    float3 halfDir = normalize(lightDir + viewDir);
    float ndotv = abs(dot(normal, viewDir));
    float ndotl = max(0, dot(normal, lightDir));
    float ndoth = max(0, dot(normal, halfDir));
    float ldoth = max(0, dot(lightDir, halfDir));
    float reflectivity = lerp(_DielectricF0, 1, metallic);
    float3 f0 = lerp(_DielectricF0, albedo.rgb, metallic);
    float diffuseTerm = Fd_Burley(ndotv, ndotl, ldoth, perceptualRoughness) * ndotl;
    float3 diffuse = albedo.rgb * (1 - reflectivity) * lightColor * diffuseTerm;
    // Indirect Diffuse
    diffuse += albedo.rgb * (1 - reflectivity) * indirectDiffuse.rgb;
    float alpha = perceptualRoughness * perceptualRoughness;
    float V = V_SmithGGXCorrelated(ndotl, ndotv, alpha);
    float D = D_GGX(perceptualRoughness, ndotv, normal, halfDir);
    float3 F = F_Schlick(f0, ldoth);
    float3 specular = V * D * F * ndotl * lightColor;
    specular *= PI;
    specular = max(0, specular);
    //Indirect Specular
    float surfaceReduction = 1.0 / (alpha * alpha + 1.0);
    float f90 = saturate((1 - perceptualRoughness) + reflectivity);
    specular += surfaceReduction * indirectSpecular * lerp(f0, f90, pow(1 - ndotv, 5));
    float3 color = diffuse.rgb + specular;
    return float4(color, albedo.a);
}

float3 SHEvalLinearL0L1(float4 normal)
{
    float3 x;
    float4 unity_SHAr = 0;
    float4 unity_SHAg = 0;
    float4 unity_SHAb = 0;

    // Linear (L1) + constant (L0) polynomial terms
    x.r = dot(unity_SHAr, normal);
    x.g = dot(unity_SHAg, normal);
    x.b = dot(unity_SHAb, normal);

    return x;
}

float3 SHEvalLinearL2(float4 normal)
{
    float3 x1, x2;
    float4 unity_SHBr = 0;
    float4 unity_SHBg = 0;
    float4 unity_SHBb = 0;
    float4 unity_SHC = 0;

    // 4 of the quadratic (L2) polynomials
    float4 vB = normal.xyzz * normal.yzzx;
    x1.r = dot(unity_SHBr, vB);
    x1.g = dot(unity_SHBg, vB);
    x1.b = dot(unity_SHBb, vB);

    // Final (5th) quadratic (L2) polynomial
    float vC = normal.x * normal.x - normal.y * normal.y;
    x2 = unity_SHC.rgb * vC;

    return x1 + x2;
}

float4 ShadeSHPerPixel(float3 normal, float4 ambient, float3 worldPos)
{
    float3 ambient_contrib = 0.0;
    ambient_contrib = SHEvalLinearL0L1(half4(normal, 1.0));
    ambient_contrib += SHEvalLinearL2(half4(normal, 1.0));
    ambient.xyz += max(half3(0, 0, 0), ambient_contrib);
    ambient = pow(ambient, GAMMA);
    return ambient;
}
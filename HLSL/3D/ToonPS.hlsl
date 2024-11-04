#include "Defalt.hlsli"
#include "../Constants.hlsli"
#include "../Common.hlsli"
#include "../3D/Light.hlsli"

Texture2D DiffuseMap : register(t0); // カラーテクスチャ
Texture2D NormalMap : register(t1); // 法線テクスチャ
Texture2D MetallicMap : register(t2); // メタリックマップ
Texture2D RoughnessMap : register(t3); // ラフネスマップ
Texture2D AOMap : register(t4); // AOマップ
Texture2D EmissionMap : register(t5); // エミッションマップ

float4 main(VS_OUT pin) : SV_TARGET
{
    // テクスチャの取得
    float3 albedo = DiffuseMap.Sample(sampler_states[ANISOTROPIC], pin.texcoord).rgb * pin.color.rgb; 
    
    // 肌色強調用の係数
    float skinSaturationBoost = 6.5; // 肌色の彩度を調整する係数
    float3 skinToneColor = float3(0.9725, 0.8902, 0.8588); // 肌色の基準色（オレンジ寄りの色）

    // 肌色の強調処理
    float similarity = dot(normalize(albedo), normalize(skinToneColor)); // アルベド色と基準色の類似度
    if (similarity > 0.95)
    { // 肌色に近い色だけを調整
        float grayscale = dot(albedo, float3(0.3, 0.59, 0.11)); // グレースケール変換
        albedo = lerp(float3(grayscale, grayscale, grayscale), albedo, skinSaturationBoost);
    }
    
    // 金属テクスチャをサンプリング
    float metallic = saturate(MetallicMap.Sample(sampler_states[LINEAR], pin.texcoord).r * Metalness);
    // 光沢テクスチャをサンプリング
    float roughness = saturate(RoughnessMap.Sample(sampler_states[LINEAR], pin.texcoord).r * Roughness);

    float3 viewDir = normalize(cameraposition- pin.world_position );

    float3 lightDir = -directionalLight.direction.xyz;
    
    // 光の強度計算
    float NdotL = dot(pin.normal, lightDir);

    // ステップ関数で光の強度を段階的にする（トゥーンシェーディング効果）
    float lightIntensity = saturate(step(0.1, NdotL));
    lightIntensity = max(lightIntensity, 0.2f);

    // ベースカラー（ディフューズ）
    float3 baseColor = albedo * directionalLight.color.rgb * lightIntensity;
    
    // リムライト（エッジを強調）
    float rim = 1.0 - max(dot(viewDir, pin.normal), 0.0);
    float rimPower = 0.55; // リムライトの強度を調整する係数
    float3 rimLight = saturate(smoothstep(0.2, 0.5, rim * rim)) * albedo * rimPower;

    // 最終カラー合成
    float3 color = baseColor + rimLight + emissivecolor * emissiveintensity;
    return float4(color, 1.0);
}
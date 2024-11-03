#include "FakeDepth.hlsli"
#include "FakeInterior.hlsli"
#include "../Constants.hlsli"
#include "../Common.hlsli"

Texture2D InteriorMap : register(t0);
Texture2D skybox : register(t10);

//------------------------------------------------
//	 部屋の描画
//------------------------------------------------
void Unity_TilingAndOffset_float(float2 UV, float2 Tiling, float2 Offset, out float2 Out)
{
    Out = UV * Tiling + Offset;
}

// 仮想の空間をシミュレートする
float3 CalculateVirtualRoomBoundingBox(float2 uv)
{
    float2 tilingUV;
	
	// UVをタイリング
    Unity_TilingAndOffset_float(uv, round(tiling), 0, tilingUV);
	
	// 小数部のみを取得し、-1~1にする
    tilingUV = frac(tilingUV);
    tilingUV *= 2.0;
    tilingUV -= 1.0;
	
	// 引数のUVが仮想空間のどの位置にあるかを返す
    return float3(tilingUV.x, tilingUV.y, 1.0);
}

// 空間を覗く窓の角を探す
float3 FindRealWindowCorner(float3 tangentView, float3 roomBound)
{
    float3 reciprocalView = 1.0 / tangentView;

    return abs(reciprocalView) - (reciprocalView * roomBound);
}

float GetWallCorners(float3 windowCorner)
{
    return min(windowCorner.x, min(windowCorner.y, windowCorner.z));
}

// ベクトルをパノラマUVに変換
float2 ViewDirToPanoramaUV(float3 dir)
{
    // アークタンジェント2を使用して水平角度を計算
    float u = 0.5 + (atan2(dir.z, dir.x) / (2.0 * PI));

    // アークサインを使用して垂直角度を計算
    float v = 0.5 - (asin(dir.y) / PI);

    // UV座標を作成
    return float2(u, v);
}

float4 SampleCubemapInterior(float3 tangentView, float wallCorner, float3 roomBound)
{
    float3 viewDir = (tangentView * wallCorner) + roomBound;
    viewDir *= float3(-1.0, -1.0, 1.0);
    
    float2 sampleUV = ViewDirToPanoramaUV(viewDir);
    
    return InteriorMap.Sample(sampler_states[LINEAR], sampleUV + float2(offset, 0.0));
}

//------------------------------------------------
//	 窓の反射
//------------------------------------------------
void Unity_FresnelEffect_float(float3 Normal, float3 ViewDir, float Power, out float Out)
{
    Out = pow((1.0 - saturate(dot(Normal, ViewDir))), Power);
}

void SampleSkyMap(float3 view, float3 normal, out float3 fresnelSkyMap, out float3 sampleSlyColor)
{
    float fresnel;
    Unity_FresnelEffect_float(normal, view, 0.7, fresnel);
    fresnel = 1 - fresnel;
    
    // スカイボックスをサンプリング
    float2 sampleUV = ViewDirToPanoramaUV(reflect(-view, normal));
    sampleSlyColor = skybox.Sample(sampler_states[LINEAR], sampleUV).rgb;
    
    fresnelSkyMap = fresnel * sampleSlyColor;
}

// 窓の縁は反射が大きくなる
float SidePoweredReflections(float3 view, float3 normal)
{
    float d = dot(normal, view);

    return 1 - saturate(d);
}

// 反射率
void LimitSideReflectionViewAngle(float3 view, float3 normal, out float clampPower, out float smoothPower)
{
    float power = SidePoweredReflections(view, normal);

    clampPower = clamp(power, 0.0, 0.8);

    smoothPower = smoothstep(0.0, 0.9, power);
}

//------------------------------------------------
//	 その他
//------------------------------------------------
float3 Unity_Blend_Screen_float3(float3 Base, float3 Blend, float Opacity)
{
    float3 Out = 1.0 - (1.0 - Blend) * (1.0 - Base);
    Out = lerp(Base, Out, Opacity);
    
    return Out;
}

//------------------------------------------------
//	 メイン
//------------------------------------------------
float4 main(VS_OUT pin) : SV_TARGET
{
    float3 tangentView = -pin.cameraTangent;
    tangentView.y *= -1;
    
    // 仮想の部屋を作成
    float3 roomBound = CalculateVirtualRoomBoundingBox(pin.texcoord);
    float wallCorner = GetWallCorners(FindRealWindowCorner(tangentView, roomBound));
    float4 interiorColor = SampleCubemapInterior(tangentView, wallCorner, roomBound);
    
    // 窓の反射
    float3 cameraVec = cameraposition - pin.world_position;
    cameraVec = normalize(cameraVec);
    
    float3 fresnelSkyColor;
    float3 SkyColor;
    SampleSkyMap(cameraVec, pin.normal, fresnelSkyColor, SkyColor);
    
    // 最終的な色
    float clampRate;
    float smoothRate;
    LimitSideReflectionViewAngle(cameraVec, pin.normal, clampRate, smoothRate);
    
    float3 Color1;
    Color1 = Unity_Blend_Screen_float3(interiorColor.rgb, fresnelSkyColor, reflectionAmount);
    
    float3 Color2;
    Color2 = lerp(interiorColor.rgb, SkyColor, smoothRate);
    
    float3 finalColor;
    finalColor = lerp(Color1, Color2, clampRate);
    
    return float4(finalColor, 1.0);
}
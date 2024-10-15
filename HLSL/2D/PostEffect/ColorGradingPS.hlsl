#include "FullScreenQuad.hlsli"
#include "PostEffect.hlsli"
#include "../../Common.hlsli"
#include "../../Constants.hlsli"

Texture2D texturemaps : register(t0);
Texture2D depth_map : register(t1);
Texture2D outlinecolor : register(t2);

//明るさ(画面全体の)とコントラスト(画像の明暗)
float3 brightness_contrast(float3 fragment_color, float brightness, float contrast)
{
    fragment_color += brightness;
    if (contrast > 0.0)
    {
        fragment_color = (fragment_color - 0.5) / (1.0 - contrast) + 0.5;
    }
    else if (contrast < 0.0)
    {
        fragment_color = (fragment_color - 0.5) * (1.0 + contrast) + 0.5;
    }
    return fragment_color;
}

//色相と彩度
float3 hue_saturation(float3 fragment_color, float hue, float saturation)
{
    float angle = hue * 3.14159265;
    float s = sin(angle), c = cos(angle);
    float3 weights = (float3(2.0 * c, -sqrt(3.0) * s - c, sqrt(3.0) * s - c) + 1.0) / 3.0;
    fragment_color = float3(dot(fragment_color, weights.xyz), dot(fragment_color, weights.zxy), dot(fragment_color, weights.yzx));
    float average = (fragment_color.r + fragment_color.g + fragment_color.b) / 3.0;
    if (saturation > 0.0)
    {
        fragment_color += (average - fragment_color) * (1.0 - 1.0 / (1.001 - saturation));
    }
    else
    {
        fragment_color += (average - fragment_color) * (-saturation);
    }
    return fragment_color;
}

// Vignetteの計算
float vignette(float2 uv)
{
    // 中心からの距離を計算
    float2 distance = uv - float2(0.5, 0.5);
    float len = length(distance);

    // 距離に基づいてビネット効果を計算
    float vignette = smoothstep(vignettesize, vignettesize - vignetteintensity, len);
    return vignette;
}

// エッジ検出関数
float ComputeSobelEdge(float2 texcoord, float width, float height, Texture2D depth_map, SamplerState sampler_states)
{
    float2 uvDist = float2(1.0 / width, 1.0 / height);
    float centerDepth = depth_map.Sample(sampler_states, texcoord); // Center
    float4 depthDiag;
    float4 depthAxis;

    // Diagonal depths
    depthDiag.x = depth_map.Sample(sampler_states, texcoord + uvDist); // TR
    depthDiag.y = depth_map.Sample(sampler_states, texcoord + uvDist * float2(-1.0f, 1.0f)); // TL
    depthDiag.z = depth_map.Sample(sampler_states, texcoord - uvDist * float2(-1.0f, 1.0f)); // BR
    depthDiag.w = depth_map.Sample(sampler_states, texcoord - uvDist); // BL

    // Axis depths
    depthAxis.x = depth_map.Sample(sampler_states, texcoord + uvDist * float2(0.0f, 1.0f)); // T
    depthAxis.y = depth_map.Sample(sampler_states, texcoord - uvDist * float2(1.0f, 0.0f)); // L
    depthAxis.z = depth_map.Sample(sampler_states, texcoord + uvDist * float2(1.0f, 0.0f)); // R
    depthAxis.w = depth_map.Sample(sampler_states, texcoord - uvDist * float2(0.0f, 1.0f)); // B

    // Sobel coefficients
    const float4 vertDiagCoeff = float4(-1.0f, -1.0f, 1.0f, 1.0f); // TR, TL, BR, BL
    const float4 horizDiagCoeff = float4(1.0f, -1.0f, 1.0f, -1.0f);
    const float4 vertAxisCoeff = float4(-2.0f, 0.0f, 0.0f, 2.0f); // T, L, R, B
    const float4 horizAxisCoeff = float4(0.0f, -2.0f, 2.0f, 0.0f);

    // Sobel horizontal and vertical calculations
    float4 sobelH = depthDiag * horizDiagCoeff + depthAxis * horizAxisCoeff;
    float4 sobelV = depthDiag * vertDiagCoeff + depthAxis * vertAxisCoeff;
    float sobelX = dot(sobelH, float4(1.0f, 1.0f, 1.0f, 1.0f));
    float sobelY = dot(sobelV, float4(1.0f, 1.0f, 1.0f, 1.0f));

    // Sobel edge magnitude
    return sqrt(sobelX * sobelX + sobelY * sobelY);
}

// ビュー空間座標をワールド空間座標に変換
float4 ConvertToWorldPosition(float2 texcoord, float centerDepth, float width, float height, float4x4 inverseviewprojection)
{
    float3 viewPos;
    viewPos.xy = (texcoord * 2.0f - 1.0f) * float2(width, height) * centerDepth;
    viewPos.z = centerDepth;

    float4 clipPos = float4(viewPos, 1.0f);
    float4 worldPos = mul(clipPos, inverseviewprojection);
    return worldPos / worldPos.w;
}

// 距離に基づいたアウトラインしきい値の計算
float ComputeOutlineThreshold(float distance, float minThreshold, float maxThreshold)
{
    return lerp(maxThreshold, minThreshold, saturate(distance / 10.0f));
}

// 輪郭線描画ロジック
float ComputeDepthEdge(float sobel, float distance, float minDistance, float threshold)
{
    bool drawOutline = distance > minDistance;
    return (drawOutline && sobel > threshold) ? 1.0f : 0.0f;
}

// メイン処理関数
float4 OutlineEffect(float2 texcoord, float width, float height, float4x4 inverseviewprojection, float3 cameraposition)
{
    // Sobel edge detection
    float sobel = ComputeSobelEdge(texcoord, width, height, depth_map, sampler_states[LINEAR]);

    // Center depth
    float centerDepth = depth_map.Sample(sampler_states[LINEAR], texcoord);

    // World position and distance
    float4 worldPos = ConvertToWorldPosition(texcoord, centerDepth, width, height, inverseviewprojection);
    float distance = length(worldPos.xyz - cameraposition);

    // Adaptive threshold
    float minThreshold = 0.001;
    float maxThreshold = 0.01;
    float threshold = ComputeOutlineThreshold(distance, minThreshold, maxThreshold);

    // Edge detection and outline drawing
    float minDistance = 1.0; // No outline if too close
    float depthEdge = ComputeDepthEdge(sobel, distance, minDistance, threshold);

    // Final color calculation
    return float4(depthEdge, depthEdge, depthEdge, 1.0f) * outlinecolor.Sample(sampler_states[LINEAR], texcoord);
}

float4 main(VS_OUT pin) : SV_TARGET
{
    // テクスチャの大きさを取得
    uint mip_level = 0, width, height, number_of_levels;
    texturemaps.GetDimensions(mip_level, width, height, number_of_levels);

    // シーンのテクスチャマップをサンプリング
    float4 sampled_color = texturemaps.Sample(sampler_states[POINT], pin.texcoord.xy);

    // 輪郭線効果の描画 (輪郭線をエッジ部分にのみ適用)
    //sampled_color.rgb += OutlineEffect(pin.texcoord.xy, width, height, inverseviewprojection, cameraposition);

    // ビネット効果の適用
    //sampled_color.rgb = lerp(sampled_color.rgb * vignettecolor.rgb, sampled_color.rgb, vignette(pin.texcoord.xy));

    // 明るさとコントラストの調整
    sampled_color.rgb = brightness_contrast(sampled_color.rgb, brightness, contrast);

    // 色相と彩度の調整
    sampled_color.rgb = hue_saturation(sampled_color.rgb, hue, saturation);

    // 最終的な色を返す
    return sampled_color;
}
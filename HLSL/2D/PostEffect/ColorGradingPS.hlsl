#include "FullScreenQuad.hlsli"
#include "PostEffect.hlsli"
#include "../../Common.hlsli"
#include "../../Constants.hlsli"
#include "../../noise.hlsli"
#include "../../3D/Light.hlsli"

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

// エッジ検出 (Sobel フィルタ)
float ComputeSobelEdge(float2 texcoord, float width, float height, Texture2D depth_map, SamplerState sampler_states)
{
    float2 uvDist = float2(1.0 / width, 1.0 / height);
    float centerDepth = depth_map.Sample(sampler_states, texcoord); // 中心の深度
    float4 depthDiag;
    float4 depthAxis;

    // 対角方向の深度
    depthDiag.x = depth_map.Sample(sampler_states, texcoord + uvDist); // 右上
    depthDiag.y = depth_map.Sample(sampler_states, texcoord + uvDist * float2(-1.0f, 1.0f)); // 左上
    depthDiag.z = depth_map.Sample(sampler_states, texcoord - uvDist * float2(-1.0f, 1.0f)); // 右下
    depthDiag.w = depth_map.Sample(sampler_states, texcoord - uvDist); // 左下

    // 軸方向の深度
    depthAxis.x = depth_map.Sample(sampler_states, texcoord + uvDist * float2(0.0f, 1.0f)); // 上
    depthAxis.y = depth_map.Sample(sampler_states, texcoord - uvDist * float2(1.0f, 0.0f)); // 左
    depthAxis.z = depth_map.Sample(sampler_states, texcoord + uvDist * float2(1.0f, 0.0f)); // 右
    depthAxis.w = depth_map.Sample(sampler_states, texcoord - uvDist * float2(0.0f, 1.0f)); // 下

    // Sobel係数
    const float4 vertDiagCoeff = float4(-1.0f, -1.0f, 1.0f, 1.0f); // 対角 (上下左右)
    const float4 horizDiagCoeff = float4(1.0f, -1.0f, 1.0f, -1.0f);
    const float4 vertAxisCoeff = float4(-2.0f, 0.0f, 0.0f, 2.0f); // 軸方向 (上下)
    const float4 horizAxisCoeff = float4(0.0f, -2.0f, 2.0f, 0.0f); // 軸方向 (左右)

    // Sobel の水平方向と垂直方向計算
    float4 sobelH = depthDiag * horizDiagCoeff + depthAxis * horizAxisCoeff;
    float4 sobelV = depthDiag * vertDiagCoeff + depthAxis * vertAxisCoeff;
    float sobelX = dot(sobelH, float4(1.0f, 1.0f, 1.0f, 1.0f));
    float sobelY = dot(sobelV, float4(1.0f, 1.0f, 1.0f, 1.0f));

    // Sobel エッジ強度 (勾配の大きさ)
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
    // 線形補間で距離に応じたしきい値を計算
    return lerp(maxThreshold, minThreshold, saturate(distance / 10.0f));
}

// 輪郭線描画の条件チェック
float ComputeDepthEdge(float sobel, float distance, float minDistance, float threshold)
{
    bool drawOutline = distance > minDistance;
    return (drawOutline && sobel > threshold) ? 1.0f : 0.0f;
}

// メイン処理関数: アウトライン描画
float4 OutlineEffect(float2 texcoord, float width, float height, float4x4 inverseviewprojection, float3 cameraposition)
{
    // Sobel エッジ検出
    float sobel = ComputeSobelEdge(texcoord, width, height, depth_map, sampler_states[LINEAR]);

    // 深度情報の取得
    float centerDepth = depth_map.Sample(sampler_states[LINEAR], texcoord);

    // ワールド座標とカメラ距離計算
    float4 worldPos = ConvertToWorldPosition(texcoord, centerDepth, width, height, inverseviewprojection);
    float distance = length(worldPos.xyz - cameraposition);

    // 距離に応じた閾値の計算
    float minThreshold = 0.001; // 最小しきい値
    float maxThreshold = 0.01; // 最大しきい値
    float threshold = ComputeOutlineThreshold(distance, minThreshold, maxThreshold);

    // エッジ検出と輪郭線描画
    float minDistance = 1.0; // カメラに近すぎる場合は描画しない
    float depthEdge = ComputeDepthEdge(sobel, distance, minDistance, threshold);

    // 最終的なアウトラインの色
    return float4(depthEdge, depthEdge, depthEdge, 1.0f) * outlinecolor.Sample(sampler_states[LINEAR], texcoord);
}

//ラジアルブラー
float3 radialblur(float2 texcoord)
{
    const int sample = 16;
    float2 centerposition = texcoord - float2(0.5f, 0.5f);
    float distance = length(centerposition);
    float factor = blurstrength / float(sample) * distance;
    factor *= smoothstep(blurradius, blurradius * blurdecay, distance);

    float3 color = float3(0.0f, 0.0f, 0.0f);
    for (int i = 0; i < sample; ++i)
    {
        float sampleoffset = 1.0f - factor * i;
        color += texturemaps.Sample(sampler_states[LINEAR], float2(0.5f, 0.5f) + (centerposition * sampleoffset)).rgb;
    }

    color /= float(sample); // 色の平均を取る
    return color;
}

float4 main(VS_OUT pin) : SV_TARGET
{
    //指定されたミップマップレベルのテクスチャマップの次元情報を取得し、アスペクト比を計算
    uint2 dimensions;
    uint mip_level = 0, number_of_samples;
    texturemaps.GetDimensions(mip_level, dimensions.x, dimensions.y, number_of_samples);

    //テクスチャマップの幅と高さの比率
    const float aspect = (float) dimensions.y / dimensions.x;

    //深度マップからピクセルの深度情報をサンプリング
    float scene_depth = depth_map.Sample(sampler_states[BLACK_BORDER_LINEAR], pin.texcoord).r;

    //深度値も NDC空間で使える(NDC座標に変換している)
    float4 ndc_position;
    ndc_position.x = pin.texcoord.x * +2 - 1;
    ndc_position.y = pin.texcoord.y * -2 + 1;
    ndc_position.z = scene_depth;
    ndc_position.w = 1;

    //（NDC）をワールド座標に変換
    float4 world_position = mul(ndc_position, inverseviewprojection);
    world_position = world_position / world_position.w;

    // シーンのテクスチャマップをサンプリング
    float4 sampled_color = texturemaps.Sample(sampler_states[POINT], pin.texcoord.xy);

    //ラジアルブラー
    sampled_color.rgb = lerp(sampled_color.rgb, radialblur(pin.texcoord).rgb, blurstrength);

    // 輪郭線効果の描画 (輪郭線をエッジ部分にのみ適用)
    sampled_color.rgb += OutlineEffect(pin.texcoord.xy, dimensions.x, dimensions.y, inverseviewprojection, cameraposition);

    // ビネット効果の適用
    sampled_color.rgb = lerp(sampled_color.rgb * vignettecolor.rgb, sampled_color.rgb, vignette(pin.texcoord.xy));

    // 明るさとコントラストの調整
    sampled_color.rgb = brightness_contrast(sampled_color.rgb, brightness, contrast);

    // 色相と彩度の調整
    sampled_color.rgb = hue_saturation(sampled_color.rgb, hue, saturation);

    // 最終的な色を返す
    return sampled_color;
}
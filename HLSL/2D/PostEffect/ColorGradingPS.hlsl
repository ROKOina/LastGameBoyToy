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

//レンズフレアとグローライトのエフェクト
float3 lens_flare(float2 uv, float2 pos)
{
    const float glory_light_intensity = 1.5;
    const float lens_flare_intensity = 3.0;

    float2 main = uv - pos;
    float2 uvd = uv * (length(uv));

    float ang = atan2(main.x, main.y);
    float dist = length(main);
    dist = pow(dist, .1);
    float n = noise(float2(ang * 16.0, dist * 32.0));

    float f0 = 1.0 / (length(uv - pos) * 16.0 + 1.0);
    f0 = f0 + f0 * (sin(noise(sin(ang * 2. + pos.x) * 4.0 - cos(ang * 3. + pos.y)) * 16.) * .1 + dist * .1 + .8);

    float f1 = max(0.01 - pow(length(uv + 1.2 * pos), 1.9), .0) * 7.0;

    float f2 = max(1.0 / (1.0 + 32.0 * pow(length(uvd + 0.8 * pos), 2.0)), .0) * 0.25;
    float f22 = max(1.0 / (1.0 + 32.0 * pow(length(uvd + 0.85 * pos), 2.0)), .0) * 0.23;
    float f23 = max(1.0 / (1.0 + 32.0 * pow(length(uvd + 0.9 * pos), 2.0)), .0) * 0.21;

    float2 uvx = lerp(uv, uvd, -0.5);

    float f4 = max(0.01 - pow(length(uvx + 0.4 * pos), 2.4), .0) * 6.0;
    float f42 = max(0.01 - pow(length(uvx + 0.45 * pos), 2.4), .0) * 5.0;
    float f43 = max(0.01 - pow(length(uvx + 0.5 * pos), 2.4), .0) * 3.0;

    uvx = lerp(uv, uvd, -.4);

    float f5 = max(0.01 - pow(length(uvx + 0.2 * pos), 5.5), .0) * 2.0;
    float f52 = max(0.01 - pow(length(uvx + 0.4 * pos), 5.5), .0) * 2.0;
    float f53 = max(0.01 - pow(length(uvx + 0.6 * pos), 5.5), .0) * 2.0;

    uvx = lerp(uv, uvd, -0.5);

    float f6 = max(0.01 - pow(length(uvx - 0.3 * pos), 1.6), .0) * 6.0;
    float f62 = max(0.01 - pow(length(uvx - 0.325 * pos), 1.6), .0) * 3.0;
    float f63 = max(0.01 - pow(length(uvx - 0.35 * pos), 1.6), .0) * 5.0;

    float3 c = 0;

    c.r += f2 + f4 + f5 + f6;
    c.g += f22 + f42 + f52 + f62;
    c.b += f23 + f43 + f53 + f63;
    c = max(0, c * 1.3 - (length(uvd) * .05));

    return f0 * glory_light_intensity + c * lens_flare_intensity;
}

//大気（ミスト）のエフェクト
float3 atmosphere(float3 fragment_color, float3 mist_color, float3 pixel_coord, float3 eye_coord)
{
    const float3 mist_flow_direction = float3(-1.0, -.2, -0.5);
    const float3 mist_flow_coord = pixel_coord.xyz + (mist_flow_direction * mist_flow_speed * time);
    const float flowing_density = lerp(mist_flow_density_lower_limit, 1.0, noise(fmod(mist_flow_coord * mist_flow_noise_scale_factor, 289)));

    float3 eye_to_pixel = pixel_coord - eye_coord;

    float z = length(pixel_coord - eye_coord);
    z = smoothstep(0, mist_cutoff_distance, z) * z;

    const float2 coefficients = mist_density * smoothstep(0.0, mist_height_falloff, height_mist_offset - pixel_coord.y) * flowing_density;

    const float2 factors = exp(-z * coefficients);

    const float extinction = factors.x;
    const float inscattering = factors.y;
    fragment_color = fragment_color * extinction + mist_color * (1.0 - inscattering);

    float3 sun_position = -normalize(directionalLight.direction.xyz) * distance_to_sun;
    float sun_highlight_factor = max(0, dot(normalize(eye_to_pixel), normalize(sun_position - eye_coord)));
    sun_highlight_factor = pow(sun_highlight_factor, sun_highlight_exponential_factor);

    const float near = 250.0;
    const float far = distance_to_sun;
    float3 sunhighlight_color = lerp(0, sun_highlight_intensity * (normalize(directionalLight.color.rgb)), sun_highlight_factor * smoothstep(near, far, z));
    fragment_color += sunhighlight_color;

    return fragment_color;
}

//色に対して修正を行い、修正された色を返します
float3 cc(float3 color, float factor, float factor2) // color modifier
{
    float w = color.x + color.y + color.z;
    return lerp(color, w * factor, w * factor2);
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

    //大気
    if (ismist == 1)
    {
        sampled_color.rgb = atmosphere(sampled_color.xyz, mist_color.rgb * directionalLight.color.rgb * directionalLight.color.w, world_position.xyz, cameraposition.xyz);
    }

    //レンズフレアとグローライトのエフェクト
    float4 ndc_sun_position = mul(float4(-normalize(directionalLight.direction.xyz) * distance_to_sun, 1), viewProjection);
    ndc_sun_position /= ndc_sun_position.w;
    if (saturate(ndc_sun_position.z) == ndc_sun_position.z)
    {
        float4 occluder;
        occluder.xy = ndc_sun_position.xy;
        occluder.z = depth_map.Sample(sampler_states[BLACK_BORDER_LINEAR], float2(ndc_sun_position.x * 0.5 + 0.5, 0.5 - ndc_sun_position.y * 0.5)).x;
        occluder = mul(float4(occluder.xyz, 1), inverseprojection);
        occluder /= occluder.w;
        float occluded_factor = step(250.0, occluder.z);

        const float2 aspect_correct = float2(1.0 / aspect, 1.0);

        float sun_highlight_factor = max(0, dot(normalize(mul(world_position - float4(cameraposition, 1.0f), view)).xyz, float3(0, 0, 1)));
        float3 lens_flare_color = float3(1.4, 1.2, 1.0) * lens_flare(ndc_position.xy * aspect_correct, ndc_sun_position.xy * aspect_correct);
        lens_flare_color -= noise(ndc_position.xy * 256) * .015;
        lens_flare_color = cc(lens_flare_color, .5, .1);
        sampled_color.rgb += max(0.0, lens_flare_color) * occluded_factor * directionalLight.color.rgb * 0.5;
    }

    // 明るさとコントラストの調整
    sampled_color.rgb = brightness_contrast(sampled_color.rgb, brightness, contrast);

    // 色相と彩度の調整
    sampled_color.rgb = hue_saturation(sampled_color.rgb, hue, saturation);

    // 最終的な色を返す
    return sampled_color;
}
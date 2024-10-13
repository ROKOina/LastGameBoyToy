#include "FullScreenQuad.hlsli"
#include "PostEffect.hlsli"
#include "../../Common.hlsli"
#include "../../Constants.hlsli"
#include "../../3D/Shadow.hlsli"

Texture2D texturemaps : register(t0);
Texture2D depth_map : register(t1);
Texture2DArray shadow_map : register(t2);
Texture2D outlinecolor : register(t3);
Texture2D mrtmap : register(t4);
Texture2D normalmap : register(t5);

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

//fxaa
// https://www.geeks3d.com/20110405/fxaa-fast-approximate-anti-aliasing-demo-glsl-opengl-test-radeon-geforce/
#define FXAA_REDUCE_MIN (1.0/128.0)
#define FXAA_REDUCE_MUL (1.0/8.0)
#define FXAA_SPAN_MAX 8.0
#define FXAA_SUBPIX_SHIFT (1.0 / 4.0)
float3 fxaa(Texture2D tex, float4 pos_pos, float2 rcp_frame)
{
    float3 sampled_nw = tex.Sample(sampler_states[LINEAR], pos_pos.zw).xyz;
    float3 sampled_ne = tex.Sample(sampler_states[LINEAR], pos_pos.zw, int2(1, 0)).xyz;
    float3 sampled_sw = tex.Sample(sampler_states[LINEAR], pos_pos.zw, int2(0, 1)).xyz;
    float3 sampled_se = tex.Sample(sampler_states[LINEAR], pos_pos.zw, int2(1, 1)).xyz;
    float3 sampled_m = tex.Sample(sampler_states[LINEAR], pos_pos.xy).xyz;

    float3 luma = float3(0.299, 0.587, 0.114);
    float luma_nw = dot(sampled_nw, luma);
    float luma_ne = dot(sampled_ne, luma);
    float luma_sw = dot(sampled_sw, luma);
    float luma_se = dot(sampled_se, luma);
    float luma_m = dot(sampled_m, luma);

    float luma_min = min(luma_m, min(min(luma_nw, luma_ne), min(luma_sw, luma_se)));
    float luma_max = max(luma_m, max(max(luma_nw, luma_ne), max(luma_sw, luma_se)));

    float2 dir;
    dir.x = -((luma_nw + luma_ne) - (luma_sw + luma_se));
    dir.y = ((luma_nw + luma_sw) - (luma_ne + luma_se));

    float dir_reduce = max((luma_nw + luma_ne + luma_sw + luma_se) * (0.25 * FXAA_REDUCE_MUL), FXAA_REDUCE_MIN);
    float rcp_dir_min = 1.0 / (min(abs(dir.x), abs(dir.y)) + dir_reduce);
    dir = min(float2(FXAA_SPAN_MAX, FXAA_SPAN_MAX), max(float2(-FXAA_SPAN_MAX, -FXAA_SPAN_MAX), dir * rcp_dir_min)) * rcp_frame;

    float3 rgb_a = (1.0 / 2.0) * (
		tex.Sample(sampler_states[LINEAR], pos_pos.xy + dir * (1.0 / 3.0 - 0.5)).xyz +
		tex.Sample(sampler_states[LINEAR], pos_pos.xy + dir * (2.0 / 3.0 - 0.5)).xyz);
    float3 rgb_b = rgb_a * (1.0 / 2.0) + (1.0 / 4.0) * (
		tex.Sample(sampler_states[LINEAR], pos_pos.xy + dir * (0.0 / 3.0 - 0.5)).xyz +
		tex.Sample(sampler_states[LINEAR], pos_pos.xy + dir * (3.0 / 3.0 - 0.5)).xyz);
    float luma_b = dot(rgb_b, luma);

    return ((luma_b < luma_min) || (luma_b > luma_max)) ? rgb_a : rgb_b;
}

// Poisson Disk PCF sampling
#define MAX_POISSON_DISC_SAMPLES 64
static const float2 poisson_samples[MAX_POISSON_DISC_SAMPLES] =
{
    float2(-0.5119625f, -0.4827938f),
	float2(-0.2171264f, -0.4768726f),
	float2(-0.7552931f, -0.2426507f),
	float2(-0.7136765f, -0.4496614f),
	float2(-0.5938849f, -0.6895654f),
	float2(-0.3148003f, -0.7047654f),
	float2(-0.42215f, -0.2024607f),
	float2(-0.9466816f, -0.2014508f),
	float2(-0.8409063f, -0.03465778f),
	float2(-0.6517572f, -0.07476326f),
	float2(-0.1041822f, -0.02521214f),
	float2(-0.3042712f, -0.02195431f),
	float2(-0.5082307f, 0.1079806f),
	float2(-0.08429877f, -0.2316298f),
	float2(-0.9879128f, 0.1113683f),
	float2(-0.3859636f, 0.3363545f),
	float2(-0.1925334f, 0.1787288f),
	float2(0.003256182f, 0.138135f),
	float2(-0.8706837f, 0.3010679f),
	float2(-0.6982038f, 0.1904326f),
	float2(0.1975043f, 0.2221317f),
	float2(0.1507788f, 0.4204168f),
	float2(0.3514056f, 0.09865579f),
	float2(0.1558783f, -0.08460935f),
	float2(-0.0684978f, 0.4461993f),
	float2(0.3780522f, 0.3478679f),
	float2(0.3956799f, -0.1469177f),
	float2(0.5838975f, 0.1054943f),
	float2(0.6155105f, 0.3245716f),
	float2(0.3928624f, -0.4417621f),
	float2(0.1749884f, -0.4202175f),
	float2(0.6813727f, -0.2424808f),
	float2(-0.6707711f, 0.4912741f),
	float2(0.0005130528f, -0.8058334f),
	float2(0.02703013f, -0.6010728f),
	float2(-0.1658188f, -0.9695674f),
	float2(0.4060591f, -0.7100726f),
	float2(0.7713396f, -0.4713659f),
	float2(0.573212f, -0.51544f),
	float2(-0.3448896f, -0.9046497f),
	float2(0.1268544f, -0.9874692f),
	float2(0.7418533f, -0.6667366f),
	float2(0.3492522f, 0.5924662f),
	float2(0.5679897f, 0.5343465f),
	float2(0.5663417f, 0.7708698f),
	float2(0.7375497f, 0.6691415f),
	float2(0.2271994f, -0.6163502f),
	float2(0.2312844f, 0.8725659f),
	float2(0.4216993f, 0.9002838f),
	float2(0.4262091f, -0.9013284f),
	float2(0.2001408f, -0.808381f),
	float2(0.149394f, 0.6650763f),
	float2(-0.09640376f, 0.9843736f),
	float2(0.7682328f, -0.07273844f),
	float2(0.04146584f, 0.8313184f),
	float2(0.9705266f, -0.1143304f),
	float2(0.9670017f, 0.1293385f),
	float2(0.9015037f, -0.3306949f),
	float2(-0.5085648f, 0.7534177f),
	float2(0.9055501f, 0.3758393f),
	float2(0.7599946f, 0.1809109f),
	float2(-0.2483695f, 0.7942952f),
	float2(-0.4241052f, 0.5581087f),
	float2(-0.1020106f, 0.6724468f),
};

//影
float4 shadow(float2 texcoord)
{
    float depth = depth_map.Sample(sampler_states[LINEAR], texcoord).x;

    uint2 shadow_map_dimensions;
    uint shadow_map_mip_level = 0, shadow_map_number_of_samples, levels;
    shadow_map.GetDimensions(shadow_map_mip_level, shadow_map_dimensions.x, shadow_map_dimensions.y, shadow_map_number_of_samples, levels);

    float4 position_ndc;
    position_ndc.x = texcoord.x * 2 - 1;
    position_ndc.y = texcoord.y * -2 + 1;
    position_ndc.z = depth;
    position_ndc.w = 1;

    float4 position_world_space = mul(position_ndc, inverseviewprojection);
    position_world_space /= position_world_space.w;

    float4 position_view_space = mul(position_ndc, inverseprojection);
    position_view_space /= position_view_space.w;

    uint cascade_index = -1;
    for (uint layer = 0; layer < 4; ++layer)
    {
        if (position_view_space.z < cascadedplanedistances[layer])
        {
            cascade_index = layer;
            break;
        }
    }
    if (cascade_index == -1)
    {
        return float4(1, 1, 1, 1);
    }

    float4 position_light_space = mul(position_world_space, lightviewprojection[cascade_index]);
    position_light_space /= position_light_space.w;

    position_light_space.x = position_light_space.x * 0.5 + 0.5;
    position_light_space.y = position_light_space.y * -0.5 + 0.5;

    float shadow_threshold = 0.0;
    shadow_threshold = shadow_map.SampleCmpLevelZero(comparison_sampler_state, float3(position_light_space.xy, cascade_index), position_light_space.z - shadowdepthbias).x;

    const float2 sample_scale = (0.5 * shadowfilterradius) / shadow_map_dimensions;

    float amass = 0.0;
    for (uint sample_index = 0; sample_index < shadowsamplecount; ++sample_index)
    {
        float2 sample_offset;
        float4 seed = float4(position_ndc.zxz, sample_index);
        uint random = (uint) (64.0 * frac(sin(dot(seed, float4(12.9898, 78.233, 45.164, 94.673))) * 43758.5453)) % 64;
        sample_offset = poisson_samples[random] * sample_scale;

        float2 sample_position = position_light_space.xy + sample_offset;
        amass += shadow_map.SampleCmpLevelZero(comparison_sampler_state, float3(sample_position, cascade_index), position_light_space.z - shadowdepthbias).x;
    }
    shadow_threshold = amass / shadowsamplecount;

    return lerp(float4(1, 1, 1, 1), shadowcolor, shadow_threshold);
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

// SSR
float4 applySSR(half3 normal, float3 viewPosition, half2 screenUV)
{
    float4 color = texturemaps.Sample(sampler_states[LINEAR], screenUV);
    half4 totalSpecular = mrtmap.Sample(sampler_states[LINEAR], screenUV);

    float3 position = viewPosition;
    float2 uvNow;
    float2 uvScale = (screenUV - 0.5f) * float2(2.0f, -2.0f);

    // 視点座標の反射ベクトル計算
    float3 viewDirection = normalize(viewPosition);
    float3 reflectVec = normalize(reflect(viewDirection, normal)); // 反射ベクトル
    const int iteration = 50; // 繰り返し数
    const int maxLength = 5; // 反射最大距離
    float3 delta = reflectVec * (maxLength / (float) iteration); // １回で進む距離

    [loop]
    for (int i = 0; i < iteration; i++)
    {
        position += delta;

        // 射影変換
        float4 projectPosition = mul(projection, float4(position, 1.0));
        uvNow = projectPosition.xy / projectPosition.w * 0.5f + 0.5f;
        uvNow.y = 1.0f - uvNow.y;

        // 深度バッファから値を取得して逆射影
        float z_ndc = depth_map.Sample(sampler_states[POINT], uvNow) * 2.0f - 1.0f; // NDCに変換
        float4 pos;
        pos.z = z_ndc; // NDCの深度値を設定
        pos.xy = uvScale;
        pos.w = 1.0f;

        // 逆射影
        pos = mul(inverseprojection, pos);
        float z = pos.z / pos.w;

        // Z値を比較して反射をブレンド
        [branch]
        if (position.z < z && position.z + ssrparameter.x > z)
        {
            return lerp(color, texturemaps.Sample(sampler_states[LINEAR], uvNow), totalSpecular * ssrparameter.y);
        }
    }
    return color;
}

float4 texViewNormalAndSpecularPower(float2 uv)
{
    // 法線マップから法線をサンプリング
    float4 normalSample = normalmap.Sample(sampler_states[LINEAR], uv);

    // 法線のスケーリング: 法線マップの値を[-1, 1]の範囲に変換
    float3 normal = normalSample.rgb * 2.0 - 1.0;

    // スペキュラーパワーを取得
    float specularPower = normalSample.a;

    // 法線とスペキュラーパワーを含むfloat4を返す
    return float4(normal, specularPower);
}

float4 main(VS_OUT pin) : SV_TARGET
{
    // シーンのテクスチャマップをサンプリング
    float4 sampled_color = texturemaps.Sample(sampler_states[POINT], pin.texcoord.xy);

    // FXAA (Fast Approximate Anti-Aliasing)の適用
    uint mip_level = 0, width, height, number_of_levels;
    texturemaps.GetDimensions(mip_level, width, height, number_of_levels);
    float2 rcp_frame = float2(1.0 / width, 1.0 / height);
    float4 pos_pos;
    pos_pos.xy = pin.texcoord.xy;
    pos_pos.zw = pin.texcoord.xy - (rcp_frame * (0.5 + FXAA_SUBPIX_SHIFT));
    sampled_color.rgb = fxaa(texturemaps, pos_pos, rcp_frame);

    // 影の計算
    float4 shadow_color = shadow(pin.texcoord.xy);
    sampled_color.rgb *= shadow_color.rgb;

    // ビネット効果の適用
    float vignette_factor = vignette(pin.texcoord.xy);
    sampled_color.rgb = lerp(sampled_color.rgb * vignettecolor.rgb, sampled_color.rgb, vignette_factor);

    // 明るさとコントラストの調整
    sampled_color.rgb = brightness_contrast(sampled_color.rgb, brightness, contrast);

    // 色相と彩度の調整
    sampled_color.rgb = hue_saturation(sampled_color.rgb, hue, saturation);

    // 輪郭線効果の描画
    sampled_color += OutlineEffect(pin.texcoord.xy, width, height, inverseviewprojection, cameraposition);

    // 深度マップからの深度値のサンプリング
    float4 pos;
    pos.z = depth_map.Sample(sampler_states[POINT], pin.texcoord.xy) * 2.0f - 1.0f; // NDCに変換
    pos.xy = pin.texcoord.zw; // uvScaleを使用
    pos.w = 1.0f;

    // Z値からビュー座標を取得
    pos = mul(inverseprojection, pos);
    float3 position = pos.xyz / pos.w;

    // 法線の取得と正規化
    half3 normal = texViewNormalAndSpecularPower(pin.texcoord.xy).rgb;
    normal = normalize(normal); // 法線を正規化

    // SSR (Screen Space Reflection)を適用
    float3 reflectionColor = applySSR(normal, position, pin.texcoord.xy).rgb;

    // 色をブレンド
    // 明るさを考慮してブレンド
    float blendFactor = ssrparameter.y * 0.5 + 0.5; // 0.0から1.0の範囲でブレンド
    sampled_color.rgb = lerp(sampled_color.rgb, reflectionColor, blendFactor);

    // 最終的な色を返す
    return sampled_color;
}
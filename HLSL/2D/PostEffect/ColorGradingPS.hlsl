#include "FullScreenQuad.hlsli"
#include "PostEffect.hlsli"
#include "../../Common.hlsli"
#include "../../Constants.hlsli"
#include "../../3D/Shadow.hlsli"

Texture2D texturemaps : register(t0);
Texture2D depth_map : register(t1);
Texture2DArray shadow_map : register(t2);

//明るさ(画面全体の)とコントラスト(画像の明暗)
float3 brightness_contrast(float3 fragment_color, float brightness, float contrast)
{
	//Brightness - Contrast Effect
	//The brightness - contrast effect allows you to modify the brightness and contrast of the rendered image.
	//Brightness: The brighness of the image.Ranges from - 1 to 1 (-1 is solid black, 0 no change, 1 solid white).
	//Contrast : The contrast of the image.Ranges from - 1 to 1 (-1 is solid gray, 0 no change, 1 maximum contrast).
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
	//Hue - Saturation Effect
	//The hue - saturation effect allows you to modify the hue and saturation of the rendered image.
	//Hue: The hue of the image.Ranges from - 1 to 1 (-1 is 180 degrees in the negative direction, 0 no change, 1 is 180 degrees in the postitive direction).
	//Saturation : The saturation of the image.Ranges from - 1 to 1 (-1 is solid gray, 0 no change, 1 maximum saturation).
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

float4 main(VS_OUT pin) : SV_TARGET
{
    //シーンのテクスチャマップをサンプリングしている
    float4 sampled_color = texturemaps.Sample(sampler_states[POINT], pin.texcoord);

    //fxaa
    uint mip_level = 0, width, height, number_of_levels;
    texturemaps.GetDimensions(mip_level, width, height, number_of_levels);
    float2 rcp_frame = float2(1.0 / width, 1.0 / height);
    float4 pos_pos;
    pos_pos.xy = pin.texcoord.xy;
    pos_pos.zw = pin.texcoord.xy - (rcp_frame * (0.5 + FXAA_SUBPIX_SHIFT));
    sampled_color.rgb = fxaa(texturemaps, pos_pos, rcp_frame);

    //影
    float4 shadow_color = shadow(pin.texcoord);
    sampled_color.rgb *= shadow_color.rgb;

    //ビネット
    float vignette_factor = vignette(pin.texcoord);
    sampled_color.rgb = lerp(sampled_color.rgb * vignettecolor.rgb, sampled_color.rgb, vignette_factor);

    //明るさ(画面全体の)とコントラスト(画面の明暗)、色相と彩度
    sampled_color.rgb = hue_saturation(sampled_color.rgb, hue, saturation);
    sampled_color.rgb = brightness_contrast(sampled_color.rgb, brightness, contrast);

    return sampled_color;
}
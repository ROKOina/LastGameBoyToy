#include "FullScreenQuad.hlsli"
#include "../../Common.hlsli"

Texture2D texturemaps : register(t0);

//fxaa
// https://www.geeks3d.com/20110405/fxaa-fast-approximate-anti-aliasing-demo-glsl-opengl-test-radeon-geforce/
#define FXAA_REDUCE_MIN (1.0/128.0)
#define FXAA_REDUCE_MUL (1.0/8.0)
#define FXAA_SPAN_MAX 8.0
#define FXAA_SUBPIX_SHIFT (1.0 / 4.0)
float3 fxaa(float2 texcoord)
{
    uint mip_level = 0, width, height, number_of_levels;
    texturemaps.GetDimensions(mip_level, width, height, number_of_levels);
    float2 rcp_frame = float2(1.0 / width, 1.0 / height);
    float4 pos_pos;
    pos_pos.xy = texcoord;
    pos_pos.zw = texcoord - (rcp_frame * (0.5 + FXAA_SUBPIX_SHIFT));

    float3 sampled_nw = texturemaps.Sample(sampler_states[LINEAR], pos_pos.zw).xyz;
    float3 sampled_ne = texturemaps.Sample(sampler_states[LINEAR], pos_pos.zw, int2(1, 0)).xyz;
    float3 sampled_sw = texturemaps.Sample(sampler_states[LINEAR], pos_pos.zw, int2(0, 1)).xyz;
    float3 sampled_se = texturemaps.Sample(sampler_states[LINEAR], pos_pos.zw, int2(1, 1)).xyz;
    float3 sampled_m = texturemaps.Sample(sampler_states[LINEAR], pos_pos.xy).xyz;

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
		texturemaps.Sample(sampler_states[LINEAR], pos_pos.xy + dir * (1.0 / 3.0 - 0.5)).xyz +
		texturemaps.Sample(sampler_states[LINEAR], pos_pos.xy + dir * (2.0 / 3.0 - 0.5)).xyz);
    float3 rgb_b = rgb_a * (1.0 / 2.0) + (1.0 / 4.0) * (
		texturemaps.Sample(sampler_states[LINEAR], pos_pos.xy + dir * (0.0 / 3.0 - 0.5)).xyz +
		texturemaps.Sample(sampler_states[LINEAR], pos_pos.xy + dir * (3.0 / 3.0 - 0.5)).xyz);
    float luma_b = dot(rgb_b, luma);

    return ((luma_b < luma_min) || (luma_b > luma_max)) ? rgb_a : rgb_b;
}

float4 main(VS_OUT pin) : SV_TARGET
{
    // シーンのテクスチャマップをサンプリング
    float4 sampled_color = texturemaps.Sample(sampler_states[POINT], pin.texcoord.xy);

     // FXAA (Fast Approximate Anti-Aliasing)の適用
    sampled_color.rgb = fxaa(pin.texcoord.xy);

    // 最終的な色を返す
    return sampled_color;
}
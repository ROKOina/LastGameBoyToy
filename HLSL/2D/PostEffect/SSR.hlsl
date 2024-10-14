#include "FullScreenQuad.hlsli"
#include "PostEffect.hlsli"
#include "../../Common.hlsli"
#include "../../Constants.hlsli"

Texture2D texturemaps : register(t0);
Texture2D depth_map : register(t1);
Texture2D normalmap : register(t2);
Texture2D emissivemap : register(t3);

// 深度からワールド空間の位置を求める
float3 ComputeWorldPositionFromDepth(float4x4 inverse_view_projection, float depth, float2 texcoord)
{
    float4 projected_position = float4(texcoord * 2.0f - 1.0f, depth, 1.0f);
    projected_position.y = -projected_position.y;
    float4 position = mul(projected_position, inverse_view_projection);
    return position.xyz / position.w;
}
float ComputeDepth(float4 clip_pos)
{
    return clip_pos.z / clip_pos.w;
}
// 法線デコード
float3 DecodeNormal(float3 enc)
{
    return (2.0f * enc) - 1.0f;
}
float noise(float2 seed)
{
    return frac(sin(dot(seed.xy, float2(12.9898, 78.233))) * 43758.5453);
}

// SSR
float4 applySSR(float2 texcoord)
{
    float depth = depth_map.Sample(sampler_states[LINEAR], texcoord).r;
    float3 position = ComputeWorldPositionFromDepth(inverseviewprojection, depth, texcoord);

    float4 color = texturemaps.Sample(sampler_states[LINEAR], texcoord);
    float4 normal_smoothness = normalmap.Sample(sampler_states[LINEAR], texcoord);
    float4 emission_reflection = emissivemap.Sample(sampler_states[LINEAR], texcoord);
    float3 normal = DecodeNormal(normal_smoothness.xyz);
    float reflection = emission_reflection.a;

    float3 cam_dir = normalize(position - cameraposition);
    float3 ref_dir = normalize(reflect(cam_dir, normal));

    int max_ray_num = ssrparameter.x;
    float max_length = ssrparameter.y;
    float3 steps = max_length / max_ray_num * ref_dir;
    float max_thickness = ssrparameter.z / max_ray_num;
    float alpha = ssrparameter.w * reflection;

	[loop]
    for (int n = 1; n < max_ray_num; ++n)
    {
        float3 ray = n * steps;
		//float3 ray = (n + noise(pin.texcoord + cb_frame_time.total_time)) * steps;
        float3 ray_pos = position.xyz + ray;
        float4 vp_pos = mul(float4(ray_pos, 1.0f), viewProjection);
        float2 ray_uv = vp_pos.xy / vp_pos.w * float2(0.5f, -0.5f) + 0.5f;
        if (max(abs(ray_uv.x - 0.5f), abs(ray_uv.y - 0.5f)) > 0.5f)
            break;

        float ray_depth = ComputeDepth(vp_pos);
        float depth2 = depth_map.SampleLevel(sampler_states[LINEAR], ray_uv, 0).r;
        [branch]
        if (ray_depth - depth2 > 0.0f && ray_depth - depth2 < max_thickness)
        {
            float a = alpha * pow(min(1.0f, (max_length * 0.5f) / length(ray)), 2.0f);
            color = color * (1 - a) + texturemaps.SampleLevel(sampler_states[LINEAR], ray_uv, 0) * a;

            break;
        }
    }

    return color;
}

float4 main(VS_OUT pin) : SV_TARGET
{
    // シーンのテクスチャマップをサンプリング
    float4 sampled_color = texturemaps.Sample(sampler_states[POINT], pin.texcoord.xy);

    // SSR (Screen Space Reflection)を適用
    sampled_color.rgb += applySSR(pin.texcoord.xy).rgb;

    return sampled_color;
}
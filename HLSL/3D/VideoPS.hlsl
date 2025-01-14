#include "Video.hlsli"
#include "../Common.hlsli"

Texture2D VideoTexture : register(t0);

float3 YUVToRGB(float3 yuv)
{
    // BT.601 coefs
    static const float3 yuvCoef_r = { 1.164f, 0.000f, 1.596f };
    static const float3 yuvCoef_g = { 1.164f, -0.392f, -0.813f };
    static const float3 yuvCoef_b = { 1.164f, 2.017f, 0.000f };
    yuv -= float3(0.0625f, 0.5f, 0.5f);
    return saturate(float3(
    dot(yuv, yuvCoef_r),
    dot(yuv, yuvCoef_g),
    dot(yuv, yuvCoef_b)
    ));
}

float4 main(VS_OUT pin) : SV_TARGET
{
    float y = VideoTexture.Sample(sampler_states[LINEAR], float2(pin.texcoord.x, pin.texcoord.y * 0.5)).r;
    float u = VideoTexture.Sample(sampler_states[LINEAR], float2(pin.texcoord.x * 0.5, 0.50 + pin.texcoord.y * 0.25)).r;
    float v = VideoTexture.Sample(sampler_states[LINEAR], float2(pin.texcoord.x * 0.5, 0.75 + pin.texcoord.y * 0.25)).r;
    float4 color = float4(YUVToRGB(float3(y, u, v)), 1.f);
    color.rgb = pow(color, GAMMA);
    color.rgb = pow(color, 1.0 / GAMMA);
    return color;
}
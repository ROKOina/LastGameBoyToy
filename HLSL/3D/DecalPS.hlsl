#include "Decal.hlsli"
#include "../Common.hlsli"
#include "../Constants.hlsli"

Texture2D DecalMap : register(t0);
Texture2D DepthMap : register(t8);

float4 main(float4 sv_position : SV_POSITION) : SV_TARGET
{
    float2 dimension;
    DepthMap.GetDimensions(dimension.x, dimension.y);
    float2 texcoord = sv_position.xy / dimension;
    float depth = DepthMap.Sample(sampler_states[BLACK_BORDER_LINEAR], texcoord);
    float4 ndc = float4(2.0 * texcoord.x - 1.0, 1.0 - 2.0 * texcoord.y, depth, 1.0);
    float4 position = mul(ndc, inverseviewprojection);
    position /= position.w;
    position = mul(position, decalinverseprojection);
    position /= position.w;
    float2 decaltexcoord = float2(position.x * +0.5f + 0.5f, position.y * -0.5f + 0.5f);
    float4 color = DecalMap.Sample(sampler_states[BLACK_BORDER_LINEAR], decaltexcoord);
    color.rgb = pow(color.rgb, GAMMA);
    clip(color.a - 0.5);

    return float4(color.rgb, color.a);
}
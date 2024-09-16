#include "../Common.hlsli"
#include "PostEffect/FullScreenQuad.hlsli"

Texture2D texturemaps : register(t0);
Texture2D positionmap : register(t1);
Texture2D generalcolor : register(t2);
Texture2D currentscene : register(t3);

cbuffer ProjectionMappingConstants : register(b12)
{
    row_major float4x4 projection_mapping_transform;
};

float4 main(VS_OUT pin) : SV_TARGET
{
    float4 wPos = positionmap.Sample(sampler_states[BLACK_BORDER_POINT], pin.texcoord);
    float4 currentscenecolor = currentscene.Sample(sampler_states[BLACK_BORDER_POINT], pin.texcoord);
    float generatecolor = positionmap.Sample(sampler_states[BLACK_BORDER_POINT], pin.texcoord).r;

    float4 projection_mapping_color = 0;
    float4 projection_texture_position = mul(wPos, projection_mapping_transform);
    projection_texture_position /= projection_texture_position.w;
    projection_texture_position.x = projection_texture_position.x * 0.5 + 0.5;
    projection_texture_position.y = -projection_texture_position.y * 0.5 + 0.5;
    if (saturate(projection_texture_position.z) == projection_texture_position.z)
    {
        float4 projection_texture_color = texturemaps.Sample(sampler_states[BLACK_BORDER_LINEAR], projection_texture_position.xy);
        projection_mapping_color = projection_texture_color;
        projection_mapping_color.rgb = pow(projection_mapping_color.rgb, GAMMA);
    }

    return float4(currentscenecolor.rgb + projection_mapping_color.rgb, projection_mapping_color.a);
}
#ifndef	__SCENE_HLSLI__
#define	__SCENE_HLSLI__

cbuffer SceneConstants : register(b10)
{
    row_major float4x4 view;
    row_major float4x4 projection;
    row_major float4x4 viewProjection;
    row_major float4x4 inverseview;
    row_major float4x4 inverseprojection;
    row_major float4x4 inverseviewprojection;
    float3 cameraposition;
    float time;
    float3 bossposiotn;
    float deltatime;
    float2 cameraScope;
    float2 screenResolution;
}

#endif
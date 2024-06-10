cbuffer SceneConstants : register(b10)
{
    row_major float4x4 view;
    row_major float4x4 projection;
    row_major float4x4 viewProjection;
    row_major float4x4 inverseview;
    row_major float4x4 inverseprojection;
    float3 cameraposition;
    float time;
    float deltatime;
    float3 SCdummy;
}
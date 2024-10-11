struct INSTANCE_VS_IN
{
    float3 position : POSITION;
    float3 normal : NORMAL;
    float4 tangent : TANGENT;
    float2 texcoord : TEXCOORD;
    float4 color : COLOR;

    row_major float4x4 transform : TRANSFORM;
};
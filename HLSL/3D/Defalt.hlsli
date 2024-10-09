struct VS_OUT
{
    float4 position : SV_POSITION;
    float3 world_position : POSITION;
    float3 normal : NORMAL;
    float3 tangent : TANGENT;
    float3 binormal : BINORMAL;
    float2 texcoord : TEXCOORD;
    float4 color : COLOR;
};

cbuffer MaterialInformation : register(b2)
{
    float4 materialcolor;
    float3 emissivecolor;
    float emissiveintensity;
    float Metalness;
    float Roughness;
    float2 Mdummy;
};
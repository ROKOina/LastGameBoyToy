struct VS_OUT
{
    float4 position : SV_POSITION;
    float2 texcoord : TEXCOORD;
    float4 color : COLOR;
};

struct VS_IN
{
    float4 position : POSITION;
    float2 texcoord : TEXCOORD;
    float4 color : COLOR;
};

cbuffer VideoConstants : register(b1)
{
    row_major float4x4 world;
};
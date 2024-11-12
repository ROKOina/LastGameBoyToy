struct VS_OUT
{
    float4 position : SV_POSITION;
    float4 color : COLOR;
    float2 texcoord : TEXCOORD;
};

struct VS_IN
{
    float3 position : POSITION;
    float2 texcoord : TEXCOORD;
    float4 color : COLOR;
};

cbuffer TrailConstants : register(b0)
{
    float4 trailcolor;
    float3 trailcolorscale;
    float padding;
    float2 uvscroll;
    float2 padding2;
}
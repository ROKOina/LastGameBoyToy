struct VS_OUT
{
	float4 position : SV_POSITION;
	float4 color    : COLOR;
	float2 texcoord : TEXCOORD;
};

cbuffer SpriteConstants : register(b0)
{
    float2 uvscroll;
    float cliptime;
    float edgethreshold;
    float edgeoffset;
    float3 edgecolor;
    float4x4 world;
};
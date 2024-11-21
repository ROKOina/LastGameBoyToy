struct VS_OUT
{
	float4 position : SV_POSITION;
	float4 color    : COLOR;
	float2 texcoord : TEXCOORD;
};

cbuffer SpriteConstants : register(b0)
{
    float3 blurcolor;
    float blurdistance;
    float blurpower;
    float luminance;
    float2 uvscroll;
    float3 edgecolor;
    float cliptime;
    float edgethreshold;
    float edgeoffset;
    float2 padding;
};
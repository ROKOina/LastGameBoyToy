#include "../../Common/Constants.hlsli"

struct VS_IN
{
    float3 Position : POSITION;
    float4 Rotate : ROTATION;
    float3 Normal : NORMAL;
    float2 Size : TEXCOORD;
    float4 Color : COLOR;
    float4 Param : PARAM;
};

struct PS_IN
{
    float4 Color : PS_COLOR;
    float2 Size : PS_TEXCOORD;
    float4 Position : SV_POSITION;
};

struct GS_IN
{
    float3 Position : GS_POSITION;
    float4 Rotate : GS_ROTATION;
    float3 Normal : GS_NORMAL;
    float2 Size : GS_TEXCOORD;
    float4 Color : GS_COLOR;
    float4 Param : GS_PARAM;
};

cbuffer CPUParticleConstants : register(b5)
{
    float4 cpuparticlecolor;
    float3 cpuparticleluminance;
    float cpuparticledummy;
}
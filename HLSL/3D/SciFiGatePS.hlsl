#include "Defalt.hlsli"
#include "../Constants.hlsli"
#include "../Common.hlsli"

Texture2D BaseTexture : register(t0);
Texture2D BaseNoise : register(t1);
Texture2D CurtainTexture : register(t2);
Texture2D GateArea : register(t3);

Texture2D DepthMap : register(t8);

cbuffer EffectCircleConstants : register(b0)
{
    float2 uvScrollDir1;
    float2 uvScrollDir2;
    
    float2 uvScale1;
    float2 uvScale2;
    float simulateTime1;
    float simulateTime2;
    float intensity1;
    float intensity2;
    
    float4 effectColor1;
    float4 effectColor2;
}

float4 main(VS_OUT pin) : SV_TARGET
{
    float4 color = 0;
    
    // ベース
    float4 base = BaseTexture.Sample(sampler_states[LINEAR], pin.texcoord * uvScale1) * effectColor1;
    // ベースのノイズ
    float baseNoise = BaseNoise.Sample(sampler_states[LINEAR], pin.texcoord + uvScrollDir1 * simulateTime1).r;
    color = base * baseNoise * intensity1;
    
    // カーテン
    float4 curtain = CurtainTexture.Sample(sampler_states[LINEAR], pin.texcoord * uvScale2 + uvScrollDir2 * simulateTime2).rgbr * effectColor2;
    color += curtain * intensity2;
    
    // エリア
    float area = GateArea.Sample(sampler_states[LINEAR], pin.texcoord).r;
    color *= area;
    
    return color;
}
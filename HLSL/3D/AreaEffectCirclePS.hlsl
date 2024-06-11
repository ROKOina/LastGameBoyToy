#include "Defalt.hlsli"
#include "../Constants.hlsli"
#include "../Common.hlsli"

cbuffer EffectCircleConstants : register(b10)
{
    float simulateTime1 = 0.0f;
    float simulateTime2 = 0.0f;
    float simulateTime3 = 0.0f;
    float waveEffectRange = 3.0f;
}

Texture2D OuterCircle : register(t0); //外側のサークル
Texture2D MiddleCircle : register(t1); //中間のサークル
Texture2D PointCircle : register(t2); //中心部のサークル
Texture2D EffectRamp : register(t3); //サークル内の光の波用のランプテクスチャ

float4 main(VS_OUT pin) : SV_TARGET
{
    float4 color = OuterCircle.Sample(sampler_states[LINEAR], pin.texcoord);
    
    
    
    clip(color.a - 0.1f);
    color.rgb += +emissivecolor.rgb * emissiveintensity;
    
    return color;
}
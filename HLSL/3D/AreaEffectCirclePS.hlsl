#include "Defalt.hlsli"
#include "../Constants.hlsli"
#include "../Common.hlsli"

cbuffer EffectCircleConstants : register(b0)
{
    float simulateTime1;
    float simulateTime2;
    float simulateTime3;
    float waveEffectRange;
    
    float4 waveEffectColor;
    
    float waveEffectIntensity;
    float3 dummy;
}

Texture2D OuterCircle : register(t0); //外側のサークル
Texture2D MiddleCircle : register(t1); //中間のサークル
Texture2D CenterCircle : register(t2); //中心部のサークル
Texture2D EffectRamp : register(t3); //サークル内の光の波用のランプテクスチャ

// UVを回転させる
half2 RotateUV(float timer, float2 texcoord)
{
    half angleCos = cos(timer);
    half angleSin = sin(timer);
    half2x2 rotateMatrix = half2x2(angleCos, -angleSin, angleSin, angleCos);
    half2 uv = texcoord - 0.5;

    return mul(uv, rotateMatrix) + 0.5;
}

float4 main(VS_OUT pin) : SV_TARGET
{
    // 外側のサークル
    float4 color = OuterCircle.Sample(sampler_states[LINEAR], pin.texcoord);
    
    // 中間のサークル
    half2 midUV = RotateUV(simulateTime1, pin.texcoord);
    color += MiddleCircle.Sample(sampler_states[LINEAR], midUV);
    
    // 中心のサークル
    half2 centerUV = RotateUV(simulateTime2, pin.texcoord);
    color += CenterCircle.Sample(sampler_states[LINEAR], centerUV);
    
    // エミッシブを反映
    color.rgb *= emissivecolor * emissiveintensity;
    
    // 周期的に現れるオーラ
    half centerDist = length(pin.texcoord * 2.0 - 1.0);
    float wave = EffectRamp.Sample(sampler_states[LINEAR], float2(abs(centerDist) - simulateTime3, 0));
    color += wave * waveEffectColor * waveEffectIntensity * step(centerDist, waveEffectRange) * step(color.a, 0);
    
    // 限りなく透明に近いピクセルは破棄する
    clip(color.a - EPSILON);
    
    return color;
}
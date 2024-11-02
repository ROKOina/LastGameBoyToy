#include "Defalt.hlsli"
#include "../Common/Constants.hlsli"
#include "../Common/Common.hlsli"
#include "../Common/DepthFunctions.hlsli"

Texture2D BaseTexture : register(t0);
Texture2D BaseNoise : register(t1);
Texture2D CurtainTexture : register(t2);
Texture2D GateArea : register(t3);

Texture2D DepthMap : register(t8);

cbuffer SciFiGateConstants : register(b0)
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

    float contourIntensity;
    float contourSensitive;
    float2 SFDummy;
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

    // 深度値による輪郭
    {
        // 深度マップの読み込み
        float2 screenUV = pin.position.xy / screenResolution;
        float depth = DepthMap.Sample(sampler_states[WHITE_BORDER_ANISOTROPIC], screenUV).r;

        float4 contour = effectColor1 * contourIntensity;
        float contourPower = (1 - saturate(contourSensitive * (depth - pin.position.z))) * step(pin.position.z, depth);

        color += contour * contourPower;
    }

    return color;
}
#include "Defalt.hlsli"
#include "../Constants.hlsli"
#include "../Common.hlsli"
#include "../DepthFunctions.hlsli"

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
    
    // �x�[�X
    float4 base = BaseTexture.Sample(sampler_states[LINEAR], pin.texcoord * uvScale1) * effectColor1;
    // �x�[�X�̃m�C�Y
    float baseNoise = BaseNoise.Sample(sampler_states[LINEAR], pin.texcoord + uvScrollDir1 * simulateTime1).r;
    color = base * baseNoise * intensity1;
    
    // �J�[�e��
    float4 curtain = CurtainTexture.Sample(sampler_states[LINEAR], pin.texcoord * uvScale2 + uvScrollDir2 * simulateTime2).rgbr * effectColor2;
    color += curtain * intensity2;
    
    // �G���A
    float area = GateArea.Sample(sampler_states[LINEAR], pin.texcoord).r;
    color *= area;
    
    // �[�x�l�ɂ��֊s
    // �[�x�}�b�v�̓ǂݍ��� ( �Ȃ������E���]���Ă���̂ŁAUV�����E���]���Ă��� )
    float depth = DepthMap.Sample(sampler_states[WHITE_BORDER_ANISOTROPIC], pin.position.xy / (float2(1920, 1080) * 0.8)).r;
    color += effectColor1 * (1 - saturate(500 * (depth - pin.position.z))) * step(pin.position.z, depth) * 2;
    
    return color;
}
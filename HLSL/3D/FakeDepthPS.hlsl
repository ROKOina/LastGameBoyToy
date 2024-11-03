#include "FakeDepth.hlsli"
#include "../Constants.hlsli"
#include "../Common.hlsli"

Texture2D BottomTex : register(t0);
Texture2D HeightMap : register(t1);

float2 BumpOffset(float3 cameraTangent, float height, float heightRatio)
{
    float2 result = cameraTangent.xy * height * heightRatio;
    result.y *= -1.0;
    
    return result;
}

float4 main(VS_OUT pin) : SV_TARGET
{
    // �[�x�̑傫��
    float depthScale = 0.25;

    float height = HeightMap.Sample(sampler_states[BLACK_BORDER_LINEAR], pin.texcoord);
    clip(height - 0.00001);
    
    // ���̊����o�����߂�UV�̃Y������
    float2 offset = BumpOffset(pin.cameraTangent, -height, depthScale);
    // �Y������UV
    float2 offsetUV = pin.texcoord + offset;
    float4 color = BottomTex.Sample(sampler_states[LINEAR], offsetUV);
    
    // �ǂ��ǂ������f����
    int isWall = step(HeightMap.Sample(sampler_states[BLACK_BORDER_LINEAR], offsetUV).r, 0);
    
    color = lerp(color, 0, saturate(isWall)) ;
    color.a = 1.0;
    
    return color;
}
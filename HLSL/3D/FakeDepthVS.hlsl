#include "FakeDepth.hlsli"
#include "../Common/Constants.hlsli"

VS_OUT main(VS_IN vin)
{
    float3 p = { 0, 0, 0 };
    float3 n = { 0, 0, 0 };
    float3 t = { 0, 0, 0 };

    for (int i = 0; i < 4; i++)
    {
        p += (vin.boneWeights[i] * mul(vin.position, boneTransforms[vin.boneIndices[i]])).xyz;
        n += (vin.boneWeights[i] * mul(float4(vin.normal.xyz, 0), boneTransforms[vin.boneIndices[i]])).xyz;
        t += (vin.boneWeights[i] * mul(float4(vin.tangent.xyz, 0), boneTransforms[vin.boneIndices[i]])).xyz;
    }

    VS_OUT vout;
    vout.position = mul(float4(p, 1.0f), viewProjection);
    vout.world_position = p;
    vout.normal = normalize(n);
    vout.tangent = normalize(t);
    vout.binormal = normalize(cross(vout.tangent, vout.normal));
    vout.color.rgb = vin.color.rgb * materialcolor.rgb;
    vout.color.a = vin.color.a * materialcolor.a;
    vout.texcoord = vin.texcoord;
    
    // 接空間(タンジェント)のカメラベクトル
    float3 cameraVec = normalize(cameraposition.xyz - p.xyz);
    float3x3 rotation = float3x3(vout.tangent.xyz, vout.binormal, vout.normal);
    vout.cameraTangent = normalize(mul(rotation, cameraVec));
    
    return vout;
}
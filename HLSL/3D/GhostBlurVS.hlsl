#include "Defalt.hlsli"
#include "../Constants.hlsli"

cbuffer CbGhostBlur : register(b0)
{
    float blurThreshold;
    float3 pad;
    
    row_major float4x4 oldBoneTransforms[MAX_BONES];
};

VS_OUT main(VS_IN vin)
{
    // スキニング
    float3 p = { 0, 0, 0 };
    float3 n = { 0, 0, 0 };
    float3 t = { 0, 0, 0 };
    float3 oldP = { 0, 0, 0 }; // 前のフレームの頂点位置
    for (int i = 0; i < 4; i++)
    {
        p += (vin.boneWeights[i] * mul(vin.position, boneTransforms[vin.boneIndices[i]])).xyz;
        n += (vin.boneWeights[i] * mul(float4(vin.normal.xyz, 0), boneTransforms[vin.boneIndices[i]])).xyz;
        t += (vin.boneWeights[i] * mul(float4(vin.tangent.xyz, 0), boneTransforms[vin.boneIndices[i]])).xyz;
        
        // 前のフレームの座標
        oldP += (vin.boneWeights[i] * mul(vin.position, oldBoneTransforms[vin.boneIndices[i]])).xyz;
    }
    n = normalize(n);
    
    // 移動ベクトル
    float3 moveVec = oldP.xyz - p.xyz;
    float moveVecLength = dot(moveVec, moveVec);
    if (moveVecLength > blurThreshold * blurThreshold)
    {
        float3 moveVecNormalize = normalize(moveVec);

        // 法線が移動ベクトルと同じ方向か確認
        float blurRate = max(dot(moveVecNormalize, n), 0.0);
        p += moveVecNormalize * sqrt(moveVecLength) * blurRate;
    }
    
    VS_OUT vout;
    vout.position = mul(float4(p, 1.0f), viewProjection);
    vout.world_position = p;
    
    vout.normal = n;
    vout.tangent = normalize(t);
    // 外積をして縦法線を算出
    vout.binormal = cross(vout.tangent, vout.normal);
    
    vout.color = vin.color * materialcolor;
    vout.texcoord = vin.texcoord;
    
    return vout;
}

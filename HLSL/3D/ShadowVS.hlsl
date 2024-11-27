#include "Defalt.hlsli"
#include "Shadow.hlsli"

VS_OUT_CSM main(VS_IN vin, uint instance_id : SV_INSTANCEID)
{
    float3 p = { 0, 0, 0 };
    for (int i = 0; i < 4; i++)
    {
        // ボーンインデックスを取得
        int boneIndex = vin.boneIndices[i];

        // 最終ボーン行列 = offsetTransform * boneTransform
        float4x4 finalBoneMatrix = mul(offsetTransforms[boneIndex], boneTransforms[boneIndex]);

        // 頂点位置のスキニング
        p += vin.boneWeights[i] * mul(vin.position, finalBoneMatrix).xyz;
    }

    VS_OUT_CSM vout;
    vout.position = mul(float4(p, 1.0f), lightviewprojection[instance_id]);
    vout.slice = instance_id;

    return vout;
}
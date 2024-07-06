#include "Defalt.hlsli"
#include "Shadow.hlsli"

VS_OUT_CSM main(VS_IN vin, uint instance_id : SV_INSTANCEID)
{
    float3 p = { 0, 0, 0 };
    for (int i = 0; i < 4; i++)
    {
        p += (vin.boneWeights[i] * mul(vin.position, boneTransforms[vin.boneIndices[i]])).xyz;
    }

    VS_OUT_CSM vout;
    vout.position = mul(float4(p, 1.0f), lightviewprojection[instance_id]);
    vout.slice = instance_id;

    return vout;
}
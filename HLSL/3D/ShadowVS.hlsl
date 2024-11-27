#include "Defalt.hlsli"
#include "Shadow.hlsli"

VS_OUT_CSM main(VS_IN vin, uint instance_id : SV_INSTANCEID)
{
    float3 p = { 0, 0, 0 };
    for (int i = 0; i < 4; i++)
    {
        // �{�[���C���f�b�N�X���擾
        int boneIndex = vin.boneIndices[i];

        // �ŏI�{�[���s�� = offsetTransform * boneTransform
        float4x4 finalBoneMatrix = mul(offsetTransforms[boneIndex], boneTransforms[boneIndex]);

        // ���_�ʒu�̃X�L�j���O
        p += vin.boneWeights[i] * mul(vin.position, finalBoneMatrix).xyz;
    }

    VS_OUT_CSM vout;
    vout.position = mul(float4(p, 1.0f), lightviewprojection[instance_id]);
    vout.slice = instance_id;

    return vout;
}
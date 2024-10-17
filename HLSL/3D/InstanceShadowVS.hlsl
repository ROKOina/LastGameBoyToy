#include "Shadow.hlsli"
#include "Defalt.hlsli"

VS_OUT_CSM main(INSTANCE_VS_IN vin, uint instance_id : SV_INSTANCEID)
{
    // �C���X�^���X�̃��[���h�ϊ��s����v�Z
    float4x4 world = vin.transform;

    VS_OUT_CSM vout;

    // ���_���W�ɃO���[�o���ϊ��s���K�p
    float3 p = mul(float4(vin.position, 1.0f), world).xyz;

    // ���C�g�̃r���[�v���W�F�N�V�����s���K�p���ăN���b�v��ԍ��W���v�Z
    vout.position = mul(float4(p, 1.0f), lightviewprojection[instance_id % 4]);
    vout.slice = instance_id;

    return vout;
}
#include "Instancing.hlsli"
#include "Defalt.hlsli"
#include "../Constants.hlsli"

VS_OUT main(INSTANCE_VS_IN vin)
{
    VS_OUT vout;
    float4x4 world = mul(global_transform, calc_world_transform(vin.InstScale, vin.InstRotation, vin.InstPosition));

    // �|�W�V�����̕ϊ�
    vout.position = mul(float4(vin.position, 1.0f), mul(world, viewProjection));
    vout.world_position = mul(float4(vin.position.xyz, 1.0f), world);

    // �@���Ɛڐ��̕ϊ�
    float4x4 normalMatrix = world;
    normalMatrix[3] = float4(0, 0, 0, 1); // ���s�ړ�����������
    vout.normal = normalize(mul(float4(vin.normal.xyz, 0.0f), normalMatrix)).xyz;
    vout.tangent = normalize(mul(float4(vin.tangent.xyz, 0.0f), normalMatrix)).xyz;
    vout.binormal = normalize(cross(vout.tangent, vout.normal));

    // �J���[�ƃe�N�X�`�����W�̕ϊ�
    vout.color.rgb = vin.color.rgb * materialcolor.rgb;
    vout.color.a = vin.color.a * materialcolor.a;
    vout.texcoord = vin.texcoord;
    return vout;
}
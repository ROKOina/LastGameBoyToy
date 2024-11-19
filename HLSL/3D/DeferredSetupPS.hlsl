#include "Defalt.hlsli"
#include "Light.hlsli"
#include "PBR+IBLFunction.hlsli"
#include "../Constants.hlsli"

// pow�֐��ɕ��̒l������\����񍐂���x���𖳌���
#pragma warning (disable : 3571)

Texture2D DiffuseMap : register(t0);   // �J���[�e�N�X�`��
Texture2D NormalMap : register(t1);    // �@���e�N�X�`��
Texture2D MetallicMap : register(t2);  // ���^���b�N�}�b�v
Texture2D RoughnessMap : register(t3); // ���t�l�X�}�b�v
Texture2D AOMap : register(t4);        // AO�}�b�v
Texture2D EmissionMap : register(t5);  // �G�~�b�V�����}�b�v
Texture2D DissolveMap : register(t23);  // �f�B�]���u�}�b�v

// �A�E�g���C���̒萔�o�b�t�@
cbuffer General : register(b11)
{
    float3 outlineColor;
    float outlineintensity;
    float dissolveThreshold; // �f�B�]���u臒l
    float3 dissolveEdgeColor; // �G�b�W�̐F
    float dissolveEdgeWidth; // �G�b�W��
    float3 lastpadding;
}

// MRT�Ή�
struct PS_OUT
{
    float4 diffuse : SV_TARGET0;
    float4 normal : SV_TARGET1;
    float4 position : SV_TARGET2;
    float4 MRO : SV_TARGET3;
    float4 emission : SV_TARGET4;
    float4 outline : SV_TARGET5;
};

/*
�f�t�@�[�h�`��p�̃����_�[�^�[�Q�b�g�ɁA
�F�A�@���A�ʒu�A�����x�A�e���A���Օ��A�����̏�����������ł����V�F�[�_�[
*/
PS_OUT main(VS_OUT pin)
{
    PS_OUT pout = (PS_OUT) 0;

    // �f�B�]���u�}�b�v���T���v�����O
    float dissolveValue = DissolveMap.Sample(sampler_states[LINEAR], pin.texcoord).r;
    float dalpha = smoothstep(dissolveThreshold, dissolveThreshold, dissolveValue);

    // �F�e�N�X�`�����T���v�����O
    float4 diffuseColor = DiffuseMap.Sample(sampler_states[ANISOTROPIC], pin.texcoord) * pin.color;

    // ���S�ɓ����ȏꍇ�̓s�N�Z����j��
    if (dalpha <= 0.0)
    {
        discard; // �s�N�Z����j��
    }

    // ���j�A��Ԃɕϊ�
    pout.diffuse = diffuseColor;
    pout.diffuse.rgb = pow(pout.diffuse.rgb, GAMMA);

     // �A���t�@�l��K�p
    pout.diffuse.a = dalpha;

    // �@���}�b�v
    float3 normal = NormalMap.Sample(sampler_states[ANISOTROPIC], pin.texcoord).xyz;
    normal = normal * 2 - 1;
    // �ϊ��p��3x3�s���p��
    float3x3 CM =
    {
        pin.tangent,
        pin.binormal,
        pin.normal
    };
    // �@���}�b�v�Ŏ擾�����@�����ɕϊ��s����|�����킹��
    float3 N = normalize(mul(normal, CM));
    // ���[���h��Ԃ̖@�� (0 �` 1) �̊ԂɃX�P�[�����O
    N = (N * 0.5) + 0.5;
    pout.normal = float4(N, 1.0);

    pout.position = float4(pin.world_position, 1.0f);

    // �����e�N�X�`�����T���v�����O
    float metallic = saturate(MetallicMap.Sample(sampler_states[LINEAR], pin.texcoord).r * Metalness);
    // ����e�N�X�`�����T���v�����O
    float roughness = saturate(RoughnessMap.Sample(sampler_states[LINEAR], pin.texcoord).r * Roughness);
    // AO�}�b�v
    float ao = AOMap.Sample(sampler_states[LINEAR], pin.texcoord).r;

    pout.MRO = float4(metallic, roughness, ao, 1.0f);

    // �G�~�b�V����
    float3 emission = EmissionMap.Sample(sampler_states[LINEAR], pin.texcoord).rgb * emissivecolor.rgb * emissiveintensity;
    pout.emission = float4(emission.rgb, alpha);

    //�A�E�g���C��
    pout.outline = float4(outlineColor * outlineintensity, 1.0f);

    return pout;
}
#include "Defalt.hlsli"
#include "Light.hlsli"
#include "PBR+IBLFunction.hlsli"
#include "../Constants.hlsli"

// pow�֐��ɕ��̒l������\����񍐂���x���𖳌���
#pragma warning (disable : 3571)

Texture2D DiffuseMap : register(t0); //�J���[�e�N�X�`��
Texture2D NormalMap : register(t1); //�@���e�N�X�`��
Texture2D MetallicMap : register(t2); //���^���b�N�}�b�v
Texture2D RoughnessMap : register(t3); //���t�l�X�}�b�v
Texture2D AOMap : register(t4); //AO�}�b�v
Texture2D EmissionMap : register(t5); //�G�~�b�V�����}�b�v
Texture2D SkyBox : register(t6); //skybox
TextureCube diffuseIem : register(t7); // �v���t�B���^�����O�σX�J�C�{�b�N�X(Diffuse)
TextureCube specularPmrem : register(t8); // �v���t�B���^�����O�σX�J�C�{�b�N�X(Specular)
Texture2D lutGGX : register(t9); // �X�J�C�{�b�N�X�̐F�Ή��\

//MRT�Ή�
struct PS_OUT
{
    float4 diffuse : SV_TARGET0;
    float4 normal : SV_TARGET1;
    float4 position : SV_TARGET2;
    float4 MRO : SV_TARGET3;
    float4 emission : SV_TARGET4;
};

/*

�f�t�@�[�h�`��p�̃����_�[�^�[�Q�b�g�ɁA
�F�A�@���A�ʒu�A�����x�A�e���A���Օ��A�����̏�����������ł����V�F�[�_�[

*/
PS_OUT main(VS_OUT pin)
{
    PS_OUT pout = (PS_OUT) 0;
    
    //�F�e�N�X�`�����T���v�����O
    {
        float4 diffusecolor = DiffuseMap.Sample(sampler_states[ANISOTROPIC], pin.texcoord) * pin.color;
        // ���j�A��Ԃɕϊ�
        pout.diffuse = diffusecolor;
        pout.diffuse.rgb = pow(pout.diffuse.rgb, GAMMA);
    }
    
    // �@���}�b�v
    {
        // �@���}�b�v����xyz�������擾����( -1 �` +1 )�̊ԂɃX�P�[�����O
        float3 normal = NormalMap.Sample(sampler_states[ANISOTROPIC], pin.texcoord).xyz;
        normal = normal * 2 - 1;
	    // �ϊ��p��3X3�s���p��
        float3x3 CM =
        {
            pin.tangent,
            pin.binormal,
            pin.normal
        };
	    // �@���}�b�v�Ŏ擾�����@�����ɕϊ��s����|�����킹��
        float3 N = normalize(mul(normal, CM));
        // ���[���h��Ԃ̖@��( 0 �` 1 )�̊ԂɃX�P�[�����O
        N = (N * 0.5) + 0.5;
        pout.normal = float4(N, 1.0);
    }
    
    pout.position = float4(pin.world_position, 1.0f);
    
    //�����e�N�X�`�����T���v�����O
    float metallic = saturate(MetallicMap.Sample(sampler_states[LINEAR], pin.texcoord).r * Metalness);
    //����e�N�X�`�����T���v�����O
    float roughness = saturate(RoughnessMap.Sample(sampler_states[LINEAR], pin.texcoord).r * Roughness);
    //AO�}�b�v
    float ao = AOMap.Sample(sampler_states[LINEAR], pin.texcoord).r;
    
    pout.MRO = float4(metallic, roughness, ao, 1.0f);

    //�G�~�b�V����
    float3 emission = EmissionMap.Sample(sampler_states[LINEAR], pin.texcoord).rgb * emissivecolor.rgb * emissiveintensity;
    pout.emission = float4(emission.rgb, 1.0f);
    
    return pout;
}
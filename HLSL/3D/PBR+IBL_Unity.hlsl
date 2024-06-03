#include "Defalt.hlsli"
#include "Light.hlsli"
#include "PBR+IBLFunction.hlsli"
#include "../Constants.hlsli"

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

PS_OUT main(VS_OUT pin)
{
    //�F�e�N�X�`�����T���v�����O
    float4 diffusecolor = DiffuseMap.Sample(sampler_states[ANISOTROPIC], pin.texcoord) * pin.color;
    diffusecolor.rgb = pow(diffusecolor.rgb, GAMMA);
    
    //�@���e�N�X�`�����T���v�����O
    float3 normal = NormalMap.Sample(sampler_states[ANISOTROPIC], pin.texcoord).xyz * 2 - 1;
    float3x3 CM = { normalize(pin.tangent), normalize(pin.binormal), normalize(pin.normal.xyz) };
    float3 N = normalize(mul(normal, CM));

    //�����e�N�X�`�����T���v�����O
    float metallic = saturate(MetallicMap.Sample(sampler_states[LINEAR], pin.texcoord).r * Metalness);

    //����e�N�X�`�����T���v�����O
    float roughness = saturate(RoughnessMap.Sample(sampler_states[LINEAR], pin.texcoord).r * Roughness);

    //AO�}�b�v
    float ao = AOMap.Sample(sampler_states[LINEAR], pin.texcoord).r;

    //�G�~�b�V����
    float3 emission = EmissionMap.Sample(sampler_states[LINEAR], pin.texcoord).rgb * emissivecolor.rgb * emissiveintensity;

    //�����x�N�g���ƃ��C�g�̕����̐��K��
    float3 L = normalize(directionalLight.direction.xyz);
    float3 E = normalize(cameraposition.xyz - pin.world_position.xyz);

     //���ˌ��̂����g�U���˂ɂȂ銄��
    float3 diffuseReflectance = lerp(diffusecolor.rgb, 0.02f, metallic);

	//�������ˎ��̃t���l�����˗�
    float3 F0 = lerp(dot(_DielectricF0, Specular * Specular), diffusecolor.rgb, metallic);

    //Indirect Diffuse
    float3 indirectDiffuse = ShadeSHPerPixel(N, diffusecolor, pin.world_position.xyz);

    //�����ɂ��g�U���ˁ{���ʔ���
    float3 envColor = 0;
    {
        envColor = IBL(lutGGX, diffuseIem, specularPmrem, sampler_states[BLACK_BORDER_ANISOTROPIC], diffuseReflectance, F0, roughness, N, E);
    }

    //�ŏI�̐F
    float4 color = BRDF(diffusecolor, metallic, roughness, N, E, -L, directionalLight.color.rgb, indirectDiffuse, envColor);

    //AO�}�b�v�K�p
    color.rgb *= ao;

    //�g�[���}�b�v
    color.rgb = saturate(color.rgb); //�N�����v

    //�G�~�b�V�����K�p
    color.rgb += emission;

    //MRT�o��
    PS_OUT ret;
    ret.diffuse = color;
    ret.normal = mul(float4(N, 0.0f), view);
    ret.position = mul(pin.world_position, view);
    ret.MRO = float4(metallic, roughness, ao, 1.0f);
    ret.emission = float4(emission.rgb, 1.0f);
    return ret;
}
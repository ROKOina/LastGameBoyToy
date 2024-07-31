#include "FullScreenQuad.hlsli"
#include "../../3D/PBR+IBLFunction.hlsli"
#include "../../3D/Light.hlsli"
#include "../../Common.hlsli"
#include "../../Constants.hlsli"

Texture2D colorMap : register(t0);
Texture2D normalMap : register(t1); // �@���}�b�v
Texture2D positionMap : register(t2); // �ʒu�}�b�v
Texture2D MRAO_Map : register(t3); // ���^���b�N���t�l�X���Օ��}�b�v
Texture2D emissiveMap : register(t4); // ���Ȕ����}�b�v
TextureCube diffuseIem : register(t11); // �v���t�B���^�����O�σX�J�C�{�b�N�X(Diffuse)
TextureCube specularPmrem : register(t12); // �v���t�B���^�����O�σX�J�C�{�b�N�X(Specular)
Texture2D lutGGX : register(t13); // �X�J�C�{�b�N�X�̐F�Ή��\

float4 main(VS_OUT pin) : SV_TARGET
{
    // �e�N�X�`������p�����[�^�[�擾
    float4 albedoColor = colorMap.Sample(sampler_states[BLACK_BORDER_POINT], pin.texcoord);
    clip(albedoColor.a - EPSILON);

    // ���[���h��Ԃ̖@��
    float3 N = normalMap.Sample(sampler_states[BLACK_BORDER_POINT], pin.texcoord).xyz;
    N = normalize(N * 2 - 1); // -1 ~ 1 �ɃX�P�[��

    // ���[���h���W
    float4 wPos = positionMap.Sample(sampler_states[BLACK_BORDER_POINT], pin.texcoord);

    // �����x�E�e���E���Օ�
    float3 MRAO = MRAO_Map.Sample(sampler_states[BLACK_BORDER_POINT], pin.texcoord).rgb;

    // ���C�g�x�N�g���A�J�����x�N�g���𐳋K��
    float3 L = normalize(directionalLight.direction.xyz);
    float3 V = normalize(cameraposition.xyz - wPos.xyz);

    // ���ˌ��̂����g�U���˂ɂȂ銄��
    float3 diffuseReflectance = lerp(albedoColor.rgb, 0.02f, MRAO.x);

    // �������ˎ��̃t���l�����˗�
    float3 F0 = lerp(dot(_DielectricF0, Specular * Specular), albedoColor.rgb, MRAO.x);

    // �Ԑڌ��ɂ��g�U����
    float3 indirectDiffuse = ShadeSHPerPixel(N, albedoColor, wPos.xyz).xyz;

    // �����ɂ��g�U���ˁ{���ʔ���
    float3 envColor = IBL(lutGGX, diffuseIem, specularPmrem, sampler_states[BLACK_BORDER_ANISOTROPIC], diffuseReflectance, F0, MRAO.r, N, V);

    // �ŏI�̐F
    float4 color = BRDF(albedoColor, MRAO.x, MRAO.y, N, V, -L, directionalLight.color.rgb, indirectDiffuse, envColor);

    // AO�}�b�v�K�p
    color.rgb *= MRAO.z;

    // �g�[���}�b�v
    color.rgb = saturate(color.rgb); // �N�����v

    // �G�~�b�V�����K�p
    color.rgb += emissiveMap.Sample(sampler_states[BLACK_BORDER_POINT], pin.texcoord).rgb;

    return color;
}
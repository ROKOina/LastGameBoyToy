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
    float4 albedoColor = colorMap.Sample(sampler_states[BLACK_BORDER_POINT], pin.texcoord.xy);

    // ���[���h��Ԃ̖@��
    float3 N = normalMap.Sample(sampler_states[BLACK_BORDER_POINT], pin.texcoord.xy).xyz;
    N = normalize(N * 2 - 1); // -1 ~ 1 �ɃX�P�[��

    // ���[���h���W
    float4 wPos = positionMap.Sample(sampler_states[BLACK_BORDER_POINT], pin.texcoord.xy);

    // �����x�E�e���E���Օ�
    float3 MRAO = MRAO_Map.Sample(sampler_states[BLACK_BORDER_POINT], pin.texcoord.xy).rgb;

    // �J�����x�N�g��
    float3 V = normalize(cameraposition.xyz - wPos.xyz);

    // ���ˌ��̂����g�U���˂ɂȂ銄��
    float3 diffuseReflectance = lerp(albedoColor.rgb, 0.02f, MRAO.x);

    // �������ˎ��̃t���l�����˗�
    float3 F0 = lerp(float3(0.04f, 0.04f, 0.04f), albedoColor.rgb, MRAO.x);

    // �Ԑڌ��ɂ��g�U����
    float3 indirectDiffuse = ShadeSHPerPixel(N, albedoColor).xyz;

    // �����ɂ��g�U���ˁ{���ʔ���
    float3 envColor = IBL(lutGGX, diffuseIem, specularPmrem, sampler_states[BLACK_BORDER_ANISOTROPIC], diffuseReflectance, F0, MRAO.r, N, V);

    // �������̊g�U���˂Ƌ��ʔ���
    float3 color = 0.0f;
    float3 L = normalize(directionalLight.direction.xyz);
    color += BRDF(albedoColor, MRAO.x, MRAO.y, N, V, -L, directionalLight.color.rgb, indirectDiffuse, envColor).rgb * directionalLight.color.rgb;

    // �|�C���g���C�g�̌v�Z
    for (int i = 0; i < 2; ++i)
    {
        float3 L = pointLight[i].position.xyz - wPos.xyz;
        float distance = length(L);
        L = normalize(L);

        // ��������
        float attenuation = saturate(1.0 - (distance / pointLight[i].range));
        attenuation *= attenuation; // ������������œK�p

        color += BRDF(albedoColor, MRAO.x, MRAO.y, N, V, L, pointLight[i].color.rgb, indirectDiffuse, envColor).rgb * pointLight[i].color.rgb * attenuation;
    }

    // �X�|�b�g���C�g�̌v�Z
    for (int k = 0; k < 2; ++k)
    {
        float3 L = spotLight[k].position.xyz - wPos.xyz;
        float distance = length(L);
        L = normalize(L);

        // �X�|�b�g���C�g�̊p�x����
        float spotEffect = dot(-L, normalize(spotLight[k].direction.xyz));
        float innerCos = cos(spotLight[k].innerCorn);
        float outerCos = cos(spotLight[k].outerCorn);
        float spotAttenuation = saturate((spotEffect - outerCos) / (innerCos - outerCos));

        // ��������
        float distanceAttenuation = saturate(1.0 - (distance / spotLight[k].range));
        distanceAttenuation *= distanceAttenuation;

        color += BRDF(albedoColor, MRAO.x, MRAO.y, N, V, L, spotLight[k].color.rgb, indirectDiffuse, envColor).rgb * spotLight[k].color.rgb * distanceAttenuation * spotAttenuation;
    }

    // AO�}�b�v�K�p
    color *= MRAO.z;

    // �G�~�b�V�����K�p
    color += emissiveMap.Sample(sampler_states[BLACK_BORDER_POINT], pin.texcoord.xy).rgb;

    if (albedoColor.a < EPSILON)
        discard;

    // ���̒l�ɂȂ�Ȃ��悤�ɐ���
    color = max(color, 0);

    return float4(color, 1.0);
}
#include "Defalt.hlsli"
#include "../Constants.hlsli"
#include "../Common.hlsli"
#include "../3D/Light.hlsli"

Texture2D DiffuseMap : register(t0); // �J���[�e�N�X�`��
Texture2D NormalMap : register(t1); // �@���e�N�X�`��
Texture2D MetallicMap : register(t2); // ���^���b�N�}�b�v
Texture2D RoughnessMap : register(t3); // ���t�l�X�}�b�v
Texture2D AOMap : register(t4); // AO�}�b�v
Texture2D EmissionMap : register(t5); // �G�~�b�V�����}�b�v

float4 main(VS_OUT pin) : SV_TARGET
{
    // �e�N�X�`���̎擾
    float3 albedo = DiffuseMap.Sample(sampler_states[ANISOTROPIC], pin.texcoord).rgb * pin.color.rgb; 
    
    // ���F�����p�̌W��
    float skinSaturationBoost = 6.5; // ���F�̍ʓx�𒲐�����W��
    float3 skinToneColor = float3(0.9725, 0.8902, 0.8588); // ���F�̊�F�i�I�����W���̐F�j

    // ���F�̋�������
    float similarity = dot(normalize(albedo), normalize(skinToneColor)); // �A���x�h�F�Ɗ�F�̗ގ��x
    if (similarity > 0.95)
    { // ���F�ɋ߂��F�����𒲐�
        float grayscale = dot(albedo, float3(0.3, 0.59, 0.11)); // �O���[�X�P�[���ϊ�
        albedo = lerp(float3(grayscale, grayscale, grayscale), albedo, skinSaturationBoost);
    }
    
    // �����e�N�X�`�����T���v�����O
    float metallic = saturate(MetallicMap.Sample(sampler_states[LINEAR], pin.texcoord).r * Metalness);
    // ����e�N�X�`�����T���v�����O
    float roughness = saturate(RoughnessMap.Sample(sampler_states[LINEAR], pin.texcoord).r * Roughness);

    float3 viewDir = normalize(cameraposition- pin.world_position );

    float3 lightDir = -directionalLight.direction.xyz;
    
    // ���̋��x�v�Z
    float NdotL = dot(pin.normal, lightDir);

    // �X�e�b�v�֐��Ō��̋��x��i�K�I�ɂ���i�g�D�[���V�F�[�f�B���O���ʁj
    float lightIntensity = saturate(step(0.1, NdotL));
    lightIntensity = max(lightIntensity, 0.2f);

    // �x�[�X�J���[�i�f�B�t���[�Y�j
    float3 baseColor = albedo * directionalLight.color.rgb * lightIntensity;
    
    // �������C�g�i�G�b�W�������j
    float rim = 1.0 - max(dot(viewDir, pin.normal), 0.0);
    float rimPower = 0.55; // �������C�g�̋��x�𒲐�����W��
    float3 rimLight = saturate(smoothstep(0.2, 0.5, rim * rim)) * albedo * rimPower;

    // �ŏI�J���[����
    float3 color = baseColor + rimLight + emissivecolor * emissiveintensity;
    return float4(color, 1.0);
}
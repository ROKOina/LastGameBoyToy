#include "../../Common/Common.hlsli"
#include "CPUParticle.hlsli"

Texture2D texturemap : register(t0); // �J���[�e�N�X�`��
Texture2D depthmap : register(t8); // �[�x�e�N�X�`���i�\�t�g�p�[�e�B�N���p�j

float4 main(PS_IN input) : SV_TARGET
{
    // �e�N�X�`���J���[���擾���A���j�A��Ԃɕϊ�
    float4 color = texturemap.Sample(sampler_states[LINEAR], input.Size);

    // �K���}�␳�̋ߎ� (�ʏ�̃K���}�l��2.2�̂��߁Asqrt�ŋߎ�)
    color.rgb = pow(color.rgb, GAMMA);

    // �J���[�ƃA���t�@�̒���
    color.a *= input.Color.a * cpuparticlecolor.a;
    color.rgb *= input.Color.rgb * cpuparticlecolor.rgb;
    color.rgb *= cpuparticleluminance;

    // �\�t�g�p�[�e�B�N������
    float particleDepth = input.Position.z / input.Position.w; // �p�[�e�B�N���̐[�x
    float sceneDepth = depthmap.Sample(sampler_states[LINEAR], input.Size).r; // �V�[���̐[�x

    // �p�[�e�B�N���ƃV�[���̐[�x�����v�Z
    float depthDifference = sceneDepth - particleDepth;

    // �J�����̋������擾�i���ɃJ�����ʒu�� (0, 0, 0) �̏ꍇ�j
    float cameraDistance = length(cameraposition); // �J��������p�[�e�B�N���܂ł̋���

    // �t�F�[�h�͈͂𓮓I�ɐݒ�
    float minFadeRange = 0.05; // �ŏ��̃t�F�[�h�J�n�ʒu
    float maxFadeRange = 0.2; // �ŏ��̃t�F�[�h�I���ʒu

    if (cameraDistance < 5.0) // �J�������߂��ꍇ
    {
        minFadeRange = 0.01; // �t�F�[�h�J�n�ʒu�����߂�
        maxFadeRange = 0.1; // �t�F�[�h�I���ʒu�����߂�
    }

    // �t�F�[�h�͈͂̒���
    float fadeFactor = saturate((depthDifference - minFadeRange) / (maxFadeRange - minFadeRange));
    color.a *= fadeFactor;

    // �A���t�@���ɒ[�ɒႢ�ꍇ�͔j�������ARGB���[����
    if (color.a < EPSILON)
    {
        color.rgb = float3(0, 0, 0);
        color.a = 0;
    }

    return color;
}
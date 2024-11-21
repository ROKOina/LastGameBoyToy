#include "../Common.hlsli"
#include "CPUParticle.hlsli"

Texture2D texturemap : register(t0); // �J���[�e�N�X�`��
Texture2D depthmap : register(t8); // �[�x�e�N�X�`���i�\�t�g�p�[�e�B�N���p�j

float4 main(PS_IN input) : SV_TARGET
{
 // �e�N�X�`���J���[���擾
    float4 color = texturemap.Sample(sampler_states[LINEAR], input.Size);
    color.rgb = pow(color.rgb, GAMMA);

    // �J���[�ƃA���t�@�̒���
    color.a *= input.Color.a * cpuparticlecolor.a;
    color.rgb *= input.Color.rgb * cpuparticlecolor.rgb;
    color.rgb *= cpuparticleluminance;

    // �����j��: �A���t�@���Ⴂ�ꍇ
    if (color.a < EPSILON)
        discard;

    // �[�x�t�F�[�h�p�ϐ��i�\�t�g�p�[�e�B�N���j
    float fadeFactor = 1.0;

    // LOD�̓���: �J���������ɉ����Čv�Z�̏ȗ�
    // �p�[�e�B�N���̃X�N���[����̑傫�����m�F
    float particleSize = abs(input.Size.x * input.Position.w); // ���_��Ԃł̃p�[�e�B�N���T�C�Y
    if (particleSize > 0.01) // ���ȏ�̃T�C�Y�̂ݐ[�x�v�Z��K�p
    {
        // �\�t�g�p�[�e�B�N������
        float particleDepth = input.Position.z / input.Position.w; // �p�[�e�B�N���̐[�x
        float sceneDepth = depthmap.Sample(sampler_states[LINEAR], input.Size).r; // �V�[���̐[�x

        // �p�[�e�B�N���ƃV�[���̐[�x�����v�Z
        float depthDifference = sceneDepth - particleDepth;

        // �[�x�����͈͓����`�F�b�N���A�͈͓��̏ꍇ�̂݃t�F�[�h�������s��
        fadeFactor = saturate((depthDifference - 0.1) / (0.5 - 0.1));
        fadeFactor = lerp(0.0, 1.0, fadeFactor); // �t�F�[�h�̐��`���
    }

    // �A���t�@�l�Ƀt�F�[�h�t�@�N�^�[��K�p
    color.a *= fadeFactor;

    // �ēx�j��: �A���t�@���Ⴂ�ꍇ
    if (color.a < EPSILON)
        discard;

    return color;
}
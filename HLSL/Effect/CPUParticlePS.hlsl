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

    // �A���t�@���ɒ[�ɒႯ��΁A�����ɔj��
    if (color.a < EPSILON)
        discard;

    // �\�t�g�p�[�e�B�N������
    float particleDepth = input.Position.z / input.Position.w; // �p�[�e�B�N���̐[�x
    float sceneDepth = depthmap.Sample(sampler_states[LINEAR], input.Size).r; // �V�[���̐[�x

    // �p�[�e�B�N���ƃV�[���̐[�x�����v�Z
    float depthDifference = sceneDepth - particleDepth;

    // �[�x�����͈͓����`�F�b�N���A�͈͓��̏ꍇ�̂݃t�F�[�h�������s��
    if (depthDifference > 0.1 && depthDifference < 0.5)
    {
        // �t�F�[�h�͈͂̒����i0.1�`0.5�͈̔͂Ńt�F�[�h���s���j
        float fadeFactor = saturate((depthDifference - 0.1) / (0.5 - 0.1));
        color.a *= fadeFactor;
    }
    else if (depthDifference <= 0.1)
    {
        // �p�[�e�B�N�����V�[��������O�ɂ���ꍇ�A�A���t�@�����炷
        color.a *= 0.0;
    }

    // �A���t�@���ēx�Ⴂ�ꍇ�A�j��
    if (color.a < EPSILON)
        discard;

    return color;
}
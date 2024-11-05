#include "GPUParticle.hlsli"
#include "../../Common/Common.hlsli"
#include "../../Common/Constants.hlsli"

Texture2D colortexture : register(t20);
Texture2D depthmap : register(t8); // �[�x�e�N�X�`���i�\�t�g�p�[�e�B�N���p�j

float4 main(GS_OUT pin) : SV_TARGET
{
    // �e�N�X�`������F���T���v�����A���j�A��Ԃɕϊ�
    float4 color = colortexture.Sample(sampler_states[LINEAR], pin.texcoord);

    // �K���}�␳���ߎ��I�ɍs�� (�ʏ�̃K���}�l�� 2.2 �̏ꍇ�Asqrt�ŋߎ�)
    color.rgb = sqrt(color.rgb);

    // �s���̐F�ƃA���t�@���|���Z
    color.a *= pin.color.a;
    color.rgb *= pin.color.rgb;

    // �\�t�g�p�[�e�B�N������
    float particleDepth = pin.position.z / pin.position.w; // �p�[�e�B�N���̐[�x
    float sceneDepth = depthmap.Sample(sampler_states[LINEAR], pin.texcoord).r; // �V�[���̐[�x

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

    // �A���t�@�����ɒႢ�ꍇ��RGB�l�����̂܂܂ɂ���idiscard �̎g�p�������j
    if (color.a < EPSILON)
    {
        color.rgb = float3(0, 0, 0);
        color.a = 0;
    }

    // 0�ȉ��ɂ��Ȃ��悤�ɂ���
    color.rgb = max(color.rgb, 0);

    return color;
}
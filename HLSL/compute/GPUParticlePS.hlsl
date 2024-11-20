#include "GPUParticle.hlsli"
#include "../Common.hlsli"

Texture2D colortexture : register(t20);
Texture2D depthmap : register(t8); // �[�x�e�N�X�`���i�\�t�g�p�[�e�B�N���p�j

float4 main(GS_OUT pin) : SV_TARGET
{
    //�e�N�X�`������F���T���v�����A���j�A��Ԃɕϊ�
    float4 color = colortexture.Sample(sampler_states[LINEAR], pin.texcoord);

    //�t�K���}�␳�Ń��j�A��Ԃɕϊ� (�ʏ�̃K���}�l�� 2.2)
    color.rgb = pow(color.rgb, GAMMA);

    //�s���̐F�ƃA���t�@���|���Z
    color.a *= pin.color.a;
    color.rgb *= pin.color.rgb;

    // �\�t�g�p�[�e�B�N������
    //float particleDepth = pin.position.z / pin.position.w; // �p�[�e�B�N���̐[�x
    //float sceneDepth = depthmap.Sample(sampler_states[LINEAR], pin.texcoord).r; // �V�[���̐[�x

     // �p�[�e�B�N���ƃV�[���̐[�x�����v�Z
    //float depthDifference = sceneDepth - particleDepth;

    // �t�F�[�h�͈͂̒����i0.1�`0.5�͈̔͂Ńt�F�[�h���s���j
    //float fadeFactor = saturate((depthDifference - 0.1) / (0.5 - 0.1));
    //color.a *= fadeFactor;

    //�A���t�@���Ⴂ�ꍇ�͔j��
    if (color.a < EPSILON)
        discard;

    //0�ȉ��ɂ��Ȃ��悤�ɂ���
    color.rgb = max(color.rgb, 0);

    return color;
}
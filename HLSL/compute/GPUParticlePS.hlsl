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

    //�A���t�@���Ⴂ�ꍇ�͔j��
    if (color.a < EPSILON)
        discard;

    //0�ȉ��ɂ��Ȃ��悤�ɂ���
    color.rgb = max(color.rgb, 0);

    return color;
}
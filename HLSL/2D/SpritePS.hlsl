#include "sprite.hlsli"
#include "../Common.hlsli"

Texture2D texturemaps : register(t0);
Texture2D noisemap : register(t1);
Texture2D rampmap : register(t2);

float delayTime = 0.4; // Clip Time�̕ϊ��Ɏg���x������

float4 main(VS_OUT pin) : SV_TARGET
{
    // �e�N�X�`������F���T���v�����A�s���̃J���[���|���Z
    float4 color = texturemaps.Sample(sampler_states[POINT], pin.texcoord) * pin.color;

    // �m�C�Y�}�b�v����m�C�Y�l���T���v��
    float noise = noisemap.Sample(sampler_states[POINT], pin.texcoord).r;

     // �m�C�Y�l�ɃX���[�W���O��K�p
    float smoothedNoise = smoothstep(0.0, 1.0, noise);

    // Clip Time��0~1����0~1.4�Ƀ}�b�v
    float clipTime = 1.0 + delayTime; // Clip Time�͈̔͏�� (1.0 + 0.4)
    float mappedClipTime = (clipTime - 1.0) / delayTime * smoothedNoise;

    // ���f����h��Ԃ����߂̃m�C�Y��l��
    float alphaClip = step(cliptime, smoothedNoise);

    // �t�`�����点�邽�߂̏���
    float edgeGlow = smoothstep(edgethreshold - edgeoffset, edgethreshold, smoothedNoise);

    //Clip Threshold �� 0 �̂Ƃ��A�t�`�̏����𖳎�
    if (cliptime <= 0.0)
    {
        // �A���t�@�N���b�v�̏������s�킸�A���̂܂ܐF��Ԃ�
        return color;
    }

    // �����v�}�b�v����J���[�O���f�[�V�������T���v��
    // �m�C�Y�l��0~1�͈̔͂Ŏg���ăJ���[����
    float4 gradientColor = rampmap.Sample(sampler_states[POINT], float2(smoothedNoise, 0.0));

    // �m�C�Y�̒l�� Clip Time ������镔���� 0 �ɂ��鏈��
    float maskedColor = step(cliptime, smoothedNoise); // �N���b�s���O�̂��߂̃}�X�N

    // Clip Threshold �� 0 ���傫���ꍇ�̏���
    // �m�C�Y�l�Ɋ�Â��ĐF��RGB�����𒲐�
    color.rgb *= maskedColor; // �m�C�Y�̐F�� ClipTime ������镔���� 0 �ɂ���

    // �t�`�̌�����F�ɒǉ�
    color.rgb += edgeGlow * edgecolor; // ���̐F�𒲐�

    // �����v�}�b�v�Œ��F
    color.rgb *= gradientColor.rgb;

    // �͈͕ϊ����Clip Time�Ɋ�Â��A���t�@�e�X�g
    if (color.a < 0.00001 || alphaClip < mappedClipTime)
        discard;

    return color;
}
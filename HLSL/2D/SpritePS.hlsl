#include "sprite.hlsli"
#include "../Common.hlsli"
#include "../Constants.hlsli"

Texture2D texturemaps : register(t0);
Texture2D noisemap : register(t1);
Texture2D rampmap : register(t2);

// �����_���Ȓl�𐶐�
float random(float2 seeds)
{
    return frac(sin(dot(seeds, float2(12.9898, 78.233))) * 43758.5453);
}

// �u���b�N��̃m�C�Y����
float blockNoise(float2 seeds)
{
    return random(floor(seeds));
}

// -1����1�܂ł͈̔͂Ń����_���l�𐶐�
float noiserandom(float2 seeds)
{
    return -1.0 + 2.0 * blockNoise(seeds);
}

// HSL����RGB�ւ̕ϊ��֐�
float3 HSLtoRGB(float h, float s, float l)
{
    float3 rgb = l.xxx;
    if (s != 0)
    {
        float q = l < 0.5 ? l * (1.0 + s) : l + s - l * s;
        float p = 2.0 * l - q;
        float3 t = float3(h + 1.0 / 3.0, h, h - 1.0 / 3.0);

        for (int i = 0; i < 3; i++)
        {
            if (t[i] < 0.0)
                t[i] += 1.0;
            if (t[i] > 1.0)
                t[i] -= 1.0;
            if (t[i] < 1.0 / 6.0)
                rgb[i] = p + (q - p) * 6.0 * t[i];
            else if (t[i] < 1.0 / 2.0)
                rgb[i] = q;
            else if (t[i] < 2.0 / 3.0)
                rgb[i] = p + (q - p) * (2.0 / 3.0 - t[i]) * 6.0;
            else
                rgb[i] = p;
        }
    }
    return rgb;
}

float delayTime = 0.4; // Clip Time�̕ϊ��Ɏg���x������

float4 main(VS_OUT pin) : SV_TARGET
{
    //// �O���b�`�G�t�F�N�g�̏����ݒ�
    //float2 gv = pin.texcoord;
    //float glichnoise = blockNoise(pin.texcoord.y * 10);
    //glichnoise += random(pin.texcoord.x) * 0.3;
    //float2 randomvalue = noiserandom(float2(pin.texcoord.y, time * 10));
    //gv.x += randomvalue * sin(sin(0.1) * .5) * sin(-sin(glichnoise) * .2) * frac(time) * 0;

    //// �z���O�������̐F�ω����v�Z (HSL�𗘗p)
    //float glitchTimeFactor = frac(time * 2.0); // �F���ς��p�x�𒲐�
    //float hue = frac(pin.texcoord.y + glitchTimeFactor); // y���W�Ǝ��ԂɊ�Â��ĐF����ω�
    //float saturation = 0.9; // �����ʓx�őN�₩�ȐF
    //float luminance = 0.5 + 0.1 * sin(time * 3.0); // ���ԂƋ��ɕω�����P�x
    //float4 glitchRGB = float4(HSLtoRGB(hue, saturation, luminance), 1.0f);

    // �e�N�X�`������F���T���v�����A�s���̃J���[���|���Z
    float4 color = texturemaps.Sample(sampler_states[LINEAR], pin.texcoord) /** glitchRGB*/;
    color.a *= pin.color.a;
    if (!(color.a - EPSILON))
        discard;
    color.rgb = pow(color.rgb, max(GAMMA, 1)) * pin.color.rgb;

    //// �m�C�Y�}�b�v����m�C�Y�l���T���v��
    //float noise = noisemap.Sample(sampler_states[POINT], pin.texcoord).r;

    // // �m�C�Y�l�ɃX���[�W���O��K�p
    //float smoothedNoise = smoothstep(0.0, 1.0, noise);

    //// Clip Time��0~1����0~1.4�Ƀ}�b�v
    //float clipTime = 1.0 + delayTime; // Clip Time�͈̔͏�� (1.0 + 0.4)
    //float mappedClipTime = (clipTime - 1.0) / delayTime * smoothedNoise;

    //// ���f����h��Ԃ����߂̃m�C�Y��l��
    //float alphaClip = step(cliptime, smoothedNoise);

    //// �t�`�����点�邽�߂̏���
    //float edgeGlow = smoothstep(edgethreshold - edgeoffset, edgethreshold, smoothedNoise);

    //// �����v�}�b�v����J���[�O���f�[�V�������T���v��
    //// �m�C�Y�l��0~1�͈̔͂Ŏg���ăJ���[����
    //float4 gradientColor = rampmap.Sample(sampler_states[POINT], float2(smoothedNoise, 0.0));

    //// �m�C�Y�̒l�� Clip Time ������镔���� 0 �ɂ��鏈��
    //float maskedColor = step(cliptime, smoothedNoise); // �N���b�s���O�̂��߂̃}�X�N

    ////Clip Threshold �� 0 �̂Ƃ��A�t�`�̏����𖳎�
    //if (cliptime <= EPSILON)
    //{
    //    // �A���t�@�N���b�v�̏������s�킸�A���̂܂ܐF��Ԃ�
    //    return color;
    //}

    //// Clip Threshold �� 0 ���傫���ꍇ�̏���
    //// �m�C�Y�l�Ɋ�Â��ĐF��RGB�����𒲐�
    //color.rgb *= maskedColor; // �m�C�Y�̐F�� ClipTime ������镔���� 0 �ɂ���

    //// �t�`�̌�����F�ɒǉ�
    //color.rgb += edgeGlow * edgecolor; // ���̐F�𒲐�

    //// �����v�}�b�v�Œ��F
    //color.rgb *= gradientColor.rgb;

    //// �͈͕ϊ����Clip Time�Ɋ�Â��A���t�@�e�X�g
    //if (color.a < EPSILON || alphaClip < mappedClipTime)
    //    discard;

    return color;
}
#include "sprite.hlsli"
#include "../Common.hlsli"
#include "../Constants.hlsli"

Texture2D texturemaps : register(t0);

float4 main(VS_OUT pin) : SV_TARGET
{
    //�e�N�X�`������F���T���v�����A���j�A��Ԃɕϊ�
    float4 color = texturemaps.Sample(sampler_states[TRANSPARENT_BORDER_LINEAR], pin.texcoord);

    //�t�K���}�␳�Ń��j�A��Ԃɕϊ� (�ʏ�̃K���}�l�� 2.2)
    color.rgb = pow(color.rgb, GAMMA);

    //�s���̐F�ƃA���t�@���|���Z
    color.a *= pin.color.a;
    color.rgb *= pin.color.rgb;

    //�A���t�@���Ⴂ�ꍇ�͔j��
    if (color.a < EPSILON)
        discard;

    //�ŏI�I�ȐF�ɃK���}�␳��K�p (�t�K���}�� 1 / 2.2 = �� 0.4545)
    color.rgb = pow(color.rgb, 1.0 / GAMMA);

    return color;
}
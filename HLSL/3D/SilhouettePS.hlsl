#include "Defalt.hlsli"
#include "../Common/Common.hlsli"
#include "../Common/Constants.hlsli"
#include "Light.hlsli"

Texture2D NormalMap : register(t1); // �@���e�N�X�`��

[earlydepthstencil]
float4 main(VS_OUT pin) : SV_TARGET
{
    // �@���}�b�v
    float3 normal = NormalMap.Sample(sampler_states[ANISOTROPIC], pin.texcoord).xyz;
    normal = normal * 2 - 1;

    // �ϊ��p��3x3�s���p��
    float3x3 CM =
    {
        pin.tangent,
        pin.binormal,
        pin.normal
    };

    // �@���}�b�v�Ŏ擾�����@�����ɕϊ��s����|�����킹��
    float3 N = normalize(mul(normal, CM));

    // ���[���h��Ԃ̖@�� (0 �` 1) �̊ԂɃX�P�[�����O
    N = (N * 0.5) + 0.5;

    // �@�����g�������ʊ�
    float bumpEffect = saturate(N.z); // �@����Z�����ŉ��ʊ��𒲐�

    // ���ʊ����x�[�X�J���[�ɔ��f(�����萔�o�b�t�@�[�ɕύX)
    float4 color = float4(1, 0.5, 0, 1);
    color.rgb *= lerp(1.0, bumpEffect, 0.8); // ���ʌ��ʂ��_�炩���F�ɓK�p

    // �ŏI�I�ȐF������N�₩�ɂ��邽�߂̒���
    color.rgb = pow(color.rgb, 1.0f / GAMMA); // �K���}�␳�Ŕ��F������

    return color;
}
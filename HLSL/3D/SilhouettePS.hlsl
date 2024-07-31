#include "Defalt.hlsli"
#include "../Common.hlsli"
#include "../Constants.hlsli"

Texture2D DiffuseMap : register(t0);// �F�e�N�X�`��
Texture2D NormalMap : register(t1); // �@���e�N�X�`��

[earlydepthstencil]
float4 main(VS_OUT pin) : SV_TARGET
{
    return float4(1, 0, 0, 1);

    uint width, height, number_of_levels;
    DiffuseMap.GetDimensions(0, width, height, number_of_levels);
    float2 texelSize = float2(1.0f / width, 1.0f / height);

    // ���݂̃s�N�Z���̖@�����擾
    float3 normal = NormalMap.Sample(sampler_states[LINEAR], pin.texcoord).xyz;

    // �אڂ���s�N�Z���̖@�����擾
    float3 normalLeft = NormalMap.Sample(sampler_states[LINEAR], pin.texcoord + float2(-1, 0) * texelSize).xyz;
    float3 normalRight = NormalMap.Sample(sampler_states[LINEAR], pin.texcoord + float2(1, 0) * texelSize).xyz;
    float3 normalUp = NormalMap.Sample(sampler_states[LINEAR], pin.texcoord + float2(0, -1) * texelSize).xyz;
    float3 normalDown = NormalMap.Sample(sampler_states[LINEAR], pin.texcoord + float2(0, 1) * texelSize).xyz;

    // �@���̍������v�Z
    float diffLeft = length(normal - normalLeft);
    float diffRight = length(normal - normalRight);
    float diffUp = length(normal - normalUp);
    float diffDown = length(normal - normalDown);

    // �@���̍�������Sobel�t�B���^���v�Z
    float sobelH = diffLeft - diffRight;
    float sobelV = diffUp - diffDown;
    float sobel = sqrt(sobelH * sobelH + sobelV * sobelV);

    // �K���I�Ȃ������l��ݒ�
    float threshold = lerp(0.01, 0.001, saturate(length(pin.texcoord.xy - 0.5f) * 2.0f)); // �����ɉ������������l�̐ݒ�

    // �J�����Ƃ̋��������ɋ߂��ꍇ�ɂ͗֊s����`�悵�Ȃ�
    bool drawOutline = length(pin.texcoord.xy - 0.5f) > 0.1f;

    // Sobel���Z�Ɋ�Â��G�b�W���o
    float edge = (drawOutline && sobel > threshold) ? 1.0f : 0.0f;

    // �A�E�g���C�������o���ꂽ�ꍇ�ɂ̂ݐF��t����
    if (edge > 0.0f)
    {
        return float4(1.0, 0.0, 0.0, 1.0f); // �A�E�g���C�������͐ԐF
    }
    else
    {
        return float4(0.0, 0.0, 0.0, 0.0f); // �A�E�g���C���ȊO�͓���
    }
}
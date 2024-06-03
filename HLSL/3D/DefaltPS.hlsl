#include "Defalt.hlsli"
#include "../Common.hlsli"

Texture2D DiffuseMap : register(t0); //�J���[�e�N�X�`��

float4 main(VS_OUT pin) : SV_TARGET
{
    //�F�e�N�X�`�����T���v�����O
    float4 diffusecolor = DiffuseMap.Sample(sampler_states[ANISOTROPIC], pin.texcoord) * pin.color;
    diffusecolor.rgb = pow(diffusecolor.rgb, GAMMA);

    return diffusecolor;
}
#include "Defalt.hlsli"
#include "../Common.hlsli"

Texture2D DiffuseMap : register(t0); //カラーテクスチャ

float4 main(VS_OUT pin) : SV_TARGET
{
    //色テクスチャをサンプリング
    float4 diffusecolor = DiffuseMap.Sample(sampler_states[ANISOTROPIC], pin.texcoord) * pin.color;
    diffusecolor.rgb = pow(diffusecolor.rgb, GAMMA);

    return diffusecolor;
}
#include "FullScreenQuad.hlsli"
#include "../../Constants.hlsli"

VS_OUT main(in uint vertexid : SV_VERTEXID)
{
    VS_OUT vout;
    const float2 position[4] = { { -1, +1 }, { +1, +1 }, { -1, -1 }, { +1, -1 } };
    const float2 texcoords[4] = { { 0, 0 }, { 1, 0 }, { 0, 1 }, { 1, 1 } };
    vout.position = float4(position[vertexid], 0, 1);
    vout.texcoord.xy = texcoords[vertexid];

    //ÉèÅ[ÉãÉhïœä∑
    float4 p = mul(vout.position, inverseprojection);
    p /= p.w;
    p = mul(p, inverseview);
    vout.worldPosition = p.xyz;

    return vout;
}
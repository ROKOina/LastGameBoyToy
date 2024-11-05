#include "Defalt.hlsli"
#include "../Common/Constants.hlsli"

VS_OUT main(VS_IN vin)
{
    float h = (time * 20) % 120;
    float d = length(vin.position.xyz - bossposiotn);
    const float w = 10.0;
    if (d > h && d < h + w)
    {
        // smoothstep‚ÅŠŠ‚ç‚©‚É’¸“_‚ðŽ‚¿ã‚°‚é
        float a = smoothstep(0.0, 1.0, (d - h) / w) * 3.0;

        vin.position.xyz += a * vin.normal.xyz;
    }

    float3 p = { 0, 0, 0 };
    float3 n = { 0, 0, 0 };
    float3 t = { 0, 0, 0 };

    for (int i = 0; i < 4; i++)
    {
        p += (vin.boneWeights[i] * mul(vin.position, boneTransforms[vin.boneIndices[i]])).xyz;
        n += (vin.boneWeights[i] * mul(float4(vin.normal.xyz, 0), boneTransforms[vin.boneIndices[i]])).xyz;
        t += (vin.boneWeights[i] * mul(float4(vin.tangent.xyz, 0), boneTransforms[vin.boneIndices[i]])).xyz;
    }

    VS_OUT vout;
    vout.position = mul(float4(p, 1.0f), viewProjection);
    vout.world_position = p;
    vout.normal = normalize(n);
    vout.tangent = normalize(t);
    vout.binormal = normalize(cross(vout.tangent, vout.normal));
    vout.color.rgb = vin.color.rgb * materialcolor.rgb;
    vout.color.a = vin.color.a * materialcolor.a;
    vout.texcoord = vin.texcoord;

    return vout;
}
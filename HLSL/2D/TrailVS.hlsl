#include "Trail.hlsli"
#include  "../Constants.hlsli"

VS_OUT main(VS_IN vin)
{
    VS_OUT vout;
    vout.position = mul(float4(vin.position, 1), viewProjection); // ƒ[ƒ‹ƒhÀ•W‚É•ÏŠ·
    vout.color = vin.color;
    vout.texcoord = vin.texcoord + time * uvscroll;

    return vout;
}
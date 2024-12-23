#include "Sprite.hlsli"
#include  "../../Common/Constants.hlsli"

VS_OUT main(float4 position : POSITION, float4 color : COLOR, float2 texcoord : TEXCOORD)
{
	VS_OUT vout;
    vout.position = position;
    vout.color = color;
    vout.texcoord = texcoord + time * uvscroll;

	return vout;
}
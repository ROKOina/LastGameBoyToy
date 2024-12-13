
#include "Font.hlsli"

VS_OUT main(
	float4 position     : POSITION,
	float4 color        : COLOR,
	float4 mask         : MASK,
	float2 texcoord     : TEXCOORD
	)
{
	VS_OUT vout;
	vout.position = position;
	vout.color = color;
	vout.mask = mask;
	vout.texcoord = texcoord;

	return vout;
}

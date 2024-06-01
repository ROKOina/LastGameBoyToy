#include "Debug.hlsli"
#include "Constants.hlsli"

VS_OUT main(float4 position : POSITION, float4 color : COLOR)
{
	VS_OUT vout;
	vout.position = mul(position, worldViewProjection);
	vout.color = color;

	return vout;
}

#include "FullScreenQuad.hlsli"
#include "../../Common/Common.hlsli"
#include "../../Common/Constants.hlsli"

Texture2D skybox : register(t10);

float4 main(VS_OUT pin) : SV_TARGET
{
	// カメラからワールド座標へのベクトル
    float3 V = normalize(pin.worldPosition.xyz - cameraposition.xyz);

    float2 sample_point;

	// atan2(z,x) = xz平面上の方向をラジアン単位で取得 ( -PI ~ PI )
	// + PI = 正の値のみにする ( 0 ~ 2PI )
	// ÷ 2PI = UV値に変換 ( 0 ~ 1 )
    sample_point.x = (atan2(V.z, V.x) + PI) / (PI * 2.0);

	// asin(y) = yの逆正弦をラジアン単位で取得 ( -PI/2 ~ PI/2 )
	// + PI/2 = 正の値のみにする ( 0 ~ PI )
	// ÷ PI = UV値に変換 ( 0 ~ 1 )
    sample_point.y = ((asin(V.y) + PI * 0.5) / PI);
	// 1 - point.y = UVに合わせる為、yの値を反転
    sample_point.y = 1 - sample_point.y;

    float3 color = skybox.SampleLevel(sampler_states[ANISOTROPIC], sample_point, 0).rgb;

    color.rgb = pow(color, GAMMA);
    color.rgb = pow(color, 1.0 / GAMMA);

    return float4(color, 1);
}
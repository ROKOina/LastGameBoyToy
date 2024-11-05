#include "FullScreenQuad.hlsli"
#include "../../Common/Common.hlsli"
#include "../../Common/Constants.hlsli"

Texture2D skybox : register(t10);

float4 main(VS_OUT pin) : SV_TARGET
{
	// �J�������烏�[���h���W�ւ̃x�N�g��
    float3 V = normalize(pin.worldPosition.xyz - cameraposition.xyz);

    float2 sample_point;

	// atan2(z,x) = xz���ʏ�̕��������W�A���P�ʂŎ擾 ( -PI ~ PI )
	// + PI = ���̒l�݂̂ɂ��� ( 0 ~ 2PI )
	// �� 2PI = UV�l�ɕϊ� ( 0 ~ 1 )
    sample_point.x = (atan2(V.z, V.x) + PI) / (PI * 2.0);

	// asin(y) = y�̋t���������W�A���P�ʂŎ擾 ( -PI/2 ~ PI/2 )
	// + PI/2 = ���̒l�݂̂ɂ��� ( 0 ~ PI )
	// �� PI = UV�l�ɕϊ� ( 0 ~ 1 )
    sample_point.y = ((asin(V.y) + PI * 0.5) / PI);
	// 1 - point.y = UV�ɍ��킹��ׁAy�̒l�𔽓]
    sample_point.y = 1 - sample_point.y;

    float3 color = skybox.SampleLevel(sampler_states[ANISOTROPIC], sample_point, 0).rgb;

    color.rgb = pow(color, GAMMA);
    color.rgb = pow(color, 1.0 / GAMMA);

    return float4(color, 1);
}
struct VS_OUT
{
    float4 position : SV_POSITION;
    float4 color : COLOR;
    float2 texcoord : TEXCOORD;
};

cbuffer ColorGradingParamater : register(b5)
{
    float brightness; //�P�x
    float contrast; //�R���g���X�g
    float saturation; //�ʓx
    float dummy;
    float4 filter; //�t�B���^�[
};

//--------------------------------------------
//	sRGB�F��Ԃ�����`�F��Ԃւ̕ϊ�
//--------------------------------------------
// srgb		: sRGB�F��ԐF
// �Ԃ��l	: ���`�F��ԐF
inline float3 sRGBToLinear(float3 colorsRGB)
{
    return pow(colorsRGB.xyz, 2.2f);
}

Texture2D mainTexture : register(t0);
SamplerState samplerLiner : register(s0);

float4 main(VS_OUT pin) : SV_TARGET
{
    float4 tex = mainTexture.Sample(samplerLiner, pin.texcoord) * pin.color;
    float4 color = tex;

    // �P�x����
    color.rgb += brightness;

    // �R���g���X�g
    color.rgb = (color.rgb - 0.5) * contrast + 0.5;

    // �ʓx
    float gray = dot(color.rgb, float3(0.299f, 0.587f, 0.114f));
    color.rgb = (color.rgb - gray) * saturation + gray;
    
	// �J���[�t�B���^�[
    color.rgb *= float3(filter.x, filter.y, filter.z);
    
    return color;
    //return float4(sRGBToLinear(color.rgb), color.a);
}
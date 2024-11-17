#include "sprite.hlsli"
#include "../Common.hlsli"
#include "../Constants.hlsli"

Texture2D texturemaps : register(t0);

// �ڂ����p�֐�
float4 blur(float2 uv, float w, float kx, float ky)
{
    float2 shiftUv = float2(uv.x + kx * blurdistance, uv.y + ky * blurdistance);
    float4 tex = texturemaps.Sample(sampler_states[TRANSPARENT_BORDER_LINEAR], shiftUv);

    tex.a = tex.a * w * blurpower * luminance;
    return tex;
}

float4 main(VS_OUT pin) : SV_TARGET
{
    // ������
    float4 col = 0;

    //x��
    col += blur(pin.texcoord, 0.01, 0, -0.0075);
    col += blur(pin.texcoord, 0.02, 0, -0.006);
    col += blur(pin.texcoord, 0.03, 0, -0.0045);
    col += blur(pin.texcoord, 0.04, 0, -0.003);
    col += blur(pin.texcoord, 0.05, 0, -0.002);
    col += blur(pin.texcoord, 0.06, 0, -0.001);
    col += blur(pin.texcoord, 0.06, 0, +0.001);
    col += blur(pin.texcoord, 0.05, 0, +0.002);
    col += blur(pin.texcoord, 0.04, 0, +0.003);
    col += blur(pin.texcoord, 0.03, 0, +0.0045);
    col += blur(pin.texcoord, 0.02, 0, +0.006);
    col += blur(pin.texcoord, 0.01, 0, +0.0075);

    //y��
    col += blur(pin.texcoord, 0.01, -0.0075, 0);
    col += blur(pin.texcoord, 0.02, -0.006, 0);
    col += blur(pin.texcoord, 0.03, -0.0045, 0);
    col += blur(pin.texcoord, 0.04, -0.003, 0);
    col += blur(pin.texcoord, 0.05, -0.002, 0);
    col += blur(pin.texcoord, 0.06, -0.001, 0);
    col += blur(pin.texcoord, 0.06, +0.001, 0);
    col += blur(pin.texcoord, 0.05, +0.002, 0);
    col += blur(pin.texcoord, 0.04, +0.003, 0);
    col += blur(pin.texcoord, 0.03, +0.0045, 0);
    col += blur(pin.texcoord, 0.02, +0.006, 0);
    col += blur(pin.texcoord, 0.01, +0.0075, 0);

    //y����傫�߂�-�ɃY��������ŁAx����傫�߂�-����+�ɕ`��
    col += blur(pin.texcoord, 0.01, -0.006, -0.006);
    col += blur(pin.texcoord, 0.02, -0.0045, -0.0045);
    col += blur(pin.texcoord, 0.03, -0.003, -0.003);
    col += blur(pin.texcoord, 0.04, -0.002, -0.002);
    col += blur(pin.texcoord, 0.05, -0.001, -0.001);
    col += blur(pin.texcoord, 0.05, +0.001, -0.001);
    col += blur(pin.texcoord, 0.04, +0.002, -0.002);
    col += blur(pin.texcoord, 0.03, +0.003, -0.003);
    col += blur(pin.texcoord, 0.02, +0.0045, -0.0045);
    col += blur(pin.texcoord, 0.01, +0.006, -0.006);

    //y����傫�߂�+�ɃY��������ŁAx����傫�߂�-����+�ɕ`��
    col += blur(pin.texcoord, 0.01, -0.006, 0.006);
    col += blur(pin.texcoord, 0.02, -0.0045, 0.0045);
    col += blur(pin.texcoord, 0.03, -0.003, 0.003);
    col += blur(pin.texcoord, 0.04, -0.002, 0.002);
    col += blur(pin.texcoord, 0.05, -0.001, 0.001);
    col += blur(pin.texcoord, 0.05, +0.001, 0.001);
    col += blur(pin.texcoord, 0.04, +0.002, 0.002);
    col += blur(pin.texcoord, 0.03, +0.003, 0.003);
    col += blur(pin.texcoord, 0.02, +0.0045, 0.0045);
    col += blur(pin.texcoord, 0.01, +0.006, 0.006);

    //y���������߂�-�ɃY��������ŁAx����傫�߂�-����+�ɕ`��
    col += blur(pin.texcoord, 0.01, -0.006, -0.003);
    col += blur(pin.texcoord, 0.02, -0.0045, -0.0022);
    col += blur(pin.texcoord, 0.03, -0.003, -0.0015);
    col += blur(pin.texcoord, 0.04, -0.002, -0.001);
    col += blur(pin.texcoord, 0.05, -0.001, -0.0005);
    col += blur(pin.texcoord, 0.05, +0.001, -0.0005);
    col += blur(pin.texcoord, 0.04, +0.002, -0.001);
    col += blur(pin.texcoord, 0.03, +0.003, -0.0015);
    col += blur(pin.texcoord, 0.02, +0.0045, -0.0022);
    col += blur(pin.texcoord, 0.01, +0.006, -0.003);

    //y���������߂�+�ɃY��������ŁAx����傫�߂�-����+�ɕ`��
    col += blur(pin.texcoord, 0.01, -0.006, 0.003);
    col += blur(pin.texcoord, 0.02, -0.0045, 0.0022);
    col += blur(pin.texcoord, 0.03, -0.003, 0.0015);
    col += blur(pin.texcoord, 0.04, -0.002, 0.001);
    col += blur(pin.texcoord, 0.05, -0.001, 0.0005);
    col += blur(pin.texcoord, 0.05, +0.001, 0.0005);
    col += blur(pin.texcoord, 0.04, +0.002, 0.001);
    col += blur(pin.texcoord, 0.03, +0.003, 0.0015);
    col += blur(pin.texcoord, 0.02, -0.0045, 0.0022);
    col += blur(pin.texcoord, 0.01, -0.006, 0.003);

    //y����傫�߂�-�ɃY��������ŁAx���������߂�-����+�ɕ`��
    col += blur(pin.texcoord, 0.01, -0.003, -0.006);
    col += blur(pin.texcoord, 0.02, -0.0022, -0.0045);
    col += blur(pin.texcoord, 0.03, -0.0015, -0.003);
    col += blur(pin.texcoord, 0.04, -0.001, -0.002);
    col += blur(pin.texcoord, 0.05, -0.0005, -0.001);
    col += blur(pin.texcoord, 0.05, +0.0005, -0.001);
    col += blur(pin.texcoord, 0.04, +0.001, -0.002);
    col += blur(pin.texcoord, 0.03, +0.0015, -0.003);
    col += blur(pin.texcoord, 0.02, +0.0022, -0.0045);
    col += blur(pin.texcoord, 0.01, +0.003, -0.006);

    //y����傫�߂�+�ɃY��������ŁAx���������߂�-����+�ɕ`��
    col += blur(pin.texcoord, 0.01, -0.003, 0.006);
    col += blur(pin.texcoord, 0.02, -0.0022, 0.0045);
    col += blur(pin.texcoord, 0.03, -0.0015, 0.003);
    col += blur(pin.texcoord, 0.04, -0.001, 0.002);
    col += blur(pin.texcoord, 0.05, -0.0005, 0.001);
    col += blur(pin.texcoord, 0.05, +0.0005, 0.001);
    col += blur(pin.texcoord, 0.04, +0.001, 0.002);
    col += blur(pin.texcoord, 0.03, +0.0015, 0.003);
    col += blur(pin.texcoord, 0.02, +0.0022, 0.0045);
    col += blur(pin.texcoord, 0.01, +0.003, 0.006);

    //�u���[�̐F���|���Z
    col.rgb *= blurcolor;

    float4 finalcolor = texturemaps.Sample(sampler_states[TRANSPARENT_BORDER_LINEAR], pin.texcoord);
    finalcolor.rgb = pow(finalcolor.rgb, GAMMA);

    finalcolor.r = finalcolor.r + (pin.color.r - 0.5f) * col.r * luminance;
    finalcolor.g = finalcolor.g + (pin.color.g - 0.5f) * col.g * luminance;
    finalcolor.b = finalcolor.b + (pin.color.b - 0.5f) * col.b * luminance;
    finalcolor.a *= pin.color.a;

    //�A���t�@���Ⴂ�ꍇ�͔j��
    if (finalcolor.a < EPSILON)
        discard;

    finalcolor.rgb = pow(finalcolor.rgb, 1.0 / GAMMA);

    return finalcolor;
}
#include "FullScreenQuad.hlsli"
#include "../../Common.hlsli"
#include "../../Constants.hlsli"

Texture2D texturemaps : register(t0);

// 2x2回転行列を生成する関数
float2x2 r2(float a)
{
    float sn = sin(a);
    float cs = cos(a);
    return float2x2(cs, -sn, sn, cs);
}

// ハッシュ関数
float4 hash(float4 a)
{
    return frac(abs(sin(a.ywxz * 766.345) + cos(normalize(a) * 4972.92855)) * 2048.97435 + abs(a.wxyz) * 0.2735);
}

// HSVからRGBへの変換関数
float3 hsvToRGB(float3 hsv)
{
    return lerp(clamp(abs(fmod(hsv.x * 6.0 + float3(0.0, 4.0, 2.0), 6.0) - 3.0) - 1.0, 0.0, 1.0), float3(1.0, 1.0, 1.0), hsv.y) * hsv.z;
}

// 三角波関数
float twave(float x)
{
    x = frac(x);
    return x * 2.0 - max(0.0, x * 4.0 - 2.0);
}

float4 main(VS_OUT pin) : SV_TARGET
{
    // フラグメントシェーダ用の座標変換
    float2 uv = (pin.position.xy * 2.0 - float2(1920.0f, 1080.0f)) / 1080.0f;

    float3 s = float3(0, 0, 0);

    // ループで色の計算を実行
    for (float l = 15.0; l < 30.0; l += 0.1)
    {
        float2 c = floor(uv.xy * l + time * float2(1.0, 1.0));
        float4 h = hash(float4(c.x, c.y, c.y, c.x));
        s += hsvToRGB(float3(
            h.x,
            h.z * 0.4,
            max(0.0, twave(l * 0.005 + time * 0.1 * h.y + h.y) * 10.0 - 9.0)
        ));
    }

    float4 finalcolor = texturemaps.Sample(sampler_states[POINT], pin.texcoord);
    finalcolor = float4(pow(s * 0.03, float3(1.6, 1.6, 1.6)), 1.0);

    // 出力の色を計算
    return finalcolor;
}
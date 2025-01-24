#include "../Common.hlsli"
#include "../Constants.hlsli"
#include "Font.hlsli"

Texture2D texture0 : register(t0);

static const float WindowSizeX = 1920.0;
static const float WindowSizeY = 1080.0;
static const float4 red = float4(1., 0.5f, 0.5f, 1);

float3 HueShift(float3 color, float shift)
{
    float angle = shift * 3.14159 * 2.0; // 色相のシフト角度
    float3 k = float3(0.57735, 0.57735, 0.57735); // 標準的なRGB軸の正規化ベクトル
    float3 p = cos(angle) * color + sin(angle) * cross(k, color) + (1.0 - cos(angle)) * dot(k, color) * k;
    return saturate(p);
}

float4 main(VS_OUT pin) : SV_TARGET
{
    // サンプルカラーとアルファ計算
    float4 color = texture0.Sample(sampler_states[LINEAR], pin.texcoord) * pin.mask;
    float alpha = color.x + color.y + color.z + color.w;

    // 文字を作成
    float4 output = pin.color * alpha;

    //ハイライトの早さ
    float speed = -0.7f;

    // 文字にハイライトを追加
    float xOffset = frac(time * speed) * WindowSizeX * 2.0 - WindowSizeX; // -1920 ~ 1920
    float a = abs((pin.position.y / WindowSizeY) - frac((pin.position.x + xOffset) / WindowSizeX));

    output.rgb = lerp(HueShift(red.rgb, time * 0.3) * alpha, output.rgb, step(0.7f, a));

    return output;
}
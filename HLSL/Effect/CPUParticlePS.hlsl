#include "../Common.hlsli"
#include "CPUParticle.hlsli"

Texture2D texturemap : register(t0); // カラーテクスチャ
Texture2D DissolveMap : register(t1); //DissolveMap

float4 main(PS_IN input) : SV_TARGET
{
    // テクスチャカラーを取得
    float4 color = texturemap.Sample(sampler_states[LINEAR], input.Size);
    color.rgb = pow(color.rgb, GAMMA);

    // ディゾルブマップをサンプリング
    float dissolveValue = DissolveMap.Sample(sampler_states[LINEAR], input.Size).r;
    float dalpha = smoothstep(dissolveThreshold, dissolveThreshold, dissolveValue);

    // カラーとアルファの調整
    color.a *= input.Color.a * cpuparticlecolor.a;
    color.a *= dalpha;
    color.rgb *= input.Color.rgb * cpuparticlecolor.rgb;
    color.rgb *= cpuparticleluminance;

    // 早期破棄: アルファが低い場合
    if (color.a < EPSILON)
        discard;

    return color;
}
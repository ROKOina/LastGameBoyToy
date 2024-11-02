#include "GPUParticle.hlsli"
#include "../../Common/Constants.hlsli"
#include "../../Common/Rand.hlsli"

RWStructuredBuffer<MainParticle> particlebuffer : register(u0);

[numthreads(THREAD, 1, 1)]
void main(uint3 dtid : SV_DispatchThreadID)
{
    uint id = dtid.x;
    MainParticle p;

    // ランダム生成
    const float noiseScale = 1.0;
    float f0 = rand(float2((id + time) * noiseScale, rand(float2((id + time) * noiseScale, (id + time) * noiseScale))));
    float f1 = rand(float2(f0 * noiseScale, rand(float2((id + time) * noiseScale, (id + time) * noiseScale))));
    float f2 = rand(float2(f1 * noiseScale, rand(float2((id + time) * noiseScale, (id + time) * noiseScale))));

    // 各フィールドを個別に初期化
    p.position = position;
    p.scale = scale * random(f0);
    p.color = baseColor * random(f2);
    p.rotation = rotation;
    p.velocity = 0;
    p.isalive = isalive;
    p.lifetime = lifeTime;
    p.age = emitTime * (1 - random(f2)); // ageもlifetimeで初期化

    // パーティクルをバッファに書き込む
    particlebuffer[id] = p;
}
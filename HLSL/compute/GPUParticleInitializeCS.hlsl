#include "GPUParticle.hlsli"
#include "../Constants.hlsli"

RWStructuredBuffer<MainParticle> particlebuffer : register(u0);

[numthreads(THREAD, 1, 1)]
void main(uint3 dtid : SV_DispatchThreadID)
{
    uint id = dtid.x;
    MainParticle p;

    // 各フィールドを個別に初期化
    p.position = position;
    p.scale = scale;
    p.color = color;
    p.rotation = rotation;
    p.velocity = velocity;
    p.strechvelocity = 0; // ここはデフォルト値を設定
    p.direction = direction;
    p.isalive = isalive;
    p.isstart = startflag;
    p.lifetime = lifetime;
    p.age = lifetime; // ageもlifetimeで初期化

    // パーティクルをバッファに書き込む
    particlebuffer[id] = p;
}
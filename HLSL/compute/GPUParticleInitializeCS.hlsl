#include "GPUParticle.hlsli"
#include "../Constants.hlsli"

RWStructuredBuffer<MainParticle> particlebuffer : register(u0);

[numthreads(THREAD, 1, 1)]
void main(uint3 dtid : SV_DispatchThreadID)
{
    uint id = dtid.x;
    MainParticle p;

    //ƒ‰ƒ“ƒ_ƒ€¶¬
    const float noiseScale = 1.0;
    float f0 = rand(float2((id + time) * noiseScale, rand(float2((id + time) * noiseScale, (id + time) * noiseScale))));
    float f1 = rand(float2(f0 * noiseScale, rand(float2((id + time) * noiseScale, (id + time) * noiseScale))));
    float f2 = rand(float2(f1 * noiseScale, rand(float2((id + time) * noiseScale, (id + time) * noiseScale))));

    //‘S‚Ä‚Ìî•ñ‚ğ‰Šú‰»
    if (loop == 1)
    {
        p.position = position;
        p.scale = scale;
        p.color = color;
        p.rotation = rotation;
        p.velocity = velocity * f2;
        p.strechvelocity = 0;
        p.direction = direction;
        p.isalive = isalive;
        p.isstart = startflag;
        p.lifetime = lifetime;
        p.age = lifetime * f2;
    }
    else
    {
        p.position = position;
        p.scale = scale * f2;
        p.color = color;
        p.rotation = rotation;
        p.velocity = velocity * f2;
        p.strechvelocity = 0;
        p.direction = direction;
        p.isalive = isalive;
        p.isstart = startflag;
        p.lifetime = lifetime;
        p.age = lifetime * f2;
    }
    
    particlebuffer[id] = p;
}
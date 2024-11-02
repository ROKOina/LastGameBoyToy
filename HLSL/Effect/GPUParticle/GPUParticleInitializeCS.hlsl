#include "GPUParticle.hlsli"
#include "../../Common/Constants.hlsli"
#include "../../Common/Rand.hlsli"

RWStructuredBuffer<MainParticle> particlebuffer : register(u0);

[numthreads(THREAD, 1, 1)]
void main(uint3 dtid : SV_DispatchThreadID)
{
    uint id = dtid.x;
    MainParticle p;

    // �����_������
    const float noiseScale = 1.0;
    float f0 = rand(float2((id + time) * noiseScale, rand(float2((id + time) * noiseScale, (id + time) * noiseScale))));
    float f1 = rand(float2(f0 * noiseScale, rand(float2((id + time) * noiseScale, (id + time) * noiseScale))));
    float f2 = rand(float2(f1 * noiseScale, rand(float2((id + time) * noiseScale, (id + time) * noiseScale))));

    // �e�t�B�[���h���ʂɏ�����
    p.position = position;
    p.scale = scale * random(f0);
    p.color = baseColor * random(f2);
    p.rotation = rotation;
    p.velocity = 0;
    p.isalive = isalive;
    p.lifetime = lifeTime;
    p.age = emitTime * (1 - random(f2)); // age��lifetime�ŏ�����

    // �p�[�e�B�N�����o�b�t�@�ɏ�������
    particlebuffer[id] = p;
}
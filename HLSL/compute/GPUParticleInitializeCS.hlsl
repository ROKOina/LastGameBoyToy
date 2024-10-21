#include "GPUParticle.hlsli"
#include "../Constants.hlsli"

RWStructuredBuffer<MainParticle> particlebuffer : register(u0);

[numthreads(THREAD, 1, 1)]
void main(uint3 dtid : SV_DispatchThreadID)
{
    uint id = dtid.x;
    MainParticle p;

    // �e�t�B�[���h���ʂɏ�����
    p.position = position;
    p.scale = scale;
    p.color = baseColor;
    p.rotation = rotation;
    p.strechvelocity = 0; // �����̓f�t�H���g�l��ݒ�
    p.isalive = isalive;
    p.lifetime = lifeTime;
    p.age = lifeTime; // age��lifetime�ŏ�����

    // �p�[�e�B�N�����o�b�t�@�ɏ�������
    particlebuffer[id] = p;
}
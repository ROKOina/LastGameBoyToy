#include "GPUParticle.hlsli"
#include "../Common.hlsli"
#include "../Constants.hlsli"

RWStructuredBuffer<MainParticle> particlebuffer : register(u0);

//�p�[�e�B�N���̍X�V
[numthreads(THREAD, 1, 1)]
void main(uint3 dtid : SV_DISPATCHTHREADID)
{
    uint id = dtid.x;
    MainParticle p = particlebuffer[id];

    //�t���O����
    p.isstart = startflag;
    p.isalive = isalive;

    //�����t���O
    if (p.isstart == 1)
    {
        //false�̏ꍇ�����I�ɐ�������
        if (p.isalive == 0)
        {
            p.isalive = 1;
        }

        //�������Ԃ��X�V
        p.lifetime -= deltatime;

        //�������Ԃ��犄�����v�Z
        float lerprate = 1 - (p.lifetime / lifetime);

        //���͂���
        p.velocity = velocity;

        //�p�[�e�B�N�������̃m�[�}���x�N�g��
        float3 normVec = normalize(position - p.position);

        //���ŉ�]
        float3 orbZ = cross(normVec, float3(0, 1, 0));
        float3 orbX = cross(normVec, float3(1, 0, 0));
        float3 orbY = cross(normVec, float3(0, 0, 1));
        float3 orbVelo = orbZ * orbitalvelocity.z + orbX * orbitalvelocity.x + orbY * orbitalvelocity.y;

        //���S�����ɓ���
        float3 radialVec = -normVec * radial;

        //����
        p.velocity += buoyancy * deltatime;

        //���͍X�V
        p.velocity.x += ((random(p.position.x * time + id) * 2) - 1) * velorandscale;
        p.velocity.y += ((random(p.position.y * time + id) * 2) - 1) * velorandscale;
        p.velocity.z += ((random(p.position.z * time + id) * 2) - 1) * velorandscale;
        //p.velocity = normalize(p.velocity);

        //�d�͓K��
        p.position.yz -= lerp(startgravity, endgravity, lerprate) * deltatime;
 
        //���͍X�V
        p.position += (p.velocity + orbVelo + radialVec) * speed * lerp(startspeed, endspeed, lerprate) * deltatime;
        p.strechvelocity = (p.velocity + orbVelo + radialVec) * speed * lerp(startspeed, endspeed, lerprate) * deltatime;
    }

    //�p�[�e�B�N���̓����X�V����
    p.age += deltatime;
    if (p.age > lifetime && p.isstart == 1)
    {
         //�������Ԃ��犄�����v�Z
        float lerprate = 1 - (p.lifetime / lifetime);

        //�����_������
        const float noiseScale = 1.0;
        float f0 = rand(float2((id + time) * noiseScale, rand(float2((id + time) * noiseScale, (id + time) * noiseScale))));
        float f1 = rand(float2(f0 * noiseScale, rand(float2((id + time) * noiseScale, (id + time) * noiseScale))));
        float f2 = rand(float2(f1 * noiseScale, rand(float2((id + time) * noiseScale, (id + time) * noiseScale))));

        //�����_������
        float theta = lerp(-PI, PI, random(id + time));
        float pp = lerp(0.0, 1.0, random(theta + id * time));
        float phi = asin((2.0f * pp) - 1.0f);

        //�ʒu,�傫��
        p.position = position.xyz;

        //�傫�������[�v�Ő���
        p.scale = scale * lerp(startsize, endsize, lerprate);

        //�F�����[�v�Ő���
        p.color = color * lerp(startcolor, endcolor, lerprate);

        //���a�̃{�����[��
        float tick = shape.z * 0.9f; //�I�t�Z�b�g�l�@0.9f

        //Cone
        p.position.x += cos(phi) * cos(theta) * random(theta * phi + id) * shape.x;
        p.position.y += sin(phi) * random(phi * id + pp) * shape.x;
        p.position.z += cos(phi) * sin(theta) * random(pp * time + id) * shape.x;

        //sphereshape
        float r = sin(3.141592654 * (rand(float2(f1, time))) * shape.w) * (rand(float2(f2, time)) * tick + (1 - tick)) * shape.y;
        p.position.x += r * sin(3.141592654 * 2 * (rand(float2(f0, time))));
        p.position.y += cos(3.141592654 * (rand(float2(f1, time))) * shape.w) * (rand(float2(f2, time)) * tick + (1 - tick)) * shape.y;
        p.position.z += r * cos(3.141592654 * 2 * (rand(float2(f0, time))));

        //�Z�b�gCPU�Ő���ł���悤�ɂ���
        p.isalive = 0;
        p.isstart = 1;
        p.lifetime = lifetime;
        p.age = lifetime * f0;
    }

    //���[�v���I��炷
    if (loop == 0)
    {
        p.age = 0;
    }

    //�������Ԃ��؂ꂽ�琶�����Ȃ�
    if (p.lifetime < 0.0f)
    {
        // �����t���O���I�t
        p.isalive = 0;

         //���[�v�I��
        if (loop == 0)
        {
            p.scale = 0;
            p.age = 0;
        }
    }

    particlebuffer[id] = p;
}
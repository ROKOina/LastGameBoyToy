#include "GPUParticle.hlsli"
#include "../Common.hlsli"
#include "../Constants.hlsli"

RWStructuredBuffer<MainParticle> particlebuffer : register(u0);

//パーティクルの更新
[numthreads(THREAD, 1, 1)]
void main(uint3 dtid : SV_DISPATCHTHREADID)
{
    uint id = dtid.x;
    MainParticle p = particlebuffer[id];

    //フラグを代入
    p.isstart = startflag;
    p.isalive = isalive;

    //生存フラグ
    if (p.isstart == 1)
    {
        //falseの場合強制的に生成する
        if (p.isalive == 0)
        {
            p.isalive = 1;
        }

        //寿命時間を更新
        p.lifetime -= deltatime;

        //寿命時間から割合を計算
        float lerprate = 1 - (p.lifetime / lifetime);

        //速力を代入
        p.velocity = velocity;

        //パーティクル方向のノーマルベクトル
        float3 normVec = normalize(position - p.position);

        //軸で回転
        float3 orbZ = cross(normVec, float3(0, 1, 0));
        float3 orbX = cross(normVec, float3(1, 0, 0));
        float3 orbY = cross(normVec, float3(0, 0, 1));
        float3 orbVelo = orbZ * orbitalvelocity.z + orbX * orbitalvelocity.x + orbY * orbitalvelocity.y;

        //中心方向に動く
        float3 radialVec = -normVec * radial;

        //浮力
        p.velocity += buoyancy * deltatime;

        //速力更新
        p.velocity.x += ((random(p.position.x * time + id) * 2) - 1) * velorandscale;
        p.velocity.y += ((random(p.position.y * time + id) * 2) - 1) * velorandscale;
        p.velocity.z += ((random(p.position.z * time + id) * 2) - 1) * velorandscale;
        //p.velocity = normalize(p.velocity);

        //重力適量
        p.position.yz -= lerp(startgravity, endgravity, lerprate) * deltatime;
 
        //速力更新
        p.position += (p.velocity + orbVelo + radialVec) * speed * lerp(startspeed, endspeed, lerprate) * deltatime;
        p.strechvelocity = (p.velocity + orbVelo + radialVec) * speed * lerp(startspeed, endspeed, lerprate) * deltatime;
    }

    //パーティクルの動き更新処理
    p.age += deltatime;
    if (p.age > lifetime && p.isstart == 1)
    {
         //寿命時間から割合を計算
        float lerprate = 1 - (p.lifetime / lifetime);

        //ランダム生成
        const float noiseScale = 1.0;
        float f0 = rand(float2((id + time) * noiseScale, rand(float2((id + time) * noiseScale, (id + time) * noiseScale))));
        float f1 = rand(float2(f0 * noiseScale, rand(float2((id + time) * noiseScale, (id + time) * noiseScale))));
        float f2 = rand(float2(f1 * noiseScale, rand(float2((id + time) * noiseScale, (id + time) * noiseScale))));

        //ランダム生成
        float theta = lerp(-PI, PI, random(id + time));
        float pp = lerp(0.0, 1.0, random(theta + id * time));
        float phi = asin((2.0f * pp) - 1.0f);

        //位置,大きさ
        p.position = position.xyz;

        //大きさをラープで制御
        p.scale = scale * lerp(startsize, endsize, lerprate);

        //色をラープで制御
        p.color = color * lerp(startcolor, endcolor, lerprate);

        //半径のボリューム
        float tick = shape.z * 0.9f; //オフセット値　0.9f

        //Cone
        p.position.x += cos(phi) * cos(theta) * random(theta * phi + id) * shape.x;
        p.position.y += sin(phi) * random(phi * id + pp) * shape.x;
        p.position.z += cos(phi) * sin(theta) * random(pp * time + id) * shape.x;

        //sphereshape
        float r = sin(3.141592654 * (rand(float2(f1, time))) * shape.w) * (rand(float2(f2, time)) * tick + (1 - tick)) * shape.y;
        p.position.x += r * sin(3.141592654 * 2 * (rand(float2(f0, time))));
        p.position.y += cos(3.141592654 * (rand(float2(f1, time))) * shape.w) * (rand(float2(f2, time)) * tick + (1 - tick)) * shape.y;
        p.position.z += r * cos(3.141592654 * 2 * (rand(float2(f0, time))));

        //セットCPUで制御できるようにする
        p.isalive = 0;
        p.isstart = 1;
        p.lifetime = lifetime;
        p.age = lifetime * f0;
    }

    //ループを終わらす
    if (loop == 0)
    {
        p.age = 0;
    }

    //寿命時間が切れたら生成しない
    if (p.lifetime < 0.0f)
    {
        // 生存フラグをオフ
        p.isalive = 0;

         //ループ終了
        if (loop == 0)
        {
            p.scale = 0;
            p.age = 0;
        }
    }

    particlebuffer[id] = p;
}
#define THREAD 1024

struct VS_OUT
{
    uint vertex_id : VERTEXID;
};

struct GS_OUT
{
    float4 position : SV_POSITION;
    float4 color : COLOR;
    float2 texcoord : TEXCOORD;
};

struct MainParticle
{
    float3 position;
    float2 scale;
    float4 rotation;
    float3 velocity;
    float3 direction;
    float4 color;
    float lifetime;
    float age;
    int isalive;
    int isstart;
};

//保存しないものを入れておく
cbuffer GPUParticleConstants : register(b6)
{
    float4 rotation;
    float3 position;
    int isalive;
    float3 direction;
    int loop;
    int startflag;
    float3 Gdummy;
}

//保存するものを入れておく
cbuffer GPUparticleSaveConstants : register(b7)
{
    float4 color;
    float4 startcolor;
    float4 endcolor;
    float4 shape;
    float3 velocity;
    float lifetime;
    float3 luminance;
    float speed;
    float2 scale;
    float startsize;
    float endsize;
    float3 orbitalvelocity;
    float radial;
    float startspeed;
    float endspeed;
    float velorandscale;
    int strechflag;
    float3 buoyancy;
    float startgravity;
    float endgravity;
    float3 savedummy;
}

//ランダム関数
float rand(float2 co) //引数はシード値と呼ばれる　同じ値を渡せば同じものを返す
{
    return frac(sin(dot(co.xy, float2(12.9898, 78.233))) * 43758.5453);
}

// 1次元のランダムな値を算出する
float random(float n)
{
    return frac(sin(n) * 43758.5453123);
}

uint XOrShift32(uint value)
{
    value = value ^ (value << 13);
    value = value ^ (value >> 17);
    value = value ^ (value << 5);
    return value;
}

float random(float2 seed)
{
    uint h = XOrShift32(asuint(seed.x));
    h = XOrShift32(h ^ asuint(seed.y));
    return asfloat((h & 0x007FFFFF) | 0x40000000) - 3.0;
}
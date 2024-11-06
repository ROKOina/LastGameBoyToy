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
    float4 color;
    float lifetime;
    float age;
    int isalive;
};

//保存しないものを入れておく
cbuffer GPUParticleConstants : register(b6)
{
    float4 rotation;
    float3 position;
    int isalive;
    float3 currentEmitVec;
    int isEmitFlg;
    row_major float4x4 world;
}

// 保存するものを入れておく
cbuffer GPUparticleSaveConstants : register(b7)
{
    float emitTime;
    float lifeTime;
    int stretchFlag;
    int isLoopFlg;

    //ｘ：生成場所ランダム、ｙ：半径、ｚ：半径のボリューム、ｗ：円形にするパラ
    float4 shape;

    float4 baseColor; // ベースとなる色

    float4 lifeStartColor; // パーティクルの生成時の色
    float4 lifeEndColor; // パーティクルの消滅時の色
    // -------------------  ↑↓ どちらかのみ  ----------------------
    float4 emitStartColor; // エフェクトの再生開始時の色
    float4 emitEndColor; // エフェクト再生後の最終的な色

    int colorVariateByLife; // 色の変化の基準を管理 ( TRUE : 寿命によって変化 )
    float3 colorScale; // 色を更に明るくするなどで使用

    float3 emitVec;
    float spiralSpeed;

    float3 orbitalVelocity;
    float spiralstrong;

    float veloRandScale;
    float speed;
    float emitStartSpeed; //  エフェクトの再生開始時の速度
    float emitEndSpeed; //  エフェクト再生後の最終的な速度

    float2 scale;
    int scaleVariateByLife; // 大きさの変化の基準を管理 ( TRUE : 寿命によって変化 )
    int worldpos; //パーティクルを全部引っ付ける

    float lifeStartSize; // パーティクルの生成時の速度
    float lifeEndSize; // パーティクルの消滅時の速度
    // -------------------  ↑↓ どちらかのみ  ----------------------
    float emitStartSize; // エフェクトの再生開始時の速度
    float emitEndSize; // エフェクト再生後の最終的な速度

    float radial;
    float buoyancy;
    float emitStartGravity;
    float emitEndGravity;

    float strechscale; //ストレッチビルボードの強度(伸びる時の大きさ)
    float3 padding;
}
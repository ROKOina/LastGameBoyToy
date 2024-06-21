#include "GPUparticle.hlsli"
#include "../Constants.hlsli"

StructuredBuffer<MainParticle> particlebuffer : register(t0);

// クォータニオン回転関数
float3 QuaternionRotate(float3 position, float4 q)
{
    float3 u = q.xyz;
    float s = q.w;

    float3 crossUPos = cross(u, position);
    float dotUPos = dot(u, position);
    float dotUU = dot(u, u);

    return 2.0f * dotUPos * u + (s * s - dotUU) * position + 2.0f * s * crossUPos;
}

[maxvertexcount(4)]
void main(point VS_OUT input[1], inout TriangleStream<GS_OUT> output)
{
    //ビルボードとテクスコード
    float3 BILLBOARD[] =
    {
        float3(-0.5f * scale.x, +0.5f * scale.y, 0.0f), // 左上
	    float3(+0.5f * scale.x, +0.5f * scale.y, 0.0f), // 右上
        float3(-0.5f * scale.x, -0.5f * scale.y, 0.0f), // 左下
	    float3(+0.5f * scale.x, -0.5f * scale.y, 0.0f), // 右下
    };
    const float2 TEXCOORD[] =
    {
        float2(0.0f, 0.0f), // 左上
	    float2(1.0f, 0.0f), // 右上
        float2(0.0f, 1.0f), // 左下
	    float2(1.0f, 1.0f), // 右下
    };

    //実体を作る
    MainParticle p = particlebuffer[input[0].vertex_id];

    //座標変換(速力と位置)
    float4 viewpos = mul(float4(p.position, 1.0), view);
    float4 viewvelo = mul(float4(p.strechvelocity, 0.0), view);

    //回転
    p.rotation = rotation;

    //生存フラグがfalseなら生成しない
    p.isalive = isalive;
    if (p.isalive == 0)
    {
        return;
    }

    GS_OUT element;
    //頂点追加
    [unroll]
    for (int i = 0; i < 4; ++i)
    {
        float3 cornerPos = 0;
        //ストレッチビルボードを使用するか否かのフラグ
        if (strechflag == 0)
        {
            cornerPos = BILLBOARD[i] * float3(p.scale, 1.0f);
        }
        else
        {
            cornerPos = BILLBOARD[i] * viewvelo.xyz * float3(p.scale, 1.0f);
        }
        cornerPos = QuaternionRotate(cornerPos, p.rotation);
        element.position = mul(float4(viewpos.xyz + cornerPos, 1.0f), projection);
        element.color = p.color * color;
        element.color.rgb *= luminance;
        element.texcoord = TEXCOORD[i];
        output.Append(element);
    }

    output.RestartStrip();
}
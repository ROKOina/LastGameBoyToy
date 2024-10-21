#include "GPUparticle.hlsli"
#include "../Constants.hlsli"
#include "..\\3D\\Light.hlsli"

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

    //簡易的なライティング計算
    float3 n = float3(1, 1, 1); // パーティクルの面法線
    float3 N = normalize(n);
    float3 L = normalize(-directionalLight.direction.xyz);
    float d = dot(L, N);
    float power = max(0, d) * 0.5f + 0.5f;

    //実体を作る
    MainParticle p = particlebuffer[input[0].vertex_id];

    //座標変換(速力と位置)
    float4 viewpos = mul(float4(p.position.xyz, 1.0), view);
    float4 viewvelo = float4(p.velocity.xyz, 0.0);

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
        if (strechflag == 1 && length(p.velocity.xyz) > 0.001f)
        {
           // パーティクルの速度方向に基づいて伸ばす
            float3 stretchDirection = normalize(viewvelo.xyz);
            float stretchAmount = length(viewvelo.xyz) * 0.5f; // ストレッチの強度は速度に基づく

            // 伸ばす軸に沿ってビルボードの一部を伸ばす
            float3 stretchedBillboard = BILLBOARD[i];
            stretchedBillboard.x += stretchDirection.x * stretchAmount * (BILLBOARD[i].x > 0 ? 1 : -1);
            stretchedBillboard.y += stretchDirection.y * stretchAmount * (BILLBOARD[i].y > 0 ? 1 : -1);
            stretchedBillboard.z += stretchDirection.z * stretchAmount * (BILLBOARD[i].z > 0 ? 1 : -1);

            cornerPos = stretchedBillboard * float3(p.scale, 1.0f);
        }
        else
        {
            cornerPos = BILLBOARD[i] * float3(p.scale, 1.0f);
        }
        cornerPos = QuaternionRotate(cornerPos, p.rotation);
        element.position = mul(float4(viewpos.xyz + cornerPos, 1.0f), projection);
        element.color.rgb = p.color.rgb * power * baseColor.rgb;
        element.color.a = p.color.a * baseColor.a;
        element.color.rgb *= colorScale;
        element.texcoord = TEXCOORD[i];
        output.Append(element);
    }

    output.RestartStrip();
}
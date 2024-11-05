#include "GPUparticle.hlsli"
#include "../Constants.hlsli"
#include "..\\3D\\Light.hlsli"
#include "../Common.hlsli"

StructuredBuffer<MainParticle> particlebuffer : register(t0);

[maxvertexcount(4)]
void main(point VS_OUT input[1], inout TriangleStream<GS_OUT> output)
{
// ビルボードとテクスチャ座標
    float3 BILLBOARD[] =
    {
        float3(-0.5f, +0.5f, 0.0f), // 左上
        float3(+0.5f, +0.5f, 0.0f), // 右上
        float3(-0.5f, -0.5f, 0.0f), // 左下
        float3(+0.5f, -0.5f, 0.0f), // 右下
    };
    const float2 TEXCOORD[] =
    {
        float2(0.0f, 0.0f), // 左上
        float2(1.0f, 0.0f), // 右上
        float2(0.0f, 1.0f), // 左下
        float2(1.0f, 1.0f), // 右下
    };

    // 簡易的なライティング計算
    float3 N = normalize(float3(0, 0, 1)); // パーティクル面法線 (デフォルトZ方向)
    float3 L = normalize(-directionalLight.direction.xyz);
    float d = dot(L, N);
    float power = max(0, d) * 0.5f + 0.5f;

    //実体を作る
    MainParticle p = particlebuffer[input[0].vertex_id];

    //座標変換(速力と位置)
    float4 viewpos = mul(float4(p.position, 1.0), view);
    float4 worldviewpos = mul(mul(float4(p.position, 1.0f), world), view);
    float4 viewvelo = mul(float4(p.velocity.xyz, 0.0), view);

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
        float3 cornerPos = BILLBOARD[i];

        // ストレッチビルボードを適用
        if (stretchFlag == 1 && length(p.velocity.xyz) > 0.001f)
        {
            float3 stretchDirection = normalize(viewvelo.xyz);
            float stretchAmount = length(viewvelo.xyz) * strechscale;
            cornerPos += stretchDirection * stretchAmount * (BILLBOARD[i].x > 0 ? 1 : -1);
        }

        // 頂点の変換
        if (worldpos == 1)
        {
            float3 scaledCornerPos = cornerPos * float3(p.scale, 1.0f);
            float3 worldPosition = worldviewpos.xyz + scaledCornerPos;
            element.position = mul(float4(worldPosition, 1.0f), projection);
        }
        else
        {
            float3 scaledCornerPos = cornerPos * float3(p.scale, 1.0f);
            float3 worldPosition = viewpos.xyz + scaledCornerPos;
            element.position = mul(float4(worldPosition, 1.0f), projection);
        }
        element.color.rgb = p.color.rgb * power * baseColor.rgb;
        element.color.a = p.color.a * baseColor.a;
        element.color.rgb *= colorScale;
        element.texcoord = TEXCOORD[i];
        output.Append(element);
    }

    output.RestartStrip();
}
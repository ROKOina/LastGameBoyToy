#include "CPUParticle.hlsli"

float4 QuaternionRotate(float3 position, float4 q)
{
    float3 u = q.xyz;
    float s = q.w;
    return float4(
        (2.0f * dot(u, position) * u) +
        ((s * s - dot(u, u)) * position) +
        (2.0f * s * cross(u, position)), 1.0f);
}

[maxvertexcount(4)]
void main(point GS_IN In[1], inout TriangleStream<PS_IN> ParticleStream)
{
    // 座標変換 (ワールド座標系 → ビュー座標系)
    float4 pos = mul(float4(In[0].Position, 1.0), view);

    // クォータニオンの定義
    float4 q = In[0].Rotate;

    // 点を面にする(４頂点を作る)
    float3 rightDir = QuaternionRotate(float3(1.0, 0.0, 0.0), q).xyz;
    float3 upDir = QuaternionRotate(float3(0.0, 1.0, 0.0), q).xyz;

    // 半分サイズを計算（不要な計算を避けるため、事前にスケールを掛けたベクトルを作成）
    float3 halfRight = rightDir * (In[0].Size.x * 0.5);
    float3 halfUp = upDir * (In[0].Size.y * 0.5);

    // 各頂点位置を計算
    float4 pos_left_top = float4(pos.xyz - halfRight + halfUp, 1.0f);
    float4 pos_left_bottom = float4(pos.xyz - halfRight - halfUp, 1.0f);
    float4 pos_right_top = float4(pos.xyz + halfRight + halfUp, 1.0f);
    float4 pos_right_bottom = float4(pos.xyz + halfRight - halfUp, 1.0f);

    // UVマッピング設定
    uint type = (int) In[0].Param.y;
    uint komax = (int) In[0].Param.z;
    int komay = (int) In[0].Param.w;
    float w = 1.0 / komax;
    float h = 1.0 / komay;
    float2 uv_base = float2((type % komax) * w, (type / komax) * h);

    // PS_IN構造体の共通設定
    PS_IN Out = (PS_IN) 0;
    Out.Color = In[0].Color; // 共通色を設定

    // 左上の頂点
    Out.Position = mul(pos_left_top, projection);
    Out.Size = uv_base; // テクスチャ左上
    ParticleStream.Append(Out);

    // 右上の頂点
    Out.Position = mul(pos_right_top, projection);
    Out.Size = uv_base + float2(w, 0); // テクスチャ右上
    ParticleStream.Append(Out);

    // 左下の頂点
    Out.Position = mul(pos_left_bottom, projection);
    Out.Size = uv_base + float2(0, h); // テクスチャ左下
    ParticleStream.Append(Out);

    // 右下の頂点
    Out.Position = mul(pos_right_bottom, projection);
    Out.Size = uv_base + float2(w, h); // テクスチャ右下
    ParticleStream.Append(Out);

    ParticleStream.RestartStrip();
}
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
    float4 q = float4(In[0].Rotate.x, In[0].Rotate.y, In[0].Rotate.z, In[0].Rotate.w);

    // 点を面にする(４頂点を作る)
    float rot = In[0].Param.x;
    float s = sin(rot);
    float c = cos(rot);
    float3 rightDir = QuaternionRotate(float3(1.0, 0.0, 0.0), q).xyz;
    float3 upDir = QuaternionRotate(float3(0.0, 1.0, 0.0), q).xyz;

    float4 right = float4(rightDir * (In[0].Size.x * 0.5), 0);
    float4 up = float4(upDir * (In[0].Size.y * 0.5), 0);

    float4 pos_left_top = pos - right + up;
    float4 pos_left_bottom = pos - right - up;
    float4 pos_right_top = pos + right + up;
    float4 pos_right_bottom = pos + right - up;

    // タイプ設定
    uint type = (int) In[0].Param.y;
    uint komax = (int) In[0].Param.z;
    int komay = (int) In[0].Param.w;
    float w = 1.0 / komax;
    float h = 1.0 / komay;
    float2 uv = float2((type % komax) * w, (type / komax) * h);
    
    // 左上の点の位置(射影座標系)・UV・色を計算して出力
    PS_IN Out = (PS_IN) 0;
    Out.Color = In[0].Color;
    Out.Position = mul(pos_left_top, projection);
    Out.Size = uv + float2(0, 0); // テクスチャ左上
    ParticleStream.Append(Out);
    
    // 右上の点の位置(射影座標系) とテクスチャ座標の計算をして出力
    Out.Color = In[0].Color;
    Out.Position = mul(pos_right_top, projection);
    Out.Size = uv + float2(w, 0); // テクスチャ
    ParticleStream.Append(Out);

    // 左下の点の位置(射影座標系) とテクスチャ座標の計算をして出力
    Out.Color = In[0].Color;
    Out.Position = mul(pos_left_bottom, projection);
    Out.Size = uv + float2(0, h); // テクスチャ
    ParticleStream.Append(Out);

    // 右下の点の位置(射影座標系) とテクスチャ座標の計算をして出力
    Out.Color = In[0].Color;
    Out.Position = mul(pos_right_bottom, projection);
    Out.Size = uv + float2(w, h); // テクスチャ
    ParticleStream.Append(Out);
    
    ParticleStream.RestartStrip();
}
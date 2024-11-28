#include "Defalt.hlsli"
#include "../Constants.hlsli"

VS_OUT main(VS_IN vin)
{
    float3 p = float3(0, 0, 0);
    float3 n = float3(0, 0, 0);
    float3 t = float3(0, 0, 0);

    // スキニング計算 (最大4つのボーンウェイトに対応)
    for (int i = 0; i < 4; ++i)
    {
        p += (vin.boneWeights[i] * mul(vin.position, boneTransforms[vin.boneIndices[i]])).xyz;
        n += (vin.boneWeights[i] * mul(float4(vin.normal.xyz, 0), boneTransforms[vin.boneIndices[i]])).xyz;
        t += (vin.boneWeights[i] * mul(float4(vin.tangent.xyz, 0), boneTransforms[vin.boneIndices[i]])).xyz;
    }

    // 出力データの作成
    VS_OUT vout;
    vout.position = mul(float4(p, 1.0f), viewProjection); // クリップ空間座標へ変換
    vout.world_position = p; // ワールド座標
    vout.normal = normalize(n); // 法線を正規化
    vout.tangent = normalize(t); // 接線を正規化
    vout.binormal = normalize(cross(vout.tangent, vout.normal)); // バイノーマルを計算
    vout.color.rgb = vin.color.rgb * materialcolor.rgb; // 頂点色とマテリアル色を掛け合わせる
    vout.color.a = vin.color.a * materialcolor.a; // アルファ値も適用
    vout.texcoord = vin.texcoord; // テクスチャ座標

    return vout;
}
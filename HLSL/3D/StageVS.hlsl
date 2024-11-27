#include "Defalt.hlsli"
#include "../Constants.hlsli"

VS_OUT main(VS_IN vin)
{
    float3 p = float3(0, 0, 0);
    float3 n = float3(0, 0, 0);
    float3 t = float3(0, 0, 0);

    // スキニング計算 (最大4つのボーンウェイトに対応)
    for (int i = 0; i < 4; i++)
    {
        // ボーンインデックスを取得
        int boneIndex = vin.boneIndices[i];

        // 最終ボーン行列 = offsetTransform * boneTransform
        float4x4 finalBoneMatrix = mul(offsetTransforms[boneIndex], boneTransforms[boneIndex]);

        // 頂点位置のスキニング
        p += vin.boneWeights[i] * mul(vin.position, finalBoneMatrix).xyz;

        // 法線のスキニング（平行移動成分は無視するためw = 0を使用）
        n += vin.boneWeights[i] * mul(float4(vin.normal, 0), finalBoneMatrix).xyz;

        // 接線のスキニング
        t += vin.boneWeights[i] * mul(float4(vin.tangent, 0), finalBoneMatrix).xyz;
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
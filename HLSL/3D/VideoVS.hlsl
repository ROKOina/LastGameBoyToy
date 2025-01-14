#include "Video.hlsli"
#include "../Constants.hlsli"

VS_OUT main(VS_IN vin)
{
    // 出力データの作成
    VS_OUT vout;
    vout.position = mul(vin.position,world);
    vout.position = mul(vout.position, viewProjection);
    vout.color = vin.color; // 頂点色とマテリアル色を掛け合わせる
    vout.texcoord = vin.texcoord; // テクスチャ座標

    return vout;
}
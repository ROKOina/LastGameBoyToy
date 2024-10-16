#include "Shadow.hlsli"
#include "Defalt.hlsli"

VS_OUT_CSM main(INSTANCE_VS_IN vin, uint instance_id : SV_INSTANCEID)
{
    // インスタンスのワールド変換行列を計算
    float4x4 world = vin.transform;

    VS_OUT_CSM vout;

    // 頂点座標にグローバル変換行列を適用
    float3 p = mul(float4(vin.position, 1.0f), world).xyz;

    // ライトのビュープロジェクション行列を適用してクリップ空間座標を計算
    vout.position = mul(float4(p, 1.0f), lightviewprojection[instance_id % 4]);
    vout.slice = instance_id;

    return vout;
}
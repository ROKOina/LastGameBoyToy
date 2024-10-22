#include "FullScreenQuad.hlsli"
#include "PostEffect.hlsli"
#include "../../Common.hlsli"
#include "../../Constants.hlsli"
#include "../../noise.hlsli"
#include "../../3D/Light.hlsli"

Texture2D texturemaps : register(t0);
Texture2D positionmap : register(t1);

float4 main(VS_OUT pin) : SV_TARGET
{
    // 呼び出されたピクセルの位置をUV値に変換
    float2 screenuv = pin.position.xy / float2(1920, 1080);

    // PositionTargetから現在呼び出されたピクセルシェーダーと同じ場所にアクセスして座標値を確認
    float4 vViewPos = positionmap.Sample(sampler_states[POINT], screenuv);

    // Deferred（ディファード）段階で描画されていない場合、光を与えることはできない。
    if (-1.f == vViewPos.w)
    {
        discard;
    }

    // Cubeボリュームメッシュのローカル空間に移動させる。
    float3 vLocal = mul(float4(vViewPos.xyz, 1.f), viewProjection).xyz;
    vLocal += 0.5f;
    if (vLocal.x < 0.f || 1.f < vLocal.x ||
    vLocal.y < 0.f || 1.f < vLocal.y ||
    vLocal.z < 0.f || 1.f < vLocal.z)
    {
        //discard;
    }

    // ボリュームメッシュ内部判定成功時
    float4 color = texturemaps.Sample(sampler_states[POINT], vLocal.xz);

    return color;
}
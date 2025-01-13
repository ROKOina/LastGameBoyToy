#include "FullScreenQuad.hlsli"
#include "../../Common.hlsli"
#include "../../Constants.hlsli"

Texture2D skybox : register(t10);

// スカイボックスのシームレス処理用のヘルパー関数
float2 SeamlessUVWrap(float2 uv)
{
    uv.x = frac(uv.x); // x方向での反復
    uv.y = clamp(uv.y, 0.001, 0.999); // y方向でのクランプ（繋ぎ目を回避）
    return uv;
}

float4 main(VS_OUT pin) : SV_TARGET
{
// カメラからワールド座標へのベクトル
    float3 V = normalize(pin.worldPosition.xyz - cameraposition.xyz);

    float2 sample_point;

    // UV計算（XZ平面とY軸を扱う）
    sample_point.x = (atan2(V.z, V.x) + PI) / (PI * 2.0);
    sample_point.y = ((asin(V.y) + PI * 0.5) / PI);

    // Y軸の反転
    sample_point.y = 1 - sample_point.y;

    // UVのシームレス処理
    sample_point = SeamlessUVWrap(sample_point);

    // テクスチャサンプリング
    float3 color = skybox.SampleLevel(sampler_states[ANISOTROPIC], sample_point, 0).rgb;

    // 色調補正
    color *= lerp(10.0, color, 0.1);
    color.rgb = pow(color, GAMMA);
    color.rgb = pow(color, 1.0 / GAMMA);

    return float4(color, 1);
}
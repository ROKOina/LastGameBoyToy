#include "Defalt.hlsli"
#include "Light.hlsli"
#include "PBR+IBLFunction.hlsli"
#include "../Constants.hlsli"

// pow関数に負の値が入る可能性を報告する警告を無効化
#pragma warning (disable : 3571)

Texture2D DiffuseMap : register(t0);   // カラーテクスチャ
Texture2D NormalMap : register(t1);    // 法線テクスチャ
Texture2D MetallicMap : register(t2);  // メタリックマップ
Texture2D RoughnessMap : register(t3); // ラフネスマップ
Texture2D AOMap : register(t4);        // AOマップ
Texture2D EmissionMap : register(t5);  // エミッションマップ
Texture2D DissolveMap : register(t23);  // ディゾルブマップ

// アウトラインの定数バッファ
cbuffer General : register(b11)
{
    float3 outlineColor;
    float outlineintensity;
    float dissolveThreshold; // ディゾルブ閾値
    float3 dissolveEdgeColor; // エッジの色
    float dissolveEdgeWidth; // エッジ幅
    float3 lastpadding;
}

// MRT対応
struct PS_OUT
{
    float4 diffuse : SV_TARGET0;
    float4 normal : SV_TARGET1;
    float4 position : SV_TARGET2;
    float4 MRO : SV_TARGET3;
    float4 emission : SV_TARGET4;
    float4 outline : SV_TARGET5;
};

/*
デファード描画用のレンダーターゲットに、
色、法線、位置、金属度、粗さ、環境遮蔽、発光の情報を書き込んでいくシェーダー
*/
PS_OUT main(VS_OUT pin)
{
    PS_OUT pout = (PS_OUT) 0;

    // ディゾルブマップをサンプリング
    float dissolveValue = DissolveMap.Sample(sampler_states[LINEAR], pin.texcoord).r;
    float dalpha = smoothstep(dissolveThreshold, dissolveThreshold, dissolveValue);

    // 色テクスチャをサンプリング
    float4 diffuseColor = DiffuseMap.Sample(sampler_states[ANISOTROPIC], pin.texcoord) * pin.color;

    // 完全に透明な場合はピクセルを破棄
    if (dalpha <= 0.0)
    {
        discard; // ピクセルを破棄
    }

    // リニア空間に変換
    pout.diffuse = diffuseColor;
    pout.diffuse.rgb = pow(pout.diffuse.rgb, GAMMA);

     // アルファ値を適用
    pout.diffuse.a = dalpha;

    // 法線マップ
    float3 normal = NormalMap.Sample(sampler_states[ANISOTROPIC], pin.texcoord).xyz;
    normal = normal * 2 - 1;
    // 変換用の3x3行列を用意
    float3x3 CM =
    {
        pin.tangent,
        pin.binormal,
        pin.normal
    };
    // 法線マップで取得した法線情報に変換行列を掛け合わせる
    float3 N = normalize(mul(normal, CM));
    // ワールド空間の法線 (0 ～ 1) の間にスケーリング
    N = (N * 0.5) + 0.5;
    pout.normal = float4(N, 1.0);

    pout.position = float4(pin.world_position, 1.0f);

    // 金属テクスチャをサンプリング
    float metallic = saturate(MetallicMap.Sample(sampler_states[LINEAR], pin.texcoord).r * Metalness);
    // 光沢テクスチャをサンプリング
    float roughness = saturate(RoughnessMap.Sample(sampler_states[LINEAR], pin.texcoord).r * Roughness);
    // AOマップ
    float ao = AOMap.Sample(sampler_states[LINEAR], pin.texcoord).r;

    pout.MRO = float4(metallic, roughness, ao, 1.0f);

    // エミッション
    float3 emission = EmissionMap.Sample(sampler_states[LINEAR], pin.texcoord).rgb * emissivecolor.rgb * emissiveintensity;
    pout.emission = float4(emission.rgb, alpha);

    //アウトライン
    pout.outline = float4(outlineColor * outlineintensity, 1.0f);

    return pout;
}
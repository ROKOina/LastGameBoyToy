#include "CPUParticle.hlsli"
#include "..\\3D\\Light.hlsli"

GS_IN main(VS_IN input)
{
    GS_IN output = (GS_IN) 0;

    // 簡易的なライティング計算
    float3 N = normalize(input.Normal);
    float3 L = normalize(-directionalLight.direction.xyz);
    float d = dot(L, N);
    float power = max(0.0f, d) * 0.5f + 0.5f; // dが0未満の場合を考慮してmax関数を使用

    // output構造体の各フィールドを入力からコピー
    output.Position = input.Position;
    output.Rotate = input.Rotate;

    // カラーのrgb成分にライティングの影響を適用
    output.Color.rgb = input.Color.rgb * power;
    output.Color.a = input.Color.a;

    output.Size = input.Size;
    output.Normal = N;
    output.Param = input.Param;

    return output;
}
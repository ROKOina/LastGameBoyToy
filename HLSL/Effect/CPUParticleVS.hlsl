#include "CPUParticle.hlsli"
#include "..\\3D\\Light.hlsli"

GS_IN main(VS_IN input)
{
    GS_IN output = (GS_IN) 0;

    // 法線と光方向の正規化
    float3 N = normalize(input.Normal);
    float3 L = normalize(-directionalLight.direction.xyz);

    // ディフューズ計算
    float d = dot(L, N);
    float diffusePower = max(0.0f, d) * 0.5f + 0.5f;

    // output構造体の各フィールドを入力からコピー
    output.Position = input.Position;
    output.Rotate = input.Rotate;

    // カラーのrgb成分にライティングの影響を適用
    output.Color.rgb = input.Color.rgb * diffusePower;
    output.Color.a = input.Color.a;

    output.Size = input.Size;
    output.Normal = N;
    output.Param = input.Param;

    return output;
}
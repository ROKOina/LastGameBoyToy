#include "Shadow.hlsli"

[maxvertexcount(3)]
void main(triangle VS_OUT_CSM input[3], inout TriangleStream<GS_OUTPUT_CSM> output)
{
    GS_OUTPUT_CSM element;

    // 各頂点を出力
    for (int i = 0; i < 3; ++i)
    {
        element.slice = input[i].slice; // 各頂点のスライスを設定
        element.position = input[i].position; // 各頂点の位置を設定
        output.Append(element); // 頂点をストリームに追加
    }

    output.RestartStrip(); // ストリップの再起動
}
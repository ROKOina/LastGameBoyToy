#include "Sprite.hlsli"
#include  "../Constants.hlsli"

VS_OUT main(float4 position : POSITION, float4 color : COLOR, float2 texcoord : TEXCOORD)
{
    VS_OUT vout;

    // 時間を使ってテクスチャ座標を変化させる
    float offset = sin(time * 2.0f + texcoord.y * 10.0f) * 0.1f; // Y方向の位置に応じてオフセットを決定
    vout.texcoord = texcoord + float2(0.2,0.2) * offset; // directionで動きの方向を制御

    vout.position = position;
    vout.color = color;

    return vout;
}
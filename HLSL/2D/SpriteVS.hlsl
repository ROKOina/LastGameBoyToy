#include "Sprite.hlsli"
#include  "../Constants.hlsli"

VS_OUT main(float4 position : POSITION, float4 color : COLOR, float2 texcoord : TEXCOORD)
{
    VS_OUT vout;

    // ポジションとカラーをそのまま渡す
    vout.position = position;
    vout.color = color;

    // アニメーション用のテクスチャオフセット計算
    float2 frameSize = float2(1.0 / columns, 1.0 / rows); // フレームのサイズ
    int totalFrames = rows * columns; // フレーム総数
    int currentFrame = (int) (time * framerate) % totalFrames; // 現在のフレーム番号
    float2 frameOffset = float2(currentFrame % columns, currentFrame / columns) * frameSize;

    // 現在のテクスチャ座標をフレームオフセットで調整
    vout.texcoord = texcoord * frameSize + frameOffset;

    return vout;
}
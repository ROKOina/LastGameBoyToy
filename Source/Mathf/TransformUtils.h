#pragma once

#include <DirectXMath.h>

class TransformUtils
{
public:
    // 行列からヨー、ピッチ、ロールを行列を計算する。
    static bool MatrixToRollPitchYaw(const DirectX::XMFLOAT4X4& m, float& pitch, float& yaw, float& roll);

    // クォータニオンからヨー、ピッチ、ロールを行列を計算する。
    static bool QuaternionToRollPitchYaw(const DirectX::XMFLOAT4& q, float& pitch, float& yaw, float& roll);

    // トランスフォーム行列からスケール値、回転値、移動値を計算する
    static void MatrixToTransformation(const DirectX::XMFLOAT4X4& m, DirectX::XMFLOAT3* scaling, DirectX::XMFLOAT4* rotation, DirectX::XMFLOAT3* translation);
};

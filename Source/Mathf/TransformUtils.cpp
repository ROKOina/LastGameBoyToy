#include "TransformUtils.h"

// 行列からヨー、ピッチ、ロールを行列を計算する。
bool TransformUtils::MatrixToRollPitchYaw(const DirectX::XMFLOAT4X4& m, float& pitch, float& yaw, float& roll)
{
    float xRadian = asinf(-m._32);
    pitch = xRadian;
    if (xRadian < DirectX::XM_PI / 2.0f)
    {
        if (xRadian > -DirectX::XM_PI / 2.0f)
        {
            roll = atan2f(m._12, m._22);
            yaw = atan2f(m._31, m._33);
            return true;
        }
        else
        {
            roll = (float)-atan2f(m._13, m._11);
            yaw = 0.0f;
            return false;
        }
    }
    else
    {
        roll = (float)atan2f(m._13, m._11);
        yaw = 0.0f;
        return false;
    }
}

// クォータニオンからヨー、ピッチ、ロールを行列を計算する。
bool TransformUtils::QuaternionToRollPitchYaw(const DirectX::XMFLOAT4& q, float& pitch, float& yaw, float& roll)
{
    DirectX::XMVECTOR Q = DirectX::XMLoadFloat4(&q);
    DirectX::XMMATRIX M = DirectX::XMMatrixRotationQuaternion(Q);
    DirectX::XMFLOAT4X4 m;
    DirectX::XMStoreFloat4x4(&m, M);
    return MatrixToRollPitchYaw(m, pitch, yaw, roll);
}

// トランスフォーム行列からスケール値、回転値、移動値を計算する
void TransformUtils::MatrixToTransformation(const DirectX::XMFLOAT4X4& m, DirectX::XMFLOAT3* scaling, DirectX::XMFLOAT4* rotation, DirectX::XMFLOAT3* translation)
{
    DirectX::XMMATRIX M = DirectX::XMLoadFloat4x4(&m);
    if (scaling != nullptr)
    {
        DirectX::XMVECTOR SX = DirectX::XMVector3Length(M.r[0]);
        DirectX::XMVECTOR SY = DirectX::XMVector3Length(M.r[1]);
        DirectX::XMVECTOR SZ = DirectX::XMVector3Length(M.r[2]);
        DirectX::XMStoreFloat(&scaling->x, SX);
        DirectX::XMStoreFloat(&scaling->y, SY);
        DirectX::XMStoreFloat(&scaling->z, SZ);
    }

    if (rotation != nullptr)
    {
        M.r[0] = DirectX::XMVector3Normalize(M.r[0]);
        M.r[1] = DirectX::XMVector3Normalize(M.r[1]);
        M.r[2] = DirectX::XMVector3Normalize(M.r[2]);
        DirectX::XMVECTOR Q = DirectX::XMQuaternionRotationMatrix(M);
        DirectX::XMStoreFloat4(rotation, Q);
    }

    if (translation != nullptr)
    {
        translation->x = m._41;
        translation->y = m._42;
        translation->z = m._43;
    }
}
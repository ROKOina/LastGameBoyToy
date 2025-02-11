#include "define.h"
#include "vector2.h"
#include "vector3.h"
#include "vector4.h"
#include "quaternion.h"
#include "matrix.h"

namespace chimera
{

    //============================================================================
    // 定数
    //----------------------------------------------------------------------------
    const Vector3		Vector3::Zero  = { 0.0f, 0.0f, 0.0f };
    const Vector3		Vector3::AxisX = { 1.0f, 0.0f, 0.0f };
    const Vector3		Vector3::AxisY = { 0.0f, 1.0f, 0.0f };
    const Vector3		Vector3::AxisZ = { 0.0f, 0.0f, 1.0f };
    const Vector3		Vector3::One   = { 1.0f, 1.0f, 1.0f };

    Vector3& Vector3::operator+=(const Vector3& v) noexcept
    {
        DirectX::XMVECTOR v1_ = DirectX::XMLoadFloat3(this);
        DirectX::XMVECTOR v2_ = DirectX::XMLoadFloat3(&v);
        DirectX::XMVECTOR x_ = DirectX::XMVectorAdd(v1_, v2_);
        DirectX::XMStoreFloat3(this, x_); return *this;
    }

    Vector3& Vector3::operator-=(const Vector3& v) noexcept
    {
        DirectX::XMVECTOR v1_ = DirectX::XMLoadFloat3(this);
        DirectX::XMVECTOR v2_ = DirectX::XMLoadFloat3(&v);
        DirectX::XMVECTOR x_ = DirectX::XMVectorSubtract(v1_, v2_);
        DirectX::XMStoreFloat3(this, x_); return *this;
    }

    Vector3& Vector3::operator*=(const Vector3& v) noexcept
    {
        DirectX::XMVECTOR v1_ = DirectX::XMLoadFloat3(this);
        DirectX::XMVECTOR v2_ = DirectX::XMLoadFloat3(&v);
        DirectX::XMVECTOR x_ = DirectX::XMVectorMultiply(v1_, v2_);
        DirectX::XMStoreFloat3(this, x_); return *this;
    }

    Vector3& Vector3::operator/=(const Vector3& v) noexcept
    {
        DirectX::XMVECTOR v1_ = DirectX::XMLoadFloat3(this);
        DirectX::XMVECTOR v2_ = DirectX::XMLoadFloat3(&v);
        DirectX::XMVECTOR x_ = DirectX::XMVectorDivide(v1_, v2_);
        DirectX::XMStoreFloat3(this, x_); return *this;
    }

    Vector3& Vector3::operator*=(float s) noexcept
    {
        DirectX::XMVECTOR v_ = DirectX::XMLoadFloat3(this);
        DirectX::XMVECTOR x_ = DirectX::XMVectorScale(v_, s);
        DirectX::XMStoreFloat3(this, x_); return *this;
    }

    Vector3& Vector3::operator/=(float s) noexcept
    {
        DirectX::XMVECTOR v_ = DirectX::XMLoadFloat3(this);
        DirectX::XMVECTOR x_ = DirectX::XMVectorScale(v_, 1.0f / s);
        DirectX::XMStoreFloat3(this, x_); return *this;
    }

    Vector3& Vector3::operator*=(const Matrix& m) noexcept
    {
        DirectX::XMVECTOR v_ = DirectX::XMLoadFloat3(this);
        DirectX::XMMATRIX m_ = DirectX::XMLoadFloat4x4(&m);
        DirectX::XMVECTOR x_ = DirectX::XMVector3Transform(v_, m_);
        DirectX::XMStoreFloat3(this, x_); return *this;
    }

    bool operator==(const Vector3& v1, const Vector3& v2) noexcept
    {
        DirectX::XMVECTOR v1_ = DirectX::XMLoadFloat3(&v1);
        DirectX::XMVECTOR v2_ = DirectX::XMLoadFloat3(&v2);
        return DirectX::XMVector3Equal(v1_, v2_);
    }

    bool operator!=(const Vector3& v1, const Vector3& v2) noexcept
    {
        DirectX::XMVECTOR v1_ = DirectX::XMLoadFloat3(&v1);
        DirectX::XMVECTOR v2_ = DirectX::XMLoadFloat3(&v2);
        return DirectX::XMVector3NotEqual(v1_, v2_);
    }

    Vector3 operator+(const Vector3& v) noexcept
    {
        return v;
    }

    Vector3 operator-(const Vector3& v) noexcept
    {
        DirectX::XMVECTOR v_ = DirectX::XMLoadFloat3(&v);
        DirectX::XMVECTOR x_ = DirectX::XMVectorNegate(v_);
        Vector3 r; DirectX::XMStoreFloat3(&r, x_); return r;
    }

    Vector3 operator+(const Vector3& v1, const Vector3& v2) noexcept
    {
        DirectX::XMVECTOR v1_ = DirectX::XMLoadFloat3(&v1);
        DirectX::XMVECTOR v2_ = DirectX::XMLoadFloat3(&v2);
        DirectX::XMVECTOR x_ = DirectX::XMVectorAdd(v1_, v2_);
        Vector3 r; DirectX::XMStoreFloat3(&r, x_); return r;
    }

    Vector3 operator-(const Vector3& v1, const Vector3& v2) noexcept
    {
        DirectX::XMVECTOR v1_ = DirectX::XMLoadFloat3(&v1);
        DirectX::XMVECTOR v2_ = DirectX::XMLoadFloat3(&v2);
        DirectX::XMVECTOR x_ = DirectX::XMVectorSubtract(v1_, v2_);
        Vector3 r; DirectX::XMStoreFloat3(&r, x_); return r;
    }

    Vector3 operator*(const Vector3& v1, const Vector3& v2) noexcept
    {
        DirectX::XMVECTOR v1_ = DirectX::XMLoadFloat3(&v1);
        DirectX::XMVECTOR v2_ = DirectX::XMLoadFloat3(&v2);
        DirectX::XMVECTOR x_ = DirectX::XMVectorMultiply(v1_, v2_);
        Vector3 r; DirectX::XMStoreFloat3(&r, x_); return r;
    }

    Vector3 operator/(const Vector3& v1, const Vector3& v2) noexcept
    {
        DirectX::XMVECTOR v1_ = DirectX::XMLoadFloat3(&v1);
        DirectX::XMVECTOR v2_ = DirectX::XMLoadFloat3(&v2);
        DirectX::XMVECTOR x_ = DirectX::XMVectorDivide(v1_, v2_);
        Vector3 r; DirectX::XMStoreFloat3(&r, x_); return r;
    }

    Vector3 operator*(const Vector3& v, float s) noexcept
    {
        DirectX::XMVECTOR v_ = DirectX::XMLoadFloat3(&v);
        DirectX::XMVECTOR x_ = DirectX::XMVectorScale(v_, s);
        Vector3 r; DirectX::XMStoreFloat3(&r, x_); return r;
    }

    Vector3 operator*(float s, const Vector3& v) noexcept
    {
        DirectX::XMVECTOR v_ = DirectX::XMLoadFloat3(&v);
        DirectX::XMVECTOR x_ = DirectX::XMVectorScale(v_, s);
        Vector3 r; DirectX::XMStoreFloat3(&r, x_); return r;
    }

    Vector3 operator/(const Vector3& v, float s) noexcept
    {
        DirectX::XMVECTOR v_ = DirectX::XMLoadFloat3(&v);
        DirectX::XMVECTOR x_ = DirectX::XMVectorScale(v_, 1.0f / s);
        Vector3 r; DirectX::XMStoreFloat3(&r, x_); return r;
    }

    Vector3 operator*(const Vector3& v, const Matrix& m) noexcept
    {
        DirectX::XMVECTOR v_ = DirectX::XMLoadFloat3(&v);
        DirectX::XMMATRIX m_ = DirectX::XMLoadFloat4x4(&m);
        DirectX::XMVECTOR x_ = DirectX::XMVector3Transform(v_, m_);
        Vector3 r; DirectX::XMStoreFloat3(&r, x_); return r;
    }

    float Vector3::Length() const noexcept
    {
        DirectX::XMVECTOR v_ = DirectX::XMLoadFloat3(this);
        DirectX::XMVECTOR x_ = DirectX::XMVector3Length(v_);
        float r; DirectX::XMStoreFloat(&r, x_); return r;
    }

    float Vector3::LengthSq() const noexcept
    {
        DirectX::XMVECTOR v_ = DirectX::XMLoadFloat3(this);
        DirectX::XMVECTOR x_ = DirectX::XMVector3LengthSq(v_);
        float r; DirectX::XMStoreFloat(&r, x_); return r;
    }

    float Dot(const Vector3& v1, const Vector3& v2) noexcept
    {
        DirectX::XMVECTOR v1_ = DirectX::XMLoadFloat3(&v1);
        DirectX::XMVECTOR v2_ = DirectX::XMLoadFloat3(&v2);
        DirectX::XMVECTOR x_ = DirectX::XMVector3Dot(v1_, v2_);
        float r; DirectX::XMStoreFloat(&r, x_); return r;
    }

    Vector3 Cross(const Vector3& v1, const Vector3& v2) noexcept
    {
        DirectX::XMVECTOR v1_ = DirectX::XMLoadFloat3(&v1);
        DirectX::XMVECTOR v2_ = DirectX::XMLoadFloat3(&v2);
        DirectX::XMVECTOR x_ = DirectX::XMVector3Cross(v1_, v2_);
        Vector3 r; DirectX::XMStoreFloat3(&r, x_); return r;
    }

    Vector3 Normalize(const Vector3& v) noexcept
    {
        DirectX::XMVECTOR v_ = DirectX::XMLoadFloat3(&v);
        DirectX::XMVECTOR x_ = DirectX::XMVector3Normalize(v_);
        Vector3 r; DirectX::XMStoreFloat3(&r, x_); return r;
    }

    float Length(const Vector3& v) noexcept
    {
        DirectX::XMVECTOR v_ = DirectX::XMLoadFloat3(&v);
        DirectX::XMVECTOR x_ = DirectX::XMVector3Length(v_);
        float r; DirectX::XMStoreFloat(&r, x_); return r;
    }

    float LengthSq(const Vector3& v) noexcept
    {
        DirectX::XMVECTOR v_ = DirectX::XMLoadFloat3(&v);
        DirectX::XMVECTOR x_ = DirectX::XMVector3LengthSq(v_);
        float r; DirectX::XMStoreFloat(&r, x_); return r;
    }

    Vector3 Clamp(const Vector3& v, const Vector3& min, const Vector3& max) noexcept
    {
        DirectX::XMVECTOR v_   = DirectX::XMLoadFloat3(&v);
        DirectX::XMVECTOR min_ = DirectX::XMLoadFloat3(&min);
        DirectX::XMVECTOR max_ = DirectX::XMLoadFloat3(&max);
        DirectX::XMVECTOR x_ = DirectX::XMVectorClamp(v_, min_, max_);
        Vector3 r; DirectX::XMStoreFloat3(&r, x_); return r;
    }

    float Distance(const Vector3& v1, const Vector3& v2) noexcept
    {
        DirectX::XMVECTOR v1_ = DirectX::XMLoadFloat3(&v1);
        DirectX::XMVECTOR v2_ = DirectX::XMLoadFloat3(&v2);
        DirectX::XMVECTOR x_ = DirectX::XMVector3Length(DirectX::XMVectorSubtract(v1_, v2_));
        float r; DirectX::XMStoreFloat(&r, x_); return r;
    }

    float DistanceSq(const Vector3& v1, const Vector3& v2) noexcept
    {
        DirectX::XMVECTOR v1_ = DirectX::XMLoadFloat3(&v1);
        DirectX::XMVECTOR v2_ = DirectX::XMLoadFloat3(&v2);
        DirectX::XMVECTOR x_ = DirectX::XMVector3LengthSq(DirectX::XMVectorSubtract(v1_, v2_));
        float r; DirectX::XMStoreFloat(&r, x_); return r;
    }

    Vector3 Min(const Vector3& v1, const Vector3& v2) noexcept
    {
        DirectX::XMVECTOR v1_ = DirectX::XMLoadFloat3(&v1);
        DirectX::XMVECTOR v2_ = DirectX::XMLoadFloat3(&v2);
        DirectX::XMVECTOR x_ = DirectX::XMVectorMin(v1_, v2_);
        Vector3 r; DirectX::XMStoreFloat3(&r, x_); return r;
    }

    Vector3 Max(const Vector3& v1, const Vector3& v2) noexcept
    {
        DirectX::XMVECTOR v1_ = DirectX::XMLoadFloat3(&v1);
        DirectX::XMVECTOR v2_ = DirectX::XMLoadFloat3(&v2);
        DirectX::XMVECTOR x_ = DirectX::XMVectorMax(v1_, v2_);
        Vector3 r; DirectX::XMStoreFloat3(&r, x_); return r;
    }

    Vector3 Lerp(const Vector3& v0, const Vector3& v1, float t) noexcept
    {
        DirectX::XMVECTOR v0_ = DirectX::XMLoadFloat3(&v0);
        DirectX::XMVECTOR v1_ = DirectX::XMLoadFloat3(&v1);
        DirectX::XMVECTOR x_ = DirectX::XMVectorLerp(v0_, v1_, t);
        Vector3 r; DirectX::XMStoreFloat3(&r, x_); return r;
    }

    Vector3 Transform(const Vector3& v, const Matrix& m) noexcept
    {
        DirectX::XMVECTOR v_ = DirectX::XMLoadFloat3(&v);
        DirectX::XMMATRIX m_ = DirectX::XMLoadFloat4x4(&m);
        DirectX::XMVECTOR x_ = DirectX::XMVector3Transform(v_, m_);
        Vector3 r; DirectX::XMStoreFloat3(&r, x_); return r;
    }

    Vector3 TransformCoord(const Vector3& v, const Matrix& m) noexcept
    {
        DirectX::XMVECTOR v_ = DirectX::XMLoadFloat3(&v);
        DirectX::XMMATRIX m_ = DirectX::XMLoadFloat4x4(&m);
        DirectX::XMVECTOR x_ = DirectX::XMVector3TransformCoord(v_, m_);
        Vector3 r; DirectX::XMStoreFloat3(&r, x_); return r;
    }

    Vector3 TransformNormal(const Vector3& v, const Matrix& m) noexcept
    {
        DirectX::XMVECTOR v_ = DirectX::XMLoadFloat3(&v);
        DirectX::XMMATRIX m_ = DirectX::XMLoadFloat4x4(&m);
        DirectX::XMVECTOR x_ = DirectX::XMVector3TransformNormal(v_, m_);
        Vector3 r; DirectX::XMStoreFloat3(&r, x_); return r;
    }

    Vector3 ToRadian(const Vector3& degrees) noexcept
    {
        Vector3 r;
        r.x = DirectX::XMConvertToRadians(degrees.x);
        r.y = DirectX::XMConvertToRadians(degrees.y);
        r.z = DirectX::XMConvertToRadians(degrees.z);
        return r;
    }
    Vector3 ToDegree(const Vector3& radians) noexcept
    {
        Vector3 r;
        r.x = DirectX::XMConvertToDegrees(radians.x);
        r.y = DirectX::XMConvertToDegrees(radians.y);
        r.z = DirectX::XMConvertToDegrees(radians.z);
        return r;
    }
    bool IsFinite(const Vector3& v1) noexcept
    {
        return (isfinite(v1.x) || isfinite(v1.y) || isfinite(v1.z));
    }
}

//#include "define.h"
#include "Vector2.h"
//#include "vector3.h"
//#include "vector4.h"
//#include "quaternion.h"
//#include "matrix.h"

    Vector2& Vector2::operator+=(const Vector2& v) noexcept
    {
        DirectX::XMVECTOR v1_ = DirectX::XMLoadFloat2(this);
        DirectX::XMVECTOR v2_ = DirectX::XMLoadFloat2(&v);
        DirectX::XMVECTOR x_ = DirectX::XMVectorAdd(v1_, v2_);
        DirectX::XMStoreFloat2(this, x_); return *this;
    }

    Vector2& Vector2::operator-=(const Vector2& v) noexcept
    {
        DirectX::XMVECTOR v1_ = DirectX::XMLoadFloat2(this);
        DirectX::XMVECTOR v2_ = DirectX::XMLoadFloat2(&v);
        DirectX::XMVECTOR x_ = DirectX::XMVectorSubtract(v1_, v2_);
        DirectX::XMStoreFloat2(this, x_); return *this;
    }

    Vector2& Vector2::operator*=(const Vector2& v) noexcept
    {
        DirectX::XMVECTOR v1_ = DirectX::XMLoadFloat2(this);
        DirectX::XMVECTOR v2_ = DirectX::XMLoadFloat2(&v);
        DirectX::XMVECTOR x_ = DirectX::XMVectorMultiply(v1_, v2_);
        DirectX::XMStoreFloat2(this, x_); return *this;
    }

    Vector2& Vector2::operator/=(const Vector2& v) noexcept
    {
        DirectX::XMVECTOR v1_ = DirectX::XMLoadFloat2(this);
        DirectX::XMVECTOR v2_ = DirectX::XMLoadFloat2(&v);
        DirectX::XMVECTOR x_ = DirectX::XMVectorDivide(v1_, v2_);
        DirectX::XMStoreFloat2(this, x_); return *this;
    }

    Vector2& Vector2::operator*=(float s) noexcept
    {
        DirectX::XMVECTOR v_ = DirectX::XMLoadFloat2(this);
        DirectX::XMVECTOR x_ = DirectX::XMVectorScale(v_, s);
        DirectX::XMStoreFloat2(this, x_); return *this;
    }

    Vector2& Vector2::operator/=(float s) noexcept
    {
        DirectX::XMVECTOR v_ = DirectX::XMLoadFloat2(this);
        DirectX::XMVECTOR x_ = DirectX::XMVectorScale(v_, 1.0f / s);
        DirectX::XMStoreFloat2(this, x_); return *this;
    }

    //Vector2& Vector2::operator*=(const Matrix& m) noexcept
    //{
    //    DirectX::XMVECTOR v_ = DirectX::XMLoadFloat2(this);
    //    DirectX::XMMATRIX m_ = DirectX::XMLoadFloat4x4(&m);
    //    DirectX::XMVECTOR x_ = DirectX::XMVector2Transform(v_, m_);
    //    DirectX::XMStoreFloat2(this, x_); return *this;
    //}

    bool operator==(const Vector2& v1, const Vector2& v2) noexcept
    {
        DirectX::XMVECTOR v1_ = DirectX::XMLoadFloat2(&v1);
        DirectX::XMVECTOR v2_ = DirectX::XMLoadFloat2(&v2);
        return DirectX::XMVector2Equal(v1_, v2_);
    }

    bool operator!=(const Vector2& v1, const Vector2& v2) noexcept
    {
        DirectX::XMVECTOR v1_ = DirectX::XMLoadFloat2(&v1);
        DirectX::XMVECTOR v2_ = DirectX::XMLoadFloat2(&v2);
        return DirectX::XMVector2NotEqual(v1_, v2_);
    }

    Vector2 operator+(const Vector2& v) noexcept
    {
        return v;
    }

    Vector2 operator-(const Vector2& v) noexcept
    {
        DirectX::XMVECTOR v_ = DirectX::XMLoadFloat2(&v);
        DirectX::XMVECTOR x_ = DirectX::XMVectorNegate(v_);
        Vector2 r; DirectX::XMStoreFloat2(&r, x_); return r;
    }

    Vector2 operator+(const Vector2& v1, const Vector2& v2) noexcept
    {
        DirectX::XMVECTOR v1_ = DirectX::XMLoadFloat2(&v1);
        DirectX::XMVECTOR v2_ = DirectX::XMLoadFloat2(&v2);
        DirectX::XMVECTOR x_ = DirectX::XMVectorAdd(v1_, v2_);
        Vector2 r; DirectX::XMStoreFloat2(&r, x_); return r;
    }

    Vector2 operator-(const Vector2& v1, const Vector2& v2) noexcept
    {
        DirectX::XMVECTOR v1_ = DirectX::XMLoadFloat2(&v1);
        DirectX::XMVECTOR v2_ = DirectX::XMLoadFloat2(&v2);
        DirectX::XMVECTOR x_ = DirectX::XMVectorSubtract(v1_, v2_);
        Vector2 r; DirectX::XMStoreFloat2(&r, x_); return r;
    }

    Vector2 operator*(const Vector2& v1, const Vector2& v2) noexcept
    {
        DirectX::XMVECTOR v1_ = DirectX::XMLoadFloat2(&v1);
        DirectX::XMVECTOR v2_ = DirectX::XMLoadFloat2(&v2);
        DirectX::XMVECTOR x_ = DirectX::XMVectorMultiply(v1_, v2_);
        Vector2 r; DirectX::XMStoreFloat2(&r, x_); return r;
    }

    Vector2 operator/(const Vector2& v1, const Vector2& v2) noexcept
    {
        DirectX::XMVECTOR v1_ = DirectX::XMLoadFloat2(&v1);
        DirectX::XMVECTOR v2_ = DirectX::XMLoadFloat2(&v2);
        DirectX::XMVECTOR x_ = DirectX::XMVectorDivide(v1_, v2_);
        Vector2 r; DirectX::XMStoreFloat2(&r, x_); return r;
    }

    Vector2 operator*(const Vector2& v, float s) noexcept
    {
        DirectX::XMVECTOR v_ = DirectX::XMLoadFloat2(&v);
        DirectX::XMVECTOR x_ = DirectX::XMVectorScale(v_, s);
        Vector2 r; DirectX::XMStoreFloat2(&r, x_); return r;
    }

    Vector2 operator*(float s, const Vector2& v) noexcept
    {
        DirectX::XMVECTOR v_ = DirectX::XMLoadFloat2(&v);
        DirectX::XMVECTOR x_ = DirectX::XMVectorScale(v_, s);
        Vector2 r; DirectX::XMStoreFloat2(&r, x_); return r;
    }

    Vector2 operator/(const Vector2& v, float s) noexcept
    {
        DirectX::XMVECTOR v1_ = DirectX::XMLoadFloat2(&v);
        DirectX::XMVECTOR x_ = DirectX::XMVectorScale(v1_, 1.0f / s);
        Vector2 r; DirectX::XMStoreFloat2(&r, x_); return r;
    }

    //Vector2 operator*(const Vector2& v, const Matrix& m) noexcept
    //{
    //    DirectX::XMVECTOR v_ = DirectX::XMLoadFloat2(&v);
    //    DirectX::XMMATRIX m_ = DirectX::XMLoadFloat4x4(&m);
    //    DirectX::XMVECTOR x_ = DirectX::XMVector2Transform(v_, m_);
    //    Vector2 r; DirectX::XMStoreFloat2(&r, x_); return r;
    //}

    float Vector2::Length() const noexcept
    {
        DirectX::XMVECTOR v_ = DirectX::XMLoadFloat2(this);
        DirectX::XMVECTOR x_ = DirectX::XMVector2Length(v_);
        float r; DirectX::XMStoreFloat(&r, x_); return r;
    }

    float Vector2::LengthSq() const noexcept
    {
        DirectX::XMVECTOR v_ = DirectX::XMLoadFloat2(this);
        DirectX::XMVECTOR x_ = DirectX::XMVector2LengthSq(v_);
        float r; DirectX::XMStoreFloat(&r, x_); return r;
    }

    float Dot(const Vector2& v1, const Vector2& v2) noexcept
    {
        DirectX::XMVECTOR v1_ = DirectX::XMLoadFloat2(&v1);
        DirectX::XMVECTOR v2_ = DirectX::XMLoadFloat2(&v2);
        DirectX::XMVECTOR x_ = DirectX::XMVector2Dot(v1_, v2_);
        float r; DirectX::XMStoreFloat(&r, x_); return r;
    }

    float Cross(const Vector2& v1, const Vector2& v2) noexcept
    {
        DirectX::XMVECTOR v1_ = DirectX::XMLoadFloat2(&v1);
        DirectX::XMVECTOR v2_ = DirectX::XMLoadFloat2(&v2);
        DirectX::XMVECTOR x_ = DirectX::XMVector2Cross(v1_, v2_);
        float r; DirectX::XMStoreFloat(&r, x_); return r;
    }

    Vector2 Normalize(const Vector2& v) noexcept
    {
        DirectX::XMVECTOR v_ = DirectX::XMLoadFloat2(&v);
        DirectX::XMVECTOR x_ = DirectX::XMVector2Normalize(v_);
        Vector2 r; DirectX::XMStoreFloat2(&r, x_); return r;
    }

    float Length(const Vector2& v) noexcept
    {
        DirectX::XMVECTOR v_ = DirectX::XMLoadFloat2(&v);
        DirectX::XMVECTOR x_ = DirectX::XMVector2Length(v_);
        float r; DirectX::XMStoreFloat(&r, x_); return r;
    }

    float LengthSq(const Vector2& v) noexcept
    {
        DirectX::XMVECTOR v_ = DirectX::XMLoadFloat2(&v);
        DirectX::XMVECTOR x_ = DirectX::XMVector2LengthSq(v_);
        float r; DirectX::XMStoreFloat(&r, x_); return r;
    }

    Vector2 Clamp(const Vector2& v, const Vector2& min, const Vector2& max) noexcept
    {
        DirectX::XMVECTOR v_ = DirectX::XMLoadFloat2(&v);
        DirectX::XMVECTOR min_ = DirectX::XMLoadFloat2(&min);
        DirectX::XMVECTOR max_ = DirectX::XMLoadFloat2(&max);
        DirectX::XMVECTOR x_ = DirectX::XMVectorClamp(v_, min_, max_);
        Vector2 r; DirectX::XMStoreFloat2(&r, x_); return r;
    }

    float Distance(const Vector2& v1, const Vector2& v2) noexcept
    {
        DirectX::XMVECTOR v1_ = DirectX::XMLoadFloat2(&v1);
        DirectX::XMVECTOR v2_ = DirectX::XMLoadFloat2(&v2);
        DirectX::XMVECTOR x_ = DirectX::XMVector2Length(DirectX::XMVectorSubtract(v1_, v2_));
        float r; DirectX::XMStoreFloat(&r, x_); return r;
    }

    float DistanceSq(const Vector2& v1, const Vector2& v2) noexcept
    {
        DirectX::XMVECTOR v1_ = DirectX::XMLoadFloat2(&v1);
        DirectX::XMVECTOR v2_ = DirectX::XMLoadFloat2(&v2);
        DirectX::XMVECTOR x_ = DirectX::XMVector2LengthSq(DirectX::XMVectorSubtract(v1_, v2_));
        float r; DirectX::XMStoreFloat(&r, x_); return r;
    }

    Vector2 Min(const Vector2& v1, const Vector2& v2) noexcept
    {
        DirectX::XMVECTOR v1_ = DirectX::XMLoadFloat2(&v1);
        DirectX::XMVECTOR v2_ = DirectX::XMLoadFloat2(&v2);
        DirectX::XMVECTOR x_ = DirectX::XMVectorMin(v1_, v2_);
        Vector2 r; DirectX::XMStoreFloat2(&r, x_); return r;
    }

    Vector2 Max(const Vector2& v1, const Vector2& v2) noexcept
    {
        DirectX::XMVECTOR v1_ = DirectX::XMLoadFloat2(&v1);
        DirectX::XMVECTOR v2_ = DirectX::XMLoadFloat2(&v2);
        DirectX::XMVECTOR x_ = DirectX::XMVectorMax(v1_, v2_);
        Vector2 r; DirectX::XMStoreFloat2(&r, x_); return r;
    }

    Vector2 Lerp(const Vector2& v0, const Vector2& v1, float t) noexcept
    {
        DirectX::XMVECTOR v0_ = DirectX::XMLoadFloat2(&v0);
        DirectX::XMVECTOR v1_ = DirectX::XMLoadFloat2(&v1);
        DirectX::XMVECTOR x_ = DirectX::XMVectorLerp(v0_, v1_, t);
        Vector2 r; DirectX::XMStoreFloat2(&r, x_); return r;
    }

    //Vector2 Transform(const Vector2& v, const Matrix& m) noexcept
    //{
    //    DirectX::XMVECTOR v_ = DirectX::XMLoadFloat2(&v);
    //    DirectX::XMMATRIX m_ = DirectX::XMLoadFloat4x4(&m);
    //    DirectX::XMVECTOR x_ = DirectX::XMVector2Transform(v_, m_);
    //    Vector2 r; DirectX::XMStoreFloat2(&r, x_); return r;
    //}

    //Vector2 TransformCoord(const Vector2& v, const Matrix& m) noexcept
    //{
    //    DirectX::XMVECTOR v_ = DirectX::XMLoadFloat2(&v);
    //    DirectX::XMMATRIX m_ = DirectX::XMLoadFloat4x4(&m);
    //    DirectX::XMVECTOR x_ = DirectX::XMVector2TransformCoord(v_, m_);
    //    Vector2 r; DirectX::XMStoreFloat2(&r, x_); return r;
    //}

    //Vector2 TransformNormal(const Vector2& v, const Matrix& m) noexcept
    //{
    //    DirectX::XMVECTOR v_ = DirectX::XMLoadFloat2(&v);
    //    DirectX::XMMATRIX m_ = DirectX::XMLoadFloat4x4(&m);
    //    DirectX::XMVECTOR x_ = DirectX::XMVector2TransformNormal(v_, m_);
    //    Vector2 r; DirectX::XMStoreFloat2(&r, x_); return r;
    //}
    Vector2 ToRadian(const Vector2& degrees) noexcept
    {
        Vector2 r;
        r.x = DirectX::XMConvertToRadians(degrees.x);
        r.y = DirectX::XMConvertToRadians(degrees.y);
        return r;
    }
    Vector2 ToDegree(const Vector2& radians) noexcept
    {
        Vector2 r;
        r.x = DirectX::XMConvertToDegrees(radians.x);
        r.y = DirectX::XMConvertToDegrees(radians.y);
        return r;
    }

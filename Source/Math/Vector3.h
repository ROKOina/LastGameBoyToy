#pragma once

namespace chimera
{
    struct Vector2;
    struct Vector3;
    struct Vector4;
    struct Quaternion;
    struct Matrix;

    struct Vector3 : public DirectX::XMFLOAT3
    {
        using DirectX::XMFLOAT3::XMFLOAT3;

        static const Vector3	Zero;		// ゼロベクトル
        static const Vector3	AxisX;		// X軸単位ベクトル
        static const Vector3	AxisY;		// Y軸単位ベクトル
        static const Vector3	AxisZ;		// Z軸単位ベクトル
        static const Vector3	One;		// オール単位ベクトル

        Vector3(const DirectX::XMFLOAT3& v) noexcept : DirectX::XMFLOAT3(v)
        {
        }
        Vector3(const DirectX::XMVECTOR& v) noexcept
        {
            DirectX::XMStoreFloat3(this, v);
        }

        Vector3& operator+=(const Vector3& v) noexcept;
        Vector3& operator-=(const Vector3& v) noexcept;
        Vector3& operator*=(const Vector3& v) noexcept;
        Vector3& operator/=(const Vector3& v) noexcept;
        Vector3& operator*=(float s) noexcept;
        Vector3& operator/=(float s) noexcept;
        Vector3& operator*=(const Matrix& m) noexcept;

        float Length() const noexcept;
        float LengthSq() const noexcept;
    };

    bool operator==(const Vector3& v1, const Vector3& v2) noexcept;
    bool operator!=(const Vector3& v1, const Vector3& v2) noexcept;

    Vector3 operator+(const Vector3& v) noexcept;
    Vector3 operator-(const Vector3& v) noexcept;

    Vector3 operator+(const Vector3& v1, const Vector3& v2) noexcept;
    Vector3 operator-(const Vector3& v1, const Vector3& v2) noexcept;
    Vector3 operator*(const Vector3& v1, const Vector3& v2) noexcept;
    Vector3 operator/(const Vector3& v1, const Vector3& v2) noexcept;
    Vector3 operator*(const Vector3& v, float s) noexcept;
    Vector3 operator*(float s, const Vector3& v) noexcept;
    Vector3 operator/(const Vector3& v, float s) noexcept;
    Vector3 operator*(const Vector3& v, const Matrix& m) noexcept;

    float Dot(const Vector3& v1, const Vector3& v2) noexcept;
    Vector3 Cross(const Vector3& v1, const Vector3& v2) noexcept;
    Vector3 Normalize(const Vector3& v) noexcept;
    float Length(const Vector3& v) noexcept;
    float LengthSq(const Vector3& v) noexcept;
    Vector3 Clamp(const Vector3& v, const Vector3& min, const Vector3& max) noexcept;
    float Distance(const Vector3& v1, const Vector3& v2) noexcept;
    float DistanceSq(const Vector3& v1, const Vector3& v2) noexcept;
    Vector3 Min(const Vector3& v1, const Vector3& v2) noexcept;
    Vector3 Max(const Vector3& v1, const Vector3& v2) noexcept;
    Vector3 Lerp(const Vector3& v0, const Vector3& v1, float t) noexcept;
    Vector3 Transform(const Vector3& v, const Matrix& m) noexcept;
    Vector3 TransformCoord(const Vector3& v, const Matrix& m) noexcept;
    Vector3 TransformNormal(const Vector3& v, const Matrix& m) noexcept;

    Vector3 ToRadian(const Vector3& degrees) noexcept;
    Vector3 ToDegree(const Vector3& radians) noexcept;

    bool IsFinite(const Vector3& v1) noexcept;

    template<class Archive>
    void serialize(Archive& archive, Vector3& v)
    {
        archive(
            cereal::make_nvp("x", v.x),
            cereal::make_nvp("y", v.y),
            cereal::make_nvp("z", v.z)
        );
    }
}


#pragma once

    struct Vector2;
    //struct Vector3;
    //struct Vector4;
    //struct Quaternion;
    //struct Matrix;

    struct Vector2 : DirectX::XMFLOAT2
    {
        using DirectX::XMFLOAT2::XMFLOAT2;

        Vector2(const DirectX::XMFLOAT2& v) noexcept : DirectX::XMFLOAT2(v)
        {
        }
        Vector2(const DirectX::XMVECTOR& v) noexcept
        {
            DirectX::XMStoreFloat2(this, v);
        }

        Vector2& operator+=(const Vector2& v) noexcept;
        Vector2& operator-=(const Vector2& v) noexcept;
        Vector2& operator*=(const Vector2& v) noexcept;
        Vector2& operator/=(const Vector2& v) noexcept;
        Vector2& operator*=(float s) noexcept;
        Vector2& operator/=(float s) noexcept;
        //Vector2& operator*=(const Matrix& m) noexcept;

        float Length() const noexcept;
        float LengthSq() const noexcept;
    };

    bool operator==(const Vector2& v1, const Vector2& v2) noexcept;
    bool operator!=(const Vector2& v1, const Vector2& v2) noexcept;

    Vector2 operator+(const Vector2& v) noexcept;
    Vector2 operator-(const Vector2& v) noexcept;

    Vector2 operator+(const Vector2& v1, const Vector2& v2) noexcept;
    Vector2 operator-(const Vector2& v1, const Vector2& v2) noexcept;
    Vector2 operator*(const Vector2& v1, const Vector2& v2) noexcept;
    Vector2 operator/(const Vector2& v1, const Vector2& v2) noexcept;
    Vector2 operator*(const Vector2& v, float s) noexcept;
    Vector2 operator*(float s, const Vector2& v) noexcept;
    Vector2 operator/(const Vector2& v, float s) noexcept;
    //Vector2 operator*(const Vector2& v, const Matrix& m) noexcept;

    float Dot(const Vector2& v1, const Vector2& v2) noexcept;
    float Cross(const Vector2& v1, const Vector2& v2) noexcept;
    Vector2 Normalize(const Vector2& v) noexcept;
    float Length(const Vector2& v) noexcept;
    float LengthSq(const Vector2& v) noexcept;
    Vector2 Clamp(const Vector2& v, const Vector2& min, const Vector2& max) noexcept;
    float Distance(const Vector2& v1, const Vector2& v2) noexcept;
    float DistanceSq(const Vector2& v1, const Vector2& v2) noexcept;
    Vector2 Min(const Vector2& v1, const Vector2& v2) noexcept;
    Vector2 Max(const Vector2& v1, const Vector2& v2) noexcept;
    Vector2 Lerp(const Vector2& v0, const Vector2& v1, float t) noexcept;
    //Vector2 Transform(const Vector2& v, const Matrix& m) noexcept;
    //Vector2 TransformCoord(const Vector2& v, const Matrix& m) noexcept;
    //Vector2 TransformNormal(const Vector2& v, const Matrix& m) noexcept;

    Vector2 ToRadian(const Vector2& degrees) noexcept;
    Vector2 ToDegree(const Vector2& radians) noexcept;

    template<class Archive>
    void serialize(Archive& archive, Vector2& v)
    {
        archive(
            cereal::make_nvp("x", v.x),
            cereal::make_nvp("y", v.y)
        );
    }


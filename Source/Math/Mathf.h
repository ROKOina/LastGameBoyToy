#pragma once

#include <cmath>
#include <limits>
#include <numbers>
#include <DirectXMath.h>

// 浮動小数算術
class Mathf
{
public:
    // 線形補間
    static float Lerp(const float& start, const float& end, const  float& time);
    static DirectX::XMFLOAT2 Lerp(const DirectX::XMFLOAT2& start, const DirectX::XMFLOAT2& end, const  float& time);
    static DirectX::XMFLOAT3 Lerp(const DirectX::XMFLOAT3& start, const DirectX::XMFLOAT3& end, const  float& time);
    static DirectX::XMFLOAT4 Lerp(const DirectX::XMFLOAT4& start, const DirectX::XMFLOAT4& end, const  float& time);
    // 球面補間
    static DirectX::XMFLOAT4 Slerp(const DirectX::XMFLOAT4& start, const DirectX::XMFLOAT4& end, const  float& time);

    static float Clamp(float value, float min, float max);

    // SourceからTargetに向くクォータニオンを算出
    static DirectX::XMFLOAT4 LookAt(const DirectX::XMFLOAT3& source, const DirectX::XMFLOAT3& target);

    static float Dot(const DirectX::XMFLOAT2& first, const DirectX::XMFLOAT2& second);
    static float Dot(const DirectX::XMFLOAT3& first, const DirectX::XMFLOAT3& second);
    static float Dot(const DirectX::XMFLOAT4& first, const DirectX::XMFLOAT4& second);
    static float Dot(const DirectX::XMFLOAT3& value);

    static DirectX::XMFLOAT3 Cross(const DirectX::XMFLOAT3& first, const DirectX::XMFLOAT3& second);

    // 正規化
    static DirectX::XMFLOAT2 Normalize(const DirectX::XMFLOAT2& value);
    static DirectX::XMFLOAT3 Normalize(const DirectX::XMFLOAT3& value);
    static DirectX::XMFLOAT4 Normalize(const DirectX::XMFLOAT4& value);
    // 正規化 ( 戻り値:正規化ベクトル、第2引数:ベクトルの長さ )
    static DirectX::XMFLOAT2 Normalize(const DirectX::XMFLOAT2& value, float& length);
    static DirectX::XMFLOAT3 Normalize(const DirectX::XMFLOAT3& value, float& length);
    static DirectX::XMFLOAT4 Normalize(const DirectX::XMFLOAT4& value, float& length);

    //長さ
    static float Length(const DirectX::XMFLOAT2 Vec);
    static float Length(const DirectX::XMFLOAT3 Vec);

    //二つの値が等しいか判別する
    static bool  Equal(const float value1, const float value2);
    static bool  Equal(const DirectX::XMFLOAT2 value1, const DirectX::XMFLOAT2 value2);
    static bool  Equal(const DirectX::XMFLOAT3 value1, const DirectX::XMFLOAT3 value2);

    // 指定範囲のランダム値を計算
    static float RandomRange(const float& min, const float& max);

    // 符号を返す ( -1 or 1 )
    static int GetSign(const float& value);
    static int GetSign(const int& value);

    // 行列から指定の列を抜き出す
    static DirectX::XMFLOAT3 TransformSamplePosition(const DirectX::XMFLOAT4X4& trans);
    static DirectX::XMFLOAT3 TransformSampleRight(const DirectX::XMFLOAT4X4& trans);
    static DirectX::XMFLOAT3 TransformSampleUp(const DirectX::XMFLOAT4X4& trans);
    static DirectX::XMFLOAT3 TransformSampleFront(const DirectX::XMFLOAT4X4& trans);

    //
};

// ※以下の参考 : https://qiita.com/HnniTns/items/6e7edc82775a86923cef

// Floatの誤差を考慮した比較関数
template<typename Ty>
// NODISCARD = 返り値を無視 (呼び出した後、何もしなかった) すると警告をコンパイラに出させる
// constexpr = constexprキーワードを付けることで、その関数は、コンパイル時と実行時、両方で呼び出せる関数となる
// 引数が定数でかつ、左辺がconstexpr修飾された変数であれば、コンパイル時に呼び出される
_NODISCARD static inline constexpr bool AdjEqual(const Ty epsilon_num, const Ty num)
{
    // テンプレートの型が浮動小数かの確認
    static_assert(std::is_floating_point<Ty>(), "This type isn't floating point");

    constexpr auto Epsilon{ std::numeric_limits<Ty>::epsilon() };
    auto dis{ std::fabs(epsilon_num - num) };

    return (dis <= Epsilon);
}

namespace DirectX
{
    // FLOAT2
#if(1)
  // XMFLOAT2 同士の足し算
    static inline XMFLOAT2 operator+(const DirectX::XMFLOAT2& f1, const DirectX::XMFLOAT2& f2)
    {
        return { f1.x + f2.x,f1.y + f2.y };
    }
    static inline void operator+= (DirectX::XMFLOAT2& f1, const DirectX::XMFLOAT2& f2)
    {
        f1.x += f2.x;
        f1.y += f2.y;
    }

    static inline XMFLOAT2 operator-(const DirectX::XMFLOAT2& f1)
    {
        return { -f1.x,-f1.y };
    }

    // XMFLOAT2 同士の引き算
    static inline XMFLOAT2 operator-(const DirectX::XMFLOAT2& f1, const DirectX::XMFLOAT2& f2)
    {
        return { f1.x - f2.x,f1.y - f2.y };
    }
    static inline void operator-= (DirectX::XMFLOAT2& f1, const DirectX::XMFLOAT2& f2)
    {
        f1.x -= f2.x;
        f1.y -= f2.y;
    }

    // XMFLOAT2 同士の掛け算
    static inline XMFLOAT2 operator*(const DirectX::XMFLOAT2& f1, const DirectX::XMFLOAT2& f2)
    {
        return { f1.x * f2.x,f1.y * f2.y };
    }
    static inline void operator*= (DirectX::XMFLOAT2& f1, const DirectX::XMFLOAT2& f2)
    {
        f1.x *= f2.x;
        f1.y *= f2.y;
    }

    // XMFLOAT2 同士の割り算
    static inline XMFLOAT2 operator/(const DirectX::XMFLOAT2& f1, const DirectX::XMFLOAT2& f2)
    {
        return { f1.x / f2.x,f1.y / f2.y };
    }
    static inline void operator/= (DirectX::XMFLOAT2& f1, const DirectX::XMFLOAT2& f2)
    {
        f1.x /= f2.x;
        f1.y /= f2.y;
    }

    // XMFLOAT2 同士の比較
    _NODISCARD static inline constexpr bool operator== (const DirectX::XMFLOAT2& f1, const DirectX::XMFLOAT2& f2)
    {
        return (AdjEqual(f1.x, f2.x) && AdjEqual(f1.y, f2.y));
    }
    _NODISCARD static inline constexpr bool operator!= (const DirectX::XMFLOAT2& f1, const DirectX::XMFLOAT2& f2)
    {
        return !(AdjEqual(f1.x, f2.x) && AdjEqual(f1.y, f2.y));
    }

    // XMFLOAT2 とfloat の足し算
    static inline XMFLOAT2 operator+(const DirectX::XMFLOAT2& f1, const float& num)
    {
        return { f1.x + num,f1.y + num };
    }
    static inline void operator+=(DirectX::XMFLOAT2& f1, const float& num)
    {
        f1.x += num;
        f1.y += num;
    }

    // XMFLOAT2 とfloat の引き算
    static inline XMFLOAT2 operator-(const DirectX::XMFLOAT2& f1, const float& num)
    {
        return { f1.x - num,f1.y - num };
    }
    static inline void operator-=(DirectX::XMFLOAT2& f1, const float& num)
    {
        f1.x -= num;
        f1.y -= num;
    }

    // XMFLOAT2 とfloat の掛け算
    static inline XMFLOAT2 operator*(const DirectX::XMFLOAT2& f1, const float& num)
    {
        return { f1.x * num,f1.y * num };
    }
    static inline void operator*=(DirectX::XMFLOAT2& f1, const float& num)
    {
        f1.x *= num;
        f1.y *= num;
    }

    // XMFLOAT2 とfloat の割り算
    static inline XMFLOAT2 operator/(const DirectX::XMFLOAT2& f1, const float& num)
    {
        return { f1.x / num,f1.y / num };
    }
    static inline void operator/=(DirectX::XMFLOAT2& f1, const float& num)
    {
        f1.x /= num;
        f1.y /= num;
    }

    // XMFLOAT2 とfloat の比較
    _NODISCARD static inline constexpr bool operator== (const DirectX::XMFLOAT2& f1, const float& num)
    {
        return (AdjEqual(f1.x, num) && AdjEqual(f1.y, num));
    }
    _NODISCARD static inline constexpr bool operator!= (const DirectX::XMFLOAT2& f1, const float& num)
    {
        return !(AdjEqual(f1.x, num) && AdjEqual(f1.y, num));
    }
#endif

    // FLOAT3
#if(1)
  // XMFLOAT3 同士の足し算
    static inline XMFLOAT3 operator+(const DirectX::XMFLOAT3& f1, const DirectX::XMFLOAT3& f2)
    {
        return { f1.x + f2.x,f1.y + f2.y,f1.z + f2.z };
    }
    static inline void operator+= (DirectX::XMFLOAT3& f1, const DirectX::XMFLOAT3& f2)
    {
        f1.x += f2.x;
        f1.y += f2.y;
        f1.z += f2.z;
    }

    static inline XMFLOAT3 operator-(const DirectX::XMFLOAT3& f1)
    {
        return { -f1.x,-f1.y,-f1.z };
    }

    // XMFLOAT3 同士の引き算
    static inline XMFLOAT3 operator-(const DirectX::XMFLOAT3& f1, const DirectX::XMFLOAT3& f2)
    {
        return { f1.x - f2.x,f1.y - f2.y ,f1.z - f2.z };
    }
    static inline void operator-= (DirectX::XMFLOAT3& f1, const DirectX::XMFLOAT3& f2)
    {
        f1.x -= f2.x;
        f1.y -= f2.y;
        f1.z -= f2.z;
    }

    // XMFLOAT3 同士の掛け算
    static inline XMFLOAT3 operator*(const DirectX::XMFLOAT3& f1, const DirectX::XMFLOAT3& f2)
    {
        return { f1.x * f2.x,f1.y * f2.y ,f1.z * f2.z };
    }
    static inline void operator*= (DirectX::XMFLOAT3& f1, const DirectX::XMFLOAT3& f2)
    {
        f1.x *= f2.x;
        f1.y *= f2.y;
        f1.z *= f2.z;
    }

    // XMFLOAT3 同士の割り算
    static inline XMFLOAT3 operator/(const DirectX::XMFLOAT3& f1, const DirectX::XMFLOAT3& f2)
    {
        return { f1.x / f2.x,f1.y / f2.y ,f1.z / f2.z };
    }
    static inline void operator/= (DirectX::XMFLOAT3& f1, const DirectX::XMFLOAT3& f2)
    {
        f1.x /= f2.x;
        f1.y /= f2.y;
        f1.z /= f2.z;
    }

    // XMFLOAT3 同士の比較
    _NODISCARD static inline constexpr bool operator== (const DirectX::XMFLOAT3& f1, const DirectX::XMFLOAT3& f2)
    {
        return (AdjEqual(f1.x, f2.x) && AdjEqual(f1.y, f2.y));
    }
    _NODISCARD static inline constexpr bool operator!= (const DirectX::XMFLOAT3& f1, const DirectX::XMFLOAT3& f2)
    {
        return !(AdjEqual(f1.x, f2.x) && AdjEqual(f1.y, f2.y));
    }

    // XMFLOAT3 とfloat の足し算
    static inline XMFLOAT3 operator+(const DirectX::XMFLOAT3& f1, const float& num)
    {
        return { f1.x + num,f1.y + num,f1.z + num };
    }
    static inline void operator+=(DirectX::XMFLOAT3& f1, const float& num)
    {
        f1.x += num;
        f1.y += num;
        f1.z += num;
    }

    // XMFLOAT3 とfloat の引き算
    static inline XMFLOAT3 operator-(const DirectX::XMFLOAT3& f1, const float& num)
    {
        return { f1.x - num,f1.y - num,f1.z - num };
    }
    static inline void operator-=(DirectX::XMFLOAT3& f1, const float& num)
    {
        f1.x -= num;
        f1.y -= num;
        f1.z -= num;
    }

    // XMFLOAT3 とfloat のかけ算
    static inline XMFLOAT3 operator*(const DirectX::XMFLOAT3& f1, const float& num)
    {
        return { f1.x * num,f1.y * num,f1.z * num };
    }
    static inline void operator*=(DirectX::XMFLOAT3& f1, const float& num)
    {
        f1.x *= num;
        f1.y *= num;
        f1.z *= num;
    }

    // XMFLOAT3 とfloat のわり算
    static inline XMFLOAT3 operator/(const DirectX::XMFLOAT3& f1, const float& num)
    {
        return { f1.x / num,f1.y / num,f1.z / num };
    }
    static inline void operator/=(DirectX::XMFLOAT3& f1, const float& num)
    {
        f1.x /= num;
        f1.y /= num;
        f1.z /= num;
    }

    // XMFLOAT3 とfloat の比較
    _NODISCARD static inline constexpr bool operator== (const DirectX::XMFLOAT3& f1, const float& num)
    {
        return (AdjEqual(f1.x, num) && AdjEqual(f1.y, num) && AdjEqual(f1.z, num));
    }
    _NODISCARD static inline constexpr bool operator!= (const DirectX::XMFLOAT3& f1, const float& num)
    {
        return !(AdjEqual(f1.x, num) && AdjEqual(f1.y, num) && AdjEqual(f1.z, num));
    }
#endif

    //FLOAT4
#if(1)
  // XMFLOAT4とXMFLOAT4の足し算
    static inline XMFLOAT4 operator+(const DirectX::XMFLOAT4& f1, const DirectX::XMFLOAT4& f2)
    {
        return { f1.x + f2.x,f1.y + f2.y,f1.z + f2.z, f1.w + f2.w };
    }

    static inline XMFLOAT4 operator-(const DirectX::XMFLOAT4& f1)
    {
        return { -f1.x,-f1.y,-f1.z,-f1.w };
    }

    // XMFLOAT4 同士の引き算
    static inline XMFLOAT4 operator-(const DirectX::XMFLOAT4& f1, const DirectX::XMFLOAT4& f2)
    {
        return { f1.x - f2.x,f1.y - f2.y ,f1.z - f2.z,f1.w - f2.w };
    }

    // XMFLOAT4とFLOATの掛け算
    static inline XMFLOAT4 operator*(const DirectX::XMFLOAT4& f1, const float& f2)
    {
        return { f1.x * f2,f1.y * f2,f1.z * f2, f1.w * f2 };
    }

    // XMFLOAT4とXMFLOAT4の掛け算
    static inline XMFLOAT4 operator*(const DirectX::XMFLOAT4& f1, const DirectX::XMFLOAT4& f2)
    {
        return { f1.x * f2.x,f1.y * f2.y,f1.z * f2.z, f1.w * f2.w };
    }

    // XMFLOAT4とFLOATの割り算
    static inline XMFLOAT4 operator/(const DirectX::XMFLOAT4& f1, const float& f2)
    {
        return { f1.x / f2,f1.y / f2,f1.z / f2, f1.w / f2 };
    }

    // XMFLOAT4とXMFLOAT4の割り算
    static inline XMFLOAT4 operator/(const DirectX::XMFLOAT4& f1, const DirectX::XMFLOAT4& f2)
    {
        return { f1.x / f2.x,f1.y / f2.y,f1.z / f2.z, f1.w / f2.w };
    }

    // XMFLOAT4とXMFLOAT4の比較
    static inline bool operator==(const DirectX::XMFLOAT4& f1, const DirectX::XMFLOAT4& f2)
    {
        return f1.x == f2.x && f1.y == f2.y && f1.z == f2.z && f1.w == f2.w;
    }

    // XMFLOAT4とXMFLOAT4の比較
    static inline bool operator!=(const DirectX::XMFLOAT4& f1, const DirectX::XMFLOAT4& f2)
    {
        return f1.x != f2.x || f1.y != f2.y || f1.z != f2.z || f1.w != f2.w;
    }

#endif
}

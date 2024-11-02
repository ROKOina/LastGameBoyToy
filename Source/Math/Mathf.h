#pragma once

#include <cmath>
#include <limits>
#include <numbers>
#include <DirectXMath.h>

// •‚“®¬”Zp
class Mathf
{
public:
    // üŒ`•âŠÔ
    static float Lerp(const float& start, const float& end, const  float& time);
    static DirectX::XMFLOAT2 Lerp(const DirectX::XMFLOAT2& start, const DirectX::XMFLOAT2& end, const  float& time);
    static DirectX::XMFLOAT3 Lerp(const DirectX::XMFLOAT3& start, const DirectX::XMFLOAT3& end, const  float& time);
    static DirectX::XMFLOAT4 Lerp(const DirectX::XMFLOAT4& start, const DirectX::XMFLOAT4& end, const  float& time);
    // ‹…–Ê•âŠÔ
    static DirectX::XMFLOAT4 Slerp(const DirectX::XMFLOAT4& start, const DirectX::XMFLOAT4& end, const  float& time);

    static float Clamp(float value, float min, float max);

    // Source‚©‚çTarget‚ÉŒü‚­ƒNƒH[ƒ^ƒjƒIƒ“‚ğZo
    static DirectX::XMFLOAT4 LookAt(const DirectX::XMFLOAT3& source, const DirectX::XMFLOAT3& target);

    static float Dot(const DirectX::XMFLOAT2& first, const DirectX::XMFLOAT2& second);
    static float Dot(const DirectX::XMFLOAT3& first, const DirectX::XMFLOAT3& second);
    static float Dot(const DirectX::XMFLOAT4& first, const DirectX::XMFLOAT4& second);
    static float Dot(const DirectX::XMFLOAT3& value);

    static DirectX::XMFLOAT3 Cross(const DirectX::XMFLOAT3& first, const DirectX::XMFLOAT3& second);

    // ³‹K‰»
    static DirectX::XMFLOAT2 Normalize(const DirectX::XMFLOAT2& value);
    static DirectX::XMFLOAT3 Normalize(const DirectX::XMFLOAT3& value);
    static DirectX::XMFLOAT4 Normalize(const DirectX::XMFLOAT4& value);
    // ³‹K‰» ( –ß‚è’l:³‹K‰»ƒxƒNƒgƒ‹A‘æ2ˆø”:ƒxƒNƒgƒ‹‚Ì’·‚³ )
    static DirectX::XMFLOAT2 Normalize(const DirectX::XMFLOAT2& value, float& length);
    static DirectX::XMFLOAT3 Normalize(const DirectX::XMFLOAT3& value, float& length);
    static DirectX::XMFLOAT4 Normalize(const DirectX::XMFLOAT4& value, float& length);

    //’·‚³
    static float Length(const DirectX::XMFLOAT2 Vec);
    static float Length(const DirectX::XMFLOAT3 Vec);

    //“ñ‚Â‚Ì’l‚ª“™‚µ‚¢‚©”»•Ê‚·‚é
    static bool  Equal(const float value1, const float value2);
    static bool  Equal(const DirectX::XMFLOAT2 value1, const DirectX::XMFLOAT2 value2);
    static bool  Equal(const DirectX::XMFLOAT3 value1, const DirectX::XMFLOAT3 value2);

    // w’è”ÍˆÍ‚Ìƒ‰ƒ“ƒ_ƒ€’l‚ğŒvZ
    static float RandomRange(const float& min, const float& max);

    // •„†‚ğ•Ô‚· ( -1 or 1 )
    static int GetSign(const float& value);
    static int GetSign(const int& value);

    // s—ñ‚©‚çw’è‚Ì—ñ‚ğ”²‚«o‚·
    static DirectX::XMFLOAT3 TransformSamplePosition(const DirectX::XMFLOAT4X4& trans);
    static DirectX::XMFLOAT3 TransformSampleRight(const DirectX::XMFLOAT4X4& trans);
    static DirectX::XMFLOAT3 TransformSampleUp(const DirectX::XMFLOAT4X4& trans);
    static DirectX::XMFLOAT3 TransformSampleFront(const DirectX::XMFLOAT4X4& trans);

    //
};

// ¦ˆÈ‰º‚ÌQl : https://qiita.com/HnniTns/items/6e7edc82775a86923cef

// Float‚ÌŒë·‚ğl—¶‚µ‚½”äŠrŠÖ”
template<typename Ty>
// NODISCARD = •Ô‚è’l‚ğ–³‹ (ŒÄ‚Ño‚µ‚½ŒãA‰½‚à‚µ‚È‚©‚Á‚½) ‚·‚é‚ÆŒx‚ğƒRƒ“ƒpƒCƒ‰‚Éo‚³‚¹‚é
// constexpr = constexprƒL[ƒ[ƒh‚ğ•t‚¯‚é‚±‚Æ‚ÅA‚»‚ÌŠÖ”‚ÍAƒRƒ“ƒpƒCƒ‹‚ÆÀsA—¼•û‚ÅŒÄ‚Ño‚¹‚éŠÖ”‚Æ‚È‚é
// ˆø”‚ª’è”‚Å‚©‚ÂA¶•Ó‚ªconstexprCü‚³‚ê‚½•Ï”‚Å‚ ‚ê‚ÎAƒRƒ“ƒpƒCƒ‹‚ÉŒÄ‚Ño‚³‚ê‚é
_NODISCARD static inline constexpr bool AdjEqual(const Ty epsilon_num, const Ty num)
{
    // ƒeƒ“ƒvƒŒ[ƒg‚ÌŒ^‚ª•‚“®¬”‚©‚ÌŠm”F
    static_assert(std::is_floating_point<Ty>(), "This type isn't floating point");

    constexpr auto Epsilon{ std::numeric_limits<Ty>::epsilon() };
    auto dis{ std::fabs(epsilon_num - num) };

    return (dis <= Epsilon);
}

namespace DirectX
{
    // FLOAT2
#if(1)
  // XMFLOAT2 “¯m‚Ì‘«‚µZ
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

    // XMFLOAT2 “¯m‚Ìˆø‚«Z
    static inline XMFLOAT2 operator-(const DirectX::XMFLOAT2& f1, const DirectX::XMFLOAT2& f2)
    {
        return { f1.x - f2.x,f1.y - f2.y };
    }
    static inline void operator-= (DirectX::XMFLOAT2& f1, const DirectX::XMFLOAT2& f2)
    {
        f1.x -= f2.x;
        f1.y -= f2.y;
    }

    // XMFLOAT2 “¯m‚ÌŠ|‚¯Z
    static inline XMFLOAT2 operator*(const DirectX::XMFLOAT2& f1, const DirectX::XMFLOAT2& f2)
    {
        return { f1.x * f2.x,f1.y * f2.y };
    }
    static inline void operator*= (DirectX::XMFLOAT2& f1, const DirectX::XMFLOAT2& f2)
    {
        f1.x *= f2.x;
        f1.y *= f2.y;
    }

    // XMFLOAT2 “¯m‚ÌŠ„‚èZ
    static inline XMFLOAT2 operator/(const DirectX::XMFLOAT2& f1, const DirectX::XMFLOAT2& f2)
    {
        return { f1.x / f2.x,f1.y / f2.y };
    }
    static inline void operator/= (DirectX::XMFLOAT2& f1, const DirectX::XMFLOAT2& f2)
    {
        f1.x /= f2.x;
        f1.y /= f2.y;
    }

    // XMFLOAT2 “¯m‚Ì”äŠr
    _NODISCARD static inline constexpr bool operator== (const DirectX::XMFLOAT2& f1, const DirectX::XMFLOAT2& f2)
    {
        return (AdjEqual(f1.x, f2.x) && AdjEqual(f1.y, f2.y));
    }
    _NODISCARD static inline constexpr bool operator!= (const DirectX::XMFLOAT2& f1, const DirectX::XMFLOAT2& f2)
    {
        return !(AdjEqual(f1.x, f2.x) && AdjEqual(f1.y, f2.y));
    }

    // XMFLOAT2 ‚Æfloat ‚Ì‘«‚µZ
    static inline XMFLOAT2 operator+(const DirectX::XMFLOAT2& f1, const float& num)
    {
        return { f1.x + num,f1.y + num };
    }
    static inline void operator+=(DirectX::XMFLOAT2& f1, const float& num)
    {
        f1.x += num;
        f1.y += num;
    }

    // XMFLOAT2 ‚Æfloat ‚Ìˆø‚«Z
    static inline XMFLOAT2 operator-(const DirectX::XMFLOAT2& f1, const float& num)
    {
        return { f1.x - num,f1.y - num };
    }
    static inline void operator-=(DirectX::XMFLOAT2& f1, const float& num)
    {
        f1.x -= num;
        f1.y -= num;
    }

    // XMFLOAT2 ‚Æfloat ‚ÌŠ|‚¯Z
    static inline XMFLOAT2 operator*(const DirectX::XMFLOAT2& f1, const float& num)
    {
        return { f1.x * num,f1.y * num };
    }
    static inline void operator*=(DirectX::XMFLOAT2& f1, const float& num)
    {
        f1.x *= num;
        f1.y *= num;
    }

    // XMFLOAT2 ‚Æfloat ‚ÌŠ„‚èZ
    static inline XMFLOAT2 operator/(const DirectX::XMFLOAT2& f1, const float& num)
    {
        return { f1.x / num,f1.y / num };
    }
    static inline void operator/=(DirectX::XMFLOAT2& f1, const float& num)
    {
        f1.x /= num;
        f1.y /= num;
    }

    // XMFLOAT2 ‚Æfloat ‚Ì”äŠr
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
  // XMFLOAT3 “¯m‚Ì‘«‚µZ
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

    // XMFLOAT3 “¯m‚Ìˆø‚«Z
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

    // XMFLOAT3 “¯m‚ÌŠ|‚¯Z
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

    // XMFLOAT3 “¯m‚ÌŠ„‚èZ
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

    // XMFLOAT3 “¯m‚Ì”äŠr
    _NODISCARD static inline constexpr bool operator== (const DirectX::XMFLOAT3& f1, const DirectX::XMFLOAT3& f2)
    {
        return (AdjEqual(f1.x, f2.x) && AdjEqual(f1.y, f2.y));
    }
    _NODISCARD static inline constexpr bool operator!= (const DirectX::XMFLOAT3& f1, const DirectX::XMFLOAT3& f2)
    {
        return !(AdjEqual(f1.x, f2.x) && AdjEqual(f1.y, f2.y));
    }

    // XMFLOAT3 ‚Æfloat ‚Ì‘«‚µZ
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

    // XMFLOAT3 ‚Æfloat ‚Ìˆø‚«Z
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

    // XMFLOAT3 ‚Æfloat ‚Ì‚©‚¯Z
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

    // XMFLOAT3 ‚Æfloat ‚Ì‚í‚èZ
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

    // XMFLOAT3 ‚Æfloat ‚Ì”äŠr
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
  // XMFLOAT4‚ÆXMFLOAT4‚Ì‘«‚µZ
    static inline XMFLOAT4 operator+(const DirectX::XMFLOAT4& f1, const DirectX::XMFLOAT4& f2)
    {
        return { f1.x + f2.x,f1.y + f2.y,f1.z + f2.z, f1.w + f2.w };
    }

    static inline XMFLOAT4 operator-(const DirectX::XMFLOAT4& f1)
    {
        return { -f1.x,-f1.y,-f1.z,-f1.w };
    }

    // XMFLOAT4 “¯m‚Ìˆø‚«Z
    static inline XMFLOAT4 operator-(const DirectX::XMFLOAT4& f1, const DirectX::XMFLOAT4& f2)
    {
        return { f1.x - f2.x,f1.y - f2.y ,f1.z - f2.z,f1.w - f2.w };
    }

    // XMFLOAT4‚ÆFLOAT‚ÌŠ|‚¯Z
    static inline XMFLOAT4 operator*(const DirectX::XMFLOAT4& f1, const float& f2)
    {
        return { f1.x * f2,f1.y * f2,f1.z * f2, f1.w * f2 };
    }

    // XMFLOAT4‚ÆXMFLOAT4‚ÌŠ|‚¯Z
    static inline XMFLOAT4 operator*(const DirectX::XMFLOAT4& f1, const DirectX::XMFLOAT4& f2)
    {
        return { f1.x * f2.x,f1.y * f2.y,f1.z * f2.z, f1.w * f2.w };
    }

    // XMFLOAT4‚ÆFLOAT‚ÌŠ„‚èZ
    static inline XMFLOAT4 operator/(const DirectX::XMFLOAT4& f1, const float& f2)
    {
        return { f1.x / f2,f1.y / f2,f1.z / f2, f1.w / f2 };
    }

    // XMFLOAT4‚ÆXMFLOAT4‚ÌŠ„‚èZ
    static inline XMFLOAT4 operator/(const DirectX::XMFLOAT4& f1, const DirectX::XMFLOAT4& f2)
    {
        return { f1.x / f2.x,f1.y / f2.y,f1.z / f2.z, f1.w / f2.w };
    }

    // XMFLOAT4‚ÆXMFLOAT4‚Ì”äŠr
    static inline bool operator==(const DirectX::XMFLOAT4& f1, const DirectX::XMFLOAT4& f2)
    {
        return f1.x == f2.x && f1.y == f2.y && f1.z == f2.z && f1.w == f2.w;
    }

    // XMFLOAT4‚ÆXMFLOAT4‚Ì”äŠr
    static inline bool operator!=(const DirectX::XMFLOAT4& f1, const DirectX::XMFLOAT4& f2)
    {
        return f1.x != f2.x || f1.y != f2.y || f1.z != f2.z || f1.w != f2.w;
    }

#endif
}

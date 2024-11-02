#pragma once

#include <cmath>
#include <limits>
#include <numbers>
#include <DirectXMath.h>

// ���������Z�p
class Mathf
{
public:
    // ���`���
    static float Lerp(const float& start, const float& end, const  float& time);
    static DirectX::XMFLOAT2 Lerp(const DirectX::XMFLOAT2& start, const DirectX::XMFLOAT2& end, const  float& time);
    static DirectX::XMFLOAT3 Lerp(const DirectX::XMFLOAT3& start, const DirectX::XMFLOAT3& end, const  float& time);
    static DirectX::XMFLOAT4 Lerp(const DirectX::XMFLOAT4& start, const DirectX::XMFLOAT4& end, const  float& time);
    // ���ʕ��
    static DirectX::XMFLOAT4 Slerp(const DirectX::XMFLOAT4& start, const DirectX::XMFLOAT4& end, const  float& time);

    static float Clamp(float value, float min, float max);

    // Source����Target�Ɍ����N�H�[�^�j�I�����Z�o
    static DirectX::XMFLOAT4 LookAt(const DirectX::XMFLOAT3& source, const DirectX::XMFLOAT3& target);

    static float Dot(const DirectX::XMFLOAT2& first, const DirectX::XMFLOAT2& second);
    static float Dot(const DirectX::XMFLOAT3& first, const DirectX::XMFLOAT3& second);
    static float Dot(const DirectX::XMFLOAT4& first, const DirectX::XMFLOAT4& second);
    static float Dot(const DirectX::XMFLOAT3& value);

    static DirectX::XMFLOAT3 Cross(const DirectX::XMFLOAT3& first, const DirectX::XMFLOAT3& second);

    // ���K��
    static DirectX::XMFLOAT2 Normalize(const DirectX::XMFLOAT2& value);
    static DirectX::XMFLOAT3 Normalize(const DirectX::XMFLOAT3& value);
    static DirectX::XMFLOAT4 Normalize(const DirectX::XMFLOAT4& value);
    // ���K�� ( �߂�l:���K���x�N�g���A��2����:�x�N�g���̒��� )
    static DirectX::XMFLOAT2 Normalize(const DirectX::XMFLOAT2& value, float& length);
    static DirectX::XMFLOAT3 Normalize(const DirectX::XMFLOAT3& value, float& length);
    static DirectX::XMFLOAT4 Normalize(const DirectX::XMFLOAT4& value, float& length);

    //����
    static float Length(const DirectX::XMFLOAT2 Vec);
    static float Length(const DirectX::XMFLOAT3 Vec);

    //��̒l�������������ʂ���
    static bool  Equal(const float value1, const float value2);
    static bool  Equal(const DirectX::XMFLOAT2 value1, const DirectX::XMFLOAT2 value2);
    static bool  Equal(const DirectX::XMFLOAT3 value1, const DirectX::XMFLOAT3 value2);

    // �w��͈͂̃����_���l���v�Z
    static float RandomRange(const float& min, const float& max);

    // ������Ԃ� ( -1 or 1 )
    static int GetSign(const float& value);
    static int GetSign(const int& value);

    // �s�񂩂�w��̗�𔲂��o��
    static DirectX::XMFLOAT3 TransformSamplePosition(const DirectX::XMFLOAT4X4& trans);
    static DirectX::XMFLOAT3 TransformSampleRight(const DirectX::XMFLOAT4X4& trans);
    static DirectX::XMFLOAT3 TransformSampleUp(const DirectX::XMFLOAT4X4& trans);
    static DirectX::XMFLOAT3 TransformSampleFront(const DirectX::XMFLOAT4X4& trans);

    //
};

// ���ȉ��̎Q�l : https://qiita.com/HnniTns/items/6e7edc82775a86923cef

// Float�̌덷���l��������r�֐�
template<typename Ty>
// NODISCARD = �Ԃ�l�𖳎� (�Ăяo������A�������Ȃ�����) ����ƌx�����R���p�C���ɏo������
// constexpr = constexpr�L�[���[�h��t���邱�ƂŁA���̊֐��́A�R���p�C�����Ǝ��s���A�����ŌĂяo����֐��ƂȂ�
// �������萔�ł��A���ӂ�constexpr�C�����ꂽ�ϐ��ł���΁A�R���p�C�����ɌĂяo�����
_NODISCARD static inline constexpr bool AdjEqual(const Ty epsilon_num, const Ty num)
{
    // �e���v���[�g�̌^�������������̊m�F
    static_assert(std::is_floating_point<Ty>(), "This type isn't floating point");

    constexpr auto Epsilon{ std::numeric_limits<Ty>::epsilon() };
    auto dis{ std::fabs(epsilon_num - num) };

    return (dis <= Epsilon);
}

namespace DirectX
{
    // FLOAT2
#if(1)
  // XMFLOAT2 ���m�̑����Z
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

    // XMFLOAT2 ���m�̈����Z
    static inline XMFLOAT2 operator-(const DirectX::XMFLOAT2& f1, const DirectX::XMFLOAT2& f2)
    {
        return { f1.x - f2.x,f1.y - f2.y };
    }
    static inline void operator-= (DirectX::XMFLOAT2& f1, const DirectX::XMFLOAT2& f2)
    {
        f1.x -= f2.x;
        f1.y -= f2.y;
    }

    // XMFLOAT2 ���m�̊|���Z
    static inline XMFLOAT2 operator*(const DirectX::XMFLOAT2& f1, const DirectX::XMFLOAT2& f2)
    {
        return { f1.x * f2.x,f1.y * f2.y };
    }
    static inline void operator*= (DirectX::XMFLOAT2& f1, const DirectX::XMFLOAT2& f2)
    {
        f1.x *= f2.x;
        f1.y *= f2.y;
    }

    // XMFLOAT2 ���m�̊���Z
    static inline XMFLOAT2 operator/(const DirectX::XMFLOAT2& f1, const DirectX::XMFLOAT2& f2)
    {
        return { f1.x / f2.x,f1.y / f2.y };
    }
    static inline void operator/= (DirectX::XMFLOAT2& f1, const DirectX::XMFLOAT2& f2)
    {
        f1.x /= f2.x;
        f1.y /= f2.y;
    }

    // XMFLOAT2 ���m�̔�r
    _NODISCARD static inline constexpr bool operator== (const DirectX::XMFLOAT2& f1, const DirectX::XMFLOAT2& f2)
    {
        return (AdjEqual(f1.x, f2.x) && AdjEqual(f1.y, f2.y));
    }
    _NODISCARD static inline constexpr bool operator!= (const DirectX::XMFLOAT2& f1, const DirectX::XMFLOAT2& f2)
    {
        return !(AdjEqual(f1.x, f2.x) && AdjEqual(f1.y, f2.y));
    }

    // XMFLOAT2 ��float �̑����Z
    static inline XMFLOAT2 operator+(const DirectX::XMFLOAT2& f1, const float& num)
    {
        return { f1.x + num,f1.y + num };
    }
    static inline void operator+=(DirectX::XMFLOAT2& f1, const float& num)
    {
        f1.x += num;
        f1.y += num;
    }

    // XMFLOAT2 ��float �̈����Z
    static inline XMFLOAT2 operator-(const DirectX::XMFLOAT2& f1, const float& num)
    {
        return { f1.x - num,f1.y - num };
    }
    static inline void operator-=(DirectX::XMFLOAT2& f1, const float& num)
    {
        f1.x -= num;
        f1.y -= num;
    }

    // XMFLOAT2 ��float �̊|���Z
    static inline XMFLOAT2 operator*(const DirectX::XMFLOAT2& f1, const float& num)
    {
        return { f1.x * num,f1.y * num };
    }
    static inline void operator*=(DirectX::XMFLOAT2& f1, const float& num)
    {
        f1.x *= num;
        f1.y *= num;
    }

    // XMFLOAT2 ��float �̊���Z
    static inline XMFLOAT2 operator/(const DirectX::XMFLOAT2& f1, const float& num)
    {
        return { f1.x / num,f1.y / num };
    }
    static inline void operator/=(DirectX::XMFLOAT2& f1, const float& num)
    {
        f1.x /= num;
        f1.y /= num;
    }

    // XMFLOAT2 ��float �̔�r
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
  // XMFLOAT3 ���m�̑����Z
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

    // XMFLOAT3 ���m�̈����Z
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

    // XMFLOAT3 ���m�̊|���Z
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

    // XMFLOAT3 ���m�̊���Z
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

    // XMFLOAT3 ���m�̔�r
    _NODISCARD static inline constexpr bool operator== (const DirectX::XMFLOAT3& f1, const DirectX::XMFLOAT3& f2)
    {
        return (AdjEqual(f1.x, f2.x) && AdjEqual(f1.y, f2.y));
    }
    _NODISCARD static inline constexpr bool operator!= (const DirectX::XMFLOAT3& f1, const DirectX::XMFLOAT3& f2)
    {
        return !(AdjEqual(f1.x, f2.x) && AdjEqual(f1.y, f2.y));
    }

    // XMFLOAT3 ��float �̑����Z
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

    // XMFLOAT3 ��float �̈����Z
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

    // XMFLOAT3 ��float �̂����Z
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

    // XMFLOAT3 ��float �̂��Z
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

    // XMFLOAT3 ��float �̔�r
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
  // XMFLOAT4��XMFLOAT4�̑����Z
    static inline XMFLOAT4 operator+(const DirectX::XMFLOAT4& f1, const DirectX::XMFLOAT4& f2)
    {
        return { f1.x + f2.x,f1.y + f2.y,f1.z + f2.z, f1.w + f2.w };
    }

    static inline XMFLOAT4 operator-(const DirectX::XMFLOAT4& f1)
    {
        return { -f1.x,-f1.y,-f1.z,-f1.w };
    }

    // XMFLOAT4 ���m�̈����Z
    static inline XMFLOAT4 operator-(const DirectX::XMFLOAT4& f1, const DirectX::XMFLOAT4& f2)
    {
        return { f1.x - f2.x,f1.y - f2.y ,f1.z - f2.z,f1.w - f2.w };
    }

    // XMFLOAT4��FLOAT�̊|���Z
    static inline XMFLOAT4 operator*(const DirectX::XMFLOAT4& f1, const float& f2)
    {
        return { f1.x * f2,f1.y * f2,f1.z * f2, f1.w * f2 };
    }

    // XMFLOAT4��XMFLOAT4�̊|���Z
    static inline XMFLOAT4 operator*(const DirectX::XMFLOAT4& f1, const DirectX::XMFLOAT4& f2)
    {
        return { f1.x * f2.x,f1.y * f2.y,f1.z * f2.z, f1.w * f2.w };
    }

    // XMFLOAT4��FLOAT�̊���Z
    static inline XMFLOAT4 operator/(const DirectX::XMFLOAT4& f1, const float& f2)
    {
        return { f1.x / f2,f1.y / f2,f1.z / f2, f1.w / f2 };
    }

    // XMFLOAT4��XMFLOAT4�̊���Z
    static inline XMFLOAT4 operator/(const DirectX::XMFLOAT4& f1, const DirectX::XMFLOAT4& f2)
    {
        return { f1.x / f2.x,f1.y / f2.y,f1.z / f2.z, f1.w / f2.w };
    }

    // XMFLOAT4��XMFLOAT4�̔�r
    static inline bool operator==(const DirectX::XMFLOAT4& f1, const DirectX::XMFLOAT4& f2)
    {
        return f1.x == f2.x && f1.y == f2.y && f1.z == f2.z && f1.w == f2.w;
    }

    // XMFLOAT4��XMFLOAT4�̔�r
    static inline bool operator!=(const DirectX::XMFLOAT4& f1, const DirectX::XMFLOAT4& f2)
    {
        return f1.x != f2.x || f1.y != f2.y || f1.z != f2.z || f1.w != f2.w;
    }

#endif
}

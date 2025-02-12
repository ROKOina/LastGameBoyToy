#pragma once

namespace chimera
{
	// イプシロン(零に近い任意の微少量)
	static const float	Epsilon = 1e-06f;

    constexpr float Pi = 3.14159265358979323846f;

	//============================================================================
	// RGBカラー
	//----------------------------------------------------------------------------
	class Color3
	{
	public:
		union {
			struct {
				float		r;					// 赤
				float		g;					// 緑
				float		b;					// 青
			};
			float			v[3];
		};

		static const Color3	White;				// 白
		static const Color3	Gray;				// 灰
		static const Color3	Black;				// 黒
		static const Color3	Red;				// 赤
		static const Color3	Green;				// 緑
		static const Color3	Blue;				// 青
		static const Color3	Yellow;				// 黄
		static const Color3	Cyan;				// 青緑
		static const Color3	Magenta;			// 赤紫
		static const Color3	Pink;				// 桃

	public:
		Color3() : Color3(White) {}
		Color3(float r, float g, float b) : r(r), g(g), b(b) {}
		Color3(const Color3& c) : r(c.r), g(c.g), b(c.b) {}
		Color3(const float* f) : r(f[0]), g(f[1]), b(f[2]) {}

	public:
		// 単項演算子
		Color3 operator + () const;
		Color3 operator - () const;
		// 二項演算子
		Color3 operator + (const Color3&) const;
		Color3 operator - (const Color3&) const;
		Color3 operator * (const Color3&) const;
		Color3 operator * (const float) const;
		Color3 operator / (const float) const;
		// 代入演算子
		Color3 operator += (const Color3&);
		Color3 operator -= (const Color3&);
		Color3 operator *= (const Color3&);
		Color3 operator *= (const float);
		// 比較演算子
		bool operator == (const Color3&) const;
		bool operator != (const Color3&) const;

		// シリアライズ
		template<class Archive>
		void serialize(Archive& archive)
		{
			archive(
				CEREAL_NVP(r),
				CEREAL_NVP(g),
				CEREAL_NVP(b)
			);
		}

	};

	//============================================================================
	// RGBAカラー
	//----------------------------------------------------------------------------
	class Color4
	{
	public:
		union {
			struct {
				float		r;					// 赤
				float		g;					// 緑
				float		b;					// 青
				float		a;					// アルファ
			};
			float			v[4];
		};

		static const Color4	White;				// 白
		static const Color4	Gray;				// 灰
		static const Color4	Black;				// 黒
		static const Color4	Red;				// 赤
		static const Color4	Green;				// 緑
		static const Color4	Blue;				// 青
		static const Color4	Yellow;				// 黄
		static const Color4	Cyan;				// 青緑
		static const Color4	Magenta;			// 赤紫
		static const Color4	Pink;				// 桃

	public:
		Color4() : Color4(White) {}
		Color4(float r, float g, float b, float a) : r(r), g(g), b(b), a(a) {}
		Color4(const Color4& c) : r(c.r), g(c.g), b(c.b), a(c.a) {}
		Color4(const float* f) : r(f[0]), g(f[1]), b(f[2]), a(f[3]) {}

	public:
		// 単項演算子
		Color4 operator + () const;
		Color4 operator - () const;
		// 二項演算子
		Color4 operator + (const Color4&) const;
		Color4 operator - (const Color4&) const;
		Color4 operator * (const Color4&) const;
		Color4 operator * (const float) const;
		Color4 operator / (const float) const;
		// 代入演算子
		Color4 operator += (const Color4&);
		Color4 operator -= (const Color4&);
		Color4 operator *= (const Color4&);
		Color4 operator *= (const float);
		// 比較演算子
		bool operator == (const Color4&) const;
		bool operator != (const Color4&) const;

		// DirectXMath
		Color4(DirectX::XMFLOAT4 const& v) : r(v.x), g(v.y), b(v.z), a(v.w) {}

		inline operator DirectX::XMFLOAT4() const
		{
			return DirectX::XMFLOAT4(r, g, b, a);
		}

		// シリアライズ
		template<class Archive>
		void serialize(Archive& archive)
		{
			archive(
				CEREAL_NVP(r),
				CEREAL_NVP(g),
				CEREAL_NVP(b),
				CEREAL_NVP(a)
			);
		}
	};
}

namespace DirectX
{
    template<class Archive>
    void serialize(Archive& archive, XMFLOAT2& v)
    {
        archive(
            cereal::make_nvp("x", v.x),
            cereal::make_nvp("y", v.y)
        );
    }

    template<class Archive>
    void serialize(Archive& archive, XMFLOAT3& v)
    {
        archive(
            cereal::make_nvp("x", v.x),
            cereal::make_nvp("y", v.y),
            cereal::make_nvp("z", v.z)
        );
    }

    template<class Archive>
    void serialize(Archive& archive, XMFLOAT4& v)
    {
        archive(
            cereal::make_nvp("x", v.x),
            cereal::make_nvp("y", v.y),
            cereal::make_nvp("z", v.z),
            cereal::make_nvp("w", v.w)
        );
    }
    template<class Archive>
    void serialize(Archive& archive, XMUINT4& v)
    {
        archive(
            cereal::make_nvp("x", v.x),
            cereal::make_nvp("y", v.y),
            cereal::make_nvp("z", v.z),
            cereal::make_nvp("w", v.w)
        );
    }

    template<class Archive>
    void serialize(Archive& archive, XMFLOAT4X4& m)
    {
        archive(
            cereal::make_nvp("_11", m._11), cereal::make_nvp("_12", m._12),
            cereal::make_nvp("_13", m._13), cereal::make_nvp("_14", m._14),
            cereal::make_nvp("_21", m._21), cereal::make_nvp("_22", m._22),
            cereal::make_nvp("_23", m._23), cereal::make_nvp("_24", m._24),
            cereal::make_nvp("_31", m._31), cereal::make_nvp("_32", m._32),
            cereal::make_nvp("_33", m._33), cereal::make_nvp("_34", m._34),
            cereal::make_nvp("_41", m._41), cereal::make_nvp("_42", m._42),
            cereal::make_nvp("_43", m._43), cereal::make_nvp("_44", m._44)
        );
    }
}

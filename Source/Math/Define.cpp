#include "define.h"

namespace chimera
{
	const Color3		Color3::White = { 1.0f, 1.0f, 1.0f };
	const Color3		Color3::Gray = { 0.5f, 0.5f, 0.5f };
	const Color3		Color3::Black = { 0.0f, 0.0f, 0.0f };
	const Color3		Color3::Red = { 1.0f, 0.0f, 0.0f };
	const Color3		Color3::Green = { 0.0f, 1.0f, 0.0f };
	const Color3		Color3::Blue = { 0.0f, 0.0f, 1.0f };
	const Color3		Color3::Yellow = { 1.0f, 1.0f, 0.0f };
	const Color3		Color3::Cyan = { 0.0f, 1.0f, 1.0f };
	const Color3		Color3::Magenta = { 1.0f, 0.0f, 1.0f };
	const Color3		Color3::Pink = { 1.0f, 0.5f, 0.5f };

	const Color4		Color4::White = { 1.0f, 1.0f, 1.0f, 1.0f };
	const Color4		Color4::Gray = { 0.5f, 0.5f, 0.5f, 1.0f };
	const Color4		Color4::Black = { 0.0f, 0.0f, 0.0f, 1.0f };
	const Color4		Color4::Red = { 1.0f, 0.0f, 0.0f, 1.0f };
	const Color4		Color4::Green = { 0.0f, 1.0f, 0.0f, 1.0f };
	const Color4		Color4::Blue = { 0.0f, 0.0f, 1.0f, 1.0f };
	const Color4		Color4::Yellow = { 1.0f, 1.0f, 0.0f, 1.0f };
	const Color4		Color4::Cyan = { 0.0f, 1.0f, 1.0f, 1.0f };
	const Color4		Color4::Magenta = { 1.0f, 0.0f, 1.0f, 1.0f };
	const Color4		Color4::Pink = { 1.0f, 0.5f, 0.5f, 1.0f };

	//============================================================================
	// RGBÉJÉâÅ[
	//----------------------------------------------------------------------------
	// íPçÄââéZéq
	inline Color3 Color3::operator + () const
	{
		return (*this);
	}

	inline Color3 Color3::operator - () const
	{
		Color3 ret;
		ret.r = -r;
		ret.g = -g;
		ret.b = -b;
		return ret;
	}

	// ìÒçÄââéZéq
	inline Color3 Color3::operator + (const Color3& color) const
	{
		return Color3(
			r + color.r,
			g + color.g,
			b + color.b
		);
	}

	inline Color3 Color3::operator - (const Color3& color) const
	{
		return Color3(
			r - color.r,
			g - color.g,
			b - color.b
		);
	}

	inline Color3 Color3::operator * (const Color3& color) const
	{
		return Color3(
			r * color.r,
			g * color.g,
			b * color.b
		);
	}

	inline Color3 Color3::operator * (const float f) const
	{
		return Color3(
			r * f,
			g * f,
			b * f
		);
	}

	inline Color3 Color3::operator / (const float f) const
	{
		float finv = 1.0f / f;
		return Color3(
			r * finv,
			g * finv,
			b * finv
		);
	}

	// ë„ì¸ââéZéq
	inline Color3 Color3::operator += (const Color3& color)
	{
		r += color.r;
		g += color.g;
		b += color.b;
		return *this;
	}

	inline Color3 Color3::operator -= (const Color3& color)
	{
		r -= color.r;
		g -= color.g;
		b -= color.b;
		return *this;
	}

	inline Color3 Color3::operator *= (const Color3& color)
	{
		r *= color.r;
		g *= color.g;
		b *= color.b;
		return *this;
	}

	inline Color3 Color3::operator *= (const float f)
	{
		r *= f;
		g *= f;
		b *= f;
		return *this;
	}

	// î‰ärââéZéq
	inline bool Color3::operator == (const Color3& color) const
	{
		return (r == color.r && g == color.g && b == color.b);
	}

	inline bool Color3::operator != (const Color3& color) const
	{
		return (r != color.r || g != color.g || b != color.b);
	}

	//============================================================================
	// RGBAÉJÉâÅ[
	//----------------------------------------------------------------------------
	// íPçÄââéZéq
	inline Color4 Color4::operator + () const
	{
		return (*this);
	}

	inline Color4 Color4::operator - () const
	{
		Color4 ret;
		ret.r = -r;
		ret.g = -g;
		ret.b = -b;
		ret.a = -a;
		return ret;
	}

	// ìÒçÄââéZéq
	inline Color4 Color4::operator + (const Color4& color) const
	{
		return Color4(
			r + color.r,
			g + color.g,
			b + color.b,
			a + color.a
		);
	}

	inline Color4 Color4::operator - (const Color4& color) const
	{
		return Color4(
			r - color.r,
			g - color.g,
			b - color.b,
			a - color.a
		);
	}

	inline Color4 Color4::operator * (const Color4& color) const
	{
		return Color4(
			r * color.r,
			g * color.g,
			b * color.b,
			a * color.a
		);
	}

	inline Color4 Color4::operator * (const float f) const
	{
		return Color4(
			r * f,
			g * f,
			b * f,
			a * f
		);
	}

	inline Color4 Color4::operator / (const float f) const
	{
		float finv = 1.0f / f;
		return Color4(
			r * finv,
			g * finv,
			b * finv,
			a * finv
		);
	}

	// ë„ì¸ââéZéq
	inline Color4 Color4::operator += (const Color4& color)
	{
		r += color.r;
		g += color.g;
		b += color.b;
		a += color.a;
		return *this;
	}

	inline Color4 Color4::operator -= (const Color4& color)
	{
		r -= color.r;
		g -= color.g;
		b -= color.b;
		a -= color.a;
		return *this;
	}

	inline Color4 Color4::operator *= (const Color4& color)
	{
		r *= color.r;
		g *= color.g;
		b *= color.b;
		a *= color.a;
		return *this;
	}

	inline Color4 Color4::operator *= (const float f)
	{
		r *= f;
		g *= f;
		b *= f;
		a *= f;
		return *this;
	}

	// î‰ärââéZéq
	inline bool Color4::operator == (const Color4& color) const
	{
		return (r == color.r && g == color.g && b == color.b && a == color.a);
	}

	inline bool Color4::operator != (const Color4& color) const
	{
		return (r != color.r || g != color.g || b != color.b || a != color.a);
	}
}

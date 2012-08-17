#pragma once
#include "CColor4.h"

/**
* Accumulative addition of two colors
*/
_MATH_INLINE_ CColor4& CColor4::operator += ( const CColor4& in )
{
	r += in.r;
	g += in.g;
	b += in.b;
	a += in.a;
	return *this;
}


/**
* Accumulative subtraction of two colors
*/
_MATH_INLINE_ CColor4& CColor4::operator -= ( const CColor4& in )
{
	r -= in.r;
	g -= in.g;
	b -= in.b;
	a -= in.a;
	return *this;
}


/**
* Accumulative multiplication of a color by a color
*/
_MATH_INLINE_ CColor4& CColor4::operator *= ( const CColor4& in )
{
	r *= in.r;
	g *= in.g;
	b *= in.b;
	a *= in.a;
	return *this;
}


/**
* Accumulative division of a color by a color
*/
_MATH_INLINE_ CColor4& CColor4::operator /= ( const CColor4& in )
{
	r /= in.r;
	g /= in.g;
	b /= in.b;
	a /= in.a;
	return *this;
}


/**
* Accumulative multiplication of a color by a scalar
*/
_MATH_INLINE_ CColor4& CColor4::operator *= ( const float& in )
{
	r *= in;
	g *= in;
	b *= in;
	a *= in;
	return *this;
}


/**
* Accumulative division of a color by a scalar
*/
_MATH_INLINE_ CColor4& CColor4::operator /= ( const float& in )
{
	float inv = 1.f / in;
	r *= inv;
	g *= inv;
	b *= inv;
	a *= inv;
	return *this;
}

//------------------------------------------------------------------------------
_MATH_INLINE_ 
CColor4::CColor4( float inR, float inG, float inB, float inA )
:r( inR ), g( inG ), b( inB ), a( inA )
{

}

//------------------------------------------------------------------------------
_MATH_INLINE_ 
CColor4::CColor4( const CColor3& in, float alpha /*= 1.f */ )
{
	r = in.r;
	g = in.g;
	b = in.b;
	a = alpha;
}

//------------------------------------------------------------------------------
_MATH_INLINE_ 
CColor4::CColor4( const CColor& color )
{
	r = float(color.R) / 255;
	g = float(color.G) / 255;
	b = float(color.B) / 255;
	a = float(color.A) / 255;
}

//------------------------------------------------------------------------------
_MATH_INLINE_
CColor4::CColor4( uint color )
{
	b = (float)(color&255) / 255.f;
	color >>= 8;
	g = (float)(color&255) / 255.f;
	color >>= 8;
	r = (float)(color&255) / 255.f;
	color >>= 8;
	a = (float)(color&255) / 255.f;
}

//------------------------------------------------------------------------------
_MATH_INLINE_ void
CColor4::Assign( float inR, float inG, float inB, float inA )
{
	r = inR;
	g = inG;
	b = inB;
	a = inA;
}

//------------------------------------------------------------------------------
_MATH_INLINE_ uint
CColor4::MakeDWord()
{
	unsigned long iA = (int)(a * 255.f ) << 24;
	unsigned long iR = (int)(r * 255.f ) << 16;
	unsigned long iG = (int)(g * 255.f ) << 8;
	unsigned long iB = (int)(b * 255.f );
	return iA | iR | iG | iB;
}

//------------------------------------------------------------------------------
_MATH_INLINE_ uint
CColor4::MakeDWordSafe()
{
	CColor4 temp = *this;
	temp.Sat();
	return temp.MakeDWord();
}

//------------------------------------------------------------------------------
_MATH_INLINE_ void
CColor4::Sat()
{
	limit(r, 0.0f, 1.0f);
	limit(g, 0.0f, 1.0f);
	limit(b, 0.0f, 1.0f);
	limit(a, 0.0f, 1.0f);
}

//------------------------------------------------------------------------------
_MATH_INLINE_ CColor4
CColor4::operator+( const CColor4& rhs ) const
{
	CColor4 newColor;

#ifdef SIMD
	__asm
	{
		mov esi,this
		mov edi,rhs 
		movups xmm0,[esi]
		movups xmm1,[edi]
		addps xmm0,xmm1
		movups newColor,xmm0
	}
#else
	newColor.r = this->r + rhs.r;
	newColor.g = this->g + rhs.g;
	newColor.b = this->b + rhs.b;
	newColor.a = this->a + rhs.a;

#endif
	return newColor;
}

//------------------------------------------------------------------------------
_MATH_INLINE_ CColor4
CColor4::operator-( const CColor4& rhs ) const
{
	CColor4 newColor;
	newColor.r = this->r - rhs.r;
	newColor.g = this->g - rhs.g;
	newColor.b = this->b - rhs.b;
	newColor.a = this->a - rhs.a;
	return newColor;
}

//------------------------------------------------------------------------------
_MATH_INLINE_ CColor4
CColor4::operator*( const CColor4& rhs ) const
{
	CColor4 newColor;
	newColor.r = this->r * rhs.r;
	newColor.g = this->g * rhs.g;
	newColor.b = this->b * rhs.b;
	newColor.a = this->a * rhs.a;
	return newColor;
}

//------------------------------------------------------------------------------
_MATH_INLINE_ CColor4
CColor4::operator*( float ratio ) const
{
	CColor4 newColor;

#ifdef SIMD
	__asm
	{
		mov esi,this
		movups xmm0,[esi]

		movss  xmm1,ratio
		shufps xmm1,xmm1,0x0

		mulps xmm0,xmm1
		movups newColor,xmm0
	}
#else
	newColor.r = this->r * ratio;
	newColor.g = this->g * ratio;
	newColor.b = this->b * ratio;
	newColor.a = this->a * ratio;
#endif
	return newColor;
}

//------------------------------------------------------------------------------
_MATH_INLINE_ CColor4
CColor4::operator/( const CColor4& rhs ) const
{
	CColor4 newColor;
	newColor.r = this->r / rhs.r;
	newColor.g = this->g / rhs.g;
	newColor.b = this->b / rhs.b;
	newColor.a = this->a / rhs.a;
	return newColor;
}

//------------------------------------------------------------------------------
_MATH_INLINE_ CColor4
CColor4::operator/( float ratio ) const
{
	return this->operator*(1.0f / ratio);
}

_MATH_INLINE_ CColor4
CColor4::Max(const CColor4& color1, const CColor4& color2)
{
	CColor4 color;
	color.r = max(color1.r, color2.r);
	color.g = max(color1.g, color2.g);
	color.b = max(color1.b, color2.b);
	color.a = max(color1.a, color2.a);

	return color;
}

_MATH_INLINE_ CColor4
CColor4::Min(const CColor4& color1, const CColor4& color2)
{
	CColor4 color;
	color.r = min(color1.r, color2.r);
	color.g = min(color1.g, color2.g);
	color.b = min(color1.b, color2.b);
	color.a = min(color1.a, color2.a);

	return color;
}


//------------------------------------------------------------------------------
_MATH_INLINE_ bool
CColor4::operator==( const CColor4& rhs ) const
{
	return gIsEqual(this->r, rhs.r) &&
		gIsEqual(this->g, rhs.g) &&
		gIsEqual(this->b, rhs.b) &&
		gIsEqual(this->a, rhs.a);
}

//------------------------------------------------------------------------------
_MATH_INLINE_ bool
CColor4::operator!=( const CColor4& rhs ) const
{
	return !(this->operator==(rhs));
}


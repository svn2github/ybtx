#pragma once
#include "CColor.h"

//------------------------------------------------------------------------------
_MATH_INLINE_
CColor::CColor()
: dwColor(0)
{

}

//------------------------------------------------------------------------------
_MATH_INLINE_
CColor::CColor( uint32 c )
: dwColor(c)
{

}

//------------------------------------------------------------------------------
_MATH_INLINE_
CColor::CColor( uint8 a, uint8 r, uint8 g, uint8 b )
: B(b)
, G(g)
, R(r)
, A(a)
{

}

//------------------------------------------------------------------------------
_MATH_INLINE_
CColor::operator uint32() const
{
	return dwColor;
}

//------------------------------------------------------------------------------
_MATH_INLINE_ const CColor&
CColor::operator=( const CColor& arg )
{
	dwColor = arg.dwColor;
	return *this;
}

//------------------------------------------------------------------------------
_MATH_INLINE_ const CColor
CColor::operator*( float arg ) const
{
	return CColor( (uint8)( A*arg + 0.5f ), (uint8)( R*arg + 0.5f ),
		(uint8)( G*arg + 0.5f ), (uint8)( B*arg + 0.5f ) );
}

//------------------------------------------------------------------------------
_MATH_INLINE_ const CColor
CColor::operator+( const CColor& arg ) const
{
	return CColor( (uint8)min( 255, (int)A + arg.A ), (uint8)min( 255, (int)R + arg.R ),
		(uint8)min( 255, (int)G + arg.G ), (uint8)min( 255, (int)B + arg.B ) );
}

//------------------------------------------------------------------------------
_MATH_INLINE_ const CColor
CColor::operator-( const CColor& arg ) const
{
	return CColor( (uint8)max( 0, (int)A - arg.A ), (uint8)max( 0, (int)R - arg.R ),
		(uint8)max( 0, (int)G - arg.G ), (uint8)max( 0, (int)B - arg.B ) );
}

//------------------------------------------------------------------------------
_MATH_INLINE_ CColor
CColor::Modulate( const CColor& rhs )
{
	return CColor(uint8(this->A / 255.0f * rhs.A + 0.5f),
		uint8(this->R / 255.0f * rhs.R + 0.5f),
		uint8(this->G / 255.0f * rhs.G + 0.5f),
		uint8(this->B / 255.0f * rhs.B + 0.5f));
}

//------------------------------------------------------------------------------
_MATH_INLINE_ uint32
CColor::ToABGR() const
{
	return CColor(A, B, G, R);
}

_MATH_INLINE_ uint32
CColor::ToRGBA() const
{
	return CColor(R, G, B, A);
}

_MATH_INLINE_ uint32
CColor::ToARGB() const
{
	return CColor(A, R, G,B);
}

//------------------------------------------------------------------------------
_MATH_INLINE_ CColor
CColor::Reverse() const
{
	return CColor(A, 255 - R, 255 - G, 255 - B);
}

//------------------------------------------------------------------------------
_MATH_INLINE_ uint8
CColor::ToGray() const
{
	float gray = R * 0.299f + G * 0.587f + B * 0.184f;
	gray = min(255.0f, gray);
	return uint8(gray);
}

//------------------------------------------------------------------------------
_MATH_INLINE_ CColor
CColor::Rand()
{
	return 0xff000000 | CQuickRand::Rand1( 0, 0xffffff );
}

//------------------------------------------------------------------------------
_MATH_INLINE_ CColor
CColor::RandDark()
{
	return CColor(255, CQuickRand::Rand1(0, 128), CQuickRand::Rand1(0, 128), CQuickRand::Rand1(0, 128));
}
//------------------------------------------------------------------------------
_MATH_INLINE_ const CColor
CColor::operator/( float arg ) const
{
	return this->operator*(1.0f / arg);
}

//------------------------------------------------------------------------------
_MATH_INLINE_ const CColor
CColor::Max( const CColor& color1, const CColor& color2 )
{
	CColor newColor;
	newColor.R = max(color1.R, color2.R);
	newColor.G = max(color1.G, color2.G);
	newColor.B = max(color1.B, color2.B);
	newColor.A = max(color1.A, color2.A);
	return newColor;
}

//------------------------------------------------------------------------------
_MATH_INLINE_ const CColor
CColor::Min( const CColor& color1, const CColor& color2 )
{
	CColor newColor;
	newColor.R = min(color1.R, color2.R);
	newColor.G = min(color1.G, color2.G);
	newColor.B = min(color1.B, color2.B);
	newColor.A = min(color1.A, color2.A);
	return newColor;
}

//------------------------------------------------------------------------------
_MATH_INLINE_ const CColor
CColor::operator*( const CColor& rhs ) const
{
	return CColor(uint8(this->A / 255.0f * rhs.A), uint8(this->R / 255.0f * rhs.R),
		uint8(this->G / 255.0f * rhs.G), uint8(this->B / 255.0f * rhs.B));
}

//------------------------------------------------------------------------------
_MATH_INLINE_ CColor&
CColor::operator+=( const CColor& rhs )
{
	this->A = min(255, int(this->A) + rhs.A);
	this->R = min(255, int(this->R) + rhs.R);
	this->G = min(255, int(this->G) + rhs.G);
	this->B = min(255, int(this->B) + rhs.B);
	return *this;
}

//------------------------------------------------------------------------------
_MATH_INLINE_ CColor&
CColor::operator-=( const CColor& rhs )
{
	this->A = max(0, int(this->A) - rhs.A);
	this->R = max(0, int(this->R) - rhs.R);
	this->G = max(0, int(this->G) - rhs.G);
	this->B = max(0, int(this->B) - rhs.B);
	return *this;
}

//------------------------------------------------------------------------------
_MATH_INLINE_ CColor&
CColor::operator*=( const CColor& rhs )
{
	this->A = uint8(this->A / 255.0f * rhs.A + 0.5f);
	this->R = uint8(this->R / 255.0f * rhs.R + 0.5f);
	this->G = uint8(this->G / 255.0f * rhs.G + 0.5f);
	this->B = uint8(this->B / 255.0f * rhs.B + 0.5f);
	return *this;
}

//------------------------------------------------------------------------------
_MATH_INLINE_ CColor&
CColor::operator*=( float ratio )
{
	this->A = uint8(this->A * ratio + 0.5f);
	this->R = uint8(this->R * ratio + 0.5f);
	this->G = uint8(this->G * ratio + 0.5f);
	this->B = uint8(this->B * ratio + 0.5f);
	return *this;
}

//------------------------------------------------------------------------------
_MATH_INLINE_ CColor&
CColor::operator/=( float ratio )
{
	return this->operator*=(1.0f / ratio);
}


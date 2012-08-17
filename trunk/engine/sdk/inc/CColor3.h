/*******************************************************************
*    Desc: RGB color structure, using floats
******************************************************************/

#ifndef _Color3_H
#define _Color3_H

#include "BaseMath.h"
#include "CMathMallocObject.h"

#ifdef _WIN32
#pragma warning(disable : 4201)
#endif

namespace sqr
{
	struct CColor3 : public CMathMallocObject
	{
		union
		{
			struct
			{
				float r, g, b; // Red, Green, and Blue color data
			};
			float c[3];
		};

		CColor3() {}

		CColor3( float inR, float inG, float inB ) :
		r( inR ), g( inG ), b( inB )
		{
		}

		void Assign( float inR, float inG, float inB )
		{
			r = inR;
			g = inG;
			b = inB;
		}

		unsigned long MakeDWord()
		{
			unsigned long iR = (int)(r * 255.f ) << 16;
			unsigned long iG = (int)(g * 255.f ) << 8;
			unsigned long iB = (int)(b * 255.f );
			return 0xff000000 | iR | iG | iB;
		}

		unsigned long MakeDWordSafe()
		{
			CColor3 temp = *this;
			temp.Sat();
			return temp.MakeDWord();
		}

		// if any of the values are >1, cap them.
		void Sat()
		{
			if ( r > 1 )
				r = 1.f;
			if ( g > 1 )
				g = 1.f;
			if ( b > 1 )
				b = 1.f;
			if ( r < 0 )
				r = 0.f;
			if ( g < 0 )
				g = 0.f;
			if ( b < 0 )
				b = 0.f;
		}

		CColor3& operator += ( const CColor3& in );
		CColor3& operator -= ( const CColor3& in );
		CColor3& operator *= ( const float& in );
		CColor3& operator /= ( const float& in );
	};

	//==========--------------------------  CColor3 Operators


	/**
	* Accumulative addition of two colors
	*/
	inline CColor3& CColor3::operator += ( const CColor3& in )
	{
		r += in.r;
		g += in.g;
		b += in.b;
		return *this;
	}


	/**
	* Accumulative subtraction of two colors
	*/
	inline CColor3& CColor3::operator -= ( const CColor3& in )
	{
		r -= in.r;
		g -= in.g;
		b -= in.b;
		return *this;
	}


	/**
	* Accumulative multiplication of a color by a scalar
	*/
	inline CColor3& CColor3::operator *= ( const float& in )
	{
		r *= in;
		g *= in;
		b *= in;
		return *this;
	}


	/**
	* Accumulative division of a color by a scalar
	*/
	inline CColor3& CColor3::operator /= ( const float& in )
	{
		float inv = 1.f / in;
		r *= inv;
		g *= inv;
		b *= inv;
		return *this;
	}


	/**
	* Adds two colors together: ret = a + b
	*/
	inline CColor3 operator+(CColor3 const &a, CColor3 const &b)
	{
		return CColor3
			(
			a.r+b.r,
			a.g+b.g,
			a.b+b.b
			);
	};


	/**
	* Subtracts two colors : ret = a - b
	*/
	inline CColor3 operator-(CColor3 const &a, CColor3 const &b)
	{
		return CColor3
			(
			a.r-b.r,
			a.g-b.g,
			a.b-b.b
			);
	};


	/**
	* Scales a color by a float : ret = a * b
	*/
	inline CColor3 operator*(CColor3 const &a, float const &b)
	{
		return CColor3
			(
			a.r*b,
			a.g*b,
			a.b*b
			);
	};


	/**
	* Scales a color by a float : ret = a * b
	*/
	inline CColor3 operator*(float  const &a, CColor3 const &b)
	{
		return CColor3
			(
			a*b.r,
			a*b.g,
			a*b.b
			);
	};


	/**
	* Divides a color by a float : ret = a / b
	*/
	inline CColor3 operator/(CColor3 const &a, float const &b)
	{
		float inv = 1.f / b;
		return CColor3
			(
			a.r*inv,
			a.g*inv,
			a.b*inv
			);
	};


	/**
	* color Equality, epsilon used due to numerical imprecision
	*/
	inline bool operator==(CColor3 const &a, CColor3 const &b)
	{
		if (fabs(a.r-b.r)<MATH_EPSILON)
			if (fabs(a.g-b.g)<MATH_EPSILON)
				if (fabs(a.b-b.b)<MATH_EPSILON)
					return true;
		return false;
	};


#define COLOR3_BLACK	CColor3( 0.f,0.f,0.f )
#define COLOR3_Gray		CColor3( 0.5f,0.5f,0.5f )
#define COLOR3_White	CColor3( 1.f,1.f,1.f )
#define COLOR3_Red		CColor3( 1.f,0.f,0.f )
#define COLOR3_Green	CColor3( 0.f,1.f,0.f )
#define COLOR3_Blue		CColor3( 0.f,0.f,1.f )
#define COLOR3_Magenta	CColor3( 1.f,0.f,1.f )
#define COLOR3_Cyan		CColor3( 0.f,1.f,1.f )
#define COLOR3_Yellow	CColor3( 1.f,1.f,0.f )
}// end of namespace sqr



#endif //_CColor3_H


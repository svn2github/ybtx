	#pragma once
#include "CMath.h"
#include "CColor3.h"
#include "CColor.h"
#include "CMathMallocObject.h"

#ifdef _WIN32
#pragma warning(disable : 4201)
#endif

namespace sqr
{
struct CColor4 : public CMathMallocObject
{
	union
	{
		struct
		{
			float r, g, b, a; // Red, Green, and Blue color data
		};
		float c[4];
	};

	CColor4() {}
	CColor4( float inR, float inG, float inB, float inA );
	CColor4( const CColor3& in, float alpha = 1.f );
	CColor4(const CColor& color);
	CColor4( uint color );

	void Assign( float inR, float inG, float inB, float inA );

	uint MakeDWord();

	uint MakeDWordSafe();

	// if any of the values are >1, cap them.
	void Sat();

	CColor4& operator += ( const CColor4& in );
	CColor4& operator -= ( const CColor4& in );
	CColor4& operator *= ( const CColor4& in );
	CColor4& operator /= ( const CColor4& in );
	CColor4& operator *= ( const float& in );
	CColor4& operator /= ( const float& in );

	CColor4 operator+(const CColor4& rhs) const;
	CColor4 operator-(const CColor4& rhs) const;
	CColor4 operator*(const CColor4& rhs) const;
	CColor4 operator*(float ratio) const;
	CColor4 operator/(const CColor4& rhs) const;
	CColor4 operator/(float ratio) const;

	bool operator==(const CColor4& rhs) const;
	bool operator!=(const CColor4& rhs) const;
public:
	static CColor4 Max(const CColor4& color1, const CColor4& color2);
	static CColor4 Min(const CColor4& color1, const CColor4& color2);

public:
	static CColor4 Black;
	static CColor4 Gray;
	static CColor4 White;
	static CColor4 Red;
	static CColor4 Green;
	static CColor4 Blue;
	static CColor4 Magenta;
	static CColor4 Cyan;
	static CColor4 Yellow;
};


//==========--------------------------  CColor4 Operators
}// end of namespace sqr


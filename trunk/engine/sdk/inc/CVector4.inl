#pragma once
#include "CVector4.h"

//œÚ¡ø≤Ê≥À
_MATH_INLINE_ void CVector4f::cross(const CVector4f& v1,const CVector4f& v2)
{
#ifdef SIMD
	__asm
	{
		mov esi,v1
			mov edi,v2
			movups xmm0,[esi]
		movups xmm1,[edi]
		movups xmm2,xmm0
			movups xmm3,xmm1

			shufps xmm0,xmm0,0xc9
			shufps xmm1,xmm1,0xd2
			mulps xmm0,xmm1

			shufps xmm2,xmm2,0xd2
			shufps xmm3,xmm3,0xc9
			mulps xmm2,xmm3

			subps xmm0,xmm2
			mov esi,this
			movups [esi],xmm0
	}
#else
	x = v1.y*v2.z - v1.z*v2.y;
	y = v1.z*v2.x - v1.x*v2.z;
	z = v1.x*v2.y - v1.y*v2.x;
#endif
}

_MATH_INLINE_ float CVector4f::Dot(const CVector4f& vec) const
{
	return x * vec.x + y * vec.y + z * vec.z + w * vec.w;
}

_MATH_INLINE_ void CVector4f::operator +=(const CVector4f& v)
{
#ifdef SIMD
	__asm
	{
		mov esi,this
			mov edi,v
			movups xmm0,[esi]
		movups xmm1,[edi]
		addps xmm0,xmm1
			movups [esi],xmm0
	}
#else
	x+=v.x; y+=v.y; z+=v.z; w+=v.w;
#endif
}

_MATH_INLINE_ void CVector4f::operator -=(const CVector4f& v)
{
#ifdef SIMD
	__asm
	{
		mov esi,this
			mov edi,v
			movups xmm0,[esi]
		movups xmm1,[edi]
		subps xmm0,xmm1
			movups [esi],xmm0
	}
#else
	x-=v.x; y-=v.y; z-=v.z; w-=v.w;
#endif
	return;
}

_MATH_INLINE_ void CVector4f::operator *=(const CVector4f& v)
{
#ifdef SIMD
	__asm
	{
		mov esi,this
			mov edi,v
			movups xmm0,[esi]
		movups xmm1,[edi]
		mulps xmm0,xmm1
			movups [esi],xmm0
	}
#else
	x*=v.x; y*=v.y; z*=v.z; w*=v.w;
#endif
}

_MATH_INLINE_ void CVector4f::operator *=(float r)
{
#ifdef SIMD
	__asm
	{
		mov esi,this
			movups xmm0,[esi]

		movss  xmm1,r
			shufps xmm1,xmm1,0x0

			mulps xmm0,xmm1
			movups [esi],xmm0
	}
#else
	x *= r; y *= r; z *= r; w*=r; 
#endif
}

_MATH_INLINE_ CVector4f CVector4f::operator +(const CVector4f& v) const
{
	CVector4f rtV;

#ifdef SIMD
	__asm
	{
		mov esi,this
			mov edi,v
			movups xmm0,[esi]
		movups xmm1,[edi]
		addps xmm0,xmm1
			movups rtV,xmm0
	}
#else
	rtV.x=x+v.x; rtV.y=y+v.y; rtV.z=z+v.z; rtV.w=w+v.w;
#endif
	return rtV;
}

_MATH_INLINE_ CVector4f CVector4f::operator -(const CVector4f& v) const
{
	CVector4f rtV;

#ifdef SIMD
	__asm
	{
		mov esi,this
			mov edi,v
			movups xmm0,[esi]
		movups xmm1,[edi]
		subps xmm0,xmm1
			movups rtV,xmm0
	}
#else
	rtV.x=x-v.x; rtV.y=y-v.y; rtV.z=z-v.z; rtV.w=w-v.w;
#endif
	return rtV;
}

_MATH_INLINE_ CVector4f CVector4f::operator *(const CVector4f& v) const
{
	CVector4f rtV;

#ifdef SIMD
	__asm
	{
		mov esi,this
			mov edi,v 
			movups xmm0,[esi]
		movups xmm1,[edi]
		mulps xmm0,xmm1
			movups rtV,xmm0
	}
#else
	rtV.x=x*v.x; rtV.y=y*v.y; rtV.z=z*v.z; rtV.w=w*v.w;
#endif
	return rtV;
}

_MATH_INLINE_ CVector4f CVector4f::operator *(float r) const
{
	CVector4f rtV;
#ifdef SIMD
	__asm
	{
		mov esi,this
			movups xmm0,[esi]

		movss  xmm1,r
			shufps xmm1,xmm1,0x0

			mulps xmm0,xmm1
			movups rtV,xmm0
	}
#else
	rtV.x=x*r; rtV.y=y*r; rtV.z=z*r; rtV.w=w*r; 
#endif
	return rtV;
}

_MATH_INLINE_ CVector4f CVector4f::operator *(const CMatrix& m) const
{
	CVector4f rtV;

#ifdef SIMD
	__asm
	{
		mov esi,[this]
		mov edi,[m]

		movups xmm0,[esi]
		movups xmm1,xmm0
			movups xmm2,xmm0
			movups xmm3,xmm0

			shufps xmm0,xmm3,0x00
			shufps xmm1,xmm3,0x55
			shufps xmm2,xmm3,0xAA
			shufps xmm3,xmm3,0xFF

			movups xmm4,[edi]
		movups xmm5,[edi+16]
		movups xmm6,[edi+32]
		movups xmm7,[edi+48]

		mulps xmm0,xmm4
			mulps xmm1,xmm5
			mulps xmm2,xmm6
			mulps xmm3,xmm7

			addps xmm0,xmm1
			addps xmm0,xmm2
			addps xmm0,xmm3

			movups rtV,xmm0
	}
#else
	float *f=(float*)&m;
	rtV.x = x*f[0] + y*f[4] + z*f[8] + w*f[12];
	rtV.y = x*f[1] + y*f[5] + z*f[9] + w*f[13];
	rtV.z = x*f[2] + y*f[6] + z*f[10] + w*f[14];
	rtV.w = x*f[3] + y*f[7] + z*f[11] + w*f[15];
#endif
	return rtV;
}

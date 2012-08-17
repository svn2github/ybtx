#pragma once
#include "CMatrix.h"
#include "CMathMallocObject.h"

namespace sqr
{
	//临时类型 还没有和CVector3f对接 接口命名也没有统一
	class CVector4f : public CMathMallocObject
	{
	public:
		union 
		{
			
// #ifdef SIMD
// 		__declspec(align(16)) float v[4];
// #else
		float v[4];
//#endif

		struct  {
			float x,y,z,w;
		};
			
		};

	public:
		CVector4f();
		CVector4f( float x0,float y0,float z0,float w0 = 1.0f );
		
		void setVec( float x0,float y0,float z0,float w0 = 1.0f );
		void setNull(void);
		float length(void)  const;
		void negate(void);
		void normalize(void);
		void cross(  const CVector4f& v1, const CVector4f& v2 );
		float Dot(const CVector4f& vec) const;

		float& operator[](int i);
		void operator +=(const CVector4f& v);
		void operator -=(const CVector4f& v);
		void operator *=(const CVector4f& v);
		void operator *=(const float r);

		CVector4f operator +(const CVector4f& v) const;
		CVector4f operator -(const CVector4f& v) const;
		CVector4f operator *(const CVector4f& v) const;
		CVector4f operator *(const CMatrix& m) const;	
		CVector4f operator *(const float r) const;
	};
}


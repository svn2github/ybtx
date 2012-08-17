#include "stdafx.h"
#include "CVector4.h"
#include "CMatrix.h"
#include "TSqrAllocator.inl"

namespace sqr
{
	CVector4f::CVector4f()
		:x(0),y(0),z(0),w(1)
	{}

	CVector4f::CVector4f(float x0,float y0,float z0,float w0 /* = 1.0f  */)
	{ x=x0; y=y0; z=z0; w=w0; }

	//设置向量
	void CVector4f::setVec(float x0,float y0,float z0,float w0 /* = 1.0f  */)
	{ x=x0; y=y0; z=z0; w=w0; }

	//向量清零
	void CVector4f::setNull(void)
	{ x=y=z=0.0f; w=1.0f; }
	
	float& CVector4f::operator []( int i)
	{ return v[i]; }

	//求向量长度
	float CVector4f::length(void) const
	{ return (float)sqrt(x*x+y*y+z*z); }

	//向量取负
	void CVector4f::negate(void)
	{ x=-x; y=-y; z=-z; }

	//向量单位话
	void CVector4f::normalize(void)
	{
		float len = (float)sqrt(x*x+y*y+z*z);
		if( 0.0f == len )
			return;
		len = 1/len;
		x*=len; y*=len; z*=len;
	}


}

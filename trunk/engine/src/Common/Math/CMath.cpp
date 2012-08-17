#include "stdafx.h"
#include "CMath.h"
#include "BaseHelper.h"
#include "TSqrAllocator.inl"


static const CMath math;
const float CMath::pi = float(PI);
const float CMath::half_pi = float(HALF_PI);
const float CMath::two_pi = float(TWO_PI);
const float CMath::qtr_pi = float(QTR_PI);
const float CMath::sqrt_2 = float(SQRT_2);
const float CMath::sqrt_3 = float(SQRT_3);

uint32 CMath::m_stTrigTableSize = 2048;
float CMath::m_stTrigTableFactor = 0;

float* CMath::m_apSinTable = NULL;
float* CMath::m_apTanTable = NULL;

CMath::CMath()
{
	buildTrigTables();
}

CMath::~CMath()
{
	SafeDelAry(m_apSinTable);
	SafeDelAry(m_apTanTable);
}

//////////////////////////////////////////////////////////////////////////
// 数学函数-sin,con,tan                                                 //
// 值域范围 0*pi ~ 2*pi                                                 //
//////////////////////////////////////////////////////////////////////////
void CMath::buildTrigTables(uint32 trigTableSize /* = 4096 */,bool mp /* = false */)
{
	if (trigTableSize == m_stTrigTableSize )
		return;
	if ( mp && trigTableSize<m_stTrigTableSize)
		return;

	m_stTrigTableSize = trigTableSize;
	m_stTrigTableFactor =  m_stTrigTableSize / CMath::two_pi;

	SafeDelAry(m_apSinTable);
	SafeDelAry(m_apTanTable);

	m_apSinTable = new float[m_stTrigTableSize];
	m_apTanTable = new float[m_stTrigTableSize];

	float angle;
	//trigTableSize--;
	for (uint32 i = 1; i < trigTableSize; i++)
	{
		angle = CMath::two_pi * i / m_stTrigTableSize;
		m_apSinTable[i] = sin(angle);
		m_apTanTable[i] = tan(angle);
	}
	m_apSinTable[0] = m_apSinTable[uint32(pi * m_stTrigTableFactor)] = 0;

	m_apSinTable[uint32(half_pi * m_stTrigTableFactor)] = 1;
	m_apSinTable[uint32((half_pi+pi) * m_stTrigTableFactor)] = -1;

	m_apTanTable[0] = m_apTanTable[uint32(pi * m_stTrigTableFactor)] = 0;
}

float CMath::SinTable(float fRadian)
{
	Ast(m_apSinTable);
	uint32 idx;
	if (fRadian >= 0)
	{
		idx = uint32(fRadian * m_stTrigTableFactor) % m_stTrigTableSize;
	}
	else
	{
		idx = m_stTrigTableSize - (int(-fRadian * m_stTrigTableFactor) % m_stTrigTableSize) - 1;
	}

	return m_apSinTable[idx];
}

float CMath::CosTable(float fRadian)
{
	return CMath::SinTable( fRadian + CMath::half_pi );
}

float CMath::TanTable(float fRadian)
{
	Ast(m_apTanTable);
	int idx = int(fRadian * m_stTrigTableFactor) % m_stTrigTableSize;
	return m_apTanTable[idx];
}

float CMath::Sin(float fRadian)
{
	return float(sin(fRadian));
}

float CMath::Cos(float fRadian)
{
	return float(cos(fRadian));
}

float CMath::Tan(float fRadian)
{
	return float(tan(fRadian));
}


float CMath::ACos( float fValue )
{
	if ( -1.0 < fValue )
	{
		if ( fValue < 1.0 )
			return float(acos(fValue));
		else
			return 0.0;
	}
	else
	{
		return pi;
	}

}

float CMath::ASin( float fValue )
{
	if ( -1.0 < fValue )
	{
		if ( fValue < 1.0 )
			return float(asin(fValue));
		else
			return -half_pi;
	}
	else
	{
		return half_pi;
	}
}

float CMath::Abs( float fValue )
{
	return fabs(fValue);
}

float CMath::Sqr(float fValue)
{
	return fValue*fValue;
}

float CMath::Sqrt(float fValue)
{
#if 0//def SIMD
	__asm
	{
		fld fValue // r0 = r
		fsqrt // r0 = sqrtf( r0 )
	} // returns r0
#else
	return sqrt( fValue );
#endif
}

float CMath::ToRadian( float fDegree )
{
	return fDegree * pi / 180.0f;
}

float CMath::ToDegree( float fRadin )
{
	return fRadin * 180.0f / pi;
}

float CMath::InvSqrt(float fValue)
{
#if 0//def SIMD
	__asm
	{
		fld1 // r0 = 1.f
		fld fValue // r1 = r0, r0 = r
		fsqrt // r0 = sqrtf( r0 )
		fdiv // r0 = r1 / r0
	} // returns r0
#else
	return 1.0f / sqrt(fValue);
#endif
}

bool CMath::IntersectTri( const CVector3f& p0, const CVector3f& p1, const CVector3f& p2, const CVector3f& vRayOrg, const CVector3f& vRayDir, float& u, float& v, float& dist )
{
	// LineP(t) = R + tD	      (R=vRayOrg)	 // 直线的参数方程
	// p0 + u(p1-p0) + v(p2-p0)					 // 三角形所在平面上任意点的重心坐标(u,v)表示
	//	  R + tD = p0 + u(p1-p0) + v(p2-p0)
	// -> R - p0 = -tD + u(p1-p0) + v(p2-p0)
	// -> [-D, p1-p0, p2-p0][t, u, v] = [R - p0] //
	// 利用克莱姆法则求解方程，得到u,v,t(Dist)

	// 求克莱姆法则的分母|A|
	CVector3f e1 = p1 - p0;
	CVector3f e2 = p2 - p0;
	CVector3f pvec = vRayDir.Cross(e2);
	float det = e1.Dot(pvec);

	CVector3f tvec;
	if ( det > 0.0f )
	{
		tvec = vRayOrg - p0;
	}
	else
	{
		tvec = p0 - vRayOrg;
		det  = -det;
	}
	if ( det < 0.0001f )
	{
		return false;
	}

	u = tvec.Dot(pvec);
	if ( u<0.0f || u>det )
	{
		return false;
	}

	CVector3f qvec = tvec.Cross(e1);
	v = vRayDir.Dot(qvec);
	if ( v<0.0f || u+v>det )
	{
		return false;
	}

	dist = e2.Dot(qvec);

	det = 1.0f / det;
	dist *= det;
	u *= det;
	v *= det;

	return true;
}


double CMath::Logbase( double a, double base )
{
	return log(a) / log(base);
}

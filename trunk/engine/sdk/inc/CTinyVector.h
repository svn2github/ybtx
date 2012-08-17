#pragma once
#include "CBitset.h"
#include "MathAlgorithm.h"
#include "CMath.h"
#include "BaseMath.h"
#include "CVector3.h"
#include "CVector2.h"
#include "CQuaternion.h"
#include "CMathMallocObject.h"

namespace sqr
{
struct _CovData : public CMathMallocObject
{
	enum ECD
	{
		eCD_MAX_YAW = 640,
		eCD_MAX_BUF = 261396,
		eCD_WGT_MSK = 0x7ff,
		eCD_MAX_PTC = eCD_WGT_MSK + 1,
		eCD_VEC_BIT = 14,
		eCD_VEC_SGX = 0x2000,
		eCD_VEC_SGY = 0x1000,
		eCD_VEC_SGZ = 0x800,

		eCD_NOR_YAW = 27,
		eCD_NOR_BUF = 512,
		eCD_NOR_SGX = 0x1,
		eCD_NOR_SGY = 0x2,
		eCD_NOR_SGZ = 0x4,
	};

	CVector3f	m_fVecInd[eCD_MAX_BUF];
	uint32		m_uIndStr[eCD_MAX_YAW+1];
	uint32		m_uIndNum[eCD_MAX_YAW+1];
	float		m_fWgtCos[eCD_MAX_PTC];
	float		m_fWgtSin[eCD_MAX_PTC];
	CVector3f	m_fNorInd[eCD_NOR_BUF*8];
	uint32		m_uNorStr[eCD_NOR_YAW+1];
	uint32		m_uNorNum[eCD_NOR_YAW+1];

	_CovData();
	static _CovData _s;
};

class CTinyQuaternion : public CMathMallocObject
{
public:

	uint32 _q;

	CTinyQuaternion() {}

	CTinyQuaternion( const CQuaternion& q );

	operator CQuaternion() const
	{
		_CovData& s = _CovData::_s;

		uint32 uWghInd = _q&_CovData::eCD_WGT_MSK;
		float* pVector = s.m_fVecInd[_q>>_CovData::eCD_VEC_BIT];
		float  fWgtSin = s.m_fWgtSin[uWghInd];
		float  fWgtCos = s.m_fWgtCos[uWghInd];
		float  xVector = _CovData::eCD_VEC_SGX&_q ? -pVector[0]*fWgtSin : pVector[0]*fWgtSin;
		float  yVector = _CovData::eCD_VEC_SGY&_q ? -pVector[1]*fWgtSin : pVector[1]*fWgtSin;
		float  zVector = _CovData::eCD_VEC_SGZ&_q ? -pVector[2]*fWgtSin : pVector[2]*fWgtSin;
		return CQuaternion( fWgtCos, xVector, yVector, zVector );
	}
};

class CTinyVector
{
public:
	uint32 _v;

	CTinyVector() {};
	CTinyVector( const CVector3f& v, float f );

	CVector3f Get( float f ) const;
};

class CTinyWeight : public CMathMallocObject
{
	uint8 _w[4];
public:
	CTinyWeight() {}
	CTinyWeight( float w0, float w1, float w2, float w3 )
	{
		_w[0] = (uint8)( w0*200 + 0.5f );
		_w[1] = (uint8)( w1*200 + 0.5f );
		_w[2] = (uint8)( w2*200 + 0.5f );
		_w[3] = (uint8)( w3*200 + 0.5f );
	}

	float GetWeight( uint32 nIndex ) const
	{
		return _w[nIndex]*0.005f;
	}
};

template< int32 pb = 10, int32 tb = 11 ,int32 mp = 1>
class CTinyVertex : public TBitset< pb*3 + 12/*nb*/ + tb*2*mp, uint32 >
{
	typedef TBitset< pb*3 + 12/*nb*/ + tb*2*mp, uint32 >	Parent_t;
	enum { nb = 12 };
public:

	CTinyVertex() {};
	CTinyVertex( CVector3f v, CVector3f n, CVector2f t );

	CTinyVertex( CVector3f v, CVector3f n, CVector2f t , CVector2f t1);
	CTinyVertex( CVector3f v, CVector3f n, CVector2f t, CVector2f t1, CVector2f t2 );
	int16		GetX()
	{
		return static_cast<int16>(Parent_t::GetBit( 0, pb ));
	}
	int16		GetY()
	{
		return static_cast<int16>(Parent_t::GetBit( pb, pb));
	}
	int16		GetZ()
	{
		return static_cast<int16>(Parent_t::GetBit( pb*2, pb ));
	}
	int16		GetN()
	{
		return static_cast<int16>(Parent_t::GetBit( pb*3, nb ));
	}
	uint16		GetU()
	{
		return static_cast<uint16>(Parent_t::GetBit( pb*3 + nb, tb ));
	}
	uint16		GetV()
	{
		return static_cast<uint16>(Parent_t::GetBit( pb*3 + nb + tb, tb ));
	}
	uint16		GetU1()
	{
		return static_cast<uint16>(Parent_t::GetBit( pb*3 + nb + tb*2,tb));
	}
	uint16		GetV1()
	{
		return static_cast<uint16>(Parent_t::GetBit( pb*3 + nb + tb*3,tb));
	}
	uint16		GetU2()
	{
		return static_cast<uint16>(Parent_t::GetBit( pb*3 + nb + tb*4,tb));
	}
	uint16		GetV2()
	{
		return static_cast<uint16>(Parent_t::GetBit( pb*3 + nb + tb*5,tb));
	}

};



//////////////////////////////////////////////////////////////////////////

inline CTinyQuaternion::CTinyQuaternion( const CQuaternion& q )
{
	_CovData& s = _CovData::_s;

	double w = acos( q.w );
	_q = (uint32)( w*_CovData::eCD_WGT_MSK/MATH_PI );
	if ( _q )
	{
		double sin_w = sin( w );
		uint32 uIndex = GetIndexByVec( q.y/sin_w, q.z/sin_w, _CovData::eCD_MAX_YAW, s.m_uIndStr, s.m_uIndNum );
		_q |= uIndex<<_CovData::eCD_VEC_BIT;

		if ( q.x < 0 )_q |= _CovData::eCD_VEC_SGX;
		if ( q.y < 0 )_q |= _CovData::eCD_VEC_SGY;
		if ( q.z < 0 )_q |= _CovData::eCD_VEC_SGZ;
	}
}

inline CTinyVector::CTinyVector( const CVector3f& v, float f )
{
	_CovData& s = _CovData::_s;

	float fLenght = v.Mag();
	_v = (uint32)( fLenght*f );
	if ( _v )
	{
		uint32 uIndex = GetIndexByVec( v.y/fLenght, v.z/fLenght, _CovData::eCD_MAX_YAW, s.m_uIndStr, s.m_uIndNum );
		_v |= uIndex<<_CovData::eCD_VEC_BIT;

		if ( v.x < 0 )_v |= _CovData::eCD_VEC_SGX;
		if ( v.y < 0 )_v |= _CovData::eCD_VEC_SGY;
		if ( v.z < 0 )_v |= _CovData::eCD_VEC_SGZ;
	}
}

inline CVector3f CTinyVector::Get( float f ) const
{
	_CovData& s = _CovData::_s;

	uint32 uWghInd = _v&_CovData::eCD_WGT_MSK;
	float* pVector = s.m_fVecInd[_v>>_CovData::eCD_VEC_BIT];
	float  fLenght = f*uWghInd;
	float  xVector = _CovData::eCD_VEC_SGX&_v ? -pVector[0]*fLenght : pVector[0]*fLenght;
	float  yVector = _CovData::eCD_VEC_SGY&_v ? -pVector[1]*fLenght : pVector[1]*fLenght;
	float  zVector = _CovData::eCD_VEC_SGZ&_v ? -pVector[2]*fLenght : pVector[2]*fLenght;
	return CVector3f( xVector, yVector, zVector );
}

template< int32 pb, int32 tb,int32 mp>
inline CTinyVertex<pb, tb, mp>::CTinyVertex( CVector3f v, CVector3f n, CVector2f t )
{
	Parent_t::SetBit( pb*0, pb,	uint16( v.x*4 + 0.5f ) );
	Parent_t::SetBit( pb*1, pb,	uint16( v.y*4 + 0.5f ) );
	Parent_t::SetBit( pb*2, pb,	uint16( v.z*4 + 0.5f ) );

	_CovData& s = _CovData::_s;
	CVector3f vNor;
	vNor = n;
	vNor.Normalize();
	double x = abs( vNor.x );
	uint32 uIndex = GetIndexByVec( vNor.y, vNor.z, _CovData::eCD_NOR_YAW, s.m_uNorStr, s.m_uNorNum );
	uint16 _n = 0;
	if ( n.x < 0 )_n |= _CovData::eCD_NOR_SGX;
	if ( n.y < 0 )_n |= _CovData::eCD_NOR_SGY;
	if ( n.z < 0 )_n |= _CovData::eCD_NOR_SGZ;
	_n = _n*_CovData::eCD_NOR_BUF + uIndex;
	Parent_t::SetBit( pb*3, nb, _n );
	Parent_t::SetBit( pb*3 + nb, tb, uint16( t.x*2000 + 0.5f ) );
	Parent_t::SetBit( pb*3 + nb + tb, tb, uint16( t.y*2000 + 0.5f ) );
}

template< int32 pb, int32 tb,int32 mp>
inline CTinyVertex<pb, tb, mp>::CTinyVertex( CVector3f v, CVector3f n, CVector2f t , CVector2f t1 )
{
	Parent_t::SetBit( pb*0, pb,	uint16( v.x*4 + 0.5f ) );
	Parent_t::SetBit( pb*1, pb,	uint16( v.y*4 + 0.5f ) );
	Parent_t::SetBit( pb*2, pb,	uint16( v.z*4 + 0.5f ) );

	_CovData& s = _CovData::_s;
	CVector3f vNor;
	vNor = n;
	vNor.Normalize();
	double x = abs( vNor.x );
	uint32 uIndex = GetIndexByVec( vNor.y, vNor.z, _CovData::eCD_NOR_YAW, s.m_uNorStr, s.m_uNorNum );
	uint16 _n = 0;
	if ( n.x < 0 )_n |= _CovData::eCD_NOR_SGX;
	if ( n.y < 0 )_n |= _CovData::eCD_NOR_SGY;
	if ( n.z < 0 )_n |= _CovData::eCD_NOR_SGZ;
	_n = _n*_CovData::eCD_NOR_BUF + uIndex;
	Parent_t::SetBit( pb*3, nb, _n );
	Parent_t::SetBit( pb*3 + nb, tb, uint16( t.x*2000 + 0.5f ) );
	Parent_t::SetBit( pb*3 + nb + tb, tb, uint16( t.y*2000 + 0.5f ) );

	Parent_t::SetBit( pb*3 + nb +tb*2,tb,uint16(t1.x*2000+0.5f));
	Parent_t::SetBit( pb*3 + nb +tb*3,tb,uint16(t1.y*2000+0.5f));
}

template< int32 pb, int32 tb,int32 mp>
inline CTinyVertex<pb, tb, mp>::CTinyVertex( CVector3f v, CVector3f n, CVector2f t, CVector2f t1, CVector2f t2 )
{
	Parent_t::SetBit( pb*0, pb,	uint16( v.x*4 + 0.5f ) );
	Parent_t::SetBit( pb*1, pb,	uint16( v.y*4 + 0.5f ) );
	Parent_t::SetBit( pb*2, pb,	uint16( v.z*4 + 0.5f ) );

	_CovData& s = _CovData::_s;
	CVector3f vNor;
	vNor = n;
	vNor.Normalize();
	double x = abs( vNor.x );
	uint32 uIndex = GetIndexByVec( vNor.y, vNor.z, _CovData::eCD_NOR_YAW, s.m_uNorStr, s.m_uNorNum );
	uint16 _n = 0;
	if ( n.x < 0 )_n |= _CovData::eCD_NOR_SGX;
	if ( n.y < 0 )_n |= _CovData::eCD_NOR_SGY;
	if ( n.z < 0 )_n |= _CovData::eCD_NOR_SGZ;
	_n = _n*_CovData::eCD_NOR_BUF + uIndex;
	Parent_t::SetBit( pb*3, nb, _n );
	Parent_t::SetBit( pb*3 + nb, tb, uint16( t.x*2000 + 0.5f ) );
	Parent_t::SetBit( pb*3 + nb + tb, tb, uint16( t.y*2000 + 0.5f ) );

	Parent_t::SetBit( pb*3 + nb +tb*2,tb,uint16(t1.x*2000+0.5f));
	Parent_t::SetBit( pb*3 + nb +tb*3,tb,uint16(t1.y*2000+0.5f));

	Parent_t::SetBit( pb*3 + nb +tb*4,tb,uint16(t2.x*2000+0.5f));
	Parent_t::SetBit( pb*3 + nb +tb*5,tb,uint16(t2.y*2000+0.5f));
}
};


//------------------------------------------------------------------------------
//  CFrustum.cpp
//  (C) 2009 ThreeOGCMan
//------------------------------------------------------------------------------
#include "StdAfx.h"
#include "CFrustum.h"
#include "CVector4.h"
#include "CMath.h"
#include "CVector4.inl"

namespace sqr
{
#define FLT_AS_DW(F) (*(DWORD*)&(F))
#define ALMOST_ZERO(F) ((FLT_AS_DW(F) & 0x7f800000L)==0)
#define IS_SPECIAL(F)  ((FLT_AS_DW(F) & 0x7f800000L)==0x7f800000L)

//------------------------------------------------------------------------------
CFrustum::CFrustum()
		: fovY(CMath::qtr_pi)
		, zNear(1.0f)
		, zFar(100.0f)
		, aspect(4.0f / 3.0f)
		, orthoWidth(0.0f)
		, orthoHeight(0.0f)
		, isOrtho(false)
{
	// empty
}

//------------------------------------------------------------------------------
///@todo recompute the following variables
CFrustum::CFrustum( const CMatrix& viewProj )
		: fovY(CMath::qtr_pi)
		, zNear(1.0f)
		, zFar(100.0f)
		, aspect(4.0f / 3.0f)
		, orthoWidth(0.0f)
		, orthoHeight(0.0f)
		, isOrtho(false)
{
	this->Update(viewProj);
}

//------------------------------------------------------------------------------
ClipStatus::Type
CFrustum::Cull( const CAxisAlignedBox& box )
{
	ClipStatus::Type result = ClipStatus::Inside;
	for (index_t i = 0; i < FrustumPlaneNum; ++i)
	{
		if (m_CamPlanes[i].DistTo(box.getPositiveCorner(m_CamPlanes[i].m_Normal)) < 0)
			return ClipStatus::Outside;
		else if (m_CamPlanes[i].DistTo(box.getNegativeCorner(m_CamPlanes[i].m_Normal)) < 0)
			result = ClipStatus::Intersect;
	}
	return result;
}

//------------------------------------------------------------------------------
ClipStatus::Type
CFrustum::Cull( const CSphere& sphere )
{
	ClipStatus::Type result = ClipStatus::Inside;
	for (index_t i = 0; i < FrustumPlaneNum; ++i)
	{
		float distance = m_CamPlanes[i].DistTo(sphere.getCenter());
		if (distance < -sphere.getRadius())
			return ClipStatus::Outside;
		else if (distance < sphere.getRadius())
			result = ClipStatus::Intersect;
	}
	return result;
}

//------------------------------------------------------------------------------
ClipStatus::Type
CFrustum::Cull( const CVector3f& point )
{
	for (index_t i = 0; i < FrustumPlaneNum; ++i)
	{
		if (m_CamPlanes[i].DistTo(point) < 0)
			return ClipStatus::Outside;
	}
	return ClipStatus::Inside;
}


bool
CFrustum::IsVisible( const CAxisAlignedBox& box )
{
	if (box.isNull())
	{
		return false;
	}

	for (uint32 i = 0; i < FrustumPlaneNum; ++i)
	{
		if (m_CamPlanes[i].DistTo(box.getCenter()) < -m_CamPlanes[i].m_Normal.AbsDot(box.getSize()*0.5f))
		{
			return false;
		}
	}
	return true;
}

//------------------------------------------------------------------------------
bool
CFrustum::IsVisible( const CSphere& sphere )
{
	for (uint32 i = 0; i < FrustumPlaneNum; ++i)
	{
		if (m_CamPlanes[i].DistTo(sphere.getCenter()) < -sphere.getRadius())
		{
			return false;
		}
	}
	return true;
}

//------------------------------------------------------------------------------
bool
CFrustum::IsVisible( const CVector3f& point )
{
	for (uint32 i = 0; i < FrustumPlaneNum; ++i)
	{
		if (m_CamPlanes[i].GetSide(point) == p2pBack)
		{
			return false;
		}
	}
	return true;
}

//------------------------------------------------------------------------------
void
CFrustum::Update( const CMatrix& matrix )
{
	//  build a view frustum based on the current view & projection matrices...
	CVector4f column4( matrix._14, matrix._24, matrix._34, matrix._44 );
	CVector4f column1( matrix._11, matrix._21, matrix._31, matrix._41 );
	CVector4f column2( matrix._12, matrix._22, matrix._32, matrix._42 );
	CVector4f column3( matrix._13, matrix._23, matrix._33, matrix._43 );

	CVector4f camPlanes[FrustumPlaneNum];
	camPlanes[0] = column4 - column1;  // left
	camPlanes[1] = column4 + column1;  // right
	camPlanes[2] = column4 - column2;  // bottom
	camPlanes[3] = column4 + column2;  // top
	camPlanes[4] = column4 - column3;  // near
	camPlanes[5] = column4 + column3;  // far
	// ignore near & far plane

	for (index_t p = 0; p < FrustumPlaneNum; ++p)  // normalize the camPlanes
	{
		float dot = camPlanes[p].x*camPlanes[p].x + camPlanes[p].y*camPlanes[p].y + camPlanes[p].z*camPlanes[p].z;
		float sdot = sqrtf(dot);
		dot = (sdot>0.0f)?(1.f / sdot):1.0f;
		camPlanes[p] = camPlanes[p] * dot;
		m_CamPlanes[p] = CPlane( camPlanes[p].x, camPlanes[p].y, camPlanes[p].z, camPlanes[p].w );
	}

	for (index_t i = 0; i < FrustumCornerNum; ++i)  // compute extrema
	{
		const CPlane& p0 = (i&1)?m_CamPlanes[4] : m_CamPlanes[5];
		const CPlane& p1 = (i&2)?m_CamPlanes[3] : m_CamPlanes[2];
		const CPlane& p2 = (i&4)?m_CamPlanes[0] : m_CamPlanes[1];

		PlaneIntersection( &m_PntList[i], &p0, &p1, &p2 );
	}
}

//------------------------------------------------------------------------------
/**
	computes the point where three camPlanes intersect
	returns whether or not the point exists.
 */
inline bool
CFrustum::PlaneIntersection( CVector3f* intersectPt, const CPlane* p0,
                             const CPlane* p1, const CPlane* p2 )
{
	CVector3f n0( p0->m_Normal );
	CVector3f n1( p1->m_Normal );
	CVector3f n2( p2->m_Normal );

	CVector3f n1_n2, n2_n0, n0_n1;
	n1_n2 = n1.Cross(n2);
	n2_n0 = n2.Cross(n0);
	n0_n1 = n0.Cross(n1);
	
	float cosTheta = n0.Dot(n1_n2);//D3DXVec3Dot( &n0, &n1_n2 );

	if ( ALMOST_ZERO(cosTheta) || IS_SPECIAL(cosTheta) )
		return false;

	float secTheta = 1.f / cosTheta;

	n1_n2 = n1_n2 * p0->m_Dist;
	n2_n0 = n2_n0 * p1->m_Dist;
	n0_n1 = n0_n1 * p2->m_Dist;

	*intersectPt = -(n1_n2 + n2_n0 + n0_n1) * secTheta;
	return true;
}
}//namespace sqr

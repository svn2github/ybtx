#include "stdafx.h"
#include "CAxisAlignedBox.h"
#include "float.h"
#include "CVector4.inl"

namespace sqr
{

#define  MAX_BOX_LEN  4096000.0f
const CAxisAlignedBox CAxisAlignedBox::MAX_BOX(
	-MAX_BOX_LEN	,-MAX_BOX_LEN	,-MAX_BOX_LEN,
	 MAX_BOX_LEN	, MAX_BOX_LEN	, MAX_BOX_LEN );

CAxisAlignedBox::CAxisAlignedBox()
		: m_bNull(true)
		, m_bUpdated(false)
{
	m_vMaximum.Init( 0.5, 0.5, 0.5 );
	m_vMinimum.Init( -0.5,-0.5,-0.5 );
}

CAxisAlignedBox::CAxisAlignedBox( const CVector3f& min, const CVector3f& max )
{
	setExtents( min,max );
}

CAxisAlignedBox::CAxisAlignedBox(float mx, float my, float mz,
                                 float Mx, float My, float Mz )
{
	setExtents( mx,my,mz, Mx,My,Mz );
}

void CAxisAlignedBox::mergeBox( const CAxisAlignedBox& rhs )
{
	if (rhs.isNull())
	{
		return;
	}
	else if (m_bNull)
	{
		setExtents( rhs.getMinimum(), rhs.getMaximum() );
	}
	else
	{
#ifdef SIMD
		__asm
		{
			mov		eax,[rhs]
			mov		ecx,[this]

			movss	xmm0,[ecx]CBaseAABB.m_vMinimum.x
			movss	xmm1,[ecx]CBaseAABB.m_vMinimum.y
			movss	xmm2,[ecx]CBaseAABB.m_vMinimum.z
			minss	xmm0,[eax]CBaseAABB.m_vMinimum.x
			minss	xmm1,[eax]CBaseAABB.m_vMinimum.y
			minss	xmm2,[eax]CBaseAABB.m_vMinimum.z
			movss	[ecx]CBaseAABB.m_vMinimum.x,xmm0
			movss	[ecx]CBaseAABB.m_vMinimum.y,xmm1
			movss	[ecx]CBaseAABB.m_vMinimum.z,xmm2

			movss	xmm0,[ecx]CBaseAABB.m_vMaximum.x
			movss	xmm1,[ecx]CBaseAABB.m_vMaximum.y
			movss	xmm2,[ecx]CBaseAABB.m_vMaximum.z
			maxss	xmm0,[eax]CBaseAABB.m_vMaximum.x
			maxss	xmm1,[eax]CBaseAABB.m_vMaximum.y
			maxss	xmm2,[eax]CBaseAABB.m_vMaximum.z
			movss	[ecx]CBaseAABB.m_vMaximum.x,xmm0
			movss	[ecx]CBaseAABB.m_vMaximum.y,xmm1
			movss	[ecx]CBaseAABB.m_vMaximum.z,xmm2
		}
#else
		CVector3f temp = rhs.getMaximum();
		if (temp.x > m_vMaximum.x) m_vMaximum.x = temp.x;
		if (temp.y > m_vMaximum.y) m_vMaximum.y = temp.y;
		if (temp.z > m_vMaximum.z) m_vMaximum.z = temp.z;
		temp = rhs.getMinimum();
		if (temp.x < m_vMinimum.x) m_vMinimum.x = temp.x;
		if (temp.y < m_vMinimum.y) m_vMinimum.y = temp.y;
		if (temp.z < m_vMinimum.z) m_vMinimum.z = temp.z;
#endif
		m_bUpdated = false;
	}
}

void CAxisAlignedBox::mergeBox( const CVector3f& point )
{
	if (m_bNull)
		setExtents(point, point);
	else
	{
#ifdef SIMD
		__asm
		{
			mov		eax,[point]
			mov		ecx,[this]

			movss	xmm3,[eax]CVector3f.x
			movss	xmm4,[eax]CVector3f.y
			movss	xmm5,[eax]CVector3f.z

			movss	xmm0,[ecx]CBaseAABB.m_vMinimum.x
			movss	xmm1,[ecx]CBaseAABB.m_vMinimum.y
			movss	xmm2,[ecx]CBaseAABB.m_vMinimum.z
			minss	xmm0,xmm3
			minss	xmm1,xmm4
			minss	xmm2,xmm5
			movss	[ecx]CBaseAABB.m_vMinimum.x,xmm0
			movss	[ecx]CBaseAABB.m_vMinimum.y,xmm1
			movss	[ecx]CBaseAABB.m_vMinimum.z,xmm2

			movss	xmm0,[ecx]CBaseAABB.m_vMaximum.x
			movss	xmm1,[ecx]CBaseAABB.m_vMaximum.y
			movss	xmm2,[ecx]CBaseAABB.m_vMaximum.z
			maxss	xmm0,xmm3
			maxss	xmm1,xmm4
			maxss	xmm2,xmm5
			movss	[ecx]CBaseAABB.m_vMaximum.x,xmm0
			movss	[ecx]CBaseAABB.m_vMaximum.y,xmm1
			movss	[ecx]CBaseAABB.m_vMaximum.z,xmm2
		}
#else
		if (point.x > m_vMaximum.x) m_vMaximum.x = point.x;
		if (point.y > m_vMaximum.y) m_vMaximum.y = point.y;
		if (point.z > m_vMaximum.z) m_vMaximum.z = point.z;

		if (point.x < m_vMinimum.x) m_vMinimum.x = point.x;
		if (point.y < m_vMinimum.y) m_vMinimum.y = point.y;
		if (point.z < m_vMinimum.z) m_vMinimum.z = point.z;
#endif
		m_bUpdated = false;
	}
}

void CAxisAlignedBox::transformBox(const CMatrix& matrix)
{
	if (m_bNull)
		return;
	_updateCorners();
	CVector4f max,min;
	CVector4f temp = m_vCorners[0] * matrix;
	max = min = temp;

	for ( int i=1; i<8; ++i)
	{
		temp = m_vCorners[i] * matrix;

		if (temp.x > max.x) max.x = temp.x;
		if (temp.y > max.y) max.y = temp.y;
		if (temp.z > max.z) max.z = temp.z;

		if (temp.x < min.x) min.x = temp.x;
		if (temp.y < min.y) min.y = temp.y;
		if (temp.z < min.z) min.z = temp.z;
	}
	m_vMinimum.x = min.x;
	m_vMinimum.y = min.y;
	m_vMinimum.z = min.z;

	m_vMaximum.x = max.x;
	m_vMaximum.y = max.y;
	m_vMaximum.z = max.z;

	m_bUpdated = false;
}

void CAxisAlignedBox::transformBoxWithProj( const CMatrix& matrix)
{

	if (m_bNull)
		return;
	_updateCorners();
	
	CVector4f max,min;
	CVector4f temp = m_vCorners[0] * matrix;

	if(gIsZero(temp.w))
		temp.setVec(0.0f,0.0f,0.0f,1.0f);
	else
	{
		temp.x /=temp.w;
		temp.y /=temp.w;
		temp.z /=temp.w;
		temp.w = 1.0f;
	}
	max = min = temp;

	for ( int i=1; i<8; ++i)
	{
		temp = m_vCorners[i] * matrix;

		if(gIsZero(temp.w))
			temp.setVec(0.0f,0.0f,0.0f,1.0f);
		else
		{
			temp.x /=temp.w;
			temp.y /=temp.w;
			temp.z /=temp.w;
			temp.w = 1.0f;
		}

		if (temp.x > max.x) max.x = temp.x;
		if (temp.y > max.y) max.y = temp.y;
		if (temp.z > max.z) max.z = temp.z;

		if (temp.x < min.x) min.x = temp.x;
		if (temp.y < min.y) min.y = temp.y;
		if (temp.z < min.z) min.z = temp.z;
	}
	m_vMinimum.x = min.x;
	m_vMinimum.y = min.y;
	m_vMinimum.z = min.z;

	m_vMaximum.x = max.x;
	m_vMaximum.y = max.y;
	m_vMaximum.z = max.z;

	m_bUpdated = false;
}

bool CAxisAlignedBox::intersects(const CAxisAlignedBox& b2) const
{
	if (m_bNull || b2.isNull())
		return false;

	CVector3f temp = b2.getMinimum();
	if (m_vMaximum.x < temp.x)
		return false;
	if (m_vMaximum.y < temp.y)
		return false;
	if (m_vMaximum.z < temp.z)
		return false;

	temp = b2.getMaximum();
	if (m_vMinimum.x > temp.x)
		return false;
	if (m_vMinimum.y > temp.y)
		return false;
	if (m_vMinimum.z > temp.z)
		return false;

	return true;
}

bool CAxisAlignedBox::intersects(const CPlane& p) const
{
	if (m_bNull)
		return false;
	_updateCorners();
	enumPoint2Plane lastSide = p.GetSide(*(CVector3f*)&(m_vCorners[0]));
	for (int corner = 1; corner < 8; ++corner )
	{
		if (p.GetSide(*(CVector3f*)&(m_vCorners[corner])) != lastSide)
			return true;
	}
	return false;
}

bool CAxisAlignedBox::intersects(const CSphere& s) const
{
	if (m_bNull)
		return false;

	const CVector3f& center = s.getCenter();
	float radius = s.getRadius();

	if (center.x < m_vMinimum.x &&
	        m_vMinimum.x - center.x > radius)
	{
		return false;
	}
	if (center.x > m_vMaximum.x &&
	        center.x  - m_vMaximum.x > radius)
	{
		return false;
	}

	if (center.y < m_vMinimum.y &&
	        m_vMinimum.y - center.y > radius)
	{
		return false;
	}
	if (center.y > m_vMaximum.y &&
	        center.y  - m_vMaximum.y > radius)
	{
		return false;
	}

	if (center.z < m_vMinimum.z &&
	        m_vMinimum.z - center.z > radius)
	{
		return false;
	}
	if (center.z > m_vMaximum.z &&
	        center.z  - m_vMaximum.z > radius)
	{
		return false;
	}
	return true;

}

bool CAxisAlignedBox::intersects(const CVector3f& v) const
{
	return(v.x >= m_vMinimum.x  &&  v.x <= m_vMaximum.x  &&
	       v.y >= m_vMinimum.y  &&  v.y <= m_vMaximum.y  &&
	       v.z >= m_vMinimum.z  &&  v.z <= m_vMaximum.z);
}

//------------------------------------------------------------------------------
bool
CAxisAlignedBox::intersects( float* distance, const CVector3f&origin, const CVector3f& direction ) const
{
	CPlane sides[6] = {	   CPlane( 1, 0, 0,-getMinimum().x),
	                       CPlane(-1, 0, 0, getMaximum().x),
	                       CPlane( 0, 1, 0,-getMinimum().y),
	                       CPlane( 0,-1, 0, getMaximum().y),
	                       CPlane( 0, 0, 1,-getMinimum().z),
	                       CPlane( 0, 0,-1, getMaximum().z)
	                  };

	*distance = 0.f;  // safe initial value
	CVector3f hitPt = origin;

	bool inside = false;

	for ( int i=0; (i<6) && !inside; i++ )
	{
		float cosTheta = sides[i].GetNormal().Dot(direction);//D3DXPlaneDotNormal( &sides[i], &direction );
		float dist = sides[i].DistTo(origin);//D3DXPlaneDotCoord ( &, &origin );

		//  if we're nearly intersecting, just punt and call it an intersection
		if ( gIsZero(dist) ) return true;
		//  skip nearly (&actually) parallel rays
		if ( gIsZero(cosTheta) ) continue;
		//  only interested in intersections along the ray, not before it.
		*distance = -dist / cosTheta;
		if ( *distance < 0.f ) continue;

		hitPt = (*distance)*direction + origin;

		inside = true;

		for ( int j=0; (j<6) && inside; j++ )
		{
			if ( j==i )
				continue;
			float d = sides[j].DistTo(hitPt);//D3DXPlaneDotCoord( &sides[j], &hitPt );

			inside = ((d + 0.00015) >= 0.f);
		}
	}

	return inside;
}

//----------------------------------------------------------------
void CAxisAlignedBox::_updateCorners(void) const
{
	if (m_bUpdated)
		return;
	// 注意:本系统使用左手坐标系:
	// Z方向近平面 由最小点 顶点0 开始 逆时针围绕组成平面
	// Z方向远平面 由最大点 顶点4 开始 逆时针围绕组成平面
	m_vCorners[0].x = m_vMinimum.x;
	m_vCorners[0].y = m_vMinimum.y;
	m_vCorners[0].z = m_vMinimum.z;
	m_vCorners[1].x = m_vMaximum.x;
	m_vCorners[1].y = m_vMinimum.y;
	m_vCorners[1].z = m_vMinimum.z;
	m_vCorners[2].x = m_vMaximum.x;
	m_vCorners[2].y = m_vMaximum.y;
	m_vCorners[2].z = m_vMinimum.z;
	m_vCorners[3].x = m_vMinimum.x;
	m_vCorners[3].y = m_vMaximum.y;
	m_vCorners[3].z = m_vMinimum.z;

	m_vCorners[4].x = m_vMaximum.x;
	m_vCorners[4].y = m_vMaximum.y;
	m_vCorners[4].z = m_vMaximum.z;
	m_vCorners[5].x = m_vMinimum.x;
	m_vCorners[5].y = m_vMaximum.y;
	m_vCorners[5].z = m_vMaximum.z;
	m_vCorners[6].x = m_vMinimum.x;
	m_vCorners[6].y = m_vMinimum.y;
	m_vCorners[6].z = m_vMaximum.z;
	m_vCorners[7].x = m_vMaximum.x;
	m_vCorners[7].y = m_vMinimum.y;
	m_vCorners[7].z = m_vMaximum.z;

	m_vCorners[0].w = 1.0f;
	m_vCorners[1].w = 1.0f;
	m_vCorners[2].w = 1.0f;
	m_vCorners[3].w = 1.0f;
	m_vCorners[4].w = 1.0f;
	m_vCorners[5].w = 1.0f;
	m_vCorners[6].w = 1.0f;
	m_vCorners[7].w = 1.0f;

	m_bUpdated = true;
}

}


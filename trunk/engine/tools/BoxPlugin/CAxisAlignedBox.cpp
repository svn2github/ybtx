#include "stdafx.h"

#include "CAxisAlignedBox.h"
#include "CSphere.h"
#include "CPlane.h"
namespace sqr
{
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
		CVector3f temp = rhs.getMaximum();
		if (temp.x > m_vMaximum.x) m_vMaximum.x = temp.x;
		if (temp.y > m_vMaximum.y) m_vMaximum.y = temp.y;
		if (temp.z > m_vMaximum.z) m_vMaximum.z = temp.z;
		temp = rhs.getMinimum();
		if (temp.x < m_vMinimum.x) m_vMinimum.x = temp.x;
		if (temp.y < m_vMinimum.y) m_vMinimum.y = temp.y;
		if (temp.z < m_vMinimum.z) m_vMinimum.z = temp.z;
		m_bUpdated = false;
	}
}

void CAxisAlignedBox::mergeBox( const CVector3f& point )
{
	if (m_bNull)
		setExtents(point, point);
	else
	{
		if (point.x > m_vMaximum.x) m_vMaximum.x = point.x;
		if (point.y > m_vMaximum.y) m_vMaximum.y = point.y;
		if (point.z > m_vMaximum.z) m_vMaximum.z = point.z;

		if (point.x < m_vMinimum.x) m_vMinimum.x = point.x;
		if (point.y < m_vMinimum.y) m_vMinimum.y = point.y;
		if (point.z < m_vMinimum.z) m_vMinimum.z = point.z;

		m_bUpdated = false;
	}
}

void CAxisAlignedBox::transformBox(const CMatrix& matrix)
{
	if (m_bNull)
		return;
	_updateCorners();
	CVector3f max,min,temp;
	temp = m_vCorners[0] * matrix;
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
	m_vMinimum = min;
	m_vMaximum = max;
	
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
	enumPoint2Plane lastSide = p.GetSide(m_vCorners[0]);
	for (int corner = 1; corner < 8; ++corner )
	{
		if (p.GetSide(m_vCorners[corner]) != lastSide)
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

//----------------------------------------------------------------
void CAxisAlignedBox::_updateCorners(void) const
{
	if (m_bUpdated)
		return;
	// 注意:本系统使用左手坐标系:
	// Z方向近平面 由最小点 顶点0 开始 逆时针围绕组成平面
	// Z方向远平面 由最大点 顶点4 开始 逆时针围绕组成平面
	m_vCorners[0] = m_vMinimum;
	m_vCorners[1].x = m_vMaximum.x;
	m_vCorners[1].y = m_vMinimum.y;
	m_vCorners[1].z = m_vMinimum.z;
	m_vCorners[2].x = m_vMaximum.x;
	m_vCorners[2].y = m_vMaximum.y;
	m_vCorners[2].z = m_vMinimum.z;
	m_vCorners[3].x = m_vMinimum.x;
	m_vCorners[3].y = m_vMaximum.y;
	m_vCorners[3].z = m_vMinimum.z;

	m_vCorners[4] = m_vMaximum;
	m_vCorners[5].x = m_vMinimum.x;
	m_vCorners[5].y = m_vMaximum.y;
	m_vCorners[5].z = m_vMaximum.z;
	m_vCorners[6].x = m_vMinimum.x;
	m_vCorners[6].y = m_vMinimum.y;
	m_vCorners[6].z = m_vMaximum.z;
	m_vCorners[7].x = m_vMaximum.x;
	m_vCorners[7].y = m_vMinimum.y;
	m_vCorners[7].z = m_vMaximum.z;

	m_bUpdated = true;
}
}
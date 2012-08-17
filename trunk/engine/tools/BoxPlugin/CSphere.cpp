#include "stdafx.h"

#include "CAxisAlignedBox.h"
#include "CSphere.h"
#include "CPlane.h"

namespace sqr
{
	CSphere::CSphere()
		:m_rRadius(1.0),m_vCenter(0.0f,0.0f,0.0f)
	{}

	CSphere::CSphere(const CVector3f& center, const float radius)
		:m_vCenter(center),m_rRadius(radius)
	{}

	float CSphere::getRadius(void) const
	{ return m_rRadius; }

	CVector3f CSphere::getCenter(void) const
	{ return m_vCenter; }

	void CSphere::setRadius(float radius)
	{ m_rRadius =  radius; }

	void CSphere::setCenter(CVector3f& center)
	{ m_vCenter = center; }

	bool CSphere::intersects(const CAxisAlignedBox& b2) const
	{
		return b2.intersects(*this);
	}

	bool CSphere::intersects(const CPlane& p) const
	{
		return (fabs(p.m_Normal.Dot(m_vCenter))<=m_rRadius);
	}

	bool CSphere::intersects(const CSphere& s) const
	{
		return (s.getCenter() - m_vCenter).Mag() <=
			(s.getRadius() + m_rRadius);
	}

	bool CSphere::intersects(const CVector3f& v) const
	{
		return ((v - m_vCenter).Mag()<= m_rRadius);
	}
}
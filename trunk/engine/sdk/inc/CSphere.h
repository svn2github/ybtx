#pragma once

#include "CPlane.h"
#include "CMathMallocObject.h"

namespace sqr
{
class CSphere : public CMathMallocObject
{
protected:
	float		m_rRadius;
	CVector3f	m_vCenter;
public:
	CSphere();
	CSphere( const CVector3f& center, const float radius );

	float	getRadius(void) const;
	const CVector3f& getCenter(void) const;
	void	setRadius( float radius );
	void	setCenter( const CVector3f& center );

	bool intersects(const CSphere& s) const;
	bool intersects(const CPlane& p) const;
	bool intersects(const CVector3f& v) const;
	bool intersects(CVector3f& result, const CVector3f& ray_pos, const CVector3f& ray_dir);
};

//------------------------------------------------------------------------------
inline
CSphere::CSphere()
:m_rRadius(1.0),m_vCenter(0.0f,0.0f,0.0f)
{}

//------------------------------------------------------------------------------
inline
CSphere::CSphere(const CVector3f& center, const float radius)
:m_rRadius(radius),m_vCenter(center)
{}

//------------------------------------------------------------------------------
inline
float CSphere::getRadius(void) const
{
	return m_rRadius;
}

//------------------------------------------------------------------------------
inline
const CVector3f& CSphere::getCenter(void) const
{
	return m_vCenter;
}

//------------------------------------------------------------------------------
inline
void CSphere::setRadius(float radius)
{
	m_rRadius =  radius;
}

//------------------------------------------------------------------------------
inline
void CSphere::setCenter(const CVector3f& center)
{
	m_vCenter = center;
}

//------------------------------------------------------------------------------
inline
bool CSphere::intersects(const CPlane& p) const
{
	return (fabs(p.m_Normal.Dot(m_vCenter))<=m_rRadius);
}

//------------------------------------------------------------------------------
inline
bool CSphere::intersects(const CSphere& s) const
{
	return (s.getCenter() - m_vCenter).Mag() <=
		(s.getRadius() + m_rRadius);
}

//------------------------------------------------------------------------------
inline
bool CSphere::intersects(const CVector3f& v) const
{
	return ((v - m_vCenter).Mag()<= m_rRadius);
}

inline
bool CSphere::intersects( CVector3f& result, const CVector3f& ray_pos, const CVector3f& ray_dir )
{
	CVector3f	v1, v2;
	float		l  = 0.0f;
	float		d2 = 0.0f;

	v1 = m_vCenter - ray_pos;
	l  = v1.Dot(ray_dir);
	v2 = l * ray_dir;

	d2 = m_rRadius*m_rRadius - (v1-v2).MagSqr();

	if ( d2 < 0.0f )
		return false;

	// check where the sphere contain the ray pos
	if ( intersects(ray_pos) )
	{
		l += CMath::Sqrt(d2);
	}
	else
	{
		l -= CMath::Sqrt(d2);
	}

	result = ray_pos + l*ray_dir;

	return true;
}

}

#ifndef _Sphere_H_
#define _Sphere_H_

#ifndef __MATH_VECTOR3_H__
#include "CVector3.h"
#endif

#include "CAxisAlignedBox.h"
#include "CPlane.h"

namespace sqr
{
class CAxisAlignedBox;
class CPlane;
class CSphere
{
protected:
	float		m_rRadius;
	CVector3f	m_vCenter;
public:
	CSphere();
	CSphere( const CVector3f& center, const float radius );

	float	getRadius(void) const;
	CVector3f getCenter(void) const;
	void	setRadius( float radius );
	void	setCenter( CVector3f& center );

	bool intersects(const CAxisAlignedBox& b2) const;
	bool intersects(const CSphere& s) const;
	bool intersects(const CPlane& p) const;
	bool intersects(const CVector3f& v) const;
};
}

#endif //_Sphere_H_
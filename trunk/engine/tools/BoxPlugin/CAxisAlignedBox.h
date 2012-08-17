#ifndef _AxisAlignedBox_H_
#define _AxisAlignedBox_H_

//#ifndef __MATH_VECTOR3_H__
#include "CVector3.h"
#include "CMatrix.h"
//#endif

#include "CPlane.h"
#include "CSphere.h"


namespace sqr
{
class sqr::CPlane;
class sqr::CSphere;
struct CBaseAABB //用于存储的结构
{
	CVector3f m_vMinimum;
	CVector3f m_vMaximum;
};

class CAxisAlignedBox : public CBaseAABB
{
protected:
	/*
	正方体的顶点定义：
	// 注意:本系统使用左手坐标系:
	// Z方向近平面 由最小点 顶点0 开始 逆时针围绕组成平面
	// Z方向远平面 由最大点 顶点4 开始 逆时针围绕组成平面
	       5-----4
	      /|    /|
		 / |   / |
	    3-----2  |
	y	|  6--|--7
	|  z| /   | /
	| /	|/    |/
	|/	0-----1
	O-----x
	关于正方体 顶点定义说明
	Z方向近平面 0 - 3 四个定点
	Z方向远平面 4 - 7 四个定点

	Z方向近平面 顶点 N+4 (N为0 - 3) 可得远平面 对顶点
	Z方向远平面 顶点 N-4 (N为4 - 7) 可得近平面 对顶点

	顶点0 为 最小点
	顶点4 为 最大点
	*/
	mutable CVector3f m_vCorners[8];
	bool m_bNull;
	mutable bool m_bUpdated;
public:
	CAxisAlignedBox();
	CAxisAlignedBox( const CVector3f& min, const CVector3f& max );
	CAxisAlignedBox(	float mx, float my, float mz,
	                 float Mx, float My, float Mz );

	void setNull(void);
	bool isNull(void) const;

	const CVector3f& getMinimum(void) const;
	const CVector3f& getMaximum(void) const;

	void setMinimum( const CVector3f& vec );
	void setMinimum( float x, float y, float z );
	void setMaximum( const CVector3f& vec );
	void setMaximum( float x, float y, float z );

	void setExtents( const CVector3f& min, const CVector3f& max );
	void setExtents( float mx, float my, float mz,
	                 float Mx, float My, float Mz );

	const CVector3f* getAllCorners(void) const;
	const CVector3f getCenter(void) const;
	const CVector3f getSize(void) const;

	void mergeBox( const CAxisAlignedBox& rhs );
	void mergeBox( const CVector3f& point );
	void transformBox( const CMatrix& matrix);
	void scaleBox(const CVector3f& s);

	bool intersects(const CAxisAlignedBox& b2) const;
	bool intersects(const CSphere& s) const;
	bool intersects(const CPlane& p) const;
	bool intersects(const CVector3f& v) const;

	inline bool operator != (const CAxisAlignedBox& box) const;
	inline bool operator == (const CAxisAlignedBox& box) const;
protected:

	void _updateCorners(void) const;
private:
};

inline bool CAxisAlignedBox::operator != (const CAxisAlignedBox& box) const
{
	return m_vMinimum!=box.m_vMinimum||m_vMaximum!=box.m_vMaximum;
}

inline bool CAxisAlignedBox::operator == (const CAxisAlignedBox& box) const
{
	return m_vMinimum==box.m_vMinimum&&m_vMaximum==box.m_vMaximum;
}

inline void CAxisAlignedBox::setNull(void)
{
	m_bNull = true;
	m_bUpdated = false;
}

inline bool CAxisAlignedBox::isNull(void) const
{
	return m_bNull;
}

inline const CVector3f& CAxisAlignedBox::getMaximum(void) const
{
	return m_vMaximum;
}

inline const CVector3f& CAxisAlignedBox::getMinimum(void) const
{
	return m_vMinimum;
}

inline void CAxisAlignedBox::setMaximum( const CVector3f& vec )
{
	m_bNull = false;
	m_bUpdated = false;
	m_vMaximum = vec;
}

inline void CAxisAlignedBox::setMaximum(float x, float y, float z)
{
	m_bNull = false;
	m_bUpdated = false;
	m_vMaximum.x = x;
	m_vMaximum.y = y;
	m_vMaximum.z = z;
}

inline void CAxisAlignedBox::setMinimum( const CVector3f& vec )
{
	m_bNull = false;
	m_bUpdated = false;
	m_vMinimum = vec;
}

inline void CAxisAlignedBox::setMinimum(float x, float y, float z)
{
	m_bNull = false;
	m_bUpdated = false;
	m_vMinimum.x = x;
	m_vMinimum.y = y;
	m_vMinimum.z = z;
}


inline void CAxisAlignedBox::setExtents( const CVector3f& min, const CVector3f& max )
{
	m_bNull = false;
	m_bUpdated = false;
	m_vMinimum = min;
	m_vMaximum = max;
}

inline void CAxisAlignedBox::setExtents( float mx, float my, float mz,
        float Mx, float My, float Mz )
{
	m_bNull = false;
	m_bUpdated = false;
	m_vMinimum.x = mx;
	m_vMinimum.y = my;
	m_vMinimum.z = mz;
	m_vMaximum.x = Mx;
	m_vMaximum.y = My;
	m_vMaximum.z = Mz;
}

inline const CVector3f* CAxisAlignedBox::getAllCorners(void) const
{
	_updateCorners();
	return (const CVector3f*)(m_vCorners);
}

inline const CVector3f CAxisAlignedBox::getCenter(void) const
{
	return CVector3f((m_vMinimum+m_vMaximum) * 0.5f );
}

inline const CVector3f  CAxisAlignedBox::getSize(void) const
{
	return CVector3f( m_vMaximum-m_vMinimum );
}

inline void CAxisAlignedBox::scaleBox(const CVector3f& s)
{
	m_vMaximum *= s;
	m_vMinimum *= s;
}
}

#endif //_AxisAlignedBox_H
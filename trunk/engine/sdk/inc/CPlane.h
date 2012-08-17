#pragma once
/*
*	CPlane结构描述了3维空间中的平面
*  平面到点的距离是有正负的，和法矢的方向一致为正，否则为负值
if the normal pointing away from the origin, which causes d to be negative
if the normal pointing towards the origin, so d is positive.
if the plane goes through the origin, d is zero

*  平面的方程是	：	X*m_Normal.x + Y*Normal.y + Z*Normal.z + m_Dist = 0;
平面的法矢已经单位化了

有几个函数没有完成，比如平面分割线段，平面分割多边形，两平面的交线，三平面的交点
*/

#include "CMatrix.h"
#include "CMathMallocObject.h"

namespace sqr
{
	// Defines the three possible locations of a point in relation to a plane
	enum enumPoint2Plane
	{
		p2pFront,
		p2pBack,
		p2pCoplanar
	};

	class CPlane : public CMathMallocObject
	{
	public:
		//	平面的法向矢量，是单位矢量
		CVector3f		m_Normal;
		// negative of the signed distance from the origin to the plane
		float		m_Dist;

		//=======================================================================
		// 构造函数
		//=======================================================================
		CPlane();

		/*!
		\param  x    n_x
		\param  y    n_y
		\param  z    n_z
		\param  d   the negative of the signed distance from
		the origin to the plane

		Construct a plane with a unit normal  (x,y,z)  and
		the negative of the signed distance from the origin
		(0,0,0) to the plane.
		*/
		CPlane(float x, float y, float z, float d);

		/*!
		\param  n   a unit normal
		\param  P   a point on the plane

		Construct a plane with a unit normal n and
		the negative of the signed distance from the origin
		(0,0,0) to the plane.
		*/
		CPlane(const CVector3f& n, float d);

		/*!
		\param  n   a unit normal
		\param  P   a point on the plane

		Construct a plane with a unit normal  and a point on the plane.
		*/
		CPlane( const CVector3f& n,  const CVector3f& p );


		// 用平面上的三个点构造一个平面, 三个点按照顺时针排列，左手规则
		CPlane(const CVector3f& a, const CVector3f& b, const CVector3f& c);

		//===================================================================================
		// 初始化函数
		//===================================================================================
		/*!
		\param  n   a unit normal
		\param  P   a point on the plane

		Initialize a plane with a unit normal and
		a point on the plane.
		*/
		void Init(const CVector3f& n, const CVector3f& P);

		/*!
		\param  n   a unit normal
		\param  P   a point on the plane

		Initialize a plane with a unit normal  n and
		the negative of the signed distance from the origin
		(0,0,0) to the plane.
		*/
		void Init(const CVector3f& n, float d);


		// return the unit normal of the plane
		const CVector3f& GetNormal() const;

		// return the negative of the signed distance from the origin (0,0,0) to the plane
		float GetDist() const;

		/*!
		\param  p   a 3D point
		\return     the signed distance from the plane to the point p

		Compute the signed distance from the plane to the point p
		点到平面的距离，有正负之分
		*/
		float DistTo(const CVector3f& p) const;

		/*!
		\return     a plane with the values \f$ \left{ -{\bf n}, -dist \right} \f$

		Negate the values of m_Normal and m_Dist.  This creates a plane
		that is a reflection about the origin of the first (so they are parallel),
		and whose normal points the opposite direction.
		*/
		const CPlane operator-();

		// 计算一个点在平面的前面，后面，还是在平面上
		enumPoint2Plane GetSide( const CVector3f& point ) const; // it was between EP and -EP

		// 计算一个线段确定的直线和平面的交点
		CVector3f Split( const CVector3f& a, const CVector3f& b ) const;

		/// 矩阵变换
		const CPlane& Transform(CMatrix matrix);
		/// 单位化
		const CPlane& Normalize();

		//******************************************************************************
		/*! \fn     bool TVector3<T>::Mirror(const CPlane& Plane)
		*   \brief  矢量关于一个平面的镜像
		*           将自身关于一个传入的平面做镜像, 自身的值将改变
		*   \param  const CPlane& Plane 平面
		*   \return bool	false 表明点在平面上
		*******************************************************************************/
		bool Mirror(CVector3f& vec);

		/// 是否为合法平面(abcd都为空时无意义)
		bool IsValid() const;
	};

	//------------------------------------------------------------------------------
	inline bool
		CPlane::IsValid() const
	{
		return !(m_Normal.IsZero() && gIsZero(m_Dist));
	}

	//------------------------------------------------------------------------------
	// return the unit normal of the plane
	inline const CVector3f&
		CPlane::GetNormal() const
	{
		return m_Normal;
	}

	//------------------------------------------------------------------------------
	// return the negative of the signed distance from the origin (0,0,0) to the plane
	inline float
		CPlane::GetDist() const
	{
		return m_Dist;
	}
	//------------------------------------------------------------------------------
	inline const CPlane&
		CPlane::Transform( CMatrix matrix )
	{
		Normalize();
		float a = m_Normal.x * matrix._11 + m_Normal.y * matrix._21 + m_Normal.z * matrix._31 + m_Dist * matrix._41;
		float b = m_Normal.x * matrix._12 + m_Normal.y * matrix._22 + m_Normal.z * matrix._32 + m_Dist * matrix._42;
		float c = m_Normal.x * matrix._13 + m_Normal.y * matrix._23 + m_Normal.z * matrix._33 + m_Dist * matrix._43;
		float d = m_Normal.x * matrix._14 + m_Normal.y * matrix._24 + m_Normal.z * matrix._34 + m_Dist * matrix._44;
		m_Normal.x = a;
		m_Normal.y = b;
		m_Normal.z = c;
		m_Dist = d;

		return *this;
	}

	//------------------------------------------------------------------------------
	inline const CPlane&
		CPlane::Normalize()
	{
		float tLen = m_Normal.normalize();
		m_Dist = tLen > 0.0f ? m_Dist / tLen : 0.0f;
		return *this;
	}
	//------------------------------------------------------------------------------
	inline CPlane::CPlane() : m_Dist(0) {}

	//------------------------------------------------------------------------------
	inline CPlane::CPlane(float x, float y, float z, float d) : m_Normal(x, y, z),m_Dist(d){}

	//------------------------------------------------------------------------------
	inline CPlane::CPlane(const CVector3f& n, float d) : m_Normal(n), m_Dist(d){}

	//------------------------------------------------------------------------------
	inline CPlane::CPlane( const CVector3f& n,  const CVector3f& p ) : m_Normal(n), m_Dist( -n.Dot(p)){}


	// 用平面上的三个点构造一个平面, 三个点按照顺时针排列，左手规则
	//------------------------------------------------------------------------------
	inline CPlane::CPlane(const CVector3f& a, const CVector3f& b, const CVector3f& c)
	{
		m_Normal	= (b-a).Cross(c-a);	// 叉积
		m_Normal.Normalize();
		m_Dist		= -(m_Normal.Dot(a));		// 点积
	}


	//------------------------------------------------------------------------------
	inline void CPlane::Init(const CVector3f& n, const CVector3f& P) 
	{
		m_Normal	= n;
		m_Dist		= n.Dot(P);
	}

	//------------------------------------------------------------------------------
	inline void CPlane::Init(const CVector3f& n, float d) 
	{
		m_Normal = n;
		m_Dist = d;
	}


	//------------------------------------------------------------------------------
	inline float CPlane::DistTo(const CVector3f& p) const 
	{
		return m_Normal.Dot(p) + m_Dist;
	}

	//------------------------------------------------------------------------------
	inline const CPlane CPlane::operator-() 
	{
		return CPlane(-m_Normal, -m_Dist);
	}

	// 计算一个点在平面的前面，后面，还是在平面上
	//------------------------------------------------------------------------------
	inline enumPoint2Plane CPlane::GetSide( const CVector3f& point ) const
	{
		float dp = m_Normal.Dot(point) + m_Dist;

		if(dp > MATH_EPSILON)
		{
			return p2pFront;
		}
		if(dp < -MATH_EPSILON )
		{
			return p2pBack;
		}

		return p2pCoplanar; // it was between EP and -EP
	}

	//------------------------------------------------------------------------------
	inline CVector3f CPlane::Split( const CVector3f& a, const CVector3f& b ) const
	{
		float aDot = (a.Dot(m_Normal));
		float bDot = (b.Dot(m_Normal));

		float scale = ( -m_Dist - aDot) / ( bDot - aDot );

		return a + (scale * (b - a));
	}

	//------------------------------------------------------------------------------
	inline bool CPlane::Mirror(CVector3f& vec)
	{
		float		fDist		= DistTo(vec);//GetPointDist(vec);     //GetPointDist()怎么不见了？－lily
		CVector3f	vNormal		= GetNormal();	

		if ( /*gBMIsZero(fDist)*/ gIsZero(fDist) )//gBMIsZero()怎么不见了？－lily
			return false;
		else if (fDist > 0.0f)
		{
			vNormal = -vNormal;
		}

		vec = vec + vNormal*fDist*2.0f;
		return true;
	}

} // sqr

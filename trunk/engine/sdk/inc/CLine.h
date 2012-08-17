/*
*	Line(线段)的定义,线段具有一个起点和一个终点(不同于Ray),和一个相关的方向
*/
#ifndef _MATH_LINE_H_
#define _MATH_LINE_H_
#include "CMathMallocObject.h"

namespace sqr
{
class CLine : public CMathMallocObject
{
public:
	CLine();
	~CLine();
	CLine(const CLine& lnOrg);					//!< 拷贝构造函数
	CLine operator =(const CLine& lnRight);		//!< 赋值构造函数

	inline CVector3f GetStart()	const
	{
		return m_vStart;    //!< 得到起点
	}
	inline CVector3f GetEnd()		const
	{
		return m_vEnd;    //!< 得到终点
	}
	inline CVector3f GetDir()		const
	{
		return m_vDir;    //!< 得到方向
	}
	inline float	  GetLen()		const
	{
		return m_fLen;    //!< 得到长度
	}

	//! 由起点和终点构造线段
	void MakeLineFrom2Point(const CVector3f& vStart, const CVector3f& vEnd);
	//! 由方向起点和一个距离构造一个线段
	void MakeLineFromDir(const CVector3f& vDir, const CVector3f& vStart,
	                     float fLen);
	//! 由一个方向，源点，线段起点距源点的距离，线段终点距源点的距离构造线段
	void MakeLineFronDir(const CVector3f& vDir, const CVector3f& vOrg,
	                     float fStartLen, float fEndLen);
	//!矢量关于一个线段的镜像
	bool	Mirror(CVector3f& vec);

protected:
	CVector3f	m_vDir;			//!< 线段的方向，内部做了归一化
	CVector3f	m_vStart;		//!< 线段的起点
	CVector3f	m_vEnd;			//!< 线段的终点
	float		m_fLen;			//!< 线段的长度
};


CLine::CLine() : m_vStart(0.0f, 0.0f, 0.0f), m_vEnd(1.0f, 0.0f, 0.0f),
		m_vDir(1.0f, 0.0f, 0.0f), m_fLen(1.0f)
{

}

CLine::~CLine()
{

}

//! 拷贝构造函数
CLine::CLine(const CLine& lnOrg)
{
	m_vStart	= lnOrg.m_vStart;
	m_vEnd		= lnOrg.m_vEnd;
	m_vDir		= lnOrg.m_vDir;
	m_fLen		= lnOrg.m_fLen;
}

//! 赋值构造函数
CLine CLine::operator = (const CLine& lnRight)
{
	m_vStart	= lnRight.m_vStart;
	m_vEnd		= lnRight.m_vEnd;
	m_vDir		= lnRight.m_vDir;
	m_fLen		= lnRight.m_fLen;

	return *this;
}

//******************************************************************************
/*! \fn     void CLine::MakeLineFrom2Point(const CVector3f& vStart,
const CVector3f& vEnd)
*   \brief  由起点和终点构造线段
*
*           内部校验传入参数的正确性，如果参数不正确构造一缺省线段,方向矢量
*			经过了归一化
*   \param  const CVector3f& vStart 线段的起点
*   \param  const CVector3f& vEnd   线段的终点
*   \return void
*   \sa     MakeLineFromDir
*******************************************************************************/
void CLine::MakeLineFrom2Point(const CVector3f& vStart, const CVector3f& vEnd)
{
	if (vStart.IsEqual(vEnd))
	{
		m_vStart.Init(0.0f, 0.0f, 0.0f);
		m_vEnd.Init(1.0f, 0.0f, 0.0f);
		m_vDir.Init(1.0f, 0.0f, 0.0f);
		m_fLen = 1.0f;
		return;
	}
	m_vDir		= vStart - vEnd;
	m_fLen		= m_vDir.Mag();
	m_vDir		= m_vDir/m_fLen;
	m_vStart	= vStart;
	m_vEnd		= vEnd;
}

//******************************************************************************
/*! \fn     void CLine::MakeLineFromDir(const CVector3f& vDir,
const CVector3f& vStart, float fLen)
*   \brief  由方向起点和一个距离构造一个线段
*
*           由方向起点和一个距离构造一个线段,内部校验传入参数的正确性，如果参数
*			不正确构造一缺省线段,方向矢量经过了归一化.
*   \param  const CVector3f& vDir		线的方向
*   \param  const CVector3f& vStart	线的起点
*   \param  float fLen					线的长度
*   \return void
*   \sa     MakeLineFrom2Point, MakeLineFromDir
*******************************************************************************/
void CLine::MakeLineFromDir(const CVector3f& vDir, const CVector3f& vStart,
                            float fLen)
{
	if (m_vDir.IsZero() || gIsZero(fLen))
	{
		m_vStart.Init(0.0f, 0.0f, 0.0f);
		m_vEnd.Init(1.0f, 0.0f, 0.0f);
		m_vDir.Init(1.0f, 0.0f, 0.0f);
		m_fLen	= 1.0f;
		return;
	}
	m_vDir		= vDir;
	m_vDir.Normalize();
	m_vStart	= vStart;
	m_fLen		= fLen;
	m_vEnd		= vStart + m_vDir*fLen;
}

//******************************************************************************
/*! \fn     void CLine::MakeLineFronDir(cont CVector3f& vDir,
const CVector3f& vOrg, float fStartLen, float fEndLen);
*   \brief  由一个方向，源点，线段起点距源点的距离，线段终点距源点的距离构造线段
*
*
*   \param  cont CVector3f&	vDir 方向
*	\param  const CVector3f&	vOrg 源点
*   \param  float fStartLen 线段起点距源点的距离
*   \param  float fEndLen	线段终点距源点的距离
*   \return void
*   \sa     MakeLineFrom2Point, MakeLineFromDir
*******************************************************************************/
void CLine::MakeLineFronDir(const CVector3f& vDir, const CVector3f& vOrg,
                            float fStartLen, float fEndLen)
{
	if (m_vDir.IsZero() || gIsZero(fStartLen-fEndLen))
	{
		m_vStart.Init(0.0f, 0.0f, 0.0f);
		m_vEnd.Init(1.0f, 0.0f, 0.0f);
		m_vDir.Init(1.0f, 0.0f, 0.0f);
		m_fLen	= 1.0f;
		return;
	}

	m_vDir		= vDir;
	m_vDir.Normalize();
	m_fLen		= (float)fabs((float)(fEndLen - fStartLen));

	m_vStart	= vOrg + m_vDir*fStartLen;
	m_vEnd		= vOrg + m_vDir*fEndLen;
}

//******************************************************************************
/*! \fn     bool TVector3<T>::Mirror(const CLine& Line)
*   \brief  矢量关于一个线段的镜像
*           将自身关于一个传入的线段做镜像, 自身的值将改变
*   \param  const CLine& Line 线段
*   \return bool false 表明点在线段上
*******************************************************************************/
bool CLine::Mirror(CVector3f& vec)
{
	CVector3f v1,v2,v3,st;
	CVector3f dir = GetDir();
	float t;

	st = Line.GetStart();
	v1 = vec - st;
	v2 = v1.Cross(dir);
	if (v2.IsZero())
		return false;
	v3 = v2.Cross(dir);
	t =	v3.Dot(st - vec) * 2 / v3.SquareMag();

	vec += v3 * t;

	return true;

}
} // end of namespace sqr
#endif

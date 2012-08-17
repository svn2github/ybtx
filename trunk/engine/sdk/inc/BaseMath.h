#pragma once
//******************************************************************************
/*! \file       BaseMath.h
*   \brief      基本的数学功能
*
*				大致上包括 浮点数相关功能，弧度角度相关功能，随机数相关功能，
*				矢量相关功能，数学常量的定义，调用这个模块中的功能必须包含
*				Math.lib
*   \version	1.0
*   \date
*   \sa			CVector3, KLELine, KLEPlane, KLEQuaternion
*   \todo		需要优化，速度是第一位的
*******************************************************************************/

#include <cmath>
#include <cstdlib>
#include "BaseTypes.h"

namespace sqr
{
using namespace std;
//-----------------------------------------------------------------------------
// Useful Math constants
//-----------------------------------------------------------------------------
#define		MATH_PI				3.14159265358979323846		// Pi
#define		MATH_2_PI			6.28318530717958623200		// 2 * Pi
#define		MATH_PI_DIV_2		1.57079632679489655800		// Pi / 2
#define		MATH_PI_DIV_4		0.78539816339744827900		// Pi / 4
#define		MATH_PI_DIV_8		0.39269908169872415480		// Pi / 8
#define		MATH_PI_DIV_16		0.19634954084936207740		// Pi / 16


#define		MATH_INV_PI			0.31830988618379069122		// 1 / Pi
#define		MATH_DEG2RAD		0.01745329251994329547		// Degrees to Radians
#define		MATH_RAD2DEG		57.29577951308232286465		// Radians to Degrees
#define		MATH_FLOATHUGE		1.0e+38f					// Huge number for float
#define		MATH_EPSILON		1.0e-5f						// Tolerance for float


//******************************************************************************
/*! \fn     inline bool gIsZero( float a, float fTol = MATH_EPSILON)
*   \brief  判断一个浮点数是否等于零
*
*   \param  float a		要判断的浮点数
*   \param  float fTol	精度，缺省为MATH_EPSILON
*   \return bool		等于零返回true, 否则返回false
*   \sa     gIsEqual
*******************************************************************************/
inline bool gIsZero( float a, float fTol = MATH_EPSILON)
{
	return ( a <= 0.0f ) ? ( a >= -fTol ) : ( a <= fTol );
}

//******************************************************************************
/*! \fn     inline bool gIsEqual(float a, float b, float fTol = MATH_EPSILON)
*   \brief  判断两个浮点数是否相等
*
*   \param  float a		要比较的浮点数
*   \param  float b		要比较的浮点数
*   \param  float fTol 	精度，缺省为MATH_EPSILON
*   \return bool		相等返回true, 否则返回false
*   \sa     gIsZero
*******************************************************************************/
inline bool gIsEqual(float a, float b, float /*fTol*/ = MATH_EPSILON)
{
	if ((float)fabs((float)(a - b)) > MATH_EPSILON)
		return false;
	else
		return true;
}

//******************************************************************************
/*! \fn     inline int gRandRage(int nMin, int nMax)
*   \brief  产生一个范围内的随机数,包括nMin 和 nMax
*
*   \param  int nMin	范围的下界
*   \param  int nMax	范围的上届
*   \return int			求得的随机数
*   \sa     gRandRage(int nMin, int nMax, int nExclude)
*******************************************************************************/
inline int gRandRage(int nMin, int nMax)
{
	int nSwap;
	if (nMin > nMax)
	{
		nSwap	= nMax;
		nMax	= nMin;
		nMin	= nSwap;
	}

	return nMin + rand()%(nMax - nMin + 1);
}

//!
//!
//******************************************************************************
/*! \fn     inline int gRandRage(int nMin, int nMax, int nExclude)
*   \brief  产生一个范围内的随机数, 包括nMin 和 nMax.但不包括 nExclude,
*			nExclude 必须在nMin, nMax中
*
*   \param  int nMin		范围的下界
*   \param  int nMax		范围的上届
*   \param  int nExclude	不包括的值
*   \return inline int		求得的随机数
*   \sa     gRandRage(int nMin, int nMax)
*******************************************************************************/
inline int gRandRage(int nMin, int nMax, int nExclude)
{
	int nTemp;

	if (nMin > nMax)
	{
		nTemp	= nMax;
		nMax	= nMin;
		nMin	= nTemp;
	}

	if ( nExclude < nMin || nExclude > nMax)
	{
		nTemp = gRandRage(nMin, nMax);
	}
	else
	{
		nTemp	= gRandRage(nExclude + 1, nMax + nExclude - 1);
		nTemp	= (nTemp > nMax) ? (nTemp - nMax) : nTemp;
	}
	return nTemp;
}

//******************************************************************************
/*! \fn     inline float gRandOneZero()
*   \brief  产生一个在0.0到1.0之间均匀分布的随机浮点数
*
*   \return inline float 返回的随机浮点数
*******************************************************************************/
inline float gRandOneZero()
{
	return ((float)rand())/((float)RAND_MAX+1.0f);
}

//******************************************************************************
/*! \fn     inline float gCos(float angle)
*   \brief  求一个角度值的余弦值
*
*   \param  float angle 传入的角度值
*   \return float		求得的余弦值
*   \sa     gSin, Deg2Rad, Rad2Deg
*******************************************************************************/
inline float gCos(float angle)
{
	return (float) cos(MATH_PI * ((double) angle / 180));
}


//******************************************************************************
/*! \fn     inline float gSin(float angle)
*   \brief  求一个角度值的正弦值
*
*   \param  float angle		传入的角度值
*   \return float			求得的正弦值
*   \sa     gBMCos, gBMDeg2Rad, gBMRad2Deg
*******************************************************************************/
inline float gSin(float angle)
{
	return (float) sin(MATH_PI * ((double) angle / 180));
}

//******************************************************************************
/*! \fn     inline float gDeg2Rad(float fDeg)
*   \brief  度数向弧度转换
*
*   \param  float fDeg		要转换的角度值
*   \return float			转换后的结果
*   \sa     gSin, gCos, gRad2Deg
*******************************************************************************/
inline float gDeg2Rad(float fDeg)
{
	return fDeg*(float)MATH_DEG2RAD;
}


//******************************************************************************
/*! \fn     inline float gRad2Deg(float fRad)
*   \brief  弧度向度数转换
*
*   \param  float fRad		要转换的弧度值
*   \return inline float	转换后的结果
*   \sa     gSin, gCos, gDeg2Rad
*******************************************************************************/
inline float gRad2Deg(float fRad)
{
	return fRad*(float)MATH_RAD2DEG;
}


//******************************************************************************
/*! \fn     inline int32 gLog2(int32 n)
*   \brief  求整数的2的对数
*
*   \param  int32 n			要求的整数
*   \return inline int32	整数的2的对数
*******************************************************************************/
inline uint8 gLog2(uint32 n)
{
	uint8 r = 0;
	for ( ; n > 1; n = n>>1 )
		r++;
	return r;
}


//******************************************************************************
/*! \fn     T TriInterpolate( T& T0, T& T1, T& T2, T& T3, float x, float y )
*   \brief  在T0~T3之间进行三角形插值
*
*   \param  T& T0, T& T1, T& T2, T& T3		插值参数		T1 _______________________ T3 ( x = 1.0f, y = 1.0f )
*			float x, float y				插值位置		|\					  	  |
*															|	 \				  	  |
*															|		  \ 		  	  |
*															|			   \	  	  |
*															|( x = 0, y = 0 )   \	  |
*															T0 ______________________\|T2
*
*
*   \return T								插值结果
*******************************************************************************/
template< class T >
T TriInterpolate( const T& T0, const T& T1, const T& T2, const T& T3, float x, float y )
{
	if ( y > 1.0f - x )
	{
		x -= 1.0f;
		y -= 1.0f;
		return (T)( ( T3 - T1 )*x + ( T3 - T2 )*y + T3 );
	}
	else
	{
		return (T)( ( T2 - T0 )*x + ( T1 - T0 )*y + T0 );
	}
}

// ***********************************************************************
// 限制数的上下限
// 返回 a>=min && a<= max
// ***********************************************************************
template<class T>
void limit( T& a, const T& min, const T& max )
{
	if ( a < min )
		a = min;
	else if ( a > max )
		a = max;
}

template<class T>
T limit2(const T& a, const T& min, const T& max)
{
	if ( a < min)
		return min;
	else if ( a > max)
		return max;

	return a;
}

template<class T>
T limit3(const T& a, const T& min)
{
	return a < min ? min : a;
}

template<class T>
T limit4(const T& a, const T& max)
{
	return a > max ? max : a;
}

}


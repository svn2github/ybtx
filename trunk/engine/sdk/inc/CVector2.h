#pragma once
#include "BaseMath.h"
#include "ExpHelper.h"
#include "CMathMallocObject.h"

#ifdef _WIN32
#pragma warning(push)
#pragma warning(disable:4201)
#endif

namespace sqr
{
class CMatrix;
class CPlane;
class CLine;

template<class T> struct TVector2;

typedef TVector2<float> CVector2f;
typedef TVector2<uint> CSize2I;
typedef TVector2<int32> CVector2I;

template<class T> struct TVector2 : public CMathMallocObject
{
	union
	{
		struct
		{
			T	x,y; // 3 real components of the vector
		};

		T	v[2]; // Array access useful in loops
	};

	// =========================================================================
	// 构造函数
	//==========================================================================
	TVector2() {}
	TVector2( const T x, const T y ) : x(x), y(y) {}
	TVector2( const TVector2& v ) : x(v.x), y(v.y) {}
	TVector2( const T f[2])
	{
		v[0] = f[0];
		v[1] = f[1];
	}

	// =========================================================================
	// 初始化函数
	// =========================================================================
	inline void Init( T a=0, T b=0);
	inline void Init(const TVector2& v);
	inline void Init(const T f[2]);

	// =========================================================================
	// 矢量的基本运算
	// =========================================================================
	// 矢量长度的平方
	T MagSqr() const;
	// 矢量的长度
	T Mag() const;
	// 矢量长度的快速计算
	T MagFast() const;

	// 返回一个原矢量的单位矢量，原矢量不改变
	const TVector2 Unit() const	;
	// 将自身单位化
	const TVector2& Normalize();
	//  将自身单位化 返回长度
	const T normalize();
	//
	void Norm(T nVal = 1);
	// 矢量单位化的快速计算
	void NormFast(T nVal = 1);


	// 判断一个矢量是否等于零(在指定的精度内) ，fTol为比较的精度
	bool IsZero(float fTol = MATH_EPSILON)	const;
	// 判断是否等于另一个矢量(在指定的精度内) ，fTol为比较的精度
	bool IsEqual(const TVector2& b, const T r = MATH_EPSILON) const;
	// 计算与另一个矢量间的距离
	float Dist( const TVector2& right ) const;

	// =====================================================================
	// 矢量的运算
	// =====================================================================
	// 矢量的点积
	T Dot( const TVector2& b ) const;

	//=====================================================================
	// 矢量的辅助功能
	//=====================================================================
	//!矢量关于一个线段的镜像
	bool	Mirror(const CLine& Line);
	///< 矢量旋转
	TVector2 Rotate(double radian, bool isClockwise)const;

	TVector2	GetMaxAxis()const;
	TVector2	GetMinAxis()const;

	// =====================================================================
	// 操作符重载
	// =====================================================================
	const T& operator [] ( const int32 i ) const;
	T& operator [] ( const int32 i );

	const TVector2 operator + (const TVector2& b) const;
	const TVector2 operator - (const TVector2& b) const;
	const TVector2 operator * (const T s) const;
	const TVector2 operator / (const T s) const;
	const TVector2 operator - () const;

	friend inline const TVector2 operator * (const T s, const TVector2& v)
	{
		return v * s;
	}

	const TVector2& operator = ( const TVector2& b );
	const TVector2& operator += (const TVector2& b);
	const TVector2& operator -= (const TVector2& b) ;
	const TVector2& operator *= (const T s);
	const TVector2& operator /= (const T s);

	TVector2	operator * (const CMatrix& mat) const;
	TVector2&	operator *= (const CMatrix& mat);

	// 点积
	T operator * (const TVector2& v) const;
	// 叉积
	TVector2 operator ^ (const TVector2& v) const;

	bool operator == ( const TVector2& b ) const;
	bool operator != ( const TVector2& b ) const;

	bool operator > (const TVector2 &other) const;
	bool operator < (const TVector2 &other) const;
	bool operator >= (const TVector2 &other) const;
	bool operator <= (const TVector2 &other) const;
};

//====================================================================
// 初始化函数
//====================================================================
template<class T> inline void TVector2<T>::Init( T a, T b)
{
	x = a;
	y = b;
}

template<class T> inline void TVector2<T>::Init(const TVector2& v)
{
	x = v.x;
	y = v.y;
}

template<class T> inline void TVector2<T>::Init(const T f[2])
{
	v[0] = f[0];
	v[1] = f[1];
}

//========================================================================
// 矢量的基本运算
//========================================================================
// 矢量的长度
template<class T> inline T TVector2<T>::Mag() const
{
	return (T)sqrtf( x*x + y*y );
}

// 矢量长度的平方
template<class T> inline T TVector2<T>::MagSqr() const
{
	return x*x + y*y;
}

// 返回一个原矢量的单位矢量，原矢量不改变
template<class T> inline const TVector2<T> TVector2<T>::Unit() const
{
	float fLen = this->Mag();

	Ast( !gIsZero(fLen) );

	return (*this) / fLen;
}

// 将自身单位化
template<class T> inline const TVector2<T>& TVector2<T>::Normalize()
{
	float fLen = this->Mag();

	//Ast(gIsZero(fLen) == false);
	(*this) /= fLen;

	return *this;
}

template<class T> inline const T TVector2<T>::normalize()
{
	T fLen = this->Mag();
	(*this) /= fLen;
	return fLen;
}

// 判断一个矢量是否等于零(在指定的精度内) ，fTol为比较的精度
template<class T> inline bool TVector2<T>::IsZero(float fTol)	const
{
	if ( gIsZero(x, fTol) && gIsZero(y, fTol))
		return true;
	else
		return false;
}

// 判断是否等于另一个矢量(在指定的精度内) ，fTol为比较的精度
template<class T> inline bool TVector2<T>::IsEqual(const TVector2& b, const T r) const
{
	//within a tolerance
	const TVector2 t = *this - b;//difference

	return t.Dot(t) <= r*r;//radius
}

// 计算与另一个矢量间的距离
template<class T> inline float TVector2<T>::Dist( const TVector2& right ) const
{
	TVector2<T> DistVec( x - right.x, y - right.y );
	return DistVec.Mag();
}

//=================================================================================
// 矢量的运算
//=================================================================================
// 矢量的点积
template<class T> inline T TVector2<T>::Dot( const TVector2& b ) const
{
	return x*b.x + y*b.y;
}

//******************************************************************************
/*! \fn     TVector2 TVector2<T>::GetMainAxis()const
*   \brief  得到矢量的最大轴
*           返回矢量中, x, y, z最大得对应得那个轴
*   \return TVector2 得到得最大轴
*   \sa     GetMinAxis
*******************************************************************************/
template<class T> inline TVector2<T> TVector2<T>::GetMaxAxis()const
{
	if (x >= y)
		return TVector2(1.0f, 0.0f);
	else
		return TVector2(0.0f, 1.0f);
}

//******************************************************************************
/*! \fn    TVector2 TVector2<T>::GetMinAxis()const
*   \brief  得到矢量的最小轴
*           返回矢量中, x, y, z最小得对应得那个轴
*   \return TVector2 得到得最小轴
*   \sa     GetMaxAxis
*******************************************************************************/
template<class T> inline TVector2<T> TVector2<T>::GetMinAxis()const
{
	if (x < y)
		return TVector2(1.0f, 0.0f);
	else
		return TVector2(0.0f, 1.0f);
}

template<class T> inline const T& TVector2<T>::operator [] ( const int32 i ) const
{
	return v[i];
}

template<class T> inline T& TVector2<T>::operator [] ( const int32 i )
{
	return v[i];
}

template<class T> inline const TVector2<T> TVector2<T>::operator + (const TVector2& b) const
{
	return TVector2(x + b.x, y + b.y);
}

template<class T> inline const TVector2<T> TVector2<T>::operator - (const TVector2& b) const
{
	return TVector2(x - b.x, y - b.y);
}

template<class T> inline const TVector2<T> TVector2<T>::operator * (const T s) const
{
	return TVector2(x*s, y*s);
}


template<class T> inline const TVector2<T> TVector2<T>::operator / (const T s) const
{
	return TVector2(x/s, y/s);
}

template<class T> inline const TVector2<T>& TVector2<T>::operator = ( const TVector2& b )
{
	x = b.x;
	y = b.y;

	return *this;
}

template<class T> inline const TVector2<T>& TVector2<T>::operator += (const TVector2& b)
{
	x += b.x;
	y += b.y;

	return *this;
}

template<class T> inline const TVector2<T>& TVector2<T>::operator -= (const TVector2& b)
{
	x -= b.x;
	y -= b.y;

	return *this;
}

template<class T> inline const TVector2<T>& TVector2<T>::operator *= (const T s)
{
	x *= s;
	y *= s;

	return *this;
}

template<class T> inline const TVector2<T>& TVector2<T>::operator /= (const T s)
{
	x /= s;
	y /= s;

	return *this;
}

// 点积
template<class T> inline T TVector2<T>::operator * (const TVector2& b) const
{
	return x*b.x + y*b.y;
}

template<class T> inline bool TVector2<T>::operator == ( const TVector2& b ) const
{
	return (b.x==x && b.y==y);
}

template<class T> inline bool TVector2<T>::operator != ( const TVector2& b ) const
{
	return !(b == *this);
}

template<class T> inline const TVector2<T> TVector2<T>::operator - () const
{
	return TVector2( -x, -y );
}


template<class T> inline bool TVector2<T>::operator > (const TVector2 &other) const
{
	return ((x>other.x) && (y>other.y));
}

template<class T> inline bool TVector2<T>::operator < (const TVector2 &other) const
{
	return ((x<other.x) && (y<other.y));
}

template<class T> inline bool TVector2<T>::operator >= (const TVector2 &other) const
{
	return ((x>=other.x) && (y>=other.y));
}

template<class T> inline bool TVector2<T>::operator <= (const TVector2 &other) const
{
	return ((x<=other.x) && (y<=other.y));
}
//如果是顺时针旋转：
//x2 = x1 * cosa + y1 * sina
//y2 = y1 * cosa - x1 * sina
//如果是逆时针旋转：
//x2 = x1 * cosa - y1 * sina
//y2 = y1 * cosa + x1 * sina
template<class T> TVector2<T> TVector2<T>::Rotate(double radian, bool isClockwise)const
{
	const double cosa = cos(radian);
	const double sina = sin(radian);
	if (isClockwise)
		return TVector2<T>(static_cast<T>(x * cosa + y * sina), static_cast<T>(y * cosa - x * sina));
	else
		return TVector2<T>(static_cast<T>(x * cosa - y * sina), static_cast<T>(y * cosa + x * sina));
}
}

#ifdef _WIN32
#pragma warning(pop)
#endif 


/*
*	矢量模板及相关的各种运算,CVector3f代表的是浮点矢量，一般直接使用他

*/
#ifndef __MATH_VECTOR3_H__
#define __MATH_VECTOR3_H__

#include "BaseMath.h"


#ifdef _WIN32
#pragma warning(disable : 4201)
#endif

namespace sqr
{
template<class T> struct TVector3;

typedef TVector3<float> CVector3f;

template<class T> struct TVector3
{
	union
	{
		// This structure here is nameless, which really isn't
		// legal C++... most compilers will deal with it,however.
		struct
		{
			T x,y,z; // 3 real components of the vector
		};
		T	v[3]; // Array access useful in loops
	};

	// =========================================================================
	// 构造函数
	//==========================================================================
	TVector3()
	{
		x = T();
		y = T();
		z = T();
	}
	TVector3( const T x, const T y, const T z ) : x(x), y(y), z(z) {}
	TVector3( const TVector3& v ) : x(v.x), y(v.y), z(v.z) {}
	TVector3( const T f[3])
	{
		v[0] = f[0];
		v[1] = f[1];
		v[2] = f[2];
	}

	// =========================================================================
	// 初始化函数
	// =========================================================================
	inline void Init( T a=0, T b=0, T c=0);
	inline void Init(const TVector3& v);
	inline void Init(const T f[3]);

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
	const TVector3 Unit() const	;
	// 将自身单位化
	const TVector3& Normalize();
	//  将自身单位化 返回长度
	const T normalize();
	//
	void Norm(T nVal = 1);
	// 矢量单位化的快速计算
	void NormFast(T nVal = 1);


	// 判断一个矢量是否等于零(在指定的精度内) ，fTol为比较的精度
	bool IsZero(float fTol = MATH_EPSILON)	const;
	// 判断是否等于另一个矢量(在指定的精度内) ，fTol为比较的精度
	bool IsEqual(const TVector3& b, const T r = MATH_EPSILON) const;
	// 计算与另一个矢量间的距离
	float Dist( const TVector3& right ) const;

	// =====================================================================
	// 矢量的运算
	// =====================================================================
	// 矢量的点积
	T Dot( const TVector3& b ) const;
	//
	T AbsDot( const TVector3& b ) const;
	// 矢量的叉积 this ^ b,注意顺序
	TVector3 Cross( const TVector3& b ) const;

	// 旋转矢量
	template<class MATRIX> const TVector3& Rotate(const MATRIX& Mat);
	//! 矢量矩阵的快速相乘
	template<class MATRIX> const TVector3& FastMultiply(const MATRIX& mat);
	// 到线段最短距离
	TVector3	ClosestPointOnLine(const TVector3 &vA, const TVector3 &vB)const;

	TVector3	GetMaxAxis()const;
	TVector3	GetMinAxis()const;

	// =====================================================================
	// 操作符重载
	// =====================================================================
	const T& operator [] ( const int32 i ) const;
	T& operator [] ( const int32 i );

	TVector3 operator + (const TVector3& b) const;
	TVector3 operator - (const TVector3& b) const;
	TVector3 operator * (const T s) const;
	TVector3 operator / (const T s) const;
	TVector3 operator - () const;

	friend inline const TVector3 operator * (const T s, const TVector3& v)
	{
		return v * s;
	}

	TVector3& operator = ( const TVector3& b);
	TVector3& operator += (const TVector3& b);
	TVector3& operator -= (const TVector3& b);
	TVector3& operator *= (const TVector3& b);
	TVector3& operator /= (const TVector3& b);


	TVector3& operator *= (const T s);
	TVector3& operator /= (const T s);

	template<class MATRIX> TVector3		operator * (const MATRIX& mat) const;
	template<class MATRIX> TVector3&	operator *= (const MATRIX& mat);

	// 点积
	T operator * (const TVector3& v) const;
	// 叉积
	TVector3 operator ^ (const TVector3& v) const;

	bool operator == ( const TVector3& b ) const;
	bool operator != ( const TVector3& b ) const;

	bool operator > (const TVector3 &other) const;
	bool operator < (const TVector3 &other) const;
	bool operator >= (const TVector3 &other) const;
	bool operator <= (const TVector3 &other) const;
};

//====================================================================
// 初始化函数
//====================================================================
// flying commented: default argument should be specified only once.
template<class T> inline void TVector3<T>::Init( T a, T b, T c)
{
	x = a;
	y = b;
	z = c;
}

template<class T> inline void TVector3<T>::Init(const TVector3& v)
{
	x = v.x;
	y = v.y;
	z = v.z;
}

template<class T> inline void TVector3<T>::Init(const T f[3])
{
	v[0] = f[0];
	v[1] = f[1];
	v[2] = f[2];
}

//========================================================================
// 矢量的基本运算
//========================================================================
// 矢量的长度
template<class T> inline T TVector3<T>::Mag() const
{
	return (T)sqrtf( x*x + y*y + z*z );
}

// 矢量长度的平方
template<class T> inline T TVector3<T>::MagSqr() const
{
	return x*x + y*y + z*z;
}

// 矢量长度的近似算法
template<class T> inline T TVector3<T>::MagFast () const
{
	T   min, med, max;
	T   temp;

	max = fabsf(x);
	med = fabsf(y);
	min = fabsf(z);

	if (max < med)
	{
		temp = max;
		max = med;
		med = temp;
	}

	if (max < min)
	{
		temp = max;
		max = min;
		min = temp;
	}

	return max + ((med + min) * 0.25f);
}

// 返回一个原矢量的单位矢量，原矢量不改变
template<class T> inline const TVector3<T> TVector3<T>::Unit() const
{
	float fLen = this->Mag();
	return (*this) / fLen;
}

// 返回单位化矢量
template<class T> inline const TVector3<T>& TVector3<T>::Normalize()
{
	float fLen = this->Mag();
	(*this) /= fLen;
	return *this;
}

template<class T> inline const T TVector3<T>::normalize()
{
	T fLen = this->Mag();
	(*this) /= fLen;
	return fLen;
}

// 判断一个矢量是否等于零(在指定的精度内) ，fTol为比较的精度
template<class T> inline bool TVector3<T>::IsZero(float fTol)	const
{
	if ( gIsZero(x, fTol) && gIsZero(y, fTol) && gIsZero(z, fTol))
		return true;
	else
		return false;
}

// 判断是否等于另一个矢量(在指定的精度内) ，fTol为比较的精度
template<class T> inline bool TVector3<T>::IsEqual(const TVector3& b, const T r) const
{
	//within a tolerance
	const TVector3 t = *this - b;//difference

	return t.Dot(t) <= r*r;//radius
}

// 计算与另一个矢量间的距离
template<class T> inline float TVector3<T>::Dist( const TVector3& right ) const
{
	TVector3 DistVec( x - right.x, y - right.y, z - right.z );
	return DistVec.Mag();
}

//=================================================================================
// 矢量的运算
//=================================================================================
// 矢量的点积
template<class T> inline T TVector3<T>::Dot( const TVector3& b ) const
{
	return x*b.x + y*b.y + z*b.z;
}

template<class T> inline T TVector3<T>::AbsDot( const TVector3& b ) const
{
	return fabs(x*b.x) + fabs(y*b.y) + fabs(z*b.z);
}

// 矢量的叉积 this ^ b,注意顺序
template<class T> inline TVector3<T> TVector3<T>::Cross( const TVector3<T>& b ) const
{
	return TVector3( y*b.z - z*b.y, z*b.x - x*b.z, x*b.y - y*b.x );
}

//******************************************************************************
/*! \fn     TVector3<T>::Rotate(const TMATRIX& Mat)
*   \brief  仅仅旋转矢量
*           不考虑矩阵中的偏移参数，仅仅利用矩阵中的旋转信息完成矢量的旋转，
*			内部去掉了矩阵的第四行，第四列。矢量自身改变
*   \sa     需要做传入的矩阵的校验
*******************************************************************************/
template<class T> template<class MATRIX> inline const TVector3<T>& TVector3<T>::Rotate(const MATRIX& Mat)
{
	T fX = x*Mat._11 + y*Mat._21 + z*Mat._31;
	T fY = x*Mat._12 + y*Mat._22 + z*Mat._32;
	T fZ = x*Mat._13 + y*Mat._23 + z*Mat._33;

	x = fX;
	y = fY;
	z = fZ;
	return *this;
}

// 矢量与快速矩阵相乘, 返回值为结果矢量,不同于重载的乘操作，最后一列不乘, 速度由所提高
template<class T> template<class MATRIX>  inline const TVector3<T>& TVector3<T>::FastMultiply(const MATRIX& Mat)
{
	T fX = x*Mat._11 + y*Mat._21 + z*Mat._31 + Mat._41;
	T fY = x*Mat._12 + y*Mat._22 + z*Mat._32 + Mat._42;
	T fZ = x*Mat._13 + y*Mat._23 + z*Mat._33 + Mat._43;

	x = fX;
	y = fY;
	z = fZ;
	return *this;
}

// 到线段最短距离
template<class T> inline TVector3<T> TVector3<T>::ClosestPointOnLine(const TVector3 &vA, const TVector3 &vB)const
{
	TVector3 vDir = vB - vA;
	T d = vDir.Mag();

	vDir /= d;
	T t = vDir * (*this - vA);

	if (t <= 0)
		return vA;
	if (t >= d)
		return vB;

	return vA + vDir * t;
}

//******************************************************************************
/*! \fn     TVector3 TVector3<T>::GetMainAxis()const
*   \brief  得到矢量的最大轴
*           返回矢量中, x, y, z最大得对应得那个轴
*   \return TVector3 得到得最大轴
*   \sa     GetMinAxis
*******************************************************************************/
template<class T> inline TVector3<T> TVector3<T>::GetMaxAxis()const
{
	if (x >= y)
	{
		if ( x >= z)
			return TVector3(1.0f, 0.0f, 0.0f);
		else
			return TVector3(0.0f, 0.0f, 1.0f);
	}
	else
	{
		if ( y >= z)
			return TVector3(0.0f, 1.0f, 0.0f);
		else
			return TVector3(0.0f, 0.0f, 1.0f);
	}
}

//******************************************************************************
/*! \fn    TVector3 TVector3<T>::GetMinAxis()const
*   \brief  得到矢量的最小轴
*           返回矢量中, x, y, z最小得对应得那个轴
*   \return TVector3 得到得最小轴
*   \sa     GetMaxAxis
*******************************************************************************/
template<class T> inline TVector3<T> TVector3<T>::GetMinAxis()const
{
	if (x < y)
	{
		if ( x < z)
			return TVector3(1.0f, 0.0f, 0.0f);
		else
			return TVector3(0.0f, 0.0f, 1.0f);
	}
	else
	{
		if ( y < z)
			return TVector3(0.0f, 1.0f, 0.0f);
		else
			return TVector3(0.0f, 0.0f, 1.0f);
	}
}

template<class T> inline const T& TVector3<T>::operator [] ( const int32 i ) const
{
	return v[i];
}

template<class T> inline T& TVector3<T>::operator [] ( const int32 i )
{
	return v[i];
}

template<class T> inline TVector3<T> TVector3<T>::operator + (const TVector3& b) const
{
	return TVector3<T>(x + b.x, y + b.y, z + b.z);
}

template<class T> inline TVector3<T> TVector3<T>::operator - (const TVector3& b) const
{
	return TVector3<T>(x - b.x, y - b.y, z - b.z);
}

template<class T> inline TVector3<T> TVector3<T>::operator * (const T s) const
{
	return TVector3<T>(x*s, y*s, z*s);
}


template<class T> inline TVector3<T> TVector3<T>::operator / (const T s) const
{
	return TVector3<T>(x/s, y/s, z/s);
}

template<class T> inline TVector3<T>& TVector3<T>::operator = ( const TVector3& b )
{
	x = b.x;
	y = b.y;
	z = b.z;

	return *this;
}

template<class T> inline TVector3<T>& TVector3<T>::operator += (const TVector3& b)
{
	x += b.x;
	y += b.y;
	z += b.z;

	return *this;
}

template<class T> inline TVector3<T>& TVector3<T>::operator -= (const TVector3& b)
{
	x -= b.x;
	y -= b.y;
	z -= b.z;

	return *this;
}

template<class T> inline TVector3<T>& TVector3<T>::operator *= (const TVector3& b)
{
	x *= b.x;
	y *= b.y;
	z *= b.z;
	return *this;
}

template<class T> inline TVector3<T>& TVector3<T>::operator /= (const TVector3& b)
{
	x /= b.x;
	y /= b.y;
	z /= b.z;
	return *this;
}

template<class T> inline TVector3<T>& TVector3<T>::operator *= (const T s)
{
	x *= s;
	y *= s;
	z *= s;
	return *this;
}

template<class T> inline TVector3<T>& TVector3<T>::operator /= (const T s)
{
	x /= s;
	y /= s;
	z /= s;

	return *this;
}


// 矢量与矩阵相乘, 返回值为结果矢量,自身不改变 (矢量在左,矩阵在右,行矢量)
template<class T> template<class MATRIX> inline TVector3<T> TVector3<T>::operator * (const MATRIX& Mat) const
{
	float fX = x*Mat._11 + y*Mat._21 + z*Mat._31 + Mat._41;
	float fY = x*Mat._12 + y*Mat._22 + z*Mat._32 + Mat._42;
	float fZ = x*Mat._13 + y*Mat._23 + z*Mat._33 + Mat._43;
	float fW = x*Mat._14 + y*Mat._24 + z*Mat._34 + Mat._44;

	if ( fW == 0.0f)
		return TVector3(0.0f, 0.0f, 0.0f);

	return TVector3((T)fX/fW, (T)fY/fW, (T)fZ/fW);
}

template<class T> template<class MATRIX> inline TVector3<T>&	TVector3<T>::operator *= (const MATRIX& Mat)
{
	float fX = x*Mat._11 + y*Mat._21 + z*Mat._31 + Mat._41;
	float fY = x*Mat._12 + y*Mat._22 + z*Mat._32 + Mat._42;
	float fZ = x*Mat._13 + y*Mat._23 + z*Mat._33 + Mat._43;
	float fW = x*Mat._14 + y*Mat._24 + z*Mat._34 + Mat._44;

	if ( gIsZero(fW) == false)
	{
		x = 0.0f;
		y = 0.0f;
		z = 0.0f;
	}
	else
	{
		x = fX/fW;
		y = fY/fW;
		z = fZ/fW;
	}

	return *this;
}


// 点积
template<class T> inline T TVector3<T>::operator * (const TVector3& b) const
{
	return x*b.x + y*b.y + z*b.z;
}

// 叉积
template<class T> inline TVector3<T> TVector3<T>::operator ^ (const TVector3& v) const
{
	return TVector3( y*v.z - z*v.y, z*v.x - x*v.z, x*v.y - y*v.x );
}


template<class T> inline bool TVector3<T>::operator == ( const TVector3& b ) const
{
	return ( gIsEqual( b.x, x ) && gIsEqual( b.y, y ) && gIsEqual( b.z, z ) );
}

template<class T> inline bool TVector3<T>::operator != ( const TVector3& b ) const
{
	return !(b == *this);
}

template<class T> inline TVector3<T> TVector3<T>::operator - () const
{
	return TVector3( -x, -y, -z );
}


template<class T> inline bool TVector3<T>::operator > (const TVector3 &other) const
{
	return ((x>other.x) && (y>other.y) && (z>other.z));
}

template<class T> inline bool TVector3<T>::operator < (const TVector3 &other) const
{
	return ((x<other.x) && (y<other.y) && (z<other.z));
}

template<class T> inline bool TVector3<T>::operator >= (const TVector3 &other) const
{
	return ((x>=other.x) && (y>=other.y) && (z>=other.z));
}

template<class T> inline bool TVector3<T>::operator <= (const TVector3 &other) const
{
	return ((x<=other.x) && (y<=other.y) && (z<=other.z));
}

#define VEC_ROTATE( v, Mat )\
	CVector3f(v).Rotate( Mat )


#define VEC_FASTMULT( v, Mat )\
	CVector3f(v).FastMultiply( Mat )


#define VEC_CROSS(dest, v1, v2) \
	{\
	(dest).x = ((v1).y*(v2).z - (v1).z*(v2).y);\
	(dest).y = ((v1).z*(v2).x - (v1).x*(v2).z);\
	(dest).z = ((v1).x*(v2).y - (v1).y*(v2).x);\
	}

#define VEC_ADD(d, v1, v2) \
	{\
	(d).x = (v1).x + (v2).x;\
	(d).y = (v1).y + (v2).y;\
	(d).z = (v1).z + (v2).z;\
	}

#define VEC_ADDSCALED(d, v1, v2, s) \
	{\
	(d).x = (v1).x + ((v2).x * (s));\
	(d).y = (v1).y + ((v2).y * (s));\
	(d).z = (v1).z + ((v2).z * (s));\
	}

#define VEC_SUB(d, v1, v2) \
	{\
	(d).x = (v1).x - (v2).x;\
	(d).y = (v1).y - (v2).y;\
	(d).z = (v1).z - (v2).z;\
	}

#define VEC_MUL(d, v1, v2) \
	{\
	(d).x = (v1).x * (v2).x;\
	(d).y = (v1).y * (v2).y;\
	(d).z = (v1).z * (v2).z;\
	}

#define VEC_DIV(d, v1, v2) \
	{\
	(d).x = (v1).x / (v2).x;\
	(d).y = (v1).y / (v2).y;\
	(d).z = (v1).z / (v2).z;\
	}

#define VEC_MULSCALAR(d, v1, s) \
	{\
	(d).x = (v1).x * (s); \
	(d).y = (v1).y * (s); \
	(d).z = (v1).z * (s); \
	}

#define VEC_DIVSCALAR(d, v1, s) \
	{\
	(d).x = (v1).x / (s); \
	(d).y = (v1).y / (s); \
	(d).z = (v1).z / (s);\
	}

#define VEC_LERP(d, v1, v2, t) \
	{\
	(d).x = (v1).x + ((v2).x - (v1).x) * t;\
	(d).y = (v1).y + ((v2).y - (v1).y) * t;\
	(d).z = (v1).z + ((v2).z - (v1).z) * t;\
	}

#define VEC_CLAMP(v, a, b) \
	{\
	(v).x = LTCLAMP((v).x, a, b);\
	(v).y = LTCLAMP((v).y, a, b);\
	(v).z = LTCLAMP((v).z, a, b);\
	}

#define VEC_MIN(v, a, b) \
	{\
	(v).x = LTMIN((a).x, (b).x);\
	(v).y = LTMIN((a).y, (b).y);\
	(v).z = LTMIN((a).z, (b).z);\
	}

#define VEC_MAX(v, a, b) \
	{\
	(v).x = LTMAX((a).x, (b).x);\
	(v).y = LTMAX((a).y, (b).y);\
	(v).z = LTMAX((a).z, (b).z);\
	}

#define VEC_NEGATE(d, s) \
	{\
	(d).x = -(s).x; \
	(d).y = -(s).y; \
	(d).z = -(s).z; \
	}

#define VEC_DOT(v1, v2) ((v1).x*(v2).x + (v1).y*(v2).y + (v1).z*(v2).z)

#define EXPANDVEC(vec) (vec).x, (vec).y, (vec).z

//---------------------------------------------------------------------------//
#define VEC_DISTSQR(v1, v2) ( \
	((v1).x-(v2).x) * ((v1).x-(v2).x) + \
	((v1).y-(v2).y) * ((v1).y-(v2).y) + \
	((v1).z-(v2).z) * ((v1).z-(v2).z) )


//---------------------------------------------------------------------------//
#define VEC_DIST(v1, v2) ((float)sqrt(VEC_DISTSQR(v1, v2)))

#define VEC_MAGSQR(v) ((v).x*(v).x + (v).y*(v).y + (v).z*(v).z)

//---------------------------------------------------------------------------//
#define VEC_MAG(v) ((float)sqrt(VEC_MAGSQR(v)))

#define VEC_INVMAG(v) (1.0f / VEC_MAG(v))

#define VEC_NORM(v) \
	{\
	float __temp_normalizer_____;\
	__temp_normalizer_____ = 1.0f / VEC_MAG(v);\
	(v).x *= __temp_normalizer_____;\
	(v).y *= __temp_normalizer_____;\
	(v).z *= __temp_normalizer_____;\
	}

#define VEC_COPY(dest, src) \
	{\
	(dest).x = (src).x; (dest).y = (src).y; (dest).z = (src).z;\
	}

#define VEC_SET(v, vx, vy, vz) \
	{\
	(v).x = (float)(vx); (v).y = (float)(vy); (v).z = (float)(vz);\
	}

#define VEC_EXPAND(v) (v).x, (v).y, (v).z

#define VEC_INIT(v) (v).x = (v).y = (v).z = 0;

#define VEC_INDEX(v, i) (((float*)&(v))[i])

} // End namespace sqr
#endif

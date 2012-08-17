#pragma once

/******************************************************************************
*! \file       CCQuaternion.h
*   \brief      四元数类的定义
*
*				定义各种可能的构造四元数的方法,和相关的四元数操作
*   \version	1.0
*   \date		04-8-2 11:42
*   \todo		还没有做完整的测试及代码的优化 根本就是不能用嘛
*******************************************************************************/

#include "CMatrix.h"
#include "CVector3.h"
#include "CMathMallocObject.h"

namespace sqr
{
	class CQuaternion : public CMathMallocObject
	{
	public:
		union
		{
			//#ifdef SIMD
			//		__declspec(align(16)) float q[4];
			//#else
			float q[4];
			//#endif
			//将w放在前面 主要方便 SSE的计算
			struct
			{
				float w,z,y,x;
			};
		};
	public:
		CQuaternion( const CQuaternion& OriQuater );
		CQuaternion( float qw = 1.0f,float qx = 0.0f,float qy = 0.0f,float qz = 0.0f );

		void setNULL(void);
		void setIdentity(void);

		void FromRotationMatrix (const CMatrix& matRot);
		void ToRotationMatrix (CMatrix& matRot) const;
		void FromAngleAxis (const float& fAngle, const CVector3f& vAxis);
		void ToAngleAxis (float& fAngle, CVector3f& vAxis) const;

		void FromAxes (const CVector3f* avAxis);
		void FromAxes (const CVector3f& xAxis, const CVector3f& yAxis, const CVector3f& zAxis);
		void ToAxes (CVector3f* avAxis) const;
		void ToAxes (CVector3f& xAxis, CVector3f& yAxis, CVector3f& zAxis) const;

		void FromTwoVector( const CVector3f& vOrg,const CVector3f& vDest );

		bool		operator ==( const CQuaternion& cQ ) const;
		bool		operator !=( const CQuaternion& cQ ) const;
		CQuaternion& operator =( const CQuaternion& cQ );
		CQuaternion	operator +( const CQuaternion& cQ ) const;
		CQuaternion	operator -( const CQuaternion& cQ ) const;
		CQuaternion	operator *( const CQuaternion& cQ ) const;
		CQuaternion	operator *( const float cr ) const;

		CQuaternion& operator *=( const CQuaternion& cQ );
		CQuaternion& operator *=( const float cr );

		friend CQuaternion operator* (const float cr,
			const CQuaternion& cQ);
		CQuaternion operator- () const;

		//功能函数
		CVector3f		operator *(const CVector3f& cV) const; //旋转一个3D向量

		//插值
		CQuaternion& lerp(const CQuaternion& rkP, const CQuaternion& rkQ, float fT );
		//球面差值
		CQuaternion& Slerp(const CQuaternion& rkP, const CQuaternion& rkQ, float fT );

		CQuaternion& SlerpExtraSpins ( const CQuaternion& rkP, const CQuaternion& rkQ, int iExtraSpins , float fT);

		// 二次球面方程插值
		CQuaternion& Squad ( const CQuaternion& rkP,
			const CQuaternion& rkA, const CQuaternion& rkB,
			const CQuaternion& rkQ, float fT );

		//数学函数
		float Dot (const CQuaternion& rkQ) const;
		float Norm () const;
		float normalize(void);
		CQuaternion Inverse () const;
		CQuaternion UnitInverse () const;
		CQuaternion Exp () const;
		CQuaternion Log () const;

		//插值函数
		// 四原数的插值
		//直线插值
		static void lerp( CQuaternion& outQ, const CQuaternion& rkP,
			const CQuaternion& rkQ, float fT );

		//球面差值
		static void Slerp ( CQuaternion& outQ, const CQuaternion& rkP,
			const CQuaternion& rkQ, float fT, bool shortestPath = true);

		static void SlerpExtraSpins ( CQuaternion& outQ, const CQuaternion& rkP, const CQuaternion& rkQ,
			int iExtraSpins , float fT);

		// 设置二次球面方程插值
		static void Intermediate (const CQuaternion& rkQ0,
			const CQuaternion& rkQ1, const CQuaternion& rkQ2,
			CQuaternion& rkA, CQuaternion& rkB);

		// 二次球面方程插值
		static void Squad (	CQuaternion& outQ, const CQuaternion& rkP,
			const CQuaternion& rkA, const CQuaternion& rkB,
			const CQuaternion& rkQ, float fT, bool shortestPath = true);
	protected:
		static const float ms_fEpsilon;
	private:
	};


	_MATH_INLINE_ CQuaternion::CQuaternion(float qw /* = 1.0f  */,float qx /* = 0.0f */,float qy /* = 0.0f */,
		float qz /* = 0.0f */)
	{ w = qw; x = qx; y = qy; z = qz; }

	_MATH_INLINE_ CQuaternion::CQuaternion( const CQuaternion& OriQuater )
	{
		x = OriQuater.x; y = OriQuater.y; 
		z = OriQuater.z; w = OriQuater.w;
	}

	_MATH_INLINE_ void CQuaternion::setNULL()
	{ x=y=z=w=0; }

	_MATH_INLINE_ void CQuaternion::setIdentity()
	{ x=y=z=0; w=1;}

	_MATH_INLINE_ void CQuaternion::FromAngleAxis (const float& fAngle, const CVector3f& vAxis)
	{
		float fHalfAngle = float(0.5f*fAngle);
		float fSin = CMath::Sin(fHalfAngle);
		w = CMath::Cos(fHalfAngle);
		x = fSin*vAxis.x;
		y = fSin*vAxis.y;
		z = fSin*vAxis.z;
	}

	_MATH_INLINE_ void CQuaternion::ToAngleAxis (float& fAngle, CVector3f& vAxis) const
	{
		float fSqrLength = x*x+y*y+z*z;
		if ( fSqrLength > 0.0f )
		{
			fAngle = float(2.0f*CMath::ACos(w));
			float fInvLength = CMath::InvSqrt(fSqrLength);
			vAxis.x = x*fInvLength;
			vAxis.y = y*fInvLength;
			vAxis.z = z*fInvLength;
		}
		else
		{
			// 角度为0
			fAngle = 0.0;
			vAxis.x = 1.0;
			vAxis.y = 0.0;
			vAxis.z = 0.0;
		}
	}

	_MATH_INLINE_ void CQuaternion::FromAxes (const CVector3f* avAxis)
	{
		CMatrix kRot;

		for (int iCol = 0; iCol < 3; ++iCol)
		{
			kRot.m[iCol][0] = avAxis[iCol].x;
			kRot.m[iCol][1] = avAxis[iCol].y;
			kRot.m[iCol][2] = avAxis[iCol].z;
		}

		FromRotationMatrix(kRot);
	}

	_MATH_INLINE_ void CQuaternion::FromAxes (const CVector3f& xAxis, const CVector3f& yAxis, const CVector3f& zAxis)
	{
		CMatrix kRot;

		kRot.m[0][0] = xAxis.x;
		kRot.m[0][1] = xAxis.y;
		kRot.m[0][2] = xAxis.z;

		kRot.m[1][0] = yAxis.x;
		kRot.m[1][1] = yAxis.y;
		kRot.m[1][2] = yAxis.z;

		kRot.m[2][0] = zAxis.x;
		kRot.m[2][1] = zAxis.y;
		kRot.m[2][2] = zAxis.z;

		FromRotationMatrix(kRot);
	}

	_MATH_INLINE_ void CQuaternion::ToAxes (CVector3f* avAxis) const
	{
		CMatrix kRot;

		ToRotationMatrix(kRot);

		for (int iCol = 0; iCol < 3; ++iCol)
		{
			avAxis[iCol].x = kRot.m[iCol][0];
			avAxis[iCol].y = kRot.m[iCol][1];
			avAxis[iCol].z = kRot.m[iCol][2];
		}
	}

	_MATH_INLINE_ void CQuaternion::ToAxes (CVector3f& xAxis, CVector3f& yAxis, CVector3f& zAxis) const
	{
		CMatrix kRot;

		ToRotationMatrix(kRot);

		xAxis.x = kRot.m[0][0];
		xAxis.y = kRot.m[0][1];
		xAxis.z = kRot.m[0][2];

		yAxis.x = kRot.m[1][0];
		yAxis.y = kRot.m[1][1];
		yAxis.z = kRot.m[1][2];

		zAxis.x = kRot.m[2][0];
		zAxis.y = kRot.m[2][1];
		zAxis.z = kRot.m[2][2];
	}

	_MATH_INLINE_ void CQuaternion::FromTwoVector(const CVector3f& vOrg,const CVector3f& vDest )
	{
		//assert:vOrg与vDest 必须均为电位向量
		CVector3f axis = vOrg.Cross(vDest);
		//axis的模为|axis| = |vOrg|*|vDest|*sin2a = 2sina*cosa
		//注：2a为vOrg和vDest中间的夹角
		//下面求夹角的cos2a 当cos2a = 1时 两条向量重合
		//或者cos2a = -1也什么都不做
		float c = vOrg.Dot(vDest);

		if (c >= 1.0f || c<=-1.0f)
		{
			x=0;y=0;z=0;w=1.0;
			return;
		}

		//根据2倍角公式，s=2cosa
		float s = CMath::Sqrt( (1+c)*2 );
		float invs = 1 / s;
		x = axis.x * invs;
		y = axis.y * invs;
		z = axis.z * invs;
		w = s * 0.5f;
		return;
	}

	//---------运算符--------
	_MATH_INLINE_ bool CQuaternion::operator ==( const CQuaternion& cQ ) const
	{
		return (cQ.x == x) && (cQ.y == y) &&
			(cQ.z == z) && (cQ.w == w);
	}

	_MATH_INLINE_ bool CQuaternion::operator !=( const CQuaternion& cQ ) const
	{
		return (cQ.x != x) || (cQ.y != y) ||
			(cQ.z != z) || (cQ.w != w);
	}

	_MATH_INLINE_ CQuaternion& CQuaternion::operator =( const CQuaternion& cQ )
	{  
		x = cQ.x; y = cQ.y; z = cQ.z; w = cQ.w;
		return *this; 
	}

	

	_MATH_INLINE_ CQuaternion CQuaternion::operator -() const
	{
		return CQuaternion( -w, -x, -y, -z );
	}

	//-----功能函数------
	_MATH_INLINE_ CVector3f CQuaternion::operator *(const CVector3f& cV) const
	{
		//以下是OGRE的算法
		//数学推导式
		//ret = v + 2*w*uv + 2*uuv + (u·v)u
		//而OGRE算法为
		//ret = v + 2*w*uv + 2*uuv
		//有待研究，暂时应用OGRE算法
		//试验结果OGRE正确，费解 
		CVector3f uv, uuv; 
		CVector3f qvec(x, y, z);
		uv = qvec.Cross(cV); 
		uuv = qvec.Cross(uv); 
		uv *= (2.0f * w); 
		uuv *= 2.0f; 

		return cV + uv + uuv;
	}

	_MATH_INLINE_ CQuaternion& CQuaternion::lerp(const CQuaternion& rkP, const CQuaternion& rkQ, float fT )
	{
		lerp( *this , rkP, rkQ, fT );
		return *this;
	}

	//球面差值
	_MATH_INLINE_ CQuaternion& CQuaternion::Slerp(const CQuaternion& rkP, const CQuaternion& rkQ, float fT )
	{
		Slerp( *this,rkP,rkQ,fT,true );
		return *this;
	}

	_MATH_INLINE_ CQuaternion& CQuaternion::SlerpExtraSpins ( const CQuaternion& rkP, const CQuaternion& rkQ, int iExtraSpins , float fT)
	{
		SlerpExtraSpins( *this,rkP,rkQ,iExtraSpins,fT );
		return *this;
	}

	// 二次球面方程插值
	_MATH_INLINE_ CQuaternion& CQuaternion::Squad ( const CQuaternion& rkP,
		const CQuaternion& rkA, const CQuaternion& rkB,
		const CQuaternion& rkQ, float fT )
	{
		Squad( *this,rkP,rkA,rkB,rkQ,fT,true );
		return *this;
	}


	//-----数学函数------
	_MATH_INLINE_ float CQuaternion::Dot (const CQuaternion& cQ) const
	{ return x*cQ.x+y*cQ.y+z*cQ.z+w*cQ.w; }

	_MATH_INLINE_ float CQuaternion::Norm () const
	{ return x*x+y*y+z*z+w*w; }

	_MATH_INLINE_ float CQuaternion::normalize(void)
	{ 
		float len = Norm();
		*this = *this * CMath::InvSqrt(len);
		return len;
	}

	_MATH_INLINE_ CQuaternion CQuaternion::UnitInverse () const
	{
		//必须为单位向量
		return CQuaternion(w,-x,-y,-z);
	}
};

#include "stdafx.h"
#include "MathAlgorithm.h"
#include "CPos.h"
#include "TSqrAllocator.inl"

namespace sqr
{
	bool _TightLine( int32 xSrc, int32 ySrc, int32 xDes, int32 yDes, ILineAction& Action )
	{
		int32 xRange = abs( xDes - xSrc );
		int32 yRange = abs( yDes - ySrc );

		int32 xDelta = xDes < xSrc ? -1 : 1;
		int32 yDelta = yDes < ySrc ? -1 : 1;

		CPos CurPos = CPos( xSrc, ySrc );

		if( !Action.Do( CurPos.x, CurPos.y ) )
			return false;

		int32 nCurDx = 0;
		int32 nCurDy = 0;
		int32 nJudge = 0;

		if( yRange > xRange )
		{
			nCurDy = 1;
			nJudge = -( max( yRange, 2 )>>1 );
		}
		else
		{
			nCurDx = 1;
			nJudge = ( xRange>>1 );
		}

		while( CurPos != CPos( xDes, yDes ) )
		{
			if( nJudge < 0 )
			{
				CurPos.y = CurPos.y + yDelta;
				nCurDy++;
				nJudge += xRange;
			}
			else
			{
				CurPos.x = CurPos.x + xDelta;
				nCurDx++;
				nJudge -= yRange;
			}

			if( !Action.Do( CurPos.x, CurPos.y ) )
				return false;
		}
		return true;
	}


	bool _Line( int32 xSrc, int32 ySrc, int32 xDes, int32 yDes, ILineAction& Action )
	{
		int32 xRange1 = abs( xDes - xSrc );
		int32 yRange1 = abs( yDes - ySrc );
		int32 xRange2 = xRange1<<1; // 该值可取xRange1+1至xRange1*2的值
		int32 yRange2 = yRange1<<1;

		int32 xDelta = xDes < xSrc ? -1 : 1;
		int32 yDelta = yDes < ySrc ? -1 : 1;

		CPos CurPos = CPos( xSrc, ySrc );
		// 探测起点是否为不可忽略的障碍
		if( !Action.Do( CurPos.x, CurPos.y ) )
			return false;

		int nJudge = 0;
		if( yRange1 > xRange1 ) // 路径Y轴偏移较远
		{
			while( CurPos.y != yDes )
			{
				if( nJudge - xRange2 < -yRange1 )
				{
					CurPos.x += xDelta;
					nJudge   += yRange2;
				}

				CurPos.y += yDelta;
				nJudge   -= xRange2;

				// 探测当前Grid是否为障碍
				if( !Action.Do( CurPos.x, CurPos.y ) )
					return false;
			}
		}
		else if( yRange1 < xRange1 ) // 路径X轴偏移较远
		{
			while( CurPos.x != xDes )
			{
				if( nJudge - yRange2 < -xRange1 )
				{
					CurPos.y += yDelta;
					nJudge   += xRange2;
				}

				CurPos.x += xDelta;
				nJudge   -= yRange2;

				// 探测当前Grid是否为障碍
				if( !Action.Do( CurPos.x, CurPos.y ) )
					return false;
			}
		}
		else // 路径X,Y轴偏移相同
		{
			while( CurPos.x != xDes )
			{
				CurPos.y += yDelta;
				CurPos.x += xDelta;

				// 探测当前Grid是否为障碍
				if( !Action.Do( CurPos.x, CurPos.y ) )
					return false;
			}
		}

		return true;
	}

	uint32 GetIndexCount( uint32 uYaw )
	{
		uint32 k = 0;
		const double fDelSin_V = sin( MATH_PI*0.5/uYaw );
		const double fDelCos_V = cos( MATH_PI*0.5/uYaw );

		double fCurSin = 0.0f;
		double fCurCos = 1.0f;
		for( uint32 i = 0; i < uYaw + 1; i++ )
		{
			k += (uint32)( uYaw*fCurSin ) + 1;
			double sinf_i = fCurSin*fDelCos_V + fCurCos*fDelSin_V;
			double cosf_i = fCurCos*fDelCos_V - fCurSin*fDelSin_V;
			fCurSin = sinf_i;
			fCurCos = max( cosf_i, 0 );
		}

		return k;
	}

	void DivSphSurface( CVector3f* pVec, uint32 uYaw, uint32* pIndStr, uint32* pIndNum )
	{
		const double fDelSin_V = sin( MATH_PI*0.5/uYaw );
		const double fDelCos_V = cos( MATH_PI*0.5/uYaw );

		double fCurSin = 0.0f;
		double fCurCos = 1.0f;
		for( uint32 i = 0,k = 0; i < uYaw + 1; i++ )
		{
			uint32 m = (uint32)( uYaw*fCurSin );
			pIndStr[i] = k;
			pIndNum[i] = m + 1;

			const double fDelSin_i = m ? sin( MATH_PI*0.5/m ) : 0.0f;
			const double fDelCos_i = m ? cos( MATH_PI*0.5/m ) : 1.0f;

			double fCurSin_i = 0.0f;
			double fCurCos_i = 1.0f;
			for( uint32 j = 0; j < m + 1; j++, k++ )
			{
				pVec[k].x = (float)( fCurCos_i*fCurSin );
				pVec[k].y = (float)( fCurCos );
				pVec[k].z = (float)( fCurSin_i*fCurSin );

				double sinf_j = fCurSin_i*fDelCos_i + fCurCos_i*fDelSin_i;
				double cosf_j = fCurCos_i*fDelCos_i - fCurSin_i*fDelSin_i;
				fCurSin_i = sinf_j;
				fCurCos_i = max( cosf_j, 0 );
			}

			double sinf_i = fCurSin*fDelCos_V + fCurCos*fDelSin_V;
			double cosf_i = fCurCos*fDelCos_V - fCurSin*fDelSin_V;
			fCurSin = sinf_i;
			fCurCos = max( cosf_i, 0 );
		}
	}

	uint32 GetIndexByVec( double yNormal, double zNormal, uint32 uMaxYaw, uint32* pIndStr, uint32* pIndNum )
	{
		double y = abs( yNormal );
		double z = abs( zNormal );
		double a = acos( limit4( y, 1.0 ) );
		uint32 uYaw = (uint32)( a*uMaxYaw/( MATH_PI*0.5 ) + 0.5 );
		uint32 uIndex = pIndStr[uYaw];
		if( uYaw )
		{
			double b = z/sin( a );
			double c = asin( limit4( b, 1.0 ) );
			uint32 p = limit4( (uint32)( c*( pIndNum[uYaw] - 1 )/( MATH_PI*0.5 ) + 0.5 ), pIndNum[uYaw] - 1 );						
			uIndex   = uIndex + p;
		}

		return uIndex;
	}

	//------------------------------------------------------------
	// 3D碰撞测试
	//-------------------------------------------------------------
	bool IsPointIntriangle( const CVector3f& v, const CVector3f& v0, const CVector3f& v1, const CVector3f& v2 )
	{
		CVector3f d0 = v0 - v;
		CVector3f d1 = v1 - v;
		CVector3f d2 = v2 - v;

		CVector3f cross0 = d0.Cross(d1);
		CVector3f cross1 = d1.Cross(d2);
		CVector3f cross2 = d2.Cross(d0);

		cross0.Normalize();
		cross1.Normalize();
		cross2.Normalize();

		if( fabs(cross0.x - cross1.x) < 0.0001f && fabs(cross0.y - cross1.y) < 0.0001f && fabs(cross0.z - cross1.z) < 0.0001f &&
			fabs(cross2.x - cross1.x) < 0.0001f && fabs(cross2.y - cross1.y) < 0.0001f && fabs(cross2.z - cross1.z) < 0.0001f )
			return true;
		else
			return false;
	}

	CVector3f SceneToScreen( CIRect rtScreen, const CVector3f& vScene, const CMatrix& matViewProject )
	{
		CVector3f vProj = vScene * matViewProject;
		float	Scale = 0.5f; //vProj.z *= 0.5f;
		++vProj.x; vProj.y = 1 - vProj.y; 
		return CVector3f( vProj.x*rtScreen.Width()*Scale, vProj.y*rtScreen.Height()*Scale, vProj.z );
	}

	bool DetectInBox( CPos posScreen, CIRect rtScreen, CVector3f vScale, CVector3f vOffset, const CMatrix& matWorldViewProject )
	{
		//              6__________4
		//             /|        /|
		//           0/_|______2/ |
		//            | |       | | 
		//            | |       | |
		//            | |7______|_|5
		//            | /       | /
		//           1|/________|/3


		float x = vScale.x / 2.0f;
		float y = vScale.y / 2.0f;
		float z = vScale.z / 2.0f;

		CVector3f av[8];
		av[0].Init( -x + vOffset.x,  y + vOffset.y, -z + vOffset.z );
		av[1].Init( -x + vOffset.x, -y + vOffset.y, -z + vOffset.z );
		av[2].Init(  x + vOffset.x,  y + vOffset.y, -z + vOffset.z );
		av[3].Init(  x + vOffset.x, -y + vOffset.y, -z + vOffset.z );
		av[4].Init(  x + vOffset.x,  y + vOffset.y,  z + vOffset.z );
		av[5].Init(  x + vOffset.x, -y + vOffset.y,  z + vOffset.z );
		av[6].Init( -x + vOffset.x,  y + vOffset.y,  z + vOffset.z );
		av[7].Init( -x + vOffset.x, -y + vOffset.y,  z + vOffset.z );

		//转成屏幕坐标
		for( int i = 0; i < 8; i++ )
		{
			av[i] = SceneToScreen( rtScreen, av[i], matWorldViewProject );
			if( av[i].z < 0.0f || av[i].z > 1.0f )
				return false;
		}

		CVector3f vPt( (float)posScreen.x, (float)posScreen.y, 0.0f );

		////计算是否相交
		// By Kedazhao
		// 因为z值都为零，所以不必要进行完全的叉乘，而且z的正负已经足以判断是否选中，不必单位化
		static int32 Index[12][3] =  
		{
			{ 0, 1, 2 },{ 1, 2, 3 },{ 2, 3, 4 },{ 3, 4, 5 },
			{ 4, 5, 6 },{ 5, 6, 7 },{ 6, 7, 0 },{ 7, 0, 1 },
			{ 1, 3, 7 },{ 3, 5, 7 },{ 0, 2, 4 },{ 0, 4, 6 },
		};

		//计算是否相交
		for( int i = 0; i < 12; i++ )
		{
			//判断一个点是否在一个三角形内
			CVector3f d0 = av[Index[i][0]] - vPt;
			CVector3f d1 = av[Index[i][1]] - vPt;
			CVector3f d2 = av[Index[i][2]] - vPt;

			float z1 = d0.x*d1.y - d0.y*d1.x;
			float z2 = d1.x*d2.y - d1.y*d2.x;
			float z3 = d2.x*d0.y - d2.y*d0.x;

			if( z1 > 0 && ( z2 < 0 || z3 < 0 ) )
				continue; 
			if( z1 < 0 && ( z2 > 0 || z3 > 0 ) )
				continue;

			return true;
		}

		return false;
	}
}

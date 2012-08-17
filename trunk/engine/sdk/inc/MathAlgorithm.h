#pragma once
#include "BaseMath.h"
#include "CommonDefs.h"
#include "CVector2.h"
#include "CVector3.h"
#include "CVector4.h"
#include "CRectangle.h"
#include "CPos.h"
#include "CMatrix.h"


#ifndef _WIN32
#define max(a,b)            (((a) > (b)) ? (a) : (b))
#define min(a,b)            (((a) < (b)) ? (a) : (b))
#endif

namespace sqr
{
//-------------------------------------------------------------
// 画线
// TightLine
//   ██
//     ██
//       ██
// Line
//   █
//     █
//       █
//--------------------------------------------------------------
struct ILineAction
{
	virtual bool Do( int32 x, int32 y ) = 0;
	virtual ~ILineAction(){}
};
bool COMMON_API _TightLine( int32 xSrc, int32 ySrc, int32 xDes, int32 yDes, ILineAction& Action );
bool COMMON_API _Line( int32 xSrc, int32 ySrc, int32 xDes, int32 yDes, ILineAction& Action );

//-------------------------------------------------------------------------------------------
// 球面切割
//
// 将( x >= 0, y >= 0, z >= 0 )的八分之一球面按照Yaw = nYaw切割成均匀的发现分布, 存于pVec处,
// 每行起始点在pVec索引值存于pIndStr,每行个数存于pIndNum处
//------------------------------------------------------------------------------------------
uint32 COMMON_API GetIndexCount( uint32 uYaw );
void   COMMON_API DivSphSurface( CVector3f* pVec, uint32 uYaw, uint32* pIndStr, uint32* pIndNum );
uint32 COMMON_API GetIndexByVec( double yNormal, double zNormal, uint32 uMaxYaw, uint32* pIndStr, uint32* pIndNum );

//----------------------------------------------------------------------------------------------
// 3D碰撞测试
//---------------------------------------------------------------------------------------------
bool	  COMMON_API IsPointIntriangle( const CVector3f& v, const CVector3f& v0, const CVector3f& v1, const CVector3f& v2 );
CVector3f COMMON_API SceneToScreen( CIRect rtScreen, const CVector3f& vScene, const CMatrix& matWorldViewProject );
bool	  COMMON_API DetectInBox( CPos posScreen, CIRect rtScreen, CVector3f vScale, CVector3f vOffset, const CMatrix& matWorldViewProject );

template<class ActionClass>
bool TightLine( int32 xSrc, int32 ySrc, int32 xDes, int32 yDes, ActionClass& Action )
{
	struct CLineAction : public ILineAction
	{
		ActionClass* pAction;
		bool Do( int32 x, int32 y )
		{
			return pAction->Do( x, y );
		}
	};

	CLineAction _Action;
	_Action.pAction = &Action;
	return _TightLine( xSrc, ySrc, xDes, yDes, _Action );
}

template<class ActionClass>
bool Line( int32 xSrc, int32 ySrc, int32 xDes, int32 yDes, ActionClass& Action )
{
	struct CLineAction : public ILineAction
	{
		ActionClass* pAction;
		bool Do( int32 x, int32 y )
		{
			return pAction->Do( x, y );
		}
	};

	CLineAction _Action;
	_Action.pAction = &Action;
	return _Line( xSrc, ySrc, xDes, yDes, _Action );
}

template< class M, class V >
inline void CreateShadowMatrixOnPlane( M& mat, const V& vecLightDir,
                                       const V& vecPlaneNormal, const V& posPointOfPlane )
{
	float fDiv = 1.0f/( vecPlaneNormal.Dot( vecLightDir ) );
	float fDot = vecPlaneNormal.Dot( posPointOfPlane );
	mat._11 = (  vecPlaneNormal.y*vecLightDir.y + vecPlaneNormal.z*vecLightDir.z )*fDiv;
	mat._12 = ( -vecPlaneNormal.x*vecLightDir.y )*fDiv;
	mat._13 = ( -vecPlaneNormal.x*vecLightDir.z )*fDiv;
	mat._14 = 0;
	mat._21 = ( -vecPlaneNormal.y*vecLightDir.x )*fDiv;
	mat._22 = (  vecPlaneNormal.x*vecLightDir.x + vecPlaneNormal.z*vecLightDir.z )*fDiv;
	mat._23 = ( -vecPlaneNormal.y*vecLightDir.z )*fDiv;
	mat._24 = 0;
	mat._31 = ( -vecPlaneNormal.z*vecLightDir.x )*fDiv;
	mat._32 = ( -vecPlaneNormal.z*vecLightDir.y )*fDiv;
	mat._33 = (  vecPlaneNormal.x*vecLightDir.x + vecPlaneNormal.y*vecLightDir.y )*fDiv;
	mat._34 = 0;
	mat._41 = ( fDot*vecLightDir.x )*fDiv;
	mat._42 = ( fDot*vecLightDir.y )*fDiv;
	mat._43 = ( fDot*vecLightDir.z )*fDiv;
	mat._44 = 1;
}

template<class _XYType>
inline bool CutLineXStart( _XYType xMin, _XYType xMax, _XYType& xStart, _XYType& yStart, _XYType xEnd, _XYType yEnd )
{
	_XYType xTempStart = xStart;
	limit<_XYType>( xTempStart, xMin, xMax );

	if ( xTempStart != xStart )
	{
		if ( abs( xEnd - xStart ) < abs( xTempStart - xStart ) )
			return false;

		yStart = ( yEnd - yStart )*( xTempStart - xStart )/( xEnd - xStart ) + yStart ;
		xStart = xTempStart;
	}
	return true;
}

template<class _Rect, class _VECTOR>
inline bool Cut2DLine( const _Rect& rt, _VECTOR& posStart, _VECTOR& posEnd )
{
	return
	    CutLineXStart( rt.left, rt.right, posStart.x, posStart.y, posEnd.x, posEnd.y ) &&
	    CutLineXStart( rt.top,  rt.bottom, posStart.y, posStart.x, posEnd.y, posEnd.x ) &&
	    CutLineXStart( rt.left, rt.right, posEnd.x, posEnd.y, posStart.x, posStart.y ) &&
	    CutLineXStart( rt.top,  rt.bottom, posEnd.y, posEnd.x, posStart.y, posStart.x ) ;
}
}

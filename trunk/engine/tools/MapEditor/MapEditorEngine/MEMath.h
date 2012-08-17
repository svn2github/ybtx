#pragma once
#include "MathAlgorithm.h"


namespace sqr
{
	BOOL  IntersectedPlane(CVector3f vPoly[], CVector3f vLine[], CVector3f &vNormal, float &originDistance);
	float PlaneDistance(CVector3f &Normal, CVector3f &Point);
	void  IntersectionPoint(CVector3f &vNormal, CVector3f vLine[], float distance,CVector3f &vIntersection);
	BOOL  InsidePolygon(CVector3f &vIntersection, CVector3f Poly[]);
	float AngleBetweenVectors(CVector3f &Vector1, CVector3f &Vector2);
	float Magnitude(const CVector3f &vNormal);

	BOOL  LineSegmentIntesectTriangle(CVector3f vLineSegment[2],CVector3f vTriangle[3],CVector3f &vIntersection);
	BOOL  RadialIntersectTriangle( const CVector3f& vOrigin,const CVector3f& vDirection,const CVector3f vTriangle[3],CVector3f &vIntersection,float & fDistance);
	BOOL  RadialIntersectedPlane(const CVector3f &vOrigin,const CVector3f &vDirection,CVector3f vTriangle[3],CVector3f &vIntersection);
	BOOL  RadialIntersectedPlane(const CVector3f &vOrigin,const CVector3f &vDirection,const CPlane &Plane,CVector3f &vIntersection);

	bool  SpherePolygonCollision(CVector3f vPolygon[],CVector3f &vCenter,float radius,CVector3f &vIntersection);
	BOOL  ClassifySphere(CVector3f &vCenter,CVector3f &vNormal, CVector3f &vPoint, float radius, float &distance);
	bool  EdgeSphereCollision(CVector3f &vCenter,CVector3f vPolygon[], int vertexCount, float radius);
	void  ClosestPointOnLine(const CVector3f &vA, const CVector3f &vB, const CVector3f &vPoint,CVector3f &vClosestPoint);
	float Distance(const CVector3f &vPoint1, const CVector3f &vPoint2);
	int	  GetRandomInt(int nLowBound, int nHighBound);
	float GetRandomFloat(float fLowBound, float fHighBound);

	////******************************************************************************
	///*! \fn     T TriInterpolate( T& T0, T& T1, T& T2, T& T3, float x, float y )
	//*   \brief  在T0~T3之间进行三角形插值
	//*
	//*   \param  T& T0, T& T1, T& T2, T& T3	插值参数		T1 _______________________ T3 ( x = 1.0f, y = 1.0f )
	//*			float x, float y				插值位置		|					  	  |
	//*															|					  	  |
	//*															|					  	  |
	//*															|					  	  |
	//*															|( x = 0, y = 0 )	  	  |
	//*															T0 _______________________ T2
	//*
	//*
	//*   \return T								插值结果
	//*******************************************************************************/

	template< class T >
	T TriInterpolate( T& T0, T& T1, T& T2, T& T3, float x, float y )
	{
		//////////////////////////////////////////////////////////////////////////
		//
		//
		//
		//////////////////////////////////////////////////////////////////////////

		if ( y > 1.0f - x )
		{
			x -= 1.0f;
			y -= 1.0f;
			return ( T3 - T1 )*x + ( T3 - T2 )*y + T3;
		}
		else
		{
			return ( T2 - T0 )*x + ( T1 - T0 )*y + T0;
		}
	}

	template< class T >
	T LinearInterpolate(const T& T0, const T& T1, float x)
	{
		return  T0*(1.0f-x) + T1*x;
	}
}
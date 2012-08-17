//////////////////////////////////////////////////////////////////////////
//	
//	Copyright (C) 2003-2004
//  All Rights Reserved
//  In memory of bluely
//  
//////////////////////////////////////////////////////////////////////////

#pragma once

#include "BluelyC.h"
#include <d3d9.h>
#include <d3dx9.h>

namespace bc
{
namespace ut
{

BOOL  IntersectedPlane(D3DXVECTOR3 vPoly[], D3DXVECTOR3 vLine[], D3DXVECTOR3 &vNormal, float &originDistance);
float PlaneDistance(D3DXVECTOR3 &Normal, D3DXVECTOR3 &Point);
void  IntersectionPoint(D3DXVECTOR3 &vNormal, D3DXVECTOR3 vLine[], float distance,D3DXVECTOR3 &vIntersection);
BOOL  InsidePolygon(D3DXVECTOR3 &vIntersection, D3DXVECTOR3 Poly[]);
float AngleBetweenVectors(D3DXVECTOR3 &Vector1, D3DXVECTOR3 &Vector2);
float Magnitude(const D3DXVECTOR3 &vNormal);

BOOL  LineSegmentIntesectTriangle(D3DXVECTOR3 vLineSegment[2],D3DXVECTOR3 vTriangle[3],D3DXVECTOR3 &vIntersection);
BOOL  RadialIntersectTriangle( const D3DXVECTOR3& vOrigin,const D3DXVECTOR3& vDirection,const D3DXVECTOR3 vTriangle[3],D3DXVECTOR3 &vIntersection,float & fDistance);
BOOL  RadialIntersectedPlane(const D3DXVECTOR3 &vOrigin,const D3DXVECTOR3 &vDirection,D3DXVECTOR3 vTriangle[3],D3DXVECTOR3 &vIntersection);
BOOL  RadialIntersectedPlane(const D3DXVECTOR3 &vOrigin,const D3DXVECTOR3 &vDirection,const D3DXPLANE &Plane,D3DXVECTOR3 &vIntersection);

bool  SpherePolygonCollision(D3DXVECTOR3 vPolygon[],D3DXVECTOR3 &vCenter,float radius,D3DXVECTOR3 &vIntersection);
BOOL  ClassifySphere(D3DXVECTOR3 &vCenter,D3DXVECTOR3 &vNormal, D3DXVECTOR3 &vPoint, float radius, float &distance);
bool  EdgeSphereCollision(D3DXVECTOR3 &vCenter,D3DXVECTOR3 vPolygon[], int vertexCount, float radius);
void  ClosestPointOnLine(const D3DXVECTOR3 &vA, const D3DXVECTOR3 &vB, const D3DXVECTOR3 &vPoint,D3DXVECTOR3 &vClosestPoint);
float Distance(const D3DXVECTOR3 &vPoint1, const D3DXVECTOR3 &vPoint2);

}
}
#include "stdafx.h"
#include "Frustum.h"
#include "BluelyMath.h"
#include "Scene.h"

namespace bly
{
void CQuadXZ::Get(D3DXVECTOR3 & vMin, D3DXVECTOR3 & vMax)
{
	vMin = m_vMin;
	vMax = m_vMax;
}

CQuadXZ::CQuadXZ(void) { }

CQuadXZ::~CQuadXZ(void) { }

void CQuadXZ::Set(float fxMin, float fxMax, float fzMin, float fzMax)
{
	m_vMin.x = fxMin;
	m_vMin.y = 0.0f;
	m_vMin.z = fzMin;

	m_vMax.x = fxMax;
	m_vMax.y = 0.0f;
	m_vMax.z = fzMax;

	//3------2
	//|	     |  z
	//|	     |  |
	//0------1  +--x
	m_vCorner[0] = D3DXVECTOR3(m_vMin.x,0.0f,m_vMin.z);
	m_vCorner[1] = D3DXVECTOR3(m_vMax.x,0.0f,m_vMin.z);
	m_vCorner[2] = D3DXVECTOR3(m_vMax.x,0.0f,m_vMax.z);
	m_vCorner[3] = D3DXVECTOR3(m_vMin.x,0.0f,m_vMax.z);
}

//////////////////////////////////////////////////////////////////////////

static const int iTrianglesIndices[3 * 12] =
{
	0,2,1, 0,3,2, 4,6,5, 4,7,6,
	1,5,6, 1,6,2, 0,4,7, 0,7,3,
	0,4,5, 0,5,1, 3,7,6, 3,6,3
};

static const int iEdgeIndices[2*12] =
{
	0,1,
	1,2,
	2,3,
	3,0,
	0,4,
	1,5,
	2,6,
	3,7,
	4,5,
	5,6,
	6,7,
	7,4
};

void CBoundingBox::operator = (const CBoundingBox & cBoundingBox)
{
	memcpy(&m_vCenter,&cBoundingBox.m_vCenter,sizeof(D3DXVECTOR3));
	m_fOutsideRadius = cBoundingBox.m_fOutsideRadius;
	m_vMin.x = cBoundingBox.m_vMin.x;
	m_vMin.y = cBoundingBox.m_vMin.y;
	m_vMin.z = cBoundingBox.m_vMin.z;
	m_vMax.x = cBoundingBox.m_vMax.x;
	m_vMax.y = cBoundingBox.m_vMax.y;
	m_vMax.z = cBoundingBox.m_vMax.z;
	memcpy(m_vCorner,cBoundingBox.m_vCorner,sizeof(D3DXVECTOR3) * 8);
	memcpy(m_plane,cBoundingBox.m_plane,sizeof(D3DXPLANE) * 6);
}

void CBoundingBox::Set(const D3DXVECTOR3 &vCenter,float fRadius)
{
	m_vMin.x = vCenter.x - fRadius;
	m_vMin.y = vCenter.y - fRadius;
	m_vMin.z = vCenter.z - fRadius;

	m_vMax.x = vCenter.x + fRadius;
	m_vMax.y = vCenter.y + fRadius;
	m_vMax.z = vCenter.z + fRadius;

	float xmin=m_vMin.x,ymin=m_vMin.y,zmin=m_vMin.z;
	float xmax=m_vMax.x,ymax=m_vMax.y,zmax=m_vMax.z;

	//          7__________6
	//         /		  /|
	//        /			 / |
	//       /			/  |
	//		4----------5   |
	//		|		   |   |
	//		|	3	   | 2 /
	//		|		   |  /
	//		|0		  1| /
	//		------------

	m_vCorner[0]=D3DXVECTOR3(xmin,ymin,zmin);
	m_vCorner[1]=D3DXVECTOR3(xmax,ymin,zmin);
	m_vCorner[2]=D3DXVECTOR3(xmax,ymin,zmax);
	m_vCorner[3]=D3DXVECTOR3(xmin,ymin,zmax);
	m_vCorner[4]=D3DXVECTOR3(xmin,ymax,zmin);
	m_vCorner[5]=D3DXVECTOR3(xmax,ymax,zmin);
	m_vCorner[6]=D3DXVECTOR3(xmax,ymax,zmax);
	m_vCorner[7]=D3DXVECTOR3(xmin,ymax,zmax);

	// build plane
	D3DXPlaneFromPoints( &m_plane[0], &m_vCorner[0],&m_vCorner[2], &m_vCorner[1] );
	D3DXPlaneFromPoints( &m_plane[1], &m_vCorner[0],&m_vCorner[5], &m_vCorner[4] );
	D3DXPlaneFromPoints( &m_plane[2], &m_vCorner[1],&m_vCorner[6], &m_vCorner[5] );
	D3DXPlaneFromPoints( &m_plane[3], &m_vCorner[6],&m_vCorner[3], &m_vCorner[7] );
	D3DXPlaneFromPoints( &m_plane[4], &m_vCorner[7],&m_vCorner[0], &m_vCorner[4] );
	D3DXPlaneFromPoints( &m_plane[5], &m_vCorner[4],&m_vCorner[6], &m_vCorner[7] );

	// bounding sphere radius
	D3DXVECTOR3 vDistance = (m_vMax - m_vMin)/2;
	m_fOutsideRadius = sqrtf(vDistance.x * vDistance.x + vDistance.y * vDistance.y + vDistance.z * vDistance.z);

	// center point
	m_vCenter = (m_vMin + m_vMax)/2;
}

void CBoundingBox::Set(const D3DXVECTOR3 & vMin, const D3DXVECTOR3 & vMax)
{
	m_vMin = vMin;
	m_vMax = vMax;
	float xmin=vMin.x,ymin=vMin.y,zmin=vMin.z;
	float xmax=vMax.x,ymax=vMax.y,zmax=vMax.z;

	//          7__________6
	//         /		  /|
	//        /			 / |
	//       /			/  |
	//		4----------5   |
	//		|		   |   |
	//		|	3	   | 2 /
	//		|		   |  /
	//		|0		  1| /
	//		------------

	m_vCorner[0]=D3DXVECTOR3(xmin,ymin,zmin);
	m_vCorner[1]=D3DXVECTOR3(xmax,ymin,zmin);
	m_vCorner[2]=D3DXVECTOR3(xmax,ymin,zmax);
	m_vCorner[3]=D3DXVECTOR3(xmin,ymin,zmax);
	m_vCorner[4]=D3DXVECTOR3(xmin,ymax,zmin);
	m_vCorner[5]=D3DXVECTOR3(xmax,ymax,zmin);
	m_vCorner[6]=D3DXVECTOR3(xmax,ymax,zmax);
	m_vCorner[7]=D3DXVECTOR3(xmin,ymax,zmax);

	D3DXPlaneFromPoints( &m_plane[0], &m_vCorner[0],&m_vCorner[2], &m_vCorner[1] );
	D3DXPlaneFromPoints( &m_plane[1], &m_vCorner[0],&m_vCorner[5], &m_vCorner[4] );
	D3DXPlaneFromPoints( &m_plane[2], &m_vCorner[1],&m_vCorner[6], &m_vCorner[5] );
	D3DXPlaneFromPoints( &m_plane[3], &m_vCorner[6],&m_vCorner[3], &m_vCorner[7] );
	D3DXPlaneFromPoints( &m_plane[4], &m_vCorner[7],&m_vCorner[0], &m_vCorner[4] );
	D3DXPlaneFromPoints( &m_plane[5], &m_vCorner[4],&m_vCorner[6], &m_vCorner[7] );

	D3DXVECTOR3 vDistance = (m_vMax - m_vMin)/2;
	m_fOutsideRadius = sqrtf(vDistance.x * vDistance.x + vDistance.y * vDistance.y + vDistance.z * vDistance.z);
	m_vCenter = (m_vMin + m_vMax)/2;
}

void CBoundingBox::Set(float x, float X, float y, float Y, float z, float Z)
{
	m_vMin.x = x;
	m_vMin.y = y;
	m_vMin.z = z;
	m_vMax.x = X;
	m_vMax.y = Y;
	m_vMax.z = Z;

	//          7__________6
	//         /		  /|
	//        /			 / |
	//       /			/  |
	//		4----------5   |
	//		|		   |   |
	//		|	3	   | 2 /
	//		|		   |  /
	//		|0		  1| /
	//		------------

	m_vCorner[0]=D3DXVECTOR3(x,y,z);
	m_vCorner[1]=D3DXVECTOR3(X,y,z);
	m_vCorner[2]=D3DXVECTOR3(X,y,Z);
	m_vCorner[3]=D3DXVECTOR3(x,y,Z);
	m_vCorner[4]=D3DXVECTOR3(x,Y,z);
	m_vCorner[5]=D3DXVECTOR3(X,Y,z);
	m_vCorner[6]=D3DXVECTOR3(X,Y,Z);
	m_vCorner[7]=D3DXVECTOR3(x,Y,Z);

	D3DXPlaneFromPoints( &m_plane[0], &m_vCorner[0],&m_vCorner[2], &m_vCorner[1] );
	D3DXPlaneFromPoints( &m_plane[1], &m_vCorner[0],&m_vCorner[5], &m_vCorner[4] );
	D3DXPlaneFromPoints( &m_plane[2], &m_vCorner[1],&m_vCorner[6], &m_vCorner[5] );
	D3DXPlaneFromPoints( &m_plane[3], &m_vCorner[6],&m_vCorner[3], &m_vCorner[7] );
	D3DXPlaneFromPoints( &m_plane[4], &m_vCorner[7],&m_vCorner[0], &m_vCorner[4] );
	D3DXPlaneFromPoints( &m_plane[5], &m_vCorner[4],&m_vCorner[6], &m_vCorner[7] );

	D3DXVECTOR3 vDistance = (m_vMax - m_vMin)/2;
	m_fOutsideRadius = sqrtf(vDistance.x * vDistance.x + vDistance.y * vDistance.y + vDistance.z * vDistance.z);
	m_vCenter = (m_vMin + m_vMax)/2;
}

bool CBoundingBox::TestPointInside(D3DXVECTOR3 * pvPoint)
{
	float x = pvPoint->x, y = pvPoint->y, z = pvPoint->z;
	for ( int i = 0; i < 6; i++ )
	{
		if ( m_plane[i].a * x + m_plane[i].b * y + m_plane[i].c * z + m_plane[i].d < 0 )
		{
			return  false;
		}
	}
	return true;
}

void CBoundingBox::Get(D3DXVECTOR3 & vMin, D3DXVECTOR3 & vMax)
{
	vMin = m_vMin;
	vMax = m_vMax;
}

bool CBoundingBox::TestLineSegmentIntersected(D3DXVECTOR3 vLineSegment[2], D3DXVECTOR3 & vIntersection)
{
	D3DXVECTOR3 vTriangle[3];

	// test if the line intersects with any triangle
	for ( int iTriangle = 0; iTriangle < 12; iTriangle++ )
	{
		vTriangle[0] = m_vCorner[iTrianglesIndices[iTriangle * 3]];
		vTriangle[1] = m_vCorner[iTrianglesIndices[iTriangle * 3 + 1]];
		vTriangle[2] = m_vCorner[iTrianglesIndices[iTriangle * 3 + 2]];

		if ( bly::LineSegmentIntesectTriangle(vLineSegment,vTriangle,vIntersection) )
			return true;
	}

	return false;
}

bool CBoundingBox::TestRadialIntersected(D3DXVECTOR3 & vOrigin, D3DXVECTOR3 & vDirection)
{
	D3DXVECTOR3 vIntersection;
	float fDistance;
	D3DXVECTOR3 vTriangle[3];

	// same as before
	for ( int iTriangle = 0; iTriangle < 12; iTriangle++ )
	{
		vTriangle[0] = m_vCorner[iTrianglesIndices[iTriangle * 3]];
		vTriangle[1] = m_vCorner[iTrianglesIndices[iTriangle * 3 + 1]];
		vTriangle[2] = m_vCorner[iTrianglesIndices[iTriangle * 3 + 2]];

		if ( bly::RadialIntersectTriangle(vOrigin,vDirection,vTriangle,vIntersection,fDistance))
			return true;
	}
	return false;
}

bool CBoundingBox::TestTriangleIntersected(D3DXVECTOR3 vTriangle[3])
{
	D3DXVECTOR3 vIntersection;

	D3DXVECTOR3 vEdge[2];

	// test 12 edges first
	for (int iEdge = 0; iEdge < 12; iEdge++)
	{
		vEdge[0] = m_vCorner[iEdgeIndices[iEdge * 2]];
		vEdge[1] = m_vCorner[iEdgeIndices[iEdge * 2 + 1]];

		if ( bly::LineSegmentIntesectTriangle(vEdge,vTriangle,vIntersection))
			return true;
	}

	// in case the triangle doesn't across any edge
	vEdge[0] = vTriangle[2];
	vEdge[1] = vTriangle[0];

	if ( TestLineSegmentIntersected(&vTriangle[0],vIntersection))
		return true;
	if ( TestLineSegmentIntersected(&vTriangle[1],vIntersection))
		return true;
	if ( TestLineSegmentIntersected(&vEdge[0],vIntersection))
		return true;

	return false;
}

bool CBoundingBox::TestSphereIntersected(D3DXVECTOR3 & vCenter,float fRadius)
{
	// get the distance
	D3DXVECTOR3 vTemp = m_vCenter - vCenter;
	float fDistance = sqrtf(vTemp.x * vTemp.x + vTemp.y * vTemp.y + vTemp.z * vTemp.z);

	if ( fDistance <= fRadius + m_fOutsideRadius )
	{
		return true;
	}
	return false;
}

////////////////////////////////////////////////////////////////////////

CFrustum::CFrustum()
{
	m_fYMin = FLT_MAX;
	m_fYMax = -FLT_MAX;
	m_fXMin = FLT_MAX;
	m_fXMax = -FLT_MAX;
	m_fZMin = FLT_MAX;
	m_fZMax = -FLT_MAX;
	m_dwHeightMask = 0;


}

DWORD YMASK(float fMin, float fMax)
{
	BYTE ymax = BYTE((fMax/HEIGHT_LIMIT + 1.0f)*HEIGHT_LEVEL/2);
	BYTE ymin = BYTE((fMin/HEIGHT_LIMIT + 1.0f)*HEIGHT_LEVEL/2);

	DWORD high = (1<<ymax);
	DWORD low = (1<<ymin);
	DWORD setMask = high-1;
	DWORD clearMask = low-1;

	DWORD result = setMask;

	if (ymin != ymax)
	{
		result &= ~clearMask;
	}

	result |= high;
	result |= low;

	return result;
}

bool CFrustum::HighEnough(D3DXVECTOR3 & v)
{
	return  ( m_planeFrustum[5].a * v.x + m_planeFrustum[5].b * v.y + m_planeFrustum[5].c * v.z + m_planeFrustum[5].d > 0 );
}

void CFrustum::CalculateFrustum(const D3DXMATRIX * pView, const D3DXMATRIX * pProjection, bool bMontain)
{
	// CAUTION£º here i just calcuate an adjusted frustum

	D3DXMATRIX mat;

	D3DXMatrixMultiply(&mat, pView, pProjection);
	D3DXMatrixInverse(&mat, NULL, &mat);

	m_vFrustum[0] = D3DXVECTOR3(-1.0f, -1.0f,  0.0f); // xyz
	m_vFrustum[1] = D3DXVECTOR3( 1.0f, -1.0f,  0.0f); // Xyz
	m_vFrustum[2] = D3DXVECTOR3(-1.0f,  1.0f,  0.0f); // xYz
	m_vFrustum[3] = D3DXVECTOR3( 1.0f,  1.0f,  0.0f); // XYz
	m_vFrustum[4] = D3DXVECTOR3(-1.0f, -1.0f,  1.0f); // xyZ
	m_vFrustum[5] = D3DXVECTOR3( 1.0f, -1.0f,  1.0f); // XyZ
	m_vFrustum[6] = D3DXVECTOR3(-1.0f,  1.0f,  1.0f); // xYZ
	m_vFrustum[7] = D3DXVECTOR3( 1.0f,  1.0f,  1.0f); // XYZ

	if ( !bMontain )
	{
		for ( int i = 0; i < 8; i++ )
		{
			D3DXVec3TransformCoord(&m_vFrustum[i], &m_vFrustum[i], &mat);
		}
	}
	else
	{
		D3DXVec3TransformCoord(&m_vFrustum[7], &m_vFrustum[7], &mat);
		D3DXVec3TransformCoord(&m_vFrustum[6], &m_vFrustum[6], &mat);
		D3DXVec3TransformCoord(&m_vFrustum[0], &m_vFrustum[0], &mat);
		D3DXVec3TransformCoord(&m_vFrustum[1], &m_vFrustum[1], &mat);

		m_vFrustum[0].y = m_vFrustum[1].y = -GRID_SPACE/2;
		m_vFrustum[6].y = m_vFrustum[7].y = GRID_SPACE/2;

		static D3DXVECTOR3 vYStrid = D3DXVECTOR3(0.0f, GRID_SPACE, 0.0f);

		m_vFrustum[2] = m_vFrustum[0] + vYStrid; // xYz
		m_vFrustum[3] = m_vFrustum[1] + vYStrid; // XYz

		m_vFrustum[4] = m_vFrustum[6] - vYStrid; // xyZ
		m_vFrustum[5] = m_vFrustum[7] - vYStrid; // XyZ
	}

	D3DXPlaneFromPoints( &m_planeFrustum[0], &m_vFrustum[0],&m_vFrustum[1], &m_vFrustum[2] ); // Near
	D3DXPlaneFromPoints( &m_planeFrustum[1], &m_vFrustum[6],&m_vFrustum[7], &m_vFrustum[5] ); // Far
	D3DXPlaneFromPoints( &m_planeFrustum[2], &m_vFrustum[2],&m_vFrustum[6], &m_vFrustum[4] ); // Left
	D3DXPlaneFromPoints( &m_planeFrustum[3], &m_vFrustum[7],&m_vFrustum[3], &m_vFrustum[5] ); // Right
	D3DXPlaneFromPoints( &m_planeFrustum[4], &m_vFrustum[2],&m_vFrustum[3], &m_vFrustum[6] ); // Top
	D3DXPlaneFromPoints( &m_planeFrustum[5], &m_vFrustum[1],&m_vFrustum[0], &m_vFrustum[4] ); // Bottom
}

bool CFrustum::BoxIntersectFrustum(const D3DXMATRIX * pWorldMatrix,const CBoundingBox * pBoundingBox)
{
	static D3DXVECTOR3 vCorner[8];
	const D3DXVECTOR3 * pvCorner = pBoundingBox->GetCornerVertex();

	for ( int v = 0; v < 8; v++ )
	{
		D3DXVec3TransformCoord(&vCorner[v], &pvCorner[v], pWorldMatrix);
	}

	float a,b,c,d;

	for ( int i = 0; i < 6; i++ )
	{
		a = m_planeFrustum[i].a;
		b = m_planeFrustum[i].b;
		c = m_planeFrustum[i].c;
		d = m_planeFrustum[i].d;

		if ( a * vCorner[0].x + b * vCorner[0].y + c * vCorner[0].z + d > 0 )
			continue;
		if ( a * vCorner[1].x + b * vCorner[1].y + c * vCorner[1].z + d > 0 )
			continue;
		if ( a * vCorner[2].x + b * vCorner[2].y + c * vCorner[2].z + d > 0 )
			continue;
		if ( a * vCorner[3].x + b * vCorner[3].y + c * vCorner[3].z + d > 0 )
			continue;
		if ( a * vCorner[4].x + b * vCorner[4].y + c * vCorner[4].z + d > 0 )
			continue;
		if ( a * vCorner[5].x + b * vCorner[5].y + c * vCorner[5].z + d > 0 )
			continue;
		if ( a * vCorner[6].x + b * vCorner[6].y + c * vCorner[6].z + d > 0 )
			continue;
		if ( a * vCorner[7].x + b * vCorner[7].y + c * vCorner[7].z + d > 0 )
			continue;

		// if we get here, it isn't in the frustum
		return false;
	}
	return true;
}

bool CFrustum::BoxIntersectFrustum(const CBoundingBox * pBoundingBox)
{
	const D3DXVECTOR3 * pvCorner = pBoundingBox->GetCornerVertex();

	float a,b,c,d;

	for ( int i = 0; i < 6; i++ )
	{
		a = m_planeFrustum[i].a;
		b = m_planeFrustum[i].b;
		c = m_planeFrustum[i].c;
		d = m_planeFrustum[i].d;

		if ( a * pvCorner[0].x + b * pvCorner[0].y + c * pvCorner[0].z + d > 0 )
			continue;
		if ( a * pvCorner[1].x + b * pvCorner[1].y + c * pvCorner[1].z + d > 0 )
			continue;
		if ( a * pvCorner[2].x + b * pvCorner[2].y + c * pvCorner[2].z + d > 0 )
			continue;
		if ( a * pvCorner[3].x + b * pvCorner[3].y + c * pvCorner[3].z + d > 0 )
			continue;
		if ( a * pvCorner[4].x + b * pvCorner[4].y + c * pvCorner[4].z + d > 0 )
			continue;
		if ( a * pvCorner[5].x + b * pvCorner[5].y + c * pvCorner[5].z + d > 0 )
			continue;
		if ( a * pvCorner[6].x + b * pvCorner[6].y + c * pvCorner[6].z + d > 0 )
			continue;
		if ( a * pvCorner[7].x + b * pvCorner[7].y + c * pvCorner[7].z + d > 0 )
			continue;

		// if we get here, it isn't in the frustum
		return false;
	}
	return true;
}

bool CFrustum::BoxIntersectFrustum(D3DXVECTOR3 * pvCorners)
{
	float a,b,c,d;

	for ( int i = 0; i < 6; i++ )
	{
		a = m_planeFrustum[i].a;
		b = m_planeFrustum[i].b;
		c = m_planeFrustum[i].c;
		d = m_planeFrustum[i].d;

		if ( a * pvCorners[0].x + b * pvCorners[0].y + c * pvCorners[0].z + d > 0 )
			continue;
		if ( a * pvCorners[1].x + b * pvCorners[1].y + c * pvCorners[1].z + d > 0 )
			continue;
		if ( a * pvCorners[2].x + b * pvCorners[2].y + c * pvCorners[2].z + d > 0 )
			continue;
		if ( a * pvCorners[3].x + b * pvCorners[3].y + c * pvCorners[3].z + d > 0 )
			continue;
		if ( a * pvCorners[4].x + b * pvCorners[4].y + c * pvCorners[4].z + d > 0 )
			continue;
		if ( a * pvCorners[5].x + b * pvCorners[5].y + c * pvCorners[5].z + d > 0 )
			continue;
		if ( a * pvCorners[6].x + b * pvCorners[6].y + c * pvCorners[6].z + d > 0 )
			continue;
		if ( a * pvCorners[7].x + b * pvCorners[7].y + c * pvCorners[7].z + d > 0 )
			continue;

		// if we get here, it isn't in the frustum
		return false;
	}
	return true;
}

bool CFrustum::BoxInFrustum(const CBoundingBox * pBoundingBox)
{
	const D3DXVECTOR3 * pvCorner = pBoundingBox->GetCornerVertex();

	float a,b,c,d;

	for ( int i = 0; i < 6; i++ )
	{
		a = m_planeFrustum[i].a;
		b = m_planeFrustum[i].b;
		c = m_planeFrustum[i].c;
		d = m_planeFrustum[i].d;

		if ( a * pvCorner[0].x + b * pvCorner[0].y + c * pvCorner[0].z + d < 0 )
			return false;
		if ( a * pvCorner[1].x + b * pvCorner[1].y + c * pvCorner[1].z + d < 0 )
			return false;
		if ( a * pvCorner[2].x + b * pvCorner[2].y + c * pvCorner[2].z + d < 0 )
			return false;
		if ( a * pvCorner[3].x + b * pvCorner[3].y + c * pvCorner[3].z + d < 0 )
			return false;
		if ( a * pvCorner[4].x + b * pvCorner[4].y + c * pvCorner[4].z + d < 0 )
			return false;
		if ( a * pvCorner[5].x + b * pvCorner[5].y + c * pvCorner[5].z + d < 0 )
			return false;
		if ( a * pvCorner[6].x + b * pvCorner[6].y + c * pvCorner[6].z + d < 0 )
			return false;
		if ( a * pvCorner[7].x + b * pvCorner[7].y + c * pvCorner[7].z + d < 0 )
			return false;
	}
	return true;
}

bool CFrustum::QuadXZIntersectFrustum(CQuadXZ * pQuadXZ, DWORD dwHeightMask)
{
	const D3DXVECTOR3 * pvCorner = pQuadXZ->GetCorner();

	for ( int i = 0; i < 6; i++ )
	{
		if ( m_planeFrustum[i].a * pvCorner[0].x + m_planeFrustum[i].b * pvCorner[0].y + m_planeFrustum[i].c * pvCorner[0].z + m_planeFrustum[i].d > 0 )
			continue;
		if ( m_planeFrustum[i].a * pvCorner[1].x + m_planeFrustum[i].b * pvCorner[1].y + m_planeFrustum[i].c * pvCorner[1].z + m_planeFrustum[i].d > 0 )
			continue;
		if ( m_planeFrustum[i].a * pvCorner[2].x + m_planeFrustum[i].b * pvCorner[2].y + m_planeFrustum[i].c * pvCorner[2].z + m_planeFrustum[i].d > 0 )
			continue;
		if ( m_planeFrustum[i].a * pvCorner[3].x + m_planeFrustum[i].b * pvCorner[3].y + m_planeFrustum[i].c * pvCorner[3].z + m_planeFrustum[i].d > 0 )
			continue;

		// if we get here, it isn't in the frustum
		return false;
	}

	return true;
}

bool CFrustum::PointInFrustum(const D3DXVECTOR3 * pPoint)
{
	float x = pPoint->x, y = pPoint->y , z = pPoint->z;

	for ( int i = 0; i < 6; i++ )
	{
		if ( m_planeFrustum[i].a * x + m_planeFrustum[i].b * y + m_planeFrustum[i].c * z + m_planeFrustum[i].d < 0 )
		{
			return false;
		}
	}
	return true;
}

bool CFrustum::QuadXZInFrustum(CQuadXZ * pQuadXZ, DWORD dwHeightMask)
{
	const D3DXVECTOR3 * pos = pQuadXZ->GetCorner();

	for ( int i = 0; i < 6; i++ )
	{
		if ( m_planeFrustum[i].a * pos[0].x +  m_planeFrustum[i].c * pos[0].z + m_planeFrustum[i].d < 0 )
			return false;
		if ( m_planeFrustum[i].a * pos[1].x +  m_planeFrustum[i].c * pos[1].z + m_planeFrustum[i].d < 0 )
			return false;
		if ( m_planeFrustum[i].a * pos[2].x +  m_planeFrustum[i].c * pos[2].z + m_planeFrustum[i].d < 0 )
			return false;
		if ( m_planeFrustum[i].a * pos[3].x +  m_planeFrustum[i].c * pos[3].z + m_planeFrustum[i].d < 0 )
			return false;
	}

	return true;
}
}
//////////////////////////////////////////////////////////////////////////
//	
//	Copyright (C) 2003-2004
//  All Rights Reserved
//  In memory of bluely
//  
//////////////////////////////////////////////////////////////////////////

#include "Render.h"
#include "../Bluely Common/Console.h"
#include "../Bluely Common/BluelyMath.h"
#include "../Bluely Common/memfile.h"

#if defined(_DEBUG) && defined(_WIN32)
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#endif

//using namespace bc::ut;
//using namespace bc::io;

namespace bg
{
namespace rd
{

CRenderDevice * CRenderDevice::one = NULL;

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

void CBoundingBox::_DEBUG_Render(const D3DXMATRIX * pMatWorld,DWORD dwColor)
{
	CRenderDevice * pRender = CRenderDevice::GetInstance();
	
	//@ CRenderDevice::Layer3D * pLayer3D  = pRender->GetLayer3D();

	static D3DXVECTOR3 vPos[8];
	for ( DWORD n = 0; n < 8; n++ )
	{
		D3DXVec3TransformCoord(&vPos[n],&m_vCorner[n],pMatWorld);
	}

	for ( DWORD dwLine = 0; dwLine < 12; dwLine++ )
	{
		//@ pLayer3D->_DEBUG_DrawLine(&vPos[iEdgeIndices[dwLine*2]], &vPos[iEdgeIndices[dwLine*2+1]], dwColor);	
	}
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

		if ( bc::ut::LineSegmentIntesectTriangle(vLineSegment,vTriangle,vIntersection) )
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

		if ( bc::ut::RadialIntersectTriangle(vOrigin,vDirection,vTriangle,vIntersection,fDistance))
			return true;
	}
	return false;
}

bool CBoundingBox::TestTriangleIntersected(D3DXVECTOR3 vTriangle[3])
{
	D3DXVECTOR3 vIntersection;
	
	D3DXVECTOR3 vEdge[2];

	// test 12 edges first
	for(int iEdge = 0; iEdge < 12; iEdge++)
	{
		vEdge[0] = m_vCorner[iEdgeIndices[iEdge * 2]];
		vEdge[1] = m_vCorner[iEdgeIndices[iEdge * 2 + 1]];

		if ( bc::ut::LineSegmentIntesectTriangle(vEdge,vTriangle,vIntersection))
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

void CFrustum::CalculateFrustum(const D3DXMATRIX * pView, const D3DXMATRIX * pProjection)
{    
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

	for( int i = 0; i < 8; i++ )
	{
		D3DXVec3TransformCoord(&m_vFrustum[i], &m_vFrustum[i], &mat);
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
		a = m_planeFrustum[i].a; b = m_planeFrustum[i].b; c = m_planeFrustum[i].c; d = m_planeFrustum[i].d;

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
		a = m_planeFrustum[i].a; b = m_planeFrustum[i].b; c = m_planeFrustum[i].c; d = m_planeFrustum[i].d;

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


void CFrustum::_DEBUG_Render()
{
	CRenderDevice * pRender = CRenderDevice::GetInstance();

	//@ CRenderDevice::Layer3D * pLayer3D = pRender->GetLayer3D();

	static const int iIndices[2*12] = 
	{
		0,1,
			1,3,
			3,2,
			2,0,
			2,6,
			6,7,
			7,3,
			7,5,
			6,4,
			0,4,
			4,5,
			5,1,
	};

	for ( int iLine = 0; iLine < 12; iLine++ )
	{
		//@ pLayer3D->_DEBUG_DrawLine(&m_vFrustum[iIndices[iLine*2]], &m_vFrustum[iIndices[iLine*2+1]], D3DCOLOR_RGBA(0,0,255,255));	
	}
}

bool CFrustum::BoxInFrustum(const CBoundingBox * pBoundingBox)
{
	const D3DXVECTOR3 * pvCorner = pBoundingBox->GetCornerVertex();

	float a,b,c,d;

	for ( int i = 0; i < 6; i++ )
	{
		a = m_planeFrustum[i].a; b = m_planeFrustum[i].b; c = m_planeFrustum[i].c; d = m_planeFrustum[i].d;

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

bool CFrustum::QuadXZIntersectFrustum(CQuadXZ * pQuadXZ)
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

bool CFrustum::QuadXZInFrustum(CQuadXZ * pQuadXZ)
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

////////////////////////////////////////////////////////////////////////

CTexture::CTexture(void)
{
	m_pBitmap = NULL;
	m_pD3DTexture = NULL;
}

CTexture::~CTexture(void) { }

bool CTexture::Create(LPDIRECT3DDEVICE9 pDevice, const char * pszFileName, bool bUse32BitTex, DWORD dwMaxTextureWidth, DWORD dwMaxTextureHeight)
{
	assert(pszFileName);

	bool bResult = false;

	m_d3dFormat = (bUse32BitTex) ? D3DFMT_A8R8G8B8 : D3DFMT_A4R4G4B4;
	
	if ( strstr(pszFileName,".tga") || strstr(pszFileName,".TGA") )
	{
		bResult = BitmapReadFileTGA(pszFileName);
	}
	else if ( strstr(pszFileName,".bmp") || strstr(pszFileName,".BMP") )
	{
		bResult = BitmapReadFileBMP(pszFileName);
	}
	else
	{
		LogConsole("error: CTexture::Create : unsupported pic format...");
		assert(0);
		return false;
	}

	//else if ( strstr(pszFileName,".bmp") || strstr(pszFileName,".BMP") )
	//	bResult = BitmapReadFileBMP(pszFileName);
	//else if ( strstr(pszFileName,".psd") || strstr(pszFileName,".PSD") )
	//	bResult = BitmapReadFilePSD(pszFileName);
	//else if ( strstr(pszFileName,".pcx") || strstr(pszFileName,".PCX") )
	//	bResult = BitmapReadFilePCX(pszFileName);
	//else if ( strstr(pszFileName,".ppm") || strstr(pszFileName,".PPM") )
	//	bResult = BitmapReadFilePPM(pszFileName);
	//else if ( strstr(pszFileName,".jpg") || strstr(pszFileName,".JPG") )
	//	bResult = BitmapReadFileJPG(pszFileName);
	
	if ( !bResult )
	{
		LogConsole("error: CTexture : read texture failed...");
		assert(0);
		return false;
	}

	if ( m_dwWidth > dwMaxTextureWidth || m_dwHeight > dwMaxTextureHeight )
	{
		LogConsole("error: CTexture: texture size dismatch...");
		DestroyBitmap();
		assert(0);
		return false;
	}

	if ( !CreateD3DTexture(pDevice) )
	{
		LogConsole("error: CTexture: create d3d texture failed...");
		DestroyBitmap();
		assert(0);
		return false;
	}

	if ( !CopyBitmapToTexture() )
	{
		LogConsole("error: CTexture: copy bitmap to texture failed");
		DestroyBitmap();
		DestroyD3DTexture();
		assert(0);
		return false;
	}

	return true;
}

bool CTexture::Create(LPDIRECT3DDEVICE9 pDevice,DWORD dwWidth,DWORD dwHeight,DWORD dwColor,bool bUse32BitTex,DWORD dwMaxTextureWidth,DWORD dwMaxTextureHeight)
{
	m_d3dFormat = (bUse32BitTex) ? D3DFMT_A8R8G8B8 : D3DFMT_A4R4G4B4;

	if ( dwWidth > dwMaxTextureWidth || dwHeight > dwMaxTextureHeight )
	{
		LogConsole("error: CTexture: texture size dismatch...");
		DestroyBitmap();
		assert(0);
		return false;
	}

	if ( !CreateBitmap(dwWidth,dwHeight,dwColor) )
	{
		LogConsole("error: CTexture : create bitmap failed...");
		assert(0);
		return false;
	}

	if ( !CreateD3DTexture(pDevice) )
	{
		LogConsole("error: CTexture: create d3d texture failed...");
		DestroyBitmap();
		assert(0);
		return false;
	}

	if ( !CopyBitmapToTexture() )
	{
		LogConsole("error: CTexture: copy bitmap to texture failed...");
		DestroyBitmap();
		DestroyD3DTexture();
		assert(0);
		return false;
	}

	return true;
}

bool CTexture::Create(LPDIRECT3DDEVICE9 pDevice,DWORD dwWidth,DWORD dwHeight,DWORD * pBitmap,bool bUse32BitTex,DWORD dwMaxTextureWidth,DWORD dwMaxTextureHeight)
{
	m_d3dFormat = (bUse32BitTex) ? D3DFMT_A8R8G8B8 : D3DFMT_A4R4G4B4;
	
	if ( dwWidth > dwMaxTextureWidth || dwHeight > dwMaxTextureHeight )
	{
		LogConsole("error: CTexture: texture size dismatch...");
		DestroyBitmap();
		assert(0);
		return false;
	}

	if ( !CreateBitmap(dwWidth,dwHeight,0x00000000) )
	{
		LogConsole("error: CTexture : create bitmap failed");
		assert(0);
		return false;
	}

	memcpy(m_pBitmap,pBitmap,sizeof(DWORD) * dwWidth * dwHeight);

	if ( !CreateD3DTexture(pDevice) )
	{
		LogConsole("error: CTexture: create d3d texture failed...");
		DestroyBitmap();
		assert(0);
		return false;
	}

	if ( !CopyBitmapToTexture() )
	{
		LogConsole("error: CTexture: copy bitmap to texture failed...");
		DestroyBitmap();
		DestroyD3DTexture();
		assert(0);
		return false;
	}

	return true;
}

void CTexture::Destroy()
{
	DestroyD3DTexture();
	DestroyBitmap();	
}

bool CTexture::CopyBitmapToTexture()
{
	HRESULT hResult;
	D3DLOCKED_RECT lr;
	
	if ( FAILED(hResult = m_pD3DTexture->LockRect(0,&lr,NULL,0)) )
	{
		LogConsole("error: CTexture: lock texture failed...");
		assert(0);
		return false;
	}

	switch ( m_d3dFormat )
	{
	case D3DFMT_A8R8G8B8:
		{
			// fill image to Texture
			DWORD * pDest = (DWORD*)lr.pBits;
			int  iPitch = lr.Pitch / sizeof(DWORD);
			DWORD * pSrc  = m_pBitmap;

			DWORD dwHeight = m_dwHeight;
			while ( dwHeight-- )
			{
				memcpy(pDest,pSrc,m_dwWidth * sizeof(DWORD));
				pDest += iPitch;
				pSrc += m_dwWidth;
			}	
		}
	break;
	
	case D3DFMT_A4R4G4B4:
		{
			// fill image to Texture
			WORD * pDest = (WORD*)lr.pBits;
			int  iPitch = lr.Pitch / sizeof(WORD);
			DWORD * pSrc  = m_pBitmap;

			DWORD dwHeight = m_dwHeight;

			while ( dwHeight-- )
			{
				for ( DWORD i = 0; i < m_dwWidth; i++ )
				{
					DWORD c = pSrc[i];

					pDest[i] = (WORD)( ((c & 0xf0000000) >> 16) | ((c & 0x00f00000) >> 12) 
						| ((c & 0x0000f000) >> 8) | ((c & 0x000000f0) >> 4));
				}
				pDest += iPitch;
				pSrc += m_dwWidth;
			}	
		}
	break;
	}

	m_pD3DTexture->UnlockRect(0);

	return true;
}

bool CTexture::CreateBitmap(DWORD dwWidth, DWORD dwHeight, DWORD dwColor)
{
	m_dwWidth  = dwWidth;
	m_dwHeight = dwHeight;
	m_dwBitmapSize = dwWidth*dwHeight;
	m_pBitmap = new DWORD[m_dwBitmapSize];
	
	if ( !m_pBitmap )
	{
		LogConsole("error: CTexture: bitmap mem allocate faild...");
		assert(0);
		return false;
	}

	DWORD * pDest = m_pBitmap;
	DWORD dwSize = m_dwBitmapSize;
	
	__asm
	{
		mov edi,[pDest]
		mov eax,dwColor
		mov ecx,dwSize
		cld
		rep stosd
	}
	
	return true;
}
void CTexture::DestroyBitmap()
{
	SAFE_DELETE_ARRAY(m_pBitmap);
}

bool CTexture::CreateD3DTexture(LPDIRECT3DDEVICE9 pDevice)
{
	HRESULT hResult = pDevice->CreateTexture(m_dwWidth,m_dwHeight,1,0,m_d3dFormat,D3DPOOL_MANAGED, &m_pD3DTexture,NULL);
	
	if ( FAILED(hResult) )
	{
		LogConsole("error: CTexture::CreateD3DTexture : create texture failed...");
		assert(0);
		return false;
	}

	return true;
}

void CTexture::DestroyD3DTexture()
{
	SAFE_RELEASE(m_pD3DTexture);
}

void CTexture::BitmapFlipVertical(void)
{
	DWORD * pLineTemp = new DWORD [m_dwWidth];
	assert(pLineTemp);

	DWORD * pUp = m_pBitmap;
	DWORD * pBottom = m_pBitmap + (m_dwHeight - 1) * m_dwWidth;

	DWORD dwCopySize = m_dwWidth * sizeof(DWORD);

	for ( DWORD i = 0; i < m_dwHeight/2; i++ )
	{
		// save line to temp
		memcpy(pLineTemp, pUp, dwCopySize);
		
		// mov line
		memcpy(pUp,pBottom,dwCopySize);
		
		// restore line
		memcpy(pBottom,pLineTemp,dwCopySize);
		pUp		+= m_dwWidth;
		pBottom -= m_dwWidth;
	}

	SAFE_DELETE(pLineTemp);
}

bool CTexture::BitmapReadFileBMP(const char *pszFileName)
{
#pragma pack(1)
	struct _tagBMPFILEHEADER{ 
		WORD   bfType; 
		DWORD  bfSize; 
		WORD   bfReserved1; 
		WORD   bfReserved2; 
		DWORD  bfOffBits; 
		DWORD  biSize; 
		LONG   biWidth; 
		LONG   biHeight; 
		WORD   biPlanes; 
		WORD   biBitCount;
		DWORD  biCompression; 
		DWORD  biSizeImage; 
		LONG   biXPelsPerMeter; 
		LONG   biYPelsPerMeter; 
		DWORD  biClrUsed; 
		DWORD  biClrImportant; 
	}; 
#pragma pack()

	bc::io::CMemFile file;

	if (!file.Open(pszFileName)) 
	{
		LogConsole("error: CTexture::BitmapReadFileBMP : Open" << pszFileName << "file failed");
		assert(0);
		return false;
	}

	_tagBMPFILEHEADER stHdr;
	file.Read(&stHdr,sizeof(_tagBMPFILEHEADER));

	if (stHdr.bfType != 0x4d42 || stHdr.biCompression != BI_RGB) 
	{
		LogConsole("error: CTexture::BitmapReadFileBMP : Open" << pszFileName << "file failed, unsupported format");
		assert(0);
		return false;
	}

	CreateBitmap(stHdr.biWidth,stHdr.biHeight,0xff000000);

	DWORD * pBuf = m_pBitmap;

	switch(stHdr.biBitCount)
	{
	case 8:
		{
			RGBQUAD *pColorTable = (RGBQUAD *)(file.GetData()+file.GetPosition());

			file.Seek(stHdr.bfOffBits,SEEK_SET);

			for(int y = 0;y < stHdr.biHeight; y++)
			{
				for(int x = 0; x < stHdr.biWidth; x++)
				{
					BYTE byIndex;
					file.Read(&byIndex,sizeof(BYTE));
					DWORD c  = pColorTable[byIndex].rgbRed << 16;
					c |= pColorTable[byIndex].rgbGreen << 8;
					c |= pColorTable[byIndex].rgbBlue;

					*pBuf++ |= c;
				}
			}
		}
		break;
	case 24:
		{
			file.Seek(stHdr.bfOffBits,SEEK_SET);
			BYTE r,g,b;
			for(int y=0;y<stHdr.biHeight;y++)
				for(int x=0;x<stHdr.biWidth;x++)
				{
					file.Read(&b,sizeof(BYTE));
					file.Read(&g,sizeof(BYTE));
					file.Read(&r,sizeof(BYTE));
					*pBuf++ |= ( r << 16) | (g << 8) | b;
				}
		}
		break;
	}

	BitmapFlipVertical();

	file.Close();

	return true;
}

bool CTexture::BitmapReadFileTGA(const char * pszFileName)
{
#pragma pack(1)
	struct _tagTGAFILEHEADER
	{	
		BYTE IdentSize;			
		BYTE ColorMapType;		
		BYTE ImageType;			
		WORD ColorMapStart;		
		WORD ColorMapLength;	
		BYTE ColorMapBits;		
		WORD XStart;			
		WORD YStart;			
		WORD Width;				
		WORD Height;			
		BYTE Bits;				
		BYTE Descriptor;		
	};
#pragma pack()

	struct _tagRGBA {
		BYTE Blue,Green,Red,Alpha;
	}; 

	_tagTGAFILEHEADER stHdr;

	bc::io::CMemFile file;
	
	if ( !file.Open(pszFileName) ) 
	{
		LogConsole("error: CTexture::BitmapReadFileTGA : open file failed..."); 
		assert(0);
		return false;
	}

	file.Read((BYTE *)&stHdr,sizeof(_tagTGAFILEHEADER));

	if ( !CreateBitmap(stHdr.Width,stHdr.Height,0xff000000) )
	{
		LogConsole("error: CTexture::BitmapReadFileTGA : create bitmap failed...");
		assert(0);
		return false;
	}
	
	DWORD * pBuf = m_pBitmap;

	if ( stHdr.ImageType == 2 )  
	{
		switch(stHdr.Bits)
		{
		case 16:
			{
				WORD c;
				for(int y=0;y<stHdr.Height;y++)
					for(int x =0;x<stHdr.Width;x++)
					{
						file.Read((BYTE*)&c,sizeof(WORD));

						DWORD dwB = (c & 0x1f) << 3;
						DWORD dwG = ((c >> 5) & 0x1f) << 3;
						DWORD dwR = ((c >> 10) & 0x1f) << 3;

						*pBuf++ |= (dwR<<16) | (dwG<<8) | dwB;
					}
			}
		break;
		
		case 24:
			{
				BYTE r,g,b;
				for(int y=0;y<stHdr.Height;y++)
					for(int x=0;x<stHdr.Width;x++)
					{
						file.Read(&b,sizeof(BYTE));
						file.Read(&g,sizeof(BYTE));
						file.Read(&r,sizeof(BYTE));

						*pBuf++ |= (r << 16) | (g << 8) | b;
					}
			}		
		break;
		
		case 32:
			{
				BYTE r,g,b,a;
				for(int y=0;y<stHdr.Height;y++)
					for(int x=0;x<stHdr.Width;x++)
					{
						file.Read(&b,sizeof(BYTE));
						file.Read(&g,sizeof(BYTE));
						file.Read(&r,sizeof(BYTE));
						file.Read(&a,sizeof(BYTE));

						*pBuf++ = (a << 24) | (r << 16) | (g << 8) | b;
					}
			}
		break;
		
		default:
			{
				LogConsole("error: CTexture::BitmapReadFileTGA : not surport this TGA format yet...");
				assert(0);
				return false;
			}
		} 
	}
	else if ( stHdr.ImageType == 10 ) // RLE 
	{
		switch(stHdr.Bits)
		{
		case 24:
			{
				BYTE byLen,r,g,b;WORD c = 0;int iPixel=0;
				do
				{
					file.Read(&byLen,sizeof(BYTE));

					if(byLen >= 0x80)
					{
						byLen -= 0x7F;
						file.Read(&b,sizeof(BYTE));
						file.Read(&g,sizeof(BYTE));
						file.Read(&r,sizeof(BYTE));

						while(byLen --)
						{
							*pBuf++ |= (r << 16) | (g << 8) | b;
							iPixel++;
						}
					}
					else
					{
						byLen ++;
						while(byLen --)
						{
							file.Read(&b,sizeof(BYTE));
							file.Read(&g,sizeof(BYTE));
							file.Read(&r,sizeof(BYTE));
							*pBuf++ |= (r << 16) | (g << 8) | b;
							iPixel++;
						}
					}
				}  while(iPixel < stHdr.Width*stHdr.Height);
			}
		break;
		
		case 32:
			{	
				BYTE byLen,r,g,b,a;int iPixel=0;
				do
				{
					file.Read(&byLen,sizeof(BYTE));

					if(byLen >= 0x80)
					{
						byLen -= 0x7F;
						file.Read(&b,sizeof(BYTE));
						file.Read(&g,sizeof(BYTE));
						file.Read(&r,sizeof(BYTE));
						file.Read(&a,sizeof(BYTE));
						while(byLen --)
						{
							*pBuf++ = (a << 24) | (r << 16) | (g << 8) | b;
							iPixel++;
						}
					}
					else
					{
						byLen ++;
						while(byLen --)
						{
							file.Read(&b,sizeof(BYTE));
							file.Read(&g,sizeof(BYTE));
							file.Read(&r,sizeof(BYTE));
							file.Read(&a,sizeof(BYTE));
							*pBuf++ = (a << 24) | (r << 16) | (g << 8) | b;
							iPixel++;
						}
					}
				}  while(iPixel < stHdr.Width*stHdr.Height);
			}
		break;
		
		case 16:
		
		default:
			{
				LogConsole("error: CTexture::BitmapReadFileTGA : not surport this TGA format yet...");
				assert(0);
				return false;
			}
		}
	}
	else
	{
		LogConsole("error: CTexture::BitmapReadFileTGA : unknown TGA format...");
		assert(0);
		return false;
	}

	BitmapFlipVertical();

	file.Close();
	return true;
}

DWORD CTexture::GetPixel(int iX, int iY)
{
	// get pixel
	return m_pBitmap[iY*m_dwWidth + iX];	
}

void CTexture::SetPixel(int iX, int iY, DWORD dwColor)
{
	// set pixel
	m_pBitmap[iY * m_dwWidth + iX] = dwColor;	
}

//////////////////////////////////////////////////////////////////////////

CRenderDevice::Layer2D::Layer2D(void) : m_dwFillMode(D3DFILL_SOLID) { }

CRenderDevice::Layer2D::~Layer2D(void) { }

bool CRenderDevice::Layer2D::Create(const char * pszFontFileName)
{
	m_pElementCache = new SElement[MAX_LAYER2D_CACHE_SIZE];
	m_dwElementCacheUsage = 0;
	
	CreateVertexBuffer();
	return true;
}

bool CRenderDevice::Layer2D::CreateVertexBuffer()
{
	CRenderDevice * pRender = CRenderDevice::GetInstance();

	if ( FAILED(pRender->GetRawDevice()->CreateVertexBuffer(sizeof(SLayer2DFVF) * 6 * MAX_LAYER2D_VERTEXBUFFER_SIZE,
		 D3DUSAGE_DYNAMIC | D3DUSAGE_WRITEONLY, Layer2D::FVF, D3DPOOL_DEFAULT , &m_pVB,NULL)) )	
	{
		LogConsole("error : Layer2D::CreateVertexBuffer : create VertexBuffer failed...");
		assert(0);
		return false;
	}
	return true;
}

void CRenderDevice::Layer2D::DestroyVertexBuffer()
{
	SAFE_RELEASE(m_pVB);
}

void CRenderDevice::Layer2D::Destroy(void)
{
	DestroyVertexBuffer();
	SAFE_DELETE_ARRAY(m_pElementCache);	
}

void CRenderDevice::Layer2D::DrawLine(int iStartX, int iStartY, int iEndX, int iEndY, D3DCOLOR color)
{
	// allocate an element
	if ( m_dwElementCacheUsage >= MAX_LAYER2D_CACHE_SIZE)
	{
		LogConsole("error: Layer2D::DrawLine : element cache overflow...");
		assert(0);
		return;
	}
	
	SElement & element = m_pElementCache[m_dwElementCacheUsage++];
	element.eType = EL2DET_LINES;
	SLines * pLines = (SLines *)element.pDataBuffer;
	pLines->dwCount  = 1;
	
	pLines->vertices[0].pos.x = (float)iStartX;
	pLines->vertices[0].pos.y = (float)iStartY;
	pLines->vertices[0].pos.z =  0.0f;
	pLines->vertices[0].rhw = 1.0f;
	pLines->vertices[0].color = color;
	
	pLines->vertices[1].pos.z =  0.0f;
	pLines->vertices[1].rhw = 1.0f;
	pLines->vertices[1].pos.x = (float)iEndX;
	pLines->vertices[1].pos.y = (float)iEndY;
	pLines->vertices[1].color = color;
}

void CRenderDevice::Layer2D::DrawWireQuad(int iX, int iY, int iWidth, int iHeight, D3DCOLOR color)
{
	// allocate an element
	if ( m_dwElementCacheUsage >= MAX_LAYER2D_CACHE_SIZE)
	{
		LogConsole("error: Layer2D::DrawWireQuad : element cache overflow...");
		assert(0);
		return;
	}

	SElement &element = m_pElementCache[m_dwElementCacheUsage++];
	element.eType = EL2DET_LINES;
	SLines * pLines = (SLines *)element.pDataBuffer;
	pLines->dwCount  = 4;

	pLines->vertices[0].pos.x = (float)iX;
	pLines->vertices[0].pos.y = (float)iY;
	pLines->vertices[0].pos.z = 0.0f; 
	pLines->vertices[0].rhw = 1.0f;

	pLines->vertices[1].pos.x = (float)iX;
	pLines->vertices[1].pos.y = (float)(iY+iHeight-1);
	pLines->vertices[1].pos.z =  0.0f;
	pLines->vertices[1].rhw = 1.0f;

	pLines->vertices[2].pos.x = (float)iX;
	pLines->vertices[2].pos.y = (float)(iY+iHeight-1);
	pLines->vertices[2].pos.z =  0.0f;
	pLines->vertices[2].rhw = 1.0f;

	pLines->vertices[3].pos.x = (float)(iX+iWidth-1);
	pLines->vertices[3].pos.y = (float)(iY+iHeight-1);
	pLines->vertices[3].pos.z =  0.0f;
	pLines->vertices[3].rhw = 1.0f;	

	pLines->vertices[4].pos.x = (float)(iX+iWidth-1);
	pLines->vertices[4].pos.y = (float)(iY+iHeight-1);
	pLines->vertices[4].pos.z =  0.0f;
	pLines->vertices[4].rhw = 1.0f;	

	pLines->vertices[5].pos.x = (float)(iX+iWidth-1);
	pLines->vertices[5].pos.y = (float)iY;
	pLines->vertices[5].pos.z =  0.0f;
	pLines->vertices[5].rhw = 1.0f;

	pLines->vertices[6].pos.x = (float)(iX+iWidth-1);
	pLines->vertices[6].pos.y = (float)iY;
	pLines->vertices[6].pos.z = 0.0f; 
	pLines->vertices[6].rhw = 1.0f;	

	pLines->vertices[7].pos.x = (float)iX;
	pLines->vertices[7].pos.y = (float)iY;
	pLines->vertices[7].pos.z = 0.0f;
	pLines->vertices[7].rhw = 1.0f;	

	pLines->vertices[0].color =
	pLines->vertices[1].color = 
	pLines->vertices[2].color = 
	pLines->vertices[3].color = 
	pLines->vertices[4].color = 
	pLines->vertices[5].color = 
	pLines->vertices[6].color = 
	pLines->vertices[7].color = color;
}

void CRenderDevice::Layer2D::DrawWireQuadRect(int iX1,int iY1,int iX2,int iY2,D3DCOLOR color)
{
	// allocate an element
	if ( m_dwElementCacheUsage >= MAX_LAYER2D_CACHE_SIZE)
	{
		LogConsole("error: Layer2D::DrawWireQuadRect : element cache overflow...");
		assert(0);
		return;
	}

	SElement &element = m_pElementCache[m_dwElementCacheUsage++];
	element.eType = EL2DET_LINES;
	SLines * pLines = (SLines *)element.pDataBuffer;
	pLines->dwCount  = 4;

	pLines->vertices[0].pos.x = (float)iX1;
	pLines->vertices[0].pos.y = (float)iY1;
	pLines->vertices[0].pos.z = 0.0f;
	pLines->vertices[0].rhw = 1.0f;

	pLines->vertices[1].pos.x = (float)iX1;
	pLines->vertices[1].pos.y = (float)(iY2);
	pLines->vertices[1].pos.z = 0.0f;
	pLines->vertices[1].rhw = 1.0f;

	pLines->vertices[2].pos.x = (float)iX1;
	pLines->vertices[2].pos.y = (float)(iY2);
	pLines->vertices[2].pos.z = 0.0f;
	pLines->vertices[2].rhw = 1.0f;

	pLines->vertices[3].pos.x = (float)(iX2);
	pLines->vertices[3].pos.y = (float)(iY2);
	pLines->vertices[3].pos.z = 0.0f;
	pLines->vertices[3].rhw = 1.0f;	

	pLines->vertices[4].pos.x = (float)(iX2);
	pLines->vertices[4].pos.y = (float)(iY2);
	pLines->vertices[4].pos.z = 0.0f;
	pLines->vertices[4].rhw = 1.0f;	

	pLines->vertices[5].pos.x = (float)(iX2);
	pLines->vertices[5].pos.y = (float)iY1;
	pLines->vertices[5].pos.z = 0.0f;
	pLines->vertices[5].rhw = 1.0f;

	pLines->vertices[6].pos.x = (float)(iX2);
	pLines->vertices[6].pos.y = (float)iY1;
	pLines->vertices[6].pos.z = 0.0f;
	pLines->vertices[6].rhw = 1.0f;	

	pLines->vertices[7].pos.x = (float)iX1;
	pLines->vertices[7].pos.y = (float)iY1;
	pLines->vertices[7].pos.z = 0.0f;
	pLines->vertices[7].rhw = 1.0f;	

	pLines->vertices[0].color =
	pLines->vertices[1].color = 
	pLines->vertices[2].color = 
	pLines->vertices[3].color = 
	pLines->vertices[4].color = 
	pLines->vertices[5].color = 
	pLines->vertices[6].color = 
	pLines->vertices[7].color = color;
}

void CRenderDevice::Layer2D::DrawSolidQuad(int iX,int iY,int iWidth,int iHeight,D3DCOLOR color)
{
	// allocate an element
	if ( m_dwElementCacheUsage >= MAX_LAYER2D_CACHE_SIZE)
	{
		LogConsole("error: Layer2D::DrawSolidQuad : element cache overflow...");
		assert(0);
		return;
	}

	SElement & element = m_pElementCache[m_dwElementCacheUsage++];
	element.eType = EL2DET_TRISTRIP;

	STristrip * pTristrip = (STristrip*)element.pDataBuffer;
	pTristrip->dwTextureHandle = INVALID;

	pTristrip->vertices[0].pos.x = pTristrip->vertices[2].pos.x = (float)iX;
	pTristrip->vertices[1].pos.x = pTristrip->vertices[3].pos.x = (float)(iX+iWidth);

	pTristrip->vertices[0].pos.y = pTristrip->vertices[1].pos.y = (float)iY;
	pTristrip->vertices[2].pos.y = pTristrip->vertices[3].pos.y = (float)(iY+iHeight);

	pTristrip->vertices[0].color = 
	pTristrip->vertices[1].color = 
	pTristrip->vertices[2].color = 
	pTristrip->vertices[3].color = color;

	pTristrip->vertices[0].rhw = 
		pTristrip->vertices[1].rhw = 
		pTristrip->vertices[2].rhw = 
		pTristrip->vertices[3].rhw = 1.0f;

	pTristrip->vertices[0].pos.z = 
		pTristrip->vertices[1].pos.z = 
		pTristrip->vertices[2].pos.z = 
		pTristrip->vertices[3].pos.z = 0.0f;
}

void CRenderDevice::Layer2D::DrawSolidQuadRect(int iX1,int iY1,int iX2,int iY2,D3DCOLOR color)
{
	// allocate an element
	if ( m_dwElementCacheUsage >= MAX_LAYER2D_CACHE_SIZE)
	{
		LogConsole("error: Layer2D::DrawSolidQuadRect : element cache overflow...");
		assert(0);
		return;
	}

	SElement & element = m_pElementCache[m_dwElementCacheUsage++];
	element.eType = EL2DET_TRISTRIP;

	STristrip * pTristrip = (STristrip*)element.pDataBuffer;
	pTristrip->dwTextureHandle = INVALID;

	pTristrip->vertices[0].pos.x = pTristrip->vertices[2].pos.x = (float)iX1;
	pTristrip->vertices[1].pos.x = pTristrip->vertices[3].pos.x = (float)(iX2);

	pTristrip->vertices[0].pos.y = pTristrip->vertices[1].pos.y = (float)iY1;
	pTristrip->vertices[2].pos.y = pTristrip->vertices[3].pos.y = (float)(iY2);

	pTristrip->vertices[0].color = 
	pTristrip->vertices[1].color = 
	pTristrip->vertices[2].color = 
	pTristrip->vertices[3].color = color;

	pTristrip->vertices[0].rhw = 
		pTristrip->vertices[1].rhw = 
		pTristrip->vertices[2].rhw = 
		pTristrip->vertices[3].rhw = 1.0f;

	pTristrip->vertices[0].pos.z = 
		pTristrip->vertices[1].pos.z = 
		pTristrip->vertices[2].pos.z = 
		pTristrip->vertices[3].pos.z = 0.0f;
}

void CRenderDevice::Layer2D::DrawTextureRotation(int iX, int iY, float fAngle, DWORD dwTextureHandle, D3DCOLOR color)
{
	// allocate an element
	if ( m_dwElementCacheUsage >= MAX_LAYER2D_CACHE_SIZE)
	{
		LogConsole("error: Layer2D::DrawTextureRotation : element cache overflow...");
		assert(0);
		return;
	}

	CRenderDevice * pRender = CRenderDevice::GetInstance();

	CTexture * pTexture = pRender->GetTexture(dwTextureHandle);
	float fHalfWidth = (float)pTexture->GetWidth()  * 0.5f;
	float fHalfHeight= (float)pTexture->GetHeight() * 0.5f;

	static D3DXMATRIX matWorld, matTrans, matRot;
	
	D3DXMatrixTranslation(&matTrans,(float)iX,(float)iY,0);
	D3DXMatrixRotationZ(&matRot,fAngle);
	D3DXMatrixMultiply(&matWorld,&matRot,&matTrans);

	SElement & element = m_pElementCache[m_dwElementCacheUsage++];
	element.eType = EL2DET_TRISTRIP;
	
	STristrip * pTristrip = (STristrip*)element.pDataBuffer;
	pTristrip->dwTextureHandle = dwTextureHandle;

	pTristrip->vertices[0].pos.x = pTristrip->vertices[2].pos.x = - fHalfWidth;
	pTristrip->vertices[1].pos.x = pTristrip->vertices[3].pos.x =   fHalfWidth;

	pTristrip->vertices[0].pos.y = pTristrip->vertices[1].pos.y = - fHalfHeight;
	pTristrip->vertices[2].pos.y = pTristrip->vertices[3].pos.y =   fHalfHeight;

	for ( int i = 0; i < 4; i++ )
	{
		D3DXVec3TransformCoord(&pTristrip->vertices[i].pos, &pTristrip->vertices[i].pos, &matWorld);
	}

	pTristrip->vertices[0].rhw = 
		pTristrip->vertices[1].rhw = 
		pTristrip->vertices[2].rhw = 
		pTristrip->vertices[3].rhw = 1.0f;

	pTristrip->vertices[0].pos.z = 
		pTristrip->vertices[1].pos.z = 
		pTristrip->vertices[2].pos.z = 
	pTristrip->vertices[3].pos.z = 0.0f;

	pTristrip->vertices[0].color =
	pTristrip->vertices[1].color =
	pTristrip->vertices[2].color =
	pTristrip->vertices[3].color = color;

	pTristrip->vertices[0].uv = D3DXVECTOR2(0.0f,0.0f);	
	pTristrip->vertices[1].uv = D3DXVECTOR2(1.0f,0.0f);	
	pTristrip->vertices[2].uv = D3DXVECTOR2(0.0f,1.0f);	
	pTristrip->vertices[3].uv = D3DXVECTOR2(1.0f,1.0f);
}

void CRenderDevice::Layer2D::DrawTexture(int iX, int iY, int iWidth, int iHeight, DWORD dwTextureHandle, D3DCOLOR color)
{
	// allocate an element
	if ( m_dwElementCacheUsage >= MAX_LAYER2D_CACHE_SIZE)
	{
		LogConsole("error: Layer2D::DrawTexture : element cache overflow...");
		assert(0);
		return;
	}
	
	CRenderDevice * pRender = CRenderDevice::GetInstance(); 
	CTexture * pTexture = pRender->GetTexture(dwTextureHandle);

	SElement & element = m_pElementCache[m_dwElementCacheUsage++];
	element.eType = EL2DET_TRISTRIP;
	STristrip * pTristrip = (STristrip*)element.pDataBuffer;
	pTristrip->dwTextureHandle = dwTextureHandle;

	pTristrip->vertices[0].pos.x = pTristrip->vertices[2].pos.x = (float)iX;
	pTristrip->vertices[1].pos.x = pTristrip->vertices[3].pos.x = (float)(iX+iWidth);

	pTristrip->vertices[0].pos.y = pTristrip->vertices[1].pos.y = (float)iY;
	pTristrip->vertices[2].pos.y = pTristrip->vertices[3].pos.y = (float)(iY+iHeight);

	pTristrip->vertices[0].rhw = 
		pTristrip->vertices[1].rhw = 
		pTristrip->vertices[2].rhw = 
		pTristrip->vertices[3].rhw = 1.0f;

	pTristrip->vertices[0].pos.z = 
		pTristrip->vertices[1].pos.z = 
		pTristrip->vertices[2].pos.z = 
		pTristrip->vertices[3].pos.z = 0.0f;

	pTristrip->vertices[0].color =
	pTristrip->vertices[1].color =
	pTristrip->vertices[2].color =
	pTristrip->vertices[3].color = color;

	pTristrip->vertices[0].uv = D3DXVECTOR2(0.0f,0.0f);	
	pTristrip->vertices[1].uv = D3DXVECTOR2(1.0f,0.0f);	
	pTristrip->vertices[2].uv = D3DXVECTOR2(0.0f,1.0f);	
	pTristrip->vertices[3].uv = D3DXVECTOR2(1.0f,1.0f);
}

void CRenderDevice::Layer2D::DrawTextureSubRect(int iX, int iY, const RECT * pRect, DWORD dwTextureHandle, D3DCOLOR color)
{
	// allocate an element
	if ( m_dwElementCacheUsage >= MAX_LAYER2D_CACHE_SIZE)
	{
		LogConsole("error: Layer2D::DrawTextureSubRect : element cache overflow...");
		assert(0);
		return;
	}
	
	CRenderDevice * pRender = CRenderDevice::GetInstance(); 
	CTexture * pTexture = pRender->GetTexture(dwTextureHandle);
	
	float fWidth = (float)pTexture->GetWidth();
	float fHeight= (float)pTexture->GetHeight();
	
	float fDrawWidth =(float)(pRect->right - pRect->left);
	float fDrawHeight=(float)(pRect->bottom- pRect->top);
	
	SElement & element = m_pElementCache[m_dwElementCacheUsage++];
	element.eType = EL2DET_TRISTRIP;
	
	STristrip * pTristrip = (STristrip*)element.pDataBuffer;
	pTristrip->dwTextureHandle = dwTextureHandle;

	pTristrip->vertices[0].pos.x = pTristrip->vertices[2].pos.x = (float)iX;
	pTristrip->vertices[1].pos.x = pTristrip->vertices[3].pos.x = (float)(iX+fDrawWidth);

	pTristrip->vertices[0].pos.y = pTristrip->vertices[1].pos.y = (float)iY;
	pTristrip->vertices[2].pos.y = pTristrip->vertices[3].pos.y = (float)(iY+fDrawHeight);
	
	pTristrip->vertices[0].rhw = 
	pTristrip->vertices[1].rhw = 
	pTristrip->vertices[2].rhw = 
	pTristrip->vertices[3].rhw = 1.0f;
	
	pTristrip->vertices[0].pos.z = 
	pTristrip->vertices[1].pos.z = 
	pTristrip->vertices[2].pos.z = 
	pTristrip->vertices[3].pos.z = 0.0f;
	
	pTristrip->vertices[0].color =
	pTristrip->vertices[1].color =
	pTristrip->vertices[2].color =
	pTristrip->vertices[3].color = color;
	
	float uLeft   = pRect->left / fWidth;
	float uRight  = pRect->right / fWidth;
	float vTop    = pRect->top / fHeight;
	float vBottom = pRect->bottom / fHeight;
	
	pTristrip->vertices[0].uv = D3DXVECTOR2(uLeft,vTop);	
	pTristrip->vertices[2].uv = D3DXVECTOR2(uLeft,vBottom);	
	pTristrip->vertices[1].uv = D3DXVECTOR2(uRight,vTop);	
	pTristrip->vertices[3].uv = D3DXVECTOR2(uRight,vBottom);
}

void CRenderDevice::Layer2D::Render(void)
{
	CRenderDevice * pRender = CRenderDevice::GetInstance(); 
	LPDIRECT3DDEVICE9 pDevice = pRender->GetRawDevice();
	
	pDevice->SetRenderState(D3DRS_FILLMODE, m_dwFillMode);
	pDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
	pDevice->SetRenderState(D3DRS_ZENABLE, true);
	pDevice->SetRenderState(D3DRS_ZWRITEENABLE, true);
	
	pDevice->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_POINT);
	pDevice->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_POINT);
	pDevice->SetSamplerState(1, D3DSAMP_MINFILTER, D3DTEXF_POINT);
	pDevice->SetSamplerState(1, D3DSAMP_MAGFILTER, D3DTEXF_POINT);
	
	pDevice->SetRenderState(D3DRS_ALPHATESTENABLE, true);
	pDevice->SetRenderState(D3DRS_ALPHAREF, 0x0);
	pDevice->SetRenderState(D3DRS_ALPHAFUNC,D3DCMP_GREATER);
	
	pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE,true);
	pDevice->SetRenderState(D3DRS_SRCBLEND,D3DBLEND_SRCALPHA);
	pDevice->SetRenderState(D3DRS_DESTBLEND,D3DBLEND_INVSRCALPHA);
	
	pDevice->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG1);
	pDevice->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
	
	pDevice->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1);
	pDevice->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
	
	pDevice->SetTextureStageState(1, D3DTSS_COLOROP, D3DTOP_DISABLE);
	pDevice->SetTextureStageState(1, D3DTSS_COLOROP, D3DTOP_DISABLE);
	
	pDevice->SetStreamSource(0, m_pVB, 0,sizeof(SLayer2DFVF));

	pDevice->SetFVF(Layer2D::FVF);

	SLayer2DFVF * pVertices(NULL);

	for ( DWORD dwElement = 0; dwElement < m_dwElementCacheUsage; dwElement++ )
	{
		SElement & element = m_pElementCache[dwElement];

		switch ( element.eType )
		{
		case EL2DET_LINES:
			{
				pRender->SetTexture(0, INVALID);

				if ( FAILED(m_pVB->Lock(0, 0, (void**)&pVertices, D3DLOCK_DISCARD)) )
				{
					LogConsole("error : CRenderDevice::Layer2D::Render : lock buffer failed...");
					assert(0);
					return;
				}
				
				SLines * pLines = (SLines *)element.pDataBuffer;
				memcpy(pVertices,pLines->vertices, sizeof(SLines::SVertex) * pLines->dwCount * 2);
				
				m_pVB->Unlock();
				
				pDevice->DrawPrimitive(D3DPT_LINELIST, 0, pLines->dwCount);
			}
		break;
		
		case EL2DET_TRISTRIP:
			{
				STristrip * pTristrip= (STristrip*)element.pDataBuffer;
				pRender->SetTexture(0, pTristrip->dwTextureHandle);
				if ( FAILED( m_pVB->Lock(0, 0, (void**)&pVertices, D3DLOCK_DISCARD)) )
				{
					LogConsole("error : CRenderDevice::Layer2D::Render : lock buffer failed...");
					assert(0);
					return ;
				}
				
				memcpy(pVertices, pTristrip->vertices, sizeof(STristrip::SVertex) * 4);
		
				m_pVB->Unlock();
				
				pDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 2);
				pRender->SetTexture(0, NULL);
			}
		break;
		
		}
	}
	m_dwElementCacheUsage = 0;
}

DWORD CRenderDevice::GetWindowHeight()
{
	return m_rc.bottom - m_rc.top;
}

DWORD CRenderDevice::GetWindowWidth()
{
	return m_rc.right - m_rc.left;
}

void CRenderDevice::AddSwapChain(DWORD dwWidth, DWORD dwHeight, LPDIRECT3DSWAPCHAIN9 * ppSwapChain)
{
	D3DPRESENT_PARAMETERS  d3dpp;		 

	ZeroMemory(&d3dpp, sizeof(d3dpp));

	d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
	d3dpp.BackBufferCount  = 1;
	d3dpp.EnableAutoDepthStencil = true;
	d3dpp.AutoDepthStencilFormat = D3DFMT_D16;
	d3dpp.Windowed = true;	
	d3dpp.BackBufferFormat = D3DFMT_UNKNOWN;
	d3dpp.BackBufferWidth  = dwWidth;
	d3dpp.BackBufferHeight = dwHeight;
	d3dpp.FullScreen_RefreshRateInHz = D3DPRESENT_RATE_DEFAULT;
	d3dpp.PresentationInterval =  D3DPRESENT_INTERVAL_IMMEDIATE;
	
	m_pD3DDevice->CreateAdditionalSwapChain(&d3dpp,ppSwapChain);
}

//?
//////////////////////////////////////////////////////////////////////////

CRenderDevice::CRenderDevice()
{
	ZeroMemory(m_pTextureArray,sizeof(CTexture*)*MAX_TEXTURE_ARRAY_SIZE);
	m_dwTextureCnt = 0;
}

CRenderDevice::~CRenderDevice() { }

bool CRenderDevice::Create(HWND hWnd, RECT rc, const char * pszConfigFile)
{
	m_hWnd = hWnd;
	
	memcpy(&m_rc,&rc,sizeof(RECT));
	
	DefaultSetting(pszConfigFile);
	
	CreateDirect3D9();
	
	CreateDevice();

	if ( !CheckHardware() )
	{
		LogConsole("error : CRenderDevice::Create : check hardware surpport failed...");
		DestroyDevice();
		return false;
	}

	CreateLayer();

	SetDefaultRenderState();

	return true;
}

void CRenderDevice::Destroy()
{
	for ( DWORD n = 0; n < MAX_TEXTURE_ARRAY_SIZE; n++)
	{
		DestroyTexture(n);
	}
	DestroyLayer();
	DestroyDevice();
	DestroyDirect3D9();
}

void CRenderDevice::DefaultSetting(const char * pszConfigString)
{
	SSetting & setting = m_setting;
	strncpy(setting.szFont,"16.font",_MAX_PATH - 1);
	setting.szFont[_MAX_PATH -1] = 0;
	setting.bHVProcess  = true;
	setting.iWndPosX = 0;
	setting.iWndPosY = 0;
	setting.iWndWidth = this->GetWindowWidth();
	setting.iWndHeight= this->GetWindowHeight();
	setting.bWndAutoStyle  = false; 
	setting.bWndAutoCenter = false;
	setting.bIsWindow = true;
	setting.iTextureMode= 32;
	setting.d3dfmtBackBuffer = D3DFMT_X8R8G8B8;
	setting.dwPresentationInterval = D3DPRESENT_INTERVAL_DEFAULT; 
	
	//@ ModifySetting(pszConfigString);
}

bool CRenderDevice::CreateDevice()
{
	// adjust windows to pre width height	
	if ( m_setting.bWndAutoStyle)
	{
		if ( !m_setting.bIsWindow )
		{
			// set fullscreen style
			DWORD dwStyle = GetWindowLong(m_hWnd, GWL_STYLE);

			dwStyle &=~WS_CAPTION;

			SetWindowLong(m_hWnd,GWL_STYLE,dwStyle);

			MoveWindow(m_hWnd,0,0,m_setting.iWndWidth,m_setting.iWndHeight,true);
		}
		else
		{
			// modify window style
			DWORD dwStyle = GetWindowLong(m_hWnd,GWL_STYLE);

			dwStyle |= WS_POPUP;

			SetWindowLong(m_hWnd,GWL_STYLE,dwStyle);

			// set the window's initial width
			RECT rc;
			SetRect(&rc, 0, 0, m_setting.iWndWidth, m_setting.iWndHeight);
			AdjustWindowRect( &rc, dwStyle, FALSE );

			int iPosX = m_setting.iWndPosX;
			int iPosY = m_setting.iWndPosY;
			if ( m_setting.bWndAutoCenter )
			{
				GetSystemMetrics(SM_CYSCREEN);
				iPosX = GetSystemMetrics(SM_CXSCREEN) / 2 - (rc.right - rc.left) /2;
				iPosY = GetSystemMetrics(SM_CYSCREEN) / 2 - (rc.bottom- rc.top) /2;
			}

			MoveWindow(m_hWnd,iPosX,iPosY,rc.right - rc.left,rc.bottom - rc.top,TRUE);
		}
	}
	
	// RECT rc;
	// GetClientRect(m_hWnd,&rc);
 
	ZeroMemory(&m_deviceInfo.d3dpp, sizeof(m_deviceInfo.d3dpp));

	m_deviceInfo.d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
	m_deviceInfo.d3dpp.BackBufferCount  = 1;
	m_deviceInfo.d3dpp.EnableAutoDepthStencil = TRUE;
	m_deviceInfo.d3dpp.AutoDepthStencilFormat = D3DFMT_D16;
	m_deviceInfo.d3dpp.Flags = 0; // D3DPRESENTFLAG_LOCKABLE_BACKBUFFER;
	m_deviceInfo.d3dpp.hDeviceWindow = m_hWnd;
	m_deviceInfo.d3dpp.Windowed = m_setting.bIsWindow;	
	
	if ( m_deviceInfo.d3dpp.Windowed )
	{
		m_deviceInfo.d3dpp.BackBufferFormat = D3DFMT_UNKNOWN;
		m_deviceInfo.d3dpp.BackBufferWidth  = m_setting.iWndWidth;
		m_deviceInfo.d3dpp.BackBufferHeight = m_setting.iWndHeight;
		m_deviceInfo.d3dpp.FullScreen_RefreshRateInHz = D3DPRESENT_RATE_DEFAULT;
		m_deviceInfo.d3dpp.PresentationInterval =  D3DPRESENT_INTERVAL_IMMEDIATE;
	}
	else
	{
		m_deviceInfo.d3dpp.BackBufferFormat = m_setting.d3dfmtBackBuffer;
		m_deviceInfo.d3dpp.BackBufferWidth  = m_setting.iWndWidth;
		m_deviceInfo.d3dpp.BackBufferHeight = m_setting.iWndHeight;
		m_deviceInfo.d3dpp.FullScreen_RefreshRateInHz = D3DPRESENT_RATE_DEFAULT;
		m_deviceInfo.d3dpp.PresentationInterval =  D3DPRESENT_INTERVAL_IMMEDIATE;
	}

	HRESULT hResult;
	
	if ( m_setting.bHVProcess )
	{
		// HAL HVP
		if (
			FAILED(
			hResult = m_pD3D->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, 
			m_hWnd,
			D3DCREATE_MIXED_VERTEXPROCESSING | D3DCREATE_MULTITHREADED,	
			&m_deviceInfo.d3dpp, &m_pD3DDevice)) 
			)
		{
			LogConsole("error: CRenderDevice::CreateDevice : create D3D device failed...");
			assert(0);
			return false;
		}
	}
	else // HAL SVP
	{
		if ( 
			FAILED(
			hResult = m_pD3D->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL,
			m_hWnd,
			D3DCREATE_SOFTWARE_VERTEXPROCESSING | D3DCREATE_MULTITHREADED,	
			&m_deviceInfo.d3dpp, &m_pD3DDevice))
			)
		{ 
			LogConsole("error: CRenderDevice::CreateDevice : create D3D device(HAL SVP) failed...");
			assert(0);
			return false;
		}
	}
	
	// get capacities
	m_pD3DDevice->GetDeviceCaps(&m_deviceInfo.d3dCaps);

	return true;
}

void CRenderDevice::DestroyDevice()
{
	SAFE_RELEASE(m_pD3DDevice);
}

bool CRenderDevice::CheckHardware(void)
{
	HRESULT hResult;

	if ( m_deviceInfo.d3dCaps.DeviceType != D3DDEVTYPE_HAL )
	{
		LogConsole("error : CRenderDevice::CheckHardware : current device isn't HAL type");
		assert(0);
		return false;
	}
	
	// check texture format 8888 4444
	
	// check 4444
	if ( 
			FAILED
			(
			hResult = m_pD3D->CheckDeviceFormat(D3DADAPTER_DEFAULT,
			D3DDEVTYPE_HAL,
			m_deviceInfo.d3dpp.BackBufferFormat,
			0,
			D3DRTYPE_TEXTURE,
			D3DFMT_A4R4G4B4)
			)
		)
	{
		LogConsole("error : CRenderDevice::CheckHardware : D3DFORMAT A4R4G4B4 test failed");
		assert(0);
		return false;
	}

	// check 8888
	if (
		FAILED(
		hResult = m_pD3D->CheckDeviceFormat( D3DADAPTER_DEFAULT,
		D3DDEVTYPE_HAL,
		m_deviceInfo.d3dpp.BackBufferFormat,
		0,
		D3DRTYPE_TEXTURE,
		D3DFMT_A8R8G8B8))
		)
	{
		LogConsole("error : CRenderDevice::CheckHardware : D3DFORMAT A8R8G8B8 test failed");
		assert(0);
		return false;	
	}

	// get adapter info
	if (
		FAILED(
		hResult = m_pD3D->GetAdapterIdentifier(D3DADAPTER_DEFAULT,
		0,&m_deviceInfo.AdapterIdentifier))
		)
	{
		LogConsole("error : CRenderDevice::CheckHardware : GetAdapterIdentifier() failed");
		assert(0);
		return false;
	}

	return true;
}

void CRenderDevice::SetDefaultRenderState(void)
{
	m_pD3DDevice->SetRenderState( D3DRS_FILLMODE, D3DFILL_SOLID);
	
	//m_pD3DDevice->SetTextureStageState( 0, D3DTSS_MINFILTER, D3DTEXF_LINEAR);
	//m_pD3DDevice->SetTextureStageState( 0, D3DTSS_MAGFILTER, D3DTEXF_LINEAR);
	
	//m_pD3DDevice->SetRenderState(D3DRS_ALPHABLENDENABLE,TRUE);
	///m_pD3DDevice->SetRenderState(D3DRS_SRCBLEND,D3DBLEND_SRCALPHA);
	//m_pD3DDevice->SetRenderState(D3DRS_DESTBLEND,D3DBLEND_INVSRCALPHA);
	
	m_pD3DDevice->SetRenderState( D3DRS_LIGHTING, FALSE );
	
	//m_pD3DDevice->SetTextureStageState( 0, D3DTSS_COLOROP, D3DTOP_MODULATE);
	//m_pD3DDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
	//m_pD3DDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);
	//m_pD3DDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);
	//m_pD3DDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
	//m_pD3DDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE);
	//m_pD3DDevice->SetTextureStageState( 0, D3DTSS_MINFILTER, D3DTEXF_LINEAR);
	
	m_pD3DDevice->SetSamplerState(0, D3DSAMP_ADDRESSU,D3DTADDRESS_WRAP);
	m_pD3DDevice->SetSamplerState(0, D3DSAMP_ADDRESSV,D3DTADDRESS_WRAP);
	m_pD3DDevice->SetSamplerState(0, D3DSAMP_ADDRESSW,D3DTADDRESS_WRAP);
	m_pD3DDevice->SetSamplerState(1, D3DSAMP_ADDRESSU,D3DTADDRESS_WRAP);
	m_pD3DDevice->SetSamplerState(1, D3DSAMP_ADDRESSV,D3DTADDRESS_WRAP);
	m_pD3DDevice->SetSamplerState(1, D3DSAMP_ADDRESSW,D3DTADDRESS_WRAP);
}

void CRenderDevice::Clear(D3DCOLOR d3dColor)
{
	m_pD3DDevice->Clear(0, NULL, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER, d3dColor, 1.0f, 0);
}

bool CRenderDevice::BeginScene()
{
	return SUCCEEDED(m_pD3DDevice->BeginScene());
}

void CRenderDevice::EndScene()
{
	m_pD3DDevice->EndScene();
}

void CRenderDevice::Present()
{
	m_pD3DDevice->Present(NULL, NULL, NULL, NULL);
}

void CRenderDevice::Present(HWND hWnd, LPDIRECT3DSWAPCHAIN9 pSwapChain)
{
	if (pSwapChain)	pSwapChain->Present(NULL, NULL, hWnd, NULL, 0);
}

void CRenderDevice::Render()
{
	m_pLayer2D->Render(); // layer2d also plays the role of 2d scene manager
}

bool CRenderDevice::CreateLayer(void)
{
	m_pLayer2D = new Layer2D;
	m_pLayer2D->Create(m_setting.szFont);

	m_pLayer3D = new Layer3D;
	m_pLayer3D->Create();
	return true;
}

void CRenderDevice::DestroyLayer(void)
{
	SAFE_DESTROY(m_pLayer2D);
	SAFE_DESTROY(m_pLayer3D);
}

CTexture * CRenderDevice::GetTexture(DWORD  dwTextureHandle)
{
	if ( dwTextureHandle == INVALID ) 
	{
		LogConsole("error : CRenderDevice::GetTexture : texture handle is invalid...");
		assert(0);
		return NULL;
	}

	if ( dwTextureHandle+1 > MAX_TEXTURE_ARRAY_SIZE ) 
	{
		LogConsole("error : CRenderDevice::CreateTexture : texture handle is invalid...");
		assert(0);
		return NULL;
	}

	return m_pTextureArray[dwTextureHandle];
}

DWORD CRenderDevice::CreateTexture(const char * pszFileName, bool bUse32Tex)
{	
	if ( m_dwTextureCnt == MAX_TEXTURE_ARRAY_SIZE ) 
	{
		LogConsole("error : CRenderDevice::CreateTexture : texture cache fullfilled...");
		assert(0);
		return INVALID;
	}

	CTexture * pTexture = new CTexture;

	bool bResult = pTexture->Create(m_pD3DDevice, pszFileName, bUse32Tex, m_deviceInfo.d3dCaps.MaxTextureWidth, m_deviceInfo.d3dCaps.MaxTextureHeight);
	if ( !bResult )
	{
		LogConsole("error : CRenderDevice::CreateTexture : creating texture" << pszFileName << "failed...");
		return INVALID;
	}

	DWORD dwTex = 0;
	for ( ; dwTex < MAX_TEXTURE_ARRAY_SIZE; dwTex++ )
	{
		if ( !m_pTextureArray[dwTex] )
		{
			m_pTextureArray[dwTex] = pTexture;
			break;
		}
	}

	m_dwTextureCnt++;

	return (int)dwTex;
}

DWORD CRenderDevice::CreateTexture(DWORD dwWidth,DWORD dwHeight,DWORD *pBitmap,bool bUse32Tex)
{	
	if ( m_dwTextureCnt == MAX_TEXTURE_ARRAY_SIZE ) 
	{
		LogConsole("error : CRenderDevice::CreateTexture : texture cache fullfilled...");
		assert(0);
		return INVALID;
	}

	CTexture * pTexture = new CTexture;
	
	bool bResult = pTexture->Create(m_pD3DDevice,dwWidth,dwHeight,pBitmap,bUse32Tex,m_deviceInfo.d3dCaps.MaxTextureWidth,m_deviceInfo.d3dCaps.MaxTextureHeight);
	
	if ( !bResult )
	{
		LogConsole("error : CRenderDevice::CreateTexture : creating " << dwWidth << "*" << dwHeight << "sized texture failed...");
		SAFE_DELETE(pTexture);
		assert(0);
		return INVALID;
	}

	DWORD dwTex = 0;
	
	for ( ; dwTex < MAX_TEXTURE_ARRAY_SIZE; dwTex++ )
	{
		if ( !m_pTextureArray[dwTex] )
		{
			m_pTextureArray[dwTex] = pTexture;
			break;
		}
	}
	
	m_dwTextureCnt++;
	
	return (int)dwTex;
}

DWORD CRenderDevice::CreateTexture(DWORD dwWidth,DWORD dwHeight,DWORD dwColor,bool bUse32Tex)
{	
	if ( m_dwTextureCnt == MAX_TEXTURE_ARRAY_SIZE ) 
	{
		LogConsole("error : CRenderDevice::CreateTexture : texture cache fullfilled...");
		assert(0);
		return INVALID;
	}
	
	CTexture * pTexture = new CTexture;
	bool bResult = pTexture->Create(m_pD3DDevice,dwWidth,dwHeight,dwColor,bUse32Tex,m_deviceInfo.d3dCaps.MaxTextureWidth, m_deviceInfo.d3dCaps.MaxTextureHeight);
	
	if ( !bResult )
	{
		LogConsole("error : CRenderDevice::CreateTexture : creating " << dwWidth << "*" << dwHeight << "sized texture failed...");
		SAFE_DELETE(pTexture);
		assert(0);
		return INVALID;
	}

	DWORD dwTex = 0; 
	for ( ; dwTex < MAX_TEXTURE_ARRAY_SIZE; dwTex++ )
	{
		if ( !m_pTextureArray[dwTex] )
		{
			m_pTextureArray[dwTex] = pTexture;
			break;
		}
	}

	m_dwTextureCnt++;

	return (int)dwTex;
}

void CRenderDevice::DestroyTexture(DWORD dwTextureHandle)
{
	assert(dwTextureHandle+1 <= MAX_TEXTURE_ARRAY_SIZE);
	
	if ( dwTextureHandle != INVALID )
	{
		CTexture * pTexture = m_pTextureArray[dwTextureHandle];
       if ( !pTexture ) return;
		SAFE_DESTROY(pTexture);
		m_pTextureArray[dwTextureHandle] = NULL;
		m_dwTextureCnt--;
	}   
}

void CRenderDevice::SetTexture(DWORD dwStage, DWORD dwTextureHandle)
{
	assert(dwTextureHandle+1 <= MAX_TEXTURE_ARRAY_SIZE);
	assert(dwStage < 8);
	
	if ( dwTextureHandle == INVALID) 
	{
		m_pD3DDevice->SetTexture(dwStage, NULL);
		return;
	}

	CTexture * pTexture  = m_pTextureArray[dwTextureHandle];
	assert(pTexture);
	m_pD3DDevice->SetTexture(dwStage, pTexture->GetD3DTexture());
}

void CRenderDevice::Cull(bool bEnable)
{
	DWORD dwCullMode = bEnable? D3DCULL_CCW : D3DCULL_NONE;
	m_pD3DDevice->SetRenderState(D3DRS_CULLMODE, dwCullMode);
}

void CRenderDevice::Dump(void)
{
	LogConsole("info : CRenderDevice::Dump : Dump Begin...");
	DWORD dwTexMemSize = 0;
	for ( DWORD n = 0; n < MAX_TEXTURE_ARRAY_SIZE; n++ )
	{
		CTexture * pTexture = m_pTextureArray[n];
		if ( pTexture )
		{
			dwTexMemSize += pTexture->GetWidth() * pTexture->GetHeight() * ((m_setting.iTextureMode == 32) ? sizeof(DWORD) : sizeof(WORD));
		}
	}
	LogConsole("Texture Count:" << m_dwTextureCnt << "Mem Usage:" << dwTexMemSize/1024.0f); 
	LogConsole("info : CRenderDevice::Dump : Dump End...");
}

bool CRenderDevice::CreateDirect3D9(void)
{
	// select default adapter
	if ( NULL == ( m_pD3D = Direct3DCreate9( D3D_SDK_VERSION ) ) )
	{
		LogConsole("error: CRenderDevice::CreateDirect3D9 : create DX9 failed..."); 
		assert(0);
		return FALSE;
	}
	return TRUE;
}

void CRenderDevice::DestroyDirect3D9()
{
	SAFE_RELEASE(m_pD3D);
}

bool CRenderDevice::Layer3D::CreateVertexBuffer()
{
	CRenderDevice * pRender = CRenderDevice::GetInstance();
	LPDIRECT3DDEVICE9 pDevice = pRender->GetRawDevice();

	// position
	if ( FAILED(pDevice->CreateVertexBuffer(sizeof(D3DXVECTOR3) * LAYER3D_VERTEXBUFFER_SIZE, 
		 D3DUSAGE_DYNAMIC | D3DUSAGE_WRITEONLY, 0, D3DPOOL_DEFAULT, &m_pVBpos, NULL)) )
	{
		LogConsole("error : CRenderDevice::Layer3D::CreateVertexBuffer : create pos stream failed...");
		assert(0);
		return false;
	}

	// normal
	if ( FAILED(pDevice->CreateVertexBuffer( sizeof(D3DXVECTOR3) * LAYER3D_VERTEXBUFFER_SIZE,
		 D3DUSAGE_DYNAMIC | D3DUSAGE_WRITEONLY, 0, D3DPOOL_DEFAULT, &m_pVBnormal, NULL)) )	
	{
		LogConsole("error : CRenderDevice::Layer3D::CreateVertexBuffer : create normal stream failed...");
		assert(0);
		return false;
	}

	// color
	if ( FAILED(pDevice->CreateVertexBuffer( sizeof(D3DCOLOR) * LAYER3D_VERTEXBUFFER_SIZE,
		 D3DUSAGE_DYNAMIC | D3DUSAGE_WRITEONLY ,0,D3DPOOL_DEFAULT , &m_pVBcolor,NULL)) )	
	{
		LogConsole("error : CRenderDevice::Layer3D::CreateVertexBuffer : create color stream failed...");
		assert(0);
		return false;
	}		

	// tex0
	if ( FAILED(pDevice->CreateVertexBuffer( sizeof(D3DXVECTOR2) * LAYER3D_VERTEXBUFFER_SIZE,
		 D3DUSAGE_DYNAMIC | D3DUSAGE_WRITEONLY, 0, D3DPOOL_DEFAULT, &m_pVBtex0, NULL)) )
	{
		LogConsole("error : CRenderDevice::Layer3D::CreateVertexBuffer : create tex0 stream failed...");
		assert(0);
		return false;
	}		

	// tex1
	if ( FAILED(pDevice->CreateVertexBuffer( sizeof(D3DXVECTOR2) * LAYER3D_VERTEXBUFFER_SIZE,
		 D3DUSAGE_DYNAMIC | D3DUSAGE_WRITEONLY, 0, D3DPOOL_DEFAULT, &m_pVBtex1, NULL)) )	
	{
		LogConsole("error : CRenderDevice::Layer3D::CreateVertexBuffer : create tex1 stream failed...");
		assert(0);
		return false;
	}		
	
	return true;
}

void CRenderDevice::Layer3D::DestroyVertexBuffer()
{
	SAFE_RELEASE(m_pVBpos);
	SAFE_RELEASE(m_pVBnormal);
	SAFE_RELEASE(m_pVBcolor);
	SAFE_RELEASE(m_pVBtex0);
	SAFE_RELEASE(m_pVBtex1);
}

void CRenderDevice::Layer3D::SetStreamType()
{
	SShader * pShader = m_pShaderArray[m_dwShaderInUse]; 
	SShader * pShaderLast = NULL;

	if ( m_dwShaderLast != INVALID )
		SShader * pShaderLast = m_pShaderArray[m_dwShaderLast];
	
	if ( pShaderLast && pShaderLast->eStreamType == pShader->eStreamType )
		return;
	
	CRenderDevice * pRender = CRenderDevice::GetInstance();
	LPDIRECT3DDEVICE9 pDevice = pRender->GetRawDevice();
	pDevice->SetVertexDeclaration(m_pVertexDeclarationArray[pShader->eStreamType]);
}

void CRenderDevice::Layer3D::CreateVertexDecl()
{
	D3DVERTEXELEMENT9 vertexDecl0[] =  // pos | color
	{
		{ 0, 0, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0 },
		{ 2, 0, D3DDECLTYPE_D3DCOLOR, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_COLOR, 0 },
		{ 0xFF, 0, D3DDECLTYPE_UNUSED, 0, 0, 0 },
	};
	
	D3DVERTEXELEMENT9 vertexDecl1[] = // pos | color | tex0
	{
		{ 0, 0, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0 },
		{ 2, 0, D3DDECLTYPE_D3DCOLOR, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_COLOR, 0 },
		{ 3, 0, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD,0 },
		{ 0xFF, 0, D3DDECLTYPE_UNUSED, 0, 0, 0},
	};

	D3DVERTEXELEMENT9 vertexDecl2[] = // pos | color | tex0 | tex1
	{
		{ 0, 0, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0 },
		{ 2, 0, D3DDECLTYPE_D3DCOLOR, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_COLOR, 0 },
		{ 3, 0, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD,0 },
		{ 4, 0, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD,1 },
		{ 0xFF, 0, D3DDECLTYPE_UNUSED, 0, 0, 0},
	};
	
	D3DVERTEXELEMENT9 vertexDecl3[] = // pos | normal | tex0 | tex1
	{
		{ 0, 0, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0 },
		{ 1, 0, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_NORMAL, 0 },
		{ 3, 0, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD,0 },
		{ 4, 0, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD,1 },
		{ 0xFF, 0, D3DDECLTYPE_UNUSED, 0, 0, 0},
	};

	D3DVERTEXELEMENT9 vertexDecl4[] = // pos | normal | color | tex0
	{
		{ 0, 0, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0 },
		{ 1, 0, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_NORMAL, 0 },
		{ 2, 0, D3DDECLTYPE_D3DCOLOR, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_COLOR, 0 },
		{ 3, 0, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD,0 },
		{ 0xFF, 0, D3DDECLTYPE_UNUSED, 0, 0, 0},
	};

	D3DVERTEXELEMENT9 vertexDecl5[] = // pos | normal | color | tex0 | tex1
	{
		{ 0, 0, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0 },
		{ 1, 0, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_NORMAL, 0 },
		{ 2, 0, D3DDECLTYPE_D3DCOLOR, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_COLOR, 0 },
		{ 3, 0, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD,0 },
		{ 4, 0, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD,1 },
		{ 0xFF, 0, D3DDECLTYPE_UNUSED, 0, 0, 0},
	};

	CRenderDevice * pRender = CRenderDevice::GetInstance();
	LPDIRECT3DDEVICE9 pDevice = pRender->GetRawDevice();
	pDevice->CreateVertexDeclaration(vertexDecl0,&m_pVertexDeclarationArray[0]);
	pDevice->CreateVertexDeclaration(vertexDecl1,&m_pVertexDeclarationArray[1]);
	pDevice->CreateVertexDeclaration(vertexDecl2,&m_pVertexDeclarationArray[2]);
	pDevice->CreateVertexDeclaration(vertexDecl3,&m_pVertexDeclarationArray[3]);
	pDevice->CreateVertexDeclaration(vertexDecl4,&m_pVertexDeclarationArray[4]);
	pDevice->CreateVertexDeclaration(vertexDecl5,&m_pVertexDeclarationArray[5]);
}

void CRenderDevice::Layer3D::DestroyVertexDecl()
{
	SAFE_RELEASE(m_pVertexDeclarationArray[0]);
	SAFE_RELEASE(m_pVertexDeclarationArray[1]);
	SAFE_RELEASE(m_pVertexDeclarationArray[2]);
	SAFE_RELEASE(m_pVertexDeclarationArray[3]);
	SAFE_RELEASE(m_pVertexDeclarationArray[4]);
	SAFE_RELEASE(m_pVertexDeclarationArray[5]);
}

bool CRenderDevice::Layer3D::Create()
{
	CreateVertexBuffer();
	CreateVertexDecl();
	return true;
}

void CRenderDevice::Layer3D::Destroy()
{
	DestroyVertexBuffer();
	DestroyVertexDecl();

	for ( DWORD n = 0; n < MAX_SHADER_ARRAY_SIZE; n++ )
	{
		SAFE_DELETE(m_pShaderArray[n]);
	}
}

DWORD CRenderDevice::Layer3D::CreateDefaultShader()
{
	if ( m_dwShaderCnt == MAX_SHADER_ARRAY_SIZE )
	{
		LogConsole("error : CRenderDevice::CreateTexture : texture cache fullfilled...");
		assert(0);
		return INVALID;
	}

	DWORD dwShaderHandle = 0;
	for ( ; dwShaderHandle < MAX_SHADER_ARRAY_SIZE; dwShaderHandle++)
	{
		if ( !m_pShaderArray[dwShaderHandle] ) break;
	}

	SShader * pShader = new SShader;
	m_pShaderArray[dwShaderHandle] = pShader;
	
	m_dwShaderCnt++;

	return dwShaderHandle;
}

DWORD CRenderDevice::Layer3D::CreateShader(
	int iTexHandle0, int iTexHandle1,
	ELAYER3D_SHADER_TYPE eShaderType,ELAYER3D_MULTISTREAM_TYPE eStreamType, bool bWireframe, bool bLighting,
	bool bZBuffer, bool bZWriteEnable, bool bBackfaceCulling,
	bool bGouraudShading, bool bBilinearFilter,bool bFogEnable)
{
	if ( m_dwShaderCnt == MAX_SHADER_ARRAY_SIZE )
	{
		LogConsole("error : CRenderDevice::CreateTexture : texture cache fullfilled...");
		assert(0);
		return INVALID;
	}

	DWORD dwShaderHandle = 0;
	for ( ; dwShaderHandle < MAX_SHADER_ARRAY_SIZE; dwShaderHandle++)
	{
		if ( !m_pShaderArray[dwShaderHandle] ) break;
	}

	SShader * pShader = new SShader;
    m_pShaderArray[dwShaderHandle] = pShader;
	pShader->bBackfaceCulling = bBackfaceCulling;
	pShader->bBilinearFilter = bBilinearFilter;
	pShader->bFogEnable = bFogEnable;
	pShader->bGouraudShading = bGouraudShading;
	pShader->bLighting = bLighting;
	pShader->bWireframe = bWireframe;
	pShader->bZBuffer = bZBuffer;
	pShader->bZWriteEnable = bZWriteEnable;
	pShader->eShaderType = eShaderType;
	pShader->eStreamType = eStreamType;
	pShader->iTexHandle0 = iTexHandle0;
	pShader->iTexHandle1 = iTexHandle1;
	m_dwShaderCnt++;
	return dwShaderHandle;
}

void CRenderDevice::Layer3D::SetShader(DWORD dwShaderHandle)
{
    //@ improve pending...
	m_dwShaderLast = m_dwShaderInUse;
	m_dwShaderInUse = dwShaderHandle;
}

CRenderDevice::Layer3D::SShader * CRenderDevice::Layer3D::GetShader(DWORD dwShaderHandle)
{
	if ( dwShaderHandle == INVALID || dwShaderHandle+1 > MAX_SHADER_ARRAY_SIZE )
	{
		LogConsole("error : CRenderDevice::Layer3D::GetShader : invalid shader handle...");
		assert(0);
		return NULL;
	}
	return m_pShaderArray[dwShaderHandle];
}

void CRenderDevice::Layer3D::DrawTriangleStrip(DWORD dwCount, const D3DXVECTOR3 * pPos,const D3DXVECTOR3 * pNml, const D3DCOLOR * pColor, const D3DXVECTOR2 * pUV0,const D3DXVECTOR2 * pUV1,bool b2D)
{
	SShader * pShader = m_pShaderArray[m_dwShaderInUse];

	if ( !pShader ) return;

	switch ( pShader->eStreamType ) 
	{
	case EL3DMST_POSCOLTEX0 :
		{
			DrawTriangleStrip(dwCount, pPos, pColor, pUV0, b2D);
		}
	break;

	case EL3DMST_POSCOLTEX0TEX1 :
		{
//			DrawTriangleStrip(dwCount, pPos, pColor, pUV0, pUV1);
		}
	break;

	case EL3DMST_POSNMLTEX0TEX1 : 
		{
//			DrawTriangleStrip(dwCount, pPos, pNormal, pUV0, pUV1);
		}
	break;
	}
}

void CRenderDevice::Layer3D::DrawTriangleList(DWORD dwCount, const D3DXVECTOR3 * pPos, const D3DXVECTOR3 * pNml, const D3DCOLOR * pColor, const D3DXVECTOR2 * pUV0, const D3DXVECTOR2 * pUV1)
{
	SShader * pShader = m_pShaderArray[m_dwShaderInUse];

	if ( !pShader ) return;

	switch ( pShader->eStreamType ) 
	{
	case EL3DMST_POSCOLTEX0 :
		{
			DrawTriangleListPCU1(dwCount, pPos, pColor, pUV0);
		}
	break;
	
	case EL3DMST_POSCOLTEX0TEX1 :
		{
			DrawTriangleListPCU2(dwCount, pPos, pColor, pUV0, pUV1);
		}
	break;

	case EL3DMST_POSNMLTEX0TEX1 : 
		{
			DrawTriangleListPNU2(dwCount, pPos, pNml, pUV0, pUV1);
		}
	break;

	case EL3DMST_POSNMLCOLTEX0 : 
		{
			DrawTriangleListPNCU1(dwCount, pPos, pNml, pColor, pUV0);
		}
	break;

	case EL3DMST_POSNMLCOLTEX0TEX1 : 
		{
			DrawTriangleListPNCU2(dwCount, pPos, pNml, pColor, pUV0, pUV1);
		}
	break;
	}
}

void CRenderDevice::Layer3D::DrawTriangleListPCU2(DWORD dwCount, const D3DXVECTOR3 * pPos, const D3DCOLOR * pColor, const D3DXVECTOR2 * pUV0, const D3DXVECTOR2 * pUV1)
{
	CRenderDevice * pRender = CRenderDevice::GetInstance();
	LPDIRECT3DDEVICE9 pDevice = pRender->GetRawDevice();

	SetTexture();
	SetStreamType();
	SetRenderState();

	D3DXVECTOR3 * pCachePos(NULL);
	D3DCOLOR    * pCacheCol(NULL);
	D3DXVECTOR2 * pCacheUV0(NULL);
	D3DXVECTOR2 * pCacheUV1(NULL);

	pDevice->SetStreamSource(0, m_pVBpos, 0, sizeof(D3DXVECTOR3));
	pDevice->SetStreamSource(1, m_pVBnormal, 0, sizeof(D3DXVECTOR3));
	pDevice->SetStreamSource(2, m_pVBcolor, 0, sizeof(D3DCOLOR));
	pDevice->SetStreamSource(3, m_pVBtex0, 0, sizeof(D3DXVECTOR2));
	pDevice->SetStreamSource(4, m_pVBtex1, 0, sizeof(D3DXVECTOR2));

	m_pVBpos->Lock(0, 0, (void**)&pCachePos, D3DLOCK_DISCARD);
	m_pVBcolor->Lock(0, 0, (void**)&pCacheCol, D3DLOCK_DISCARD);
	m_pVBtex0->Lock(0, 0, (void**)&pCacheUV0, D3DLOCK_DISCARD);
	m_pVBtex1->Lock(0, 0, (void**)&pCacheUV1, D3DLOCK_DISCARD);

	memcpy(pCacheCol, pColor, sizeof(DWORD) * 3 * dwCount);
	memcpy(pCachePos, pPos, sizeof(D3DXVECTOR3) * 3 * dwCount);
	memcpy(pCacheUV0, pUV0, sizeof(D3DXVECTOR2) * 3 * dwCount);
	memcpy(pCacheUV1, pUV1, sizeof(D3DXVECTOR2) * 3 * dwCount);

	m_pVBpos->Unlock();
	m_pVBcolor->Unlock();
	m_pVBtex0->Unlock();
	m_pVBtex1->Unlock();

	pDevice->DrawPrimitive( D3DPT_TRIANGLELIST, 0, dwCount);
	pDevice->SetStreamSource(0,NULL,0,0);
	pDevice->SetStreamSource(1,NULL,0,0);
	pDevice->SetStreamSource(2,NULL,0,0);
	pDevice->SetStreamSource(3,NULL,0,0);
	pDevice->SetStreamSource(4,NULL,0,0);
}

void CRenderDevice::Layer3D::DrawTriangleListPNU2(DWORD dwCount, const D3DXVECTOR3 * pPos, const D3DXVECTOR3 * pNml, const D3DXVECTOR2 * pUV0, const D3DXVECTOR2 * pUV1)
{
	CRenderDevice * pRender = CRenderDevice::GetInstance();
	LPDIRECT3DDEVICE9 pDevice = pRender->GetRawDevice();

	SetTexture();
	SetStreamType();
	SetRenderState();

	D3DXVECTOR3 * pCachePos(NULL);
	D3DCOLOR    * pCacheNml(NULL);
	D3DXVECTOR2 * pCacheUV0(NULL);
	D3DXVECTOR2 * pCacheUV1(NULL);

	pDevice->SetStreamSource(0, m_pVBpos, 0, sizeof(D3DXVECTOR3));
	pDevice->SetStreamSource(1, m_pVBnormal, 0, sizeof(D3DXVECTOR3));
	pDevice->SetStreamSource(2, m_pVBcolor, 0, sizeof(D3DCOLOR));
	pDevice->SetStreamSource(3, m_pVBtex0, 0, sizeof(D3DXVECTOR2));
	pDevice->SetStreamSource(4, m_pVBtex1, 0, sizeof(D3DXVECTOR2));

	m_pVBpos->Lock(0, 0, (void**)&pCachePos, D3DLOCK_DISCARD);
	m_pVBnormal->Lock(0, 0, (void**)&pCacheNml, D3DLOCK_DISCARD);
	m_pVBtex0->Lock(0, 0, (void**)&pCacheUV0, D3DLOCK_DISCARD);
	m_pVBtex1->Lock(0, 0, (void**)&pCacheUV1, D3DLOCK_DISCARD);

	memcpy(pCacheNml, pNml, sizeof(D3DXVECTOR3) * 3 * dwCount);
	memcpy(pCachePos, pPos, sizeof(D3DXVECTOR3) * 3 * dwCount);
	memcpy(pCacheUV0, pUV0, sizeof(D3DXVECTOR2) * 3 * dwCount);
	memcpy(pCacheUV1, pUV1, sizeof(D3DXVECTOR2) * 3 * dwCount);

	m_pVBpos->Unlock();
	m_pVBnormal->Unlock();
	m_pVBtex0->Unlock();
	m_pVBtex1->Unlock();

	pDevice->DrawPrimitive( D3DPT_TRIANGLELIST, 0, dwCount);
	pDevice->SetStreamSource(0,NULL,0,0);
	pDevice->SetStreamSource(1,NULL,0,0);
	pDevice->SetStreamSource(2,NULL,0,0);
	pDevice->SetStreamSource(3,NULL,0,0);
	pDevice->SetStreamSource(4,NULL,0,0);
}

void CRenderDevice::Layer3D::DrawTriangleListPNCU1(DWORD dwCount,const D3DXVECTOR3 * pPos, const D3DXVECTOR3 * pNml, const D3DCOLOR * pColor, const D3DXVECTOR2 * pUV0)
{
	CRenderDevice * pRender = CRenderDevice::GetInstance();
	LPDIRECT3DDEVICE9 pDevice = pRender->GetRawDevice();

	SetTexture();
	SetStreamType();
	SetRenderState();

	D3DXVECTOR3 * pCachePos(NULL);
	D3DXVECTOR3 * pCacheNml(NULL);
	D3DCOLOR    * pCacheCol(NULL);
	D3DXVECTOR2 * pCacheUV0(NULL);

	pDevice->SetStreamSource(0, m_pVBpos, 0, sizeof(D3DXVECTOR3));
	pDevice->SetStreamSource(1, m_pVBnormal, 0, sizeof(D3DXVECTOR3));
	pDevice->SetStreamSource(2, m_pVBcolor, 0, sizeof(D3DCOLOR));
	pDevice->SetStreamSource(3, m_pVBtex0, 0, sizeof(D3DXVECTOR2));
	pDevice->SetStreamSource(4, m_pVBtex1, 0, sizeof(D3DXVECTOR2));

	m_pVBpos->Lock(0, 0, (void**)&pCachePos, D3DLOCK_DISCARD);
	m_pVBnormal->Lock(0, 0, (void**)&pCacheNml, D3DLOCK_DISCARD);
	m_pVBcolor->Lock(0, 0, (void**)&pCacheCol, D3DLOCK_DISCARD);
	m_pVBtex0->Lock(0, 0, (void**)&pCacheUV0, D3DLOCK_DISCARD);

	memcpy(pCacheCol, pColor, sizeof(DWORD) * 3 * dwCount);
	memcpy(pCacheNml, pNml, sizeof(D3DXVECTOR3) * 3 * dwCount);
	memcpy(pCachePos, pPos, sizeof(D3DXVECTOR3) * 3 * dwCount);
	memcpy(pCacheUV0, pUV0, sizeof(D3DXVECTOR2) * 3 * dwCount);

	m_pVBpos->Unlock();
	m_pVBnormal->Unlock();
	m_pVBcolor->Unlock();
	m_pVBtex0->Unlock();

	pDevice->DrawPrimitive( D3DPT_TRIANGLELIST, 0, dwCount);
	pDevice->SetStreamSource(0,NULL,0,0);
	pDevice->SetStreamSource(1,NULL,0,0);
	pDevice->SetStreamSource(2,NULL,0,0);
	pDevice->SetStreamSource(3,NULL,0,0);
	pDevice->SetStreamSource(4,NULL,0,0);
}

void CRenderDevice::Layer3D::DrawTriangleListPNCU2(DWORD dwCount,const D3DXVECTOR3 * pPos, const D3DXVECTOR3 * pNml, const D3DCOLOR * pColor, const D3DXVECTOR2 * pUV0, const D3DXVECTOR2 * pUV1)
{
	CRenderDevice * pRender = CRenderDevice::GetInstance();
	LPDIRECT3DDEVICE9 pDevice = pRender->GetRawDevice();

	SetTexture();
	SetStreamType();
	SetRenderState();
	
	D3DXVECTOR3 * pCachePos(NULL);
	D3DXVECTOR3 * pCacheNml(NULL);
	D3DCOLOR    * pCacheCol(NULL);
	D3DXVECTOR2 * pCacheUV0(NULL);
	D3DXVECTOR2 * pCacheUV1(NULL);
	
	pDevice->SetStreamSource(0, m_pVBpos, 0, sizeof(D3DXVECTOR3));
	pDevice->SetStreamSource(1, m_pVBnormal, 0, sizeof(D3DXVECTOR3));
	pDevice->SetStreamSource(2, m_pVBcolor, 0, sizeof(D3DCOLOR));
	pDevice->SetStreamSource(3, m_pVBtex0, 0, sizeof(D3DXVECTOR2));
	pDevice->SetStreamSource(4, m_pVBtex1, 0, sizeof(D3DXVECTOR2));
	
	m_pVBpos->Lock(0, 0, (void**)&pCachePos, D3DLOCK_DISCARD);
	m_pVBnormal->Lock(0, 0, (void**)&pCacheNml, D3DLOCK_DISCARD);
	m_pVBcolor->Lock(0, 0, (void**)&pCacheCol, D3DLOCK_DISCARD);
	m_pVBtex0->Lock(0, 0, (void**)&pCacheUV0, D3DLOCK_DISCARD);
	m_pVBtex1->Lock(0, 0, (void**)&pCacheUV1, D3DLOCK_DISCARD);
	
	memcpy(pCacheCol, pColor, sizeof(DWORD) * 3 * dwCount);
	memcpy(pCacheNml, pNml, sizeof(D3DXVECTOR3) * 3 * dwCount);
	memcpy(pCachePos, pPos, sizeof(D3DXVECTOR3) * 3 * dwCount);
	memcpy(pCacheUV0, pUV0, sizeof(D3DXVECTOR2) * 3 * dwCount);
	memcpy(pCacheUV1, pUV1, sizeof(D3DXVECTOR2) * 3 * dwCount);

	m_pVBpos->Unlock();
	m_pVBnormal->Unlock();
	m_pVBcolor->Unlock();
	m_pVBtex0->Unlock();
	m_pVBtex1->Unlock();

	pDevice->DrawPrimitive( D3DPT_TRIANGLELIST, 0, dwCount);
	pDevice->SetStreamSource(0,NULL,0,0);
	pDevice->SetStreamSource(1,NULL,0,0);
	pDevice->SetStreamSource(2,NULL,0,0);
	pDevice->SetStreamSource(3,NULL,0,0);
	pDevice->SetStreamSource(4,NULL,0,0);
}

void CRenderDevice::Layer3D::DrawTriangleListPCU1(DWORD dwCount, const D3DXVECTOR3 * pPos, const D3DCOLOR * pColor, const D3DXVECTOR2 * pUV0)
{
	CRenderDevice * pRender = CRenderDevice::GetInstance();
	LPDIRECT3DDEVICE9 pDevice = pRender->GetRawDevice();

	SetTexture();
	SetStreamType();
	SetRenderState();

	D3DXVECTOR3 * pCachePos(NULL);
	D3DCOLOR    * pCacheCol(NULL);
	D3DXVECTOR2 * pCacheUV0(NULL);

	pDevice->SetStreamSource(0, m_pVBpos, 0, sizeof(D3DXVECTOR3));
	pDevice->SetStreamSource(1, m_pVBnormal, 0, sizeof(D3DXVECTOR3));
	pDevice->SetStreamSource(2, m_pVBcolor, 0, sizeof(D3DCOLOR));
	pDevice->SetStreamSource(3, m_pVBtex0, 0, sizeof(D3DXVECTOR2));
	pDevice->SetStreamSource(4, m_pVBtex1, 0, sizeof(D3DXVECTOR2));

	m_pVBpos->Lock(0, 0, (void**)&pCachePos, D3DLOCK_DISCARD);
	m_pVBcolor->Lock(0, 0, (void**)&pCacheCol, D3DLOCK_DISCARD);
	m_pVBtex0->Lock(0, 0, (void**)&pCacheUV0, D3DLOCK_DISCARD);

	memcpy(pCacheCol, pColor, sizeof(DWORD) * 3 * dwCount);
	memcpy(pCachePos, pPos, sizeof(D3DXVECTOR3) * 3 * dwCount);
	memcpy(pCacheUV0, pUV0, sizeof(D3DXVECTOR2) * 3 * dwCount);

	m_pVBpos->Unlock();
	m_pVBcolor->Unlock();
	m_pVBtex0->Unlock();

	pDevice->DrawPrimitive( D3DPT_TRIANGLELIST, 0, dwCount);
	pDevice->SetStreamSource(0,NULL,0,0);
	pDevice->SetStreamSource(1,NULL,0,0);
	pDevice->SetStreamSource(2,NULL,0,0);
	pDevice->SetStreamSource(3,NULL,0,0);
	pDevice->SetStreamSource(4,NULL,0,0);
}

void CRenderDevice::Layer3D::DrawLineStrip(DWORD dwCount, const D3DXVECTOR3 * pPos, const D3DCOLOR * pClr)
{
	CRenderDevice * pRender = CRenderDevice::GetInstance();
	LPDIRECT3DDEVICE9 pDevice = pRender->GetRawDevice();

	SetTexture();
	SetStreamType();
	SetRenderState();

	D3DXVECTOR3 * pCachePos;
	D3DCOLOR    * pCacheClr;

	pDevice->SetStreamSource(0, m_pVBpos, 0, sizeof(D3DXVECTOR3));
	pDevice->SetStreamSource(1, m_pVBnormal, 0, sizeof(D3DXVECTOR3));
	pDevice->SetStreamSource(2, m_pVBcolor, 0, sizeof(D3DCOLOR));
	pDevice->SetStreamSource(3, m_pVBtex0, 0, sizeof(D3DXVECTOR2));
	pDevice->SetStreamSource(4, m_pVBtex1, 0, sizeof(D3DXVECTOR2));

	m_pVBpos->Lock(0, 0, (void**)&pCachePos, D3DLOCK_DISCARD);
	m_pVBcolor->Lock(0, 0, (void**)&pCacheClr, D3DLOCK_DISCARD);

	memcpy(pCacheClr, pClr, sizeof(DWORD) * (dwCount+1));
	memcpy(pCachePos, pPos, sizeof(D3DXVECTOR3) * (dwCount+1));

	m_pVBpos->Unlock();
	m_pVBcolor->Unlock();

	pDevice->DrawPrimitive(D3DPT_LINESTRIP, 0, dwCount);
	pDevice->SetStreamSource(0,NULL,0,0);
	pDevice->SetStreamSource(1,NULL,0,0);
	pDevice->SetStreamSource(2,NULL,0,0);
	pDevice->SetStreamSource(3,NULL,0,0);
	pDevice->SetStreamSource(4,NULL,0,0);
}


void CRenderDevice::Layer3D::DrawTriangleStrip(DWORD dwCount, const D3DXVECTOR3 * pPos, const D3DCOLOR * pColor, const D3DXVECTOR2 * pUV0, bool b2D)
{
	CRenderDevice * pRender = CRenderDevice::GetInstance();
	LPDIRECT3DDEVICE9 pDevice = pRender->GetRawDevice();

	SetTexture();
	SetStreamType();
	
	if ( !b2D )	
		SetRenderState();
	else
		SetRenderState2D();

	D3DXVECTOR3 * pCachePos;
	D3DCOLOR    * pCacheCol;
	D3DXVECTOR2 * pCacheUV0;

	pDevice->SetStreamSource(0, m_pVBpos, 0, sizeof(D3DXVECTOR3));
	pDevice->SetStreamSource(1, m_pVBnormal, 0, sizeof(D3DXVECTOR3));
	pDevice->SetStreamSource(2, m_pVBcolor, 0, sizeof(D3DCOLOR));
	pDevice->SetStreamSource(3, m_pVBtex0, 0, sizeof(D3DXVECTOR2));
	pDevice->SetStreamSource(4, m_pVBtex1, 0, sizeof(D3DXVECTOR2));

	m_pVBpos->Lock(0, 0, (void**)&pCachePos, D3DLOCK_DISCARD);
	m_pVBcolor->Lock(0, 0, (void**)&pCacheCol, D3DLOCK_DISCARD);
	m_pVBtex0->Lock(0, 0, (void**)&pCacheUV0, D3DLOCK_DISCARD);

	memcpy(pCacheCol, pColor, sizeof(DWORD) * (dwCount+2));
	memcpy(pCachePos, pPos, sizeof(D3DXVECTOR3) * (dwCount+2));
	memcpy(pCacheUV0, pUV0, sizeof(D3DXVECTOR2) * (dwCount+2));

	m_pVBpos->Unlock();
	m_pVBcolor->Unlock();
	m_pVBtex0->Unlock();

	pDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, dwCount);
	pDevice->SetStreamSource(0,NULL,0,0);
	pDevice->SetStreamSource(1,NULL,0,0);
	pDevice->SetStreamSource(2,NULL,0,0);
	pDevice->SetStreamSource(3,NULL,0,0);
	pDevice->SetStreamSource(4,NULL,0,0);
}

CRenderDevice::Layer3D::Layer3D()
{
	ZeroMemory(m_pShaderArray,MAX_SHADER_ARRAY_SIZE * sizeof(SShader*));
	m_dwShaderCnt = 0;
	m_dwShaderInUse = m_dwShaderLast = INVALID;
	m_b2DState = false;
}

CRenderDevice::Layer3D::~Layer3D()
{
}

void CRenderDevice::Layer3D::SetTexture()
{
	CRenderDevice * pRender = CRenderDevice::GetInstance();
	LPDIRECT3DDEVICE9 pDevice = pRender->GetRawDevice();
	SShader * pShader = m_pShaderArray[m_dwShaderInUse];
	pRender->SetTexture(0,pShader->iTexHandle0);
	pRender->SetTexture(1,pShader->iTexHandle1);
}

void CRenderDevice::Layer3D::SetRenderState()
{
	m_b2DState = false;
	CRenderDevice * pRender = CRenderDevice::GetInstance();
	LPDIRECT3DDEVICE9 pDevice = pRender->GetRawDevice();
	SShader * pShader = m_pShaderArray[m_dwShaderInUse];
	SShader * pShaderLast = NULL;
	
	if ( m_dwShaderLast != INVALID )
		SShader * pShaderLast = m_pShaderArray[m_dwShaderLast];
	
	if ( !pShaderLast || pShaderLast->eShaderType != pShader->eShaderType )
	{
		switch ( pShader->eShaderType )
		{

		// for our blenddiffusealpha
		case EL3DST_SOLID_2_LAYER :
			{

//////////////////////////////////////////////////////////////////////////

				//pDevice->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG1);
				//pDevice->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);

				//pDevice->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);
				//pDevice->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
				//pDevice->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE);

				//pDevice->SetTextureStageState(1, D3DTSS_COLOROP, D3DTOP_BLENDCURRENTALPHA);
				//pDevice->SetTextureStageState(1, D3DTSS_COLORARG1, D3DTA_CURRENT);
				//pDevice->SetTextureStageState(1, D3DTSS_COLORARG2, D3DTA_TEXTURE);
				//
				//pDevice->SetTextureStageState(1, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1);
				//pDevice->SetTextureStageState(1, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);

				//pDevice->SetTextureStageState(2, D3DTSS_COLOROP, D3DTOP_MODULATE);
				//pDevice->SetTextureStageState(2, D3DTSS_COLORARG2, D3DTA_CURRENT);
				//pDevice->SetTextureStageState(2, D3DTSS_COLORARG1, D3DTA_DIFFUSE);

				//pDevice->SetRenderState(D3DRS_TEXTUREFACTOR,0xff000000);
				//pDevice->SetTextureStageState(2, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1);
				//pDevice->SetTextureStageState(2, D3DTSS_ALPHAARG1, D3DTA_TFACTOR);

//////////////////////////////////////////////////////////////////////////
				
				pDevice->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
				pDevice->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
				pDevice->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);
				pDevice->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1);
				pDevice->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
				pDevice->SetTextureStageState(1, D3DTSS_COLOROP, D3DTOP_BLENDDIFFUSEALPHA);
				pDevice->SetTextureStageState(1, D3DTSS_COLORARG2, D3DTA_CURRENT);
				pDevice->SetTextureStageState(1, D3DTSS_COLORARG1, D3DTA_TEXTURE);
				pDevice->SetRenderState(D3DRS_TEXTUREFACTOR,0xff000000);
				pDevice->SetTextureStageState(1, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1);
				pDevice->SetTextureStageState(1, D3DTSS_ALPHAARG1, D3DTA_TFACTOR);
				pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE,true);
				pDevice->SetRenderState(D3DRS_SRCBLEND,D3DBLEND_SRCALPHA);
				pDevice->SetRenderState(D3DRS_DESTBLEND,D3DBLEND_INVSRCALPHA);

//				pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);		
			}
		break;
			
		case EL3DST_SOLID_1_LAYER:
			{
				pDevice->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
				pDevice->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
				pDevice->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);
				pDevice->SetTextureStageState(0,D3DTSS_ALPHAOP,D3DTOP_SELECTARG1);
				pDevice->SetTextureStageState(0,D3DTSS_ALPHAARG1,D3DTA_TEXTURE);
				pDevice->SetTextureStageState(1,D3DTSS_COLOROP,D3DTOP_DISABLE);
				pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE,true);
				pDevice->SetRenderState(D3DRS_SRCBLEND,D3DBLEND_SRCALPHA);
				pDevice->SetRenderState(D3DRS_DESTBLEND,D3DBLEND_INVSRCALPHA);
			}
		break;
		
		case EL3DST_WIREFRAME_0_LAYER:
			{
				pDevice->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_DISABLE);
				pDevice->SetTextureStageState(1, D3DTSS_COLOROP, D3DTOP_DISABLE);
			}
		break;

		//case EL3DST_SOLID_ARKOBJECT:
		//	{
		//		pDevice->SetRenderState( D3DRS_LIGHTING,TRUE );
		//		pDevice->SetSamplerState(0,D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
		//		pDevice->SetSamplerState(0,D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP);
		//		pDevice->SetTextureStageState(0,D3DTSS_COLOROP,D3DTOP_MODULATE); 
		//		pDevice->SetTextureStageState(0,D3DTSS_COLORARG1,D3DTA_TEXTURE);  
		//		pDevice->SetTextureStageState(0,D3DTSS_COLORARG2,D3DTA_DIFFUSE); 
		//		pDevice->SetTextureStageState(0,D3DTSS_ALPHAOP,D3DTOP_SELECTARG1); 
		//		pDevice->SetTextureStageState(0,D3DTSS_ALPHAARG1,D3DTA_TEXTURE);  
		//	}
		//break;

		}
	}
	
	// bilinear	
	if ( !pShaderLast || pShaderLast->bBilinearFilter != pShader->bBilinearFilter )
	{
		if ( pShader->bBilinearFilter )
		{
			pDevice->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
			pDevice->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
			pDevice->SetSamplerState(0, D3DSAMP_MIPFILTER,  D3DTEXF_LINEAR);
			pDevice->SetSamplerState(1, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
			pDevice->SetSamplerState(1, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
			pDevice->SetSamplerState(1, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);

		}
		else
		{
			pDevice->SetSamplerState(0, D3DSAMP_MINFILTER,  D3DTEXF_POINT);
			pDevice->SetSamplerState(0, D3DSAMP_MIPFILTER,  D3DTEXF_POINT);
			pDevice->SetSamplerState(0, D3DSAMP_MAGFILTER,  D3DTEXF_POINT);
			pDevice->SetSamplerState(1, D3DSAMP_MINFILTER,  D3DTEXF_POINT);
			pDevice->SetSamplerState(1, D3DSAMP_MIPFILTER,  D3DTEXF_POINT);
			pDevice->SetSamplerState(1, D3DSAMP_MAGFILTER,  D3DTEXF_POINT);
		}
	}

	// fillmode
	if ( !pShaderLast || pShaderLast->bWireframe != pShader->bWireframe )
	{
		if ( !pShader->bWireframe )
			pDevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
		else
			pDevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);
	}

	// shademode
	if ( !pShaderLast || pShaderLast->bGouraudShading != pShader->bGouraudShading )
	{
		if ( pShader->bGouraudShading )
			pDevice->SetRenderState(D3DRS_SHADEMODE, D3DSHADE_GOURAUD);
		else
			pDevice->SetRenderState(D3DRS_SHADEMODE, D3DSHADE_FLAT);
	}
	
	// lighting
	if ( !pShaderLast || pShaderLast->bLighting != pShader->bLighting )
	{
		if ( pShader->bLighting )
			pDevice->SetRenderState(D3DRS_LIGHTING, TRUE);
		else
			pDevice->SetRenderState(D3DRS_LIGHTING, FALSE);
	}

	// zbuffer
	if ( !pShaderLast || pShaderLast->bZBuffer != pShader->bZBuffer )
	{
		if ( pShader->bZBuffer )
			pDevice->SetRenderState(D3DRS_ZENABLE, TRUE);
		else
			pDevice->SetRenderState(D3DRS_ZENABLE, FALSE);
	}

	// zwrite
	if ( !pShaderLast || pShaderLast->bZWriteEnable != pShader->bZWriteEnable )
	{
		if ( pShader->bZWriteEnable )
			pDevice->SetRenderState( D3DRS_ZWRITEENABLE, TRUE);
		else
			pDevice->SetRenderState( D3DRS_ZWRITEENABLE, FALSE);
	}
	
	// back face culling
	if ( !pShaderLast || pShaderLast->bBackfaceCulling != pShader->bBackfaceCulling )
	{
		if ( pShader->bBackfaceCulling )
			pDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
		else
			pDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
	}

	// fog
	if ( !pShaderLast || pShaderLast->bFogEnable != pShader->bFogEnable )
	{
		pDevice->SetRenderState(D3DRS_FOGENABLE, pShader->bFogEnable); 
	}
}

void CRenderDevice::Layer3D::SetRenderState2D()
{
	CRenderDevice * pRender = CRenderDevice::GetInstance();
	LPDIRECT3DDEVICE9 pDevice = pRender->GetRawDevice();
	
	D3DXMATRIX matIdentity;
	D3DXMatrixIdentity(&matIdentity);
	
	if ( !m_b2DState )
	{
//		pDevice->SetTransform(D3DTS_VIEW, &matIdentity);
//		pDevice->SetTransform(D3DTS_WORLD, &matIdentity);
//		pDevice->SetTransform(D3DTS_PROJECTION, &matIdentity);
		pDevice->SetRenderState(D3DRS_ZENABLE, FALSE);
		pDevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
		pDevice->SetRenderState(D3DRS_LIGHTING, FALSE);
		pDevice->SetRenderState(D3DRS_FOGENABLE, FALSE);
		pDevice->SetRenderState( D3DRS_STENCILENABLE, FALSE );
		pDevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_CCW);
		pDevice->SetTransform( D3DTS_TEXTURE0, &matIdentity );
		pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE,true);
		pDevice->SetRenderState(D3DRS_SRCBLEND,D3DBLEND_SRCALPHA);
		pDevice->SetRenderState(D3DRS_DESTBLEND,D3DBLEND_INVSRCALPHA);
		pDevice->SetTextureStageState(0,D3DTSS_COLOROP,D3DTOP_ADD);
		pDevice->SetTextureStageState(0,D3DTSS_COLORARG1,D3DTA_TEXTURE);
		pDevice->SetTextureStageState(0,D3DTSS_COLORARG2,D3DTA_DIFFUSE);
		pDevice->SetTextureStageState(0,D3DTSS_ALPHAOP,D3DTOP_SELECTARG1);
		pDevice->SetTextureStageState(0,D3DTSS_ALPHAARG1,D3DTA_DIFFUSE);
	}
	m_b2DState = true;
}

}
}
//////////////////////////////////////////////////////////////////////////
//
//	Copyright (C) 2003-2004
//  All Rights Reserved
//  In memory of Millennium 2000
//
//////////////////////////////////////////////////////////////////////////

#pragma once

#include "GraphicBase.h"

namespace bly
{
//////////////////////////////////////////////////////////////////////////

class CQuadXZ
{
	D3DXVECTOR3 m_vMin,m_vMax;

	D3DXVECTOR3 m_vCorner[4];

public:
	void	Set(float fxMin , float fxMax , float fzMin , float fzMax);
	inline 	const D3DXVECTOR3 * GetCorner() const
	{
		return m_vCorner;
	}

	void Get(D3DXVECTOR3 &vMin,D3DXVECTOR3 &vMax);

	CQuadXZ(void);
	~CQuadXZ(void);
};

//////////////////////////////////////////////////////////////////////////

class CBoundingBox
{
	D3DXVECTOR3 m_vCenter;
	float m_fOutsideRadius;

	D3DXVECTOR3 m_vMin,m_vMax;
	D3DXVECTOR3 m_vCorner[8];
	D3DXPLANE	m_plane[6];

public:
	void operator = (const CBoundingBox &BoundingBox);
	void Set(const D3DXVECTOR3 & vMin, const D3DXVECTOR3 & vMax);
	void Set(const D3DXVECTOR3 &vCenter,float fRadius);
	void Set(float x, float X, float y, float Y, float z, float Z);
	void SetMinY(float y);
	void SetMaxY(float Y);

	bool TestPointInside(D3DXVECTOR3 * pvPoint);
	bool TestRadialIntersected(D3DXVECTOR3 &vOrigin,D3DXVECTOR3 &vDirection);
	bool TestLineSegmentIntersected(D3DXVECTOR3 vLineSegment[2],D3DXVECTOR3 &vIntersection);
	bool TestTriangleIntersected(D3DXVECTOR3 vTriangle[3]);
	bool TestSphereIntersected(D3DXVECTOR3 &vCenter,float fRadius);

	void Get(D3DXVECTOR3 &vMin,D3DXVECTOR3 &vMax);
	void GetCenter(D3DXVECTOR3 &vCenter)
	{
		vCenter = m_vCenter;
	}
	inline float GetOutsideRadius()
	{
		return m_fOutsideRadius;
	}
	inline const D3DXVECTOR3 * GetCornerVertex() const
	{
		return m_vCorner;
	}
};

//////////////////////////////////////////////////////////////////////////

class CFrustum
{
	D3DXVECTOR3 m_vFrustum[8];
	D3DXPLANE   m_planeFrustum[6];
	float m_fYMin;
	float m_fYMax;
	float m_fXMin;
	float m_fXMax;
	float m_fZMin;
	float m_fZMax;
	DWORD m_dwHeightMask;

public:
	CFrustum();
	void CalculateFrustum(const D3DXMATRIX *pView,const D3DXMATRIX *pProjection, bool bMontain = false);
	bool BoxIntersectFrustum(const CBoundingBox * pBoundingBox);
	bool BoxIntersectFrustum(const D3DXMATRIX * pWorldMatrix,const CBoundingBox * pBoundingBox);
	bool BoxInFrustum(const CBoundingBox * pBoundingBox);
	bool PointInFrustum(const D3DXVECTOR3 * pPoint);
	bool QuadXZInFrustum(CQuadXZ * pQuadXZ, DWORD dwHeightMask);
	bool QuadXZIntersectFrustum(CQuadXZ * pQuadXZ, DWORD dwHeightMask);

	bool BoxIntersectFrustum(D3DXVECTOR3 * pvCorners);

	bool HighEnough(D3DXVECTOR3 & v);

	const D3DXVECTOR3 * GetCornerVertex() const
	{
		return m_vFrustum;
	}
};

}

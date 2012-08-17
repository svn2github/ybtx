//////////////////////////////////////////////////////////////////////////
//
//	Copyright (C) 2003-2004
//  All Rights Reserved
//  In memory of Millennium 2000
//
//////////////////////////////////////////////////////////////////////////

#pragma once
#include "Frustum.h"

#define MAX_GRID_IN_NODE 64

namespace bly
{

class CTerrainMesh;
class CVisibleSet;

/// ËÄ²æÊ÷½áµã
class CQTNode
{
	CBoundingBox m_box;

	D3DXVECTOR3 m_vMin;
	D3DXVECTOR3 m_vMax;

	CQTNode * m_pChildren[4];

	int * m_pIndices;
	int m_iIndexCnt;

	std::vector<int> m_vecVertexIndices;

	bool m_bLeaf;

public:
	CQTNode();
	~CQTNode();

	CBoundingBox * GetBox()
	{
		return & m_box;
	}
	bool IsLeaf();
	void Create(std::vector<int> & vIndices, CTerrainMesh * pMap);
	void UpdateBoundingBoxByHeight();
	void TestVisibility(CVisibleSet * pVisibleSet, CFrustum * pFrustum);
};

}
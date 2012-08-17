//////////////////////////////////////////////////////////////////////////
//	
//	Copyright (C) 2003-2004
//  All Rights Reserved
//  In memory of bluely
//  
//////////////////////////////////////////////////////////////////////////

#pragma once

#include "BluelyG.h"
#include "Render.h"

#define MAX_GRID_IN_NODE 64

namespace bg
{
namespace sc
{

class CTerrainMesh;
class CVisibleSet;

class CQTNode
{
	rd::CQuadXZ m_quad;
    
	CQTNode * m_pParent;
	CQTNode * m_pChildren[4];

	int * m_pIndices;
	int m_iIndexCnt;

public:
	CQTNode();
	~CQTNode();

	rd::CQuadXZ * GetQuadXZ() { return & m_quad;}
	bool IsLeaf();
	void Create(std::vector<int> & vIndices, CTerrainMesh * pMap);
	void TestVisibility(CVisibleSet * pVisibleSet);
};

}
}
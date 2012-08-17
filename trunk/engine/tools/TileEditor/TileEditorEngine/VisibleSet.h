//////////////////////////////////////////////////////////////////////////
//	
//	Copyright (C) 2003-2004
//  All Rights Reserved
//  In memory of bluely
//  
//////////////////////////////////////////////////////////////////////////

#pragma once

#include "BluelyG.h"

namespace bg
{
namespace sc
{

class CTerrainMesh;

class CVisibleSet
{
	bool * m_pGridFlags;
	DWORD * m_pGridIndices;
	DWORD	m_dwVisibleGridCnt;
	DWORD   m_dwGridCnt;
	DWORD   m_dwMapWidth;
	DWORD   m_dwMapDepth;
	
public:
	CVisibleSet();
	~CVisibleSet();

	void Create(CTerrainMesh * pMap);
	void Destroy();
	
	void SetVisible(int iGridIndex);
	
	DWORD * GetVisibleGridIndices() { return m_pGridIndices;}
	DWORD   GetVisibleGridCnt() { return m_dwVisibleGridCnt;}
	
	void Reset();
};

}
}
//////////////////////////////////////////////////////////////////////////
//  	
//	Copyright (C) 2003-2004
//	All Rights Reserved
//  In memory of bluely
//  
//////////////////////////////////////////////////////////////////////////

#pragma once

#include "BluelyG.h"
#include "Cmd.h"
#include "TerrainMesh.h"
#include "Operator.h"

namespace bg
{
namespace sc
{

class CCmdSetTexture : public CCmd
{
	struct SDiffGridTextureInfo
	{
		CTerrainMesh::STextureNodeInfo nodeInfo;
		D3DXVECTOR2  uv[2][4]; 
	};
	
	std::vector<SDiffGridTextureInfo> m_vecDiffs;
	
	COperator::SProcessParams m_processParams;

	DWORD m_dwGridCnt;

public:
	CCmdSetTexture() : m_dwGridCnt(0) { }
	~CCmdSetTexture() { }
	virtual void Execute();
	virtual void UnExecute();
	void AddGrid(DWORD dwGridIndex);
	DWORD GetGridCnt() { return m_dwGridCnt; }
};

}
}
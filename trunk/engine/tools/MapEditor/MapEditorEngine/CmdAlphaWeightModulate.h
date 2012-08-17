//////////////////////////////////////////////////////////////////////////
//
//	Copyright (C) 2003-2004
//	All Rights Reserved
//  In memory of Millennium 2000
//
//////////////////////////////////////////////////////////////////////////

#pragma once

#include "BluelyG.h"
#include "Cmd.h"
#include "TerrainMesh.h"
#include "Operator.h"
#include "ToolSetTileTexture.h"

namespace bly
{

class CCmdAlphaWeightModulate : public CCmd
{
public:
	struct SVertex
	{
		DWORD       dwVertexIndex;
		DWORD		weightColor;//保存对应点的alpha权重
	};

private:
	std::vector<SVertex> m_vecDiffs;

	CToolSetTileTexture::SProcessParams m_processParams;

	int m_nGridIndexHot;

	DWORD m_dwVertexCnt;

	class IsEqual
	{
		SVertex v;
	public:
		IsEqual(SVertex s) : v(s) { }
		~IsEqual() { }
		bool operator ()(SVertex s)
		{
			return s.dwVertexIndex == v.dwVertexIndex;
		}
	};

public:
	CCmdAlphaWeightModulate() : m_dwVertexCnt(0)
	{
		::ZeroMemory(&m_processParams,sizeof(CToolSetTileTexture::SProcessParams));
	}

	~CCmdAlphaWeightModulate() { }

	virtual void Execute();
	virtual void UnExecute();
	void AddVertex(DWORD dwVertexIndex);
	DWORD GetVertexCnt()
	{
		return m_dwVertexCnt;
	}
	void SetGridIndexHot(int n);
	void SetProcessParams(CToolSetTileTexture::SProcessParams processParams);
};

}
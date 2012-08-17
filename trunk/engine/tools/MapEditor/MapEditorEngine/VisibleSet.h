//////////////////////////////////////////////////////////////////////////
//
//	Copyright (C) 2003-2004
//  All Rights Reserved
//  In memory of Millennium 2000
//
//////////////////////////////////////////////////////////////////////////

#pragma once

namespace bly
{

class CTerrainMesh;

class CVisibleSet
{
	int		m_nVisibleGridCnt;
	int		m_nVisibleVertexCnt;
	int		m_nVertexCnt;
	int		m_nGridCnt;
	int   m_nMapWidth;
	int   m_nMapDepth;

	//可见格子以及顶点索引
	set<int> VisibleGridIndices;
	set<int> VisibleVertexIndices;

public:
	CVisibleSet();
	~CVisibleSet();

	void Create(CTerrainMesh * pMap);
	void Destroy();

	void SetVisible(const int& iGridIndex);
	void SetVertexVisible(const int& nVertexIndex);

	set<int>& GetVisibleGridIndices();
	int GetVisibleGridCnt();

	set<int>&  GetVisibleVertexIndices();
	int	 GetVisibleVertexCnt();

	bool IsVisibleGrid(const int& iGridIndex);

	void Reset();
};

}
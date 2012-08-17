#include "stdafx.h"
#include "VisibleSet.h"
#include "TerrainMesh.h"
#include "CMapEditApp.h"


namespace bly
{
	CVisibleSet::CVisibleSet()
	{
		m_nVisibleGridCnt = 0;
		m_nVisibleVertexCnt = 0;
		m_nVertexCnt = 0;
		m_nGridCnt = 0;
		m_nMapWidth = 0;
		m_nVertexCnt = 0;
	}

	CVisibleSet::~CVisibleSet()
	{
	}

	void CVisibleSet::Create(CTerrainMesh * pMap)
	{
		m_nMapWidth  = pMap->GetWidth();
		m_nMapDepth  = pMap->GetDepth();
		
		m_nGridCnt =  m_nMapWidth * m_nMapDepth;
		m_nVertexCnt =  (m_nMapWidth+1) * (m_nMapDepth+1);
	}

	void CVisibleSet::Destroy()
	{
		VisibleGridIndices.clear();
		VisibleVertexIndices.clear();
	}

	void CVisibleSet::SetVisible(const int& iGridIndex)
	{
		VisibleGridIndices.insert(iGridIndex);
	}

	void CVisibleSet::SetVertexVisible(const int& nVertexIndex)
	{
		VisibleVertexIndices.insert(nVertexIndex);
	}

	bool CVisibleSet::IsVisibleGrid(const int& iGridIndex)
	{
		if( VisibleGridIndices.find(iGridIndex) != VisibleGridIndices.end() )
			return true;
		else
			return false;
	}

	//ÖØÖÃ
	void CVisibleSet::Reset()
	{
		m_nVisibleGridCnt = 0;
		m_nVisibleVertexCnt = 0;
		VisibleGridIndices.clear();
		VisibleVertexIndices.clear();
	}

	int CVisibleSet::GetVisibleGridCnt()
	{
		return 0;//m_nVisibleGridCnt;
	}

	int CVisibleSet::GetVisibleVertexCnt()
	{
		return 0;//m_nVisibleVertexCnt;
	}

	set<int>& CVisibleSet::GetVisibleVertexIndices()
	{
		return VisibleVertexIndices;
	}

	set<int>& CVisibleSet::GetVisibleGridIndices()
	{
		return VisibleGridIndices;
	}

}
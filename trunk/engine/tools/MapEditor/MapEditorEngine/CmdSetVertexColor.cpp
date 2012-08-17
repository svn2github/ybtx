#include "stdafx.h"
#include "CmdSetVertexColor.h"
#include "TerrainMesh.h"
#include "CMapEditApp.h"
#include "TerrainAlgorithm.h"
#include "River.h"
#include "ModelGroup.h"
#include "TerrainMeshObject.h"
#include "Operator.h"
#include "Operator.inl"
#include "OperationLog.h"
#include "EditStateOpertiaonMgr.h"
#include <algorithm>

namespace sqr
{
	typedef set<uint32> TileCoordList;

	void CCmdSetVertexColor::_Execute()
	{
		CTerrainMesh* pTerrain = CMapEditApp::GetInst()->GetEditingMesh();
		if ( pTerrain == NULL )
			return;

		set<DWORD> bufferindexs;
		SGrid& grid = pTerrain->GetGrid(m_nGridIndexHot);

		int nVertexIndexHot = grid.dwVertexIndex[0];

		BYTE byRed		= BYTE(m_dwColor >> 16);
		BYTE byGreen	= BYTE(m_dwColor >> 8);
		BYTE byBlue		= BYTE(m_dwColor);

		char szColorBuf[128];

		bool bSetVertexDiffuseColor = CMapEditApp::GetInst()->GetOperator()->GetIsSetVertexDiffuseColor();
		if( bSetVertexDiffuseColor )
		{
			sprintf(szColorBuf, "DO 顶点diffuse色：r=%d g = %d b = %d", byRed, byGreen, byBlue);
			for ( vector<SVertex>::iterator iter = m_vecDiffs.begin(); iter != m_vecDiffs.end(); ++iter )
			{
				CTerrainAlgorithm::GetInst()->ModulateVertexDiffuseRGB(iter->dwVertexIndex,m_dwBrushInX, m_dwBrushInZ, byRed, byGreen, byBlue, nVertexIndexHot);
			}
		}
		else
		{
			bool bUseSpecular = CMapEditApp::GetInst()->GetOperator()->GetIsUseSpecular();
			if( !bUseSpecular )
				return;

			sprintf(szColorBuf, "DO 顶点高光色：r=%d g = %d b = %d", byRed, byGreen, byBlue);
			for ( vector<SVertex>::iterator iter = m_vecDiffs.begin(); iter != m_vecDiffs.end(); ++iter )
			{
				CTerrainAlgorithm::GetInst()->ModulateVertexSpecularRGB(iter->dwVertexIndex,m_dwBrushInX, m_dwBrushInZ, byRed, byGreen, byBlue, nVertexIndexHot);
			}
		}

		COpearationLog::GetInst()->WriteOperationLog(m_nGridIndexHot, szColorBuf);

		//TileCoordList tileCoordList;
		size_t size = m_vecGridIndices.size();
		for ( size_t i = 0; i <size; ++i )
		{
			int nGridIndex = m_vecGridIndices[i];
			SGrid &grid = pTerrain->GetGrid(nGridIndex);

			bufferindexs.insert(grid.dwBufferIndex);

			bool bEffectGridModel = false;

#ifdef USE_OLD_LIGHT_MODEL
			bEffectGridModel = grid.bEffectByVertexColor;
#else
			bEffectGridModel = true;
#endif
			if ( bEffectGridModel )
			{
				if ( !grid.vecCenterObjects.empty() && bSetVertexDiffuseColor )
				{
					for ( size_t j = 0; j < grid.vecCenterObjects.size(); ++j )
					{
						grid.vecCenterObjects[j]->GetModelGroup()->UpdateOverallAmbientRatio();
					}
				} 
			}

			// update water mesh
			if (pTerrain->IsWater() && bSetVertexDiffuseColor)
			{
				if( grid.pRiver )
					grid.pRiver->AddDirtyRegion(pTerrain->GetRegionIndexByGridIndex(nGridIndex));
			}

			//tileCoordList.insert(pTerrain->GetTileCoordByGrid(nGridIndex));
		}

// 		TileCoordList::iterator it  = tileCoordList.begin();
// 		TileCoordList::iterator eit = tileCoordList.end();
// 		for ( ; it != eit; ++it )
// 		{
// 			uint32 tileCoord = *it;
// 			CVertexDiffuseColor::GetInst()->BakeTileDiffuse(TILE_COORD_X(tileCoord), TILE_COORD_Y(tileCoord));
// 		}

 		//烘焙就不要
		if( !pTerrain->IsWater() && !bufferindexs.empty() )
		{
			if( CMapEditApp::GetInst()->GetOperator()->GetIsSetVertexDiffuseColor() )
				pTerrain->ReWriteTerrainBuffer( bufferindexs, 2 );
			else
				pTerrain->ReWriteTerrainBuffer( bufferindexs, 1 );///刷高光
		}
	}

	void CCmdSetVertexColor::_UnExecute()
	{
		CTerrainMesh * pTerrain = NULL;
		pTerrain = CMapEditApp::GetInst()->GetEditingMesh();
		if ( pTerrain == NULL )
			return;

		COpearationLog::GetInst()->WriteOperationLog(m_nGridIndexHot, "UNDO 顶点色");
		bool bSetVertexDiffuseColor = CMapEditApp::GetInst()->GetOperator()->GetIsSetVertexDiffuseColor();
		set<DWORD> bufferindexs;

		if( bSetVertexDiffuseColor )
		{
			for ( vector<SVertex>::iterator iter = m_vecDiffs.begin(); iter != m_vecDiffs.end(); ++iter )
			{
				CTerrainAlgorithm::GetInst()->SetVertexDiffuseRGB(iter->dwVertexIndex, iter->oColor);
			}
		}
		else
		{
			for ( vector<SVertex>::iterator iter = m_vecDiffs.begin(); iter != m_vecDiffs.end(); ++iter )
			{
				CTerrainAlgorithm::GetInst()->SetVertexSpecularRGB(iter->dwVertexIndex, iter->sColor);
			}
		}
		
		//TileCoordList tileCoordList;
		for ( size_t i = 0; i < m_vecGridIndices.size(); ++i )
		{
			SGrid & grid = pTerrain->GetGrid(m_vecGridIndices[i]);
			bufferindexs.insert(grid.dwBufferIndex);

			 bool bEffectGridModel = false;
#ifdef USE_OLD_LIGHT_MODEL
			bEffectGridModel = grid.bEffectByVertexColor;
#else
			bEffectGridModel = true;
#endif
			if ( bEffectGridModel )
 			{
 				if ( !grid.vecCenterObjects.empty() )
 				{
 					for ( size_t j = 0; j < grid.vecCenterObjects.size(); ++j )
 					{
 						grid.vecCenterObjects[j]->GetModelGroup()->UpdateOverallAmbientRatio();
 					}
 				} 
 			}

			// update water mesh
			if (pTerrain->IsWater())
			{
				if( grid.pRiver )
					grid.pRiver->AddDirtyRegion(pTerrain->GetRegionIndexByGridIndex(grid.dwGridIndex));
			}

			//tileCoordList.insert(pTerrain->GetTileCoordByGrid(m_vecGridIndices[i]));
		}

// 		TileCoordList::iterator it  = tileCoordList.begin();
// 		TileCoordList::iterator eit = tileCoordList.end();
// 		for ( ; it != eit; ++it )
// 		{
// 			uint32 tileCoord = *it;
// 			CVertexDiffuseColor::GetInst()->BakeTileDiffuse(TILE_COORD_X(tileCoord), TILE_COORD_Y(tileCoord));
// 		}

 		//烘焙就不要
		if( !pTerrain->IsWater() )
		{
			if( bSetVertexDiffuseColor )
				pTerrain->ReWriteTerrainBuffer( bufferindexs, 2 );
			else
				pTerrain->ReWriteTerrainBuffer( bufferindexs, 1 );///刷高光
		}
	}

	void CCmdSetVertexColor::AddVertex(DWORD dwVertexIndex)
	{
		CTerrainMesh* pTerrain = CMapEditApp::GetInst()->GetEditingMesh();
		if ( pTerrain == NULL )
			return;

		SVertex s;
		s.dwVertexIndex = dwVertexIndex;
		if ( find_if(m_vecDiffs.begin(),m_vecDiffs.end(),IsEqual(s)) != m_vecDiffs.end() )
		{
			return;
		}
		sqr::SVertex& vertex = pTerrain->GetVertex(dwVertexIndex);
		s.oColor = vertex.oColor;
		s.sColor = vertex.sColor;

		m_vecDiffs.push_back(s);
		m_dwVertexCnt = m_vecDiffs.size();
	}

	void CCmdSetVertexColor::SetProcessParams(const DWORD dwBrushInX, const DWORD dwBrushInZ, const DWORD dwColor)
	{
		m_dwBrushInX = dwBrushInX;
		m_dwBrushInZ = dwBrushInZ;
		m_dwColor    = dwColor;
	}

	void CCmdSetVertexColor::SetGridIndexHot(int n)
	{
		m_nGridIndexHot = n;
	}

	void CCmdSetVertexColor::AddGrid(int nGridIndex)
	{
		m_vecGridIndices.push_back(nGridIndex);
	}

	//------------------------------------------------------------------------------
	CCmdSetVertexColor::CCmdSetVertexColor()
			: m_dwVertexCnt(0)
	{
	}
}
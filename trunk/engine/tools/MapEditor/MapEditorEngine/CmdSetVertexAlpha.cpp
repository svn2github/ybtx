#include "stdafx.h"
#include "CmdSetVertexAlpha.h"
#include "TerrainMesh.h"
#include "CMapEditApp.h"
#include "TerrainAlgorithm.h"
#include "River.h"
#include "OperationLog.h"

namespace sqr
{
	void CCmdSetVertexAlpha::_Execute()
	{
		CTerrainMesh * pTerrain = NULL;
		pTerrain = CMapEditApp::GetInst()->GetEditingMesh();
		if ( pTerrain == NULL )
			return;

		SGrid & hotgrid = pTerrain->GetGrid(m_nGridIndexHot);
		int nVertexIndexHot = hotgrid.dwVertexIndex[0];

		char szAlphaBuf[256];
		sprintf(szAlphaBuf, "DO 刷第2层顶点 %d 混合,type = %d", nVertexIndexHot, m_processParams.eWayType);
		COpearationLog::GetInst()->WriteOperationLog(szAlphaBuf);

		for ( vector<SVertexAlpha>::iterator iter = m_vecDiffs.begin(); iter != m_vecDiffs.end(); ++iter )
		{
			if ( m_processParams.nLayerHot == 2 )
				break;

			if ( m_processParams.eWayType == sqr::EWT_CUT )
			{
				CTerrainAlgorithm::GetInst()->CutVertexAlpha(iter->dwVertexIndex);
				continue;
			}

			switch ( m_processParams.eDirType )
			{
				case sqr::EDT_X:
				{
					switch ( m_processParams.eWayType )
					{
						case sqr::EWT_INC:
						{
							CTerrainAlgorithm::GetInst()->IncVertexAlphaX(m_processParams.eAlphaType, iter->dwVertexIndex,m_processParams.dwBrushSizeInX, nVertexIndexHot);
						}
						break;

						case sqr::EWT_DEC:
						{
							CTerrainAlgorithm::GetInst()->DecVertexAlphaX(m_processParams.eAlphaType, iter->dwVertexIndex,m_processParams.dwBrushSizeInX, nVertexIndexHot);
						}
						break;
					}
				}
				break;

				case sqr::EDT_Z:
				{
					switch ( m_processParams.eWayType )
					{
						case sqr::EWT_INC:
						{
							CTerrainAlgorithm::GetInst()->IncVertexAlphaZ(m_processParams.eAlphaType, iter->dwVertexIndex,m_processParams.dwBrushSizeInZ, nVertexIndexHot);
						}
						break;

						case sqr::EWT_DEC:
						{
							CTerrainAlgorithm::GetInst()->DecVertexAlphaZ(m_processParams.eAlphaType, iter->dwVertexIndex,m_processParams.dwBrushSizeInZ, nVertexIndexHot);
						}
						break;
					}
				}
				break;

				case sqr::EDT_BOTH:
				{
					switch ( m_processParams.eWayType )
					{
						case sqr::EWT_INC:
						{
							CTerrainAlgorithm::GetInst()->IncVertexAlphaB(m_processParams.eAlphaType, iter->dwVertexIndex, m_processParams.dwBrushSizeInX, m_processParams.dwBrushSizeInZ, nVertexIndexHot);
						}
						break;

						case sqr::EWT_DEC:
						{
							CTerrainAlgorithm::GetInst()->DecVertexAlphaB(m_processParams.eAlphaType, iter->dwVertexIndex, m_processParams.dwBrushSizeInX, m_processParams.dwBrushSizeInZ, nVertexIndexHot);
						}
						break;
					}
				}
				break;
			}
		}

		set<DWORD> bufferindexs;
		size_t gridCnts = m_vecGridIndices.size();

		for ( size_t i = 0; i < gridCnts; ++i )
		{
			SGrid& grid = pTerrain->GetGrid(m_vecGridIndices[i]);

			bufferindexs.insert(grid.dwBufferIndex);
			// update water mesh
			if (pTerrain->IsWater() && grid.pRiver)
			{
				grid.pRiver->AddDirtyRegion(pTerrain->GetRegionIndexByGridIndex(grid.dwGridIndex));
			}
		}

		if ( !bufferindexs.empty() && !pTerrain->IsWater() )
		{
			pTerrain->ReWriteTerrainBuffer( bufferindexs, 2 );
		}
	}

	void CCmdSetVertexAlpha::_UnExecute()
	{
		if ( m_processParams.nLayerHot == 2 )
			return;

		CTerrainMesh * pTerrain = NULL;
		pTerrain = CMapEditApp::GetInst()->GetEditingMesh();
		if ( pTerrain == NULL )
			return;

		char szAlphaBuf[256];
		sprintf(szAlphaBuf, "UNDO 刷第2层顶点混合,type = %d", m_processParams.eWayType);
		COpearationLog::GetInst()->WriteOperationLog(szAlphaBuf);

		for ( vector<SVertexAlpha>::iterator iter = m_vecDiffs.begin(); iter != m_vecDiffs.end(); ++iter )
		{
			CTerrainAlgorithm::GetInst()->SetVertexAlpha(iter->dwVertexIndex, iter->oColor);
		}

		set<DWORD> bufferindexs;
		size_t gridCnts = m_vecGridIndices.size();

		for ( size_t i = 0; i < gridCnts; ++i )
		{
			SGrid &grid = pTerrain->GetGrid(m_vecGridIndices[i]);

			bufferindexs.insert(grid.dwBufferIndex);
			// update water mesh
			if (pTerrain->IsWater())
			{
				grid.pRiver->AddDirtyRegion(pTerrain->GetRegionIndexByGridIndex(grid.dwGridIndex));
			}
		}

		if ( !bufferindexs.empty() && !pTerrain->IsWater() )
		{
			pTerrain->ReWriteTerrainBuffer( bufferindexs, 2 );
		}
	}

	void CCmdSetVertexAlpha::AddVertex(DWORD dwVertexIndex)
	{
		CTerrainMesh * pTerrain = NULL;
		pTerrain = CMapEditApp::GetInst()->GetEditingMesh();
		if ( pTerrain == NULL )
			return;

		SVertexAlpha s;
		s.dwVertexIndex = dwVertexIndex;
		if ( find_if(m_vecDiffs.begin(),m_vecDiffs.end(),IsEqual(s)) != m_vecDiffs.end() )
		{
			return;
		}
		SVertex & vertex = pTerrain->GetVertex(dwVertexIndex);
		s.oColor = vertex.oColor;
		m_vecDiffs.push_back(s);
		m_dwVertexCnt = m_vecDiffs.size();
	}

	void CCmdSetVertexAlpha::AddGrid(int nGridIndex)
	{
		m_vecGridIndices.push_back(nGridIndex);
	}

	void CCmdSetVertexAlpha::SetProcessParams(CToolSetTileTexture::SProcessParams processParams)
	{
		m_processParams = processParams;
	}

	void CCmdSetVertexAlpha::SetGridIndexHot(int n)
	{
		m_nGridIndexHot = n;
	}
}
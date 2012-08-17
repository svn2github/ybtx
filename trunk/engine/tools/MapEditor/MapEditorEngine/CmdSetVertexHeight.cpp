#include "stdafx.h"
#include "CmdSetVertexHeight.h"
#include "TerrainMesh.h"
#include "CMapEditApp.h"
#include "TerrainAlgorithm.h"
#include "TerrainMeshObject.h"
#include "ModelGroup.h"
#include "OperationLog.h"
#include "WireBox.h"

namespace sqr
{
	void CCmdSetVertexHeight::_Execute()
	{
		CTerrainMesh * pTerrain = NULL;
		pTerrain = CMapEditApp::GetInst()->GetEditingMesh();
		if ( pTerrain == NULL )
			return;;

		set<DWORD> bufferindexs;
		SGrid & grid = pTerrain->GetGrid(m_nGridIndexHot);
		int nVertexIndexHot = grid.dwVertexIndex[0];

		COpearationLog::GetInst()->WriteOperationLog("DO网格面板下拉地形");

		for ( vector<SCmdVertex>::iterator iter = m_vecDiffs.begin(); iter != m_vecDiffs.end(); ++iter )
		{
			//////////////////////////////////////////////////////////////////////////

			sqr::SVertex & vertex = pTerrain->GetVertex(iter->dwVertexIndex);

			for ( int i = 0; i < 4; ++i )
			{
				SGrid & grid = pTerrain->GetGrid(vertex.dwGridsBelongto[i]);

				for ( size_t j = 0; j < grid.vecCenterObjects.size(); ++j )
				{
					CTObject * pObject = grid.vecCenterObjects[j];
					pObject->GetModelGroup()->SetIsChanged(true);
				}
			}

			//////////////////////////////////////////////////////////////////////////

			if ( m_processParams.eWayType == sqr::EWT_CUT )
			{
				CTerrainAlgorithm::GetInst()->CutVertexHeight(iter->dwVertexIndex,m_fReferenceHeight);
			}
			//////////////////////////////////////////////////////////////////////////

			if ( m_processParams.eWayType == sqr::EWT_SMOOTH )
			{
				CTerrainAlgorithm::GetInst()->SmoothVertexHeight(iter->dwVertexIndex);
				continue;
			}

			//////////////////////////////////////////////////////////////////////////

			switch ( m_processParams.eDirType )
			{
				case sqr::EDT_X:
				{
					switch ( m_processParams.eHeightType )
					{
						case sqr::EHT_LINEAR:
						{
							switch ( m_processParams.eWayType )
							{
								case sqr::EWT_INC:
								{
									CTerrainAlgorithm::GetInst()->IncLinearVertexHeightX(iter->dwVertexIndex, m_processParams.dwBrushSizeInX, nVertexIndexHot);
								}
								break;

								case sqr::EWT_DEC:
								{
									CTerrainAlgorithm::GetInst()->DecLinearVertexHeightX(iter->dwVertexIndex, m_processParams.dwBrushSizeInX, nVertexIndexHot);
								}
								break;
							}
						}
						break;
					}
				}
				break;

				case sqr::EDT_Z:
				{
					switch ( m_processParams.eHeightType )
					{
						case sqr::EHT_LINEAR:
						{
							switch ( m_processParams.eWayType )
							{
								case sqr::EWT_INC:
								{
									CTerrainAlgorithm::GetInst()->IncLinearVertexHeightZ(iter->dwVertexIndex, m_processParams.dwBrushSizeInZ, nVertexIndexHot);
								}
								break;

								case sqr::EWT_DEC:
								{
									CTerrainAlgorithm::GetInst()->DecLinearVertexHeightZ(iter->dwVertexIndex, m_processParams.dwBrushSizeInZ, nVertexIndexHot);
								}
								break;
							}
						}
						break;
					}
				}
				break;

				case sqr::EDT_BOTH:
				{
					switch ( m_processParams.eHeightType )
					{
						case sqr::EHT_LINEAR:
						{
							switch ( m_processParams.eWayType )
							{
								case sqr::EWT_INC:
								{
									CTerrainAlgorithm::GetInst()->IncLinearVertexHeightB(iter->dwVertexIndex, m_processParams.dwBrushSizeInX, m_processParams.dwBrushSizeInZ, nVertexIndexHot);
								}
								break;

								case sqr::EWT_DEC:
								{
									CTerrainAlgorithm::GetInst()->DecLinearVertexHeightB(iter->dwVertexIndex, m_processParams.dwBrushSizeInX, m_processParams.dwBrushSizeInZ, nVertexIndexHot);
								}
								break;
							}
						}
						break;

						case sqr::EHT_GAUSS:
						{
							switch ( m_processParams.eWayType )
							{
								case sqr::EWT_INC:
								{
									CTerrainAlgorithm::GetInst()->IncGaussVertexHeight(iter->dwVertexIndex, m_processParams.dwBrushSizeInX, m_processParams.dwBrushSizeInZ, nVertexIndexHot);
								}
								break;

								case sqr::EWT_DEC:
								{
									CTerrainAlgorithm::GetInst()->DecGaussVertexHeight(iter->dwVertexIndex, m_processParams.dwBrushSizeInX, m_processParams.dwBrushSizeInZ, nVertexIndexHot);
								}
								break;
							}
						}
						break;
					}
				}
				break;
			}
		}

		for ( vector<SCmdVertex>::iterator iter = m_vecDiffs.begin(); iter != m_vecDiffs.end(); ++iter )
		{
			CTerrainAlgorithm::GetInst()->CalculateVertexNormal(iter->dwVertexIndex);
		}

		for ( vector<SCmdVertex>::iterator iter = m_vecDiffs.begin(); iter != m_vecDiffs.end(); ++iter )
		{
			CTerrainAlgorithm::GetInst()->ReCalculateGridsHeight(iter->dwVertexIndex);
		}


		for ( size_t i = 0; i < m_vecGridIndices.size(); ++i )
		{
			SGrid &Grid = pTerrain->GetGrid(m_vecGridIndices[i]);

			bufferindexs.insert(Grid.dwBufferIndex);
		}

		if ( !bufferindexs.empty() && !pTerrain->IsWater() )
		{
			pTerrain->ReWriteTerrainBuffer( bufferindexs, 0 );
		}

		CTerrainMesh * pWater = NULL;

		pWater = CMapEditApp::GetInst()->GetWater();

		if ( pWater == NULL )
			return;

		for ( size_t i = 0; i < m_vecGridIndices.size(); ++i )
		{
			pWater->AdjustWaterMeshVisibility(m_vecGridIndices[i]);
		}

		///update logicheight wirebox
		for ( vector<SCmdVertex>::iterator iter = m_vecDiffs.begin(); iter != m_vecDiffs.end(); ++iter )
		{
 			SVertex &vertex = pTerrain->GetVertex(iter->dwVertexIndex);
 			if ( vertex.GetWireBoxLogicHeight() && vertex.fLogicHeight > 1.0f || vertex.fLogicHeight < -1.0f )
 			{
 				CVector3f vCenter = vertex.vPosition;

 				vertex.GetWireBoxLogicHeight()->SetRelativePos(vCenter);
 				vertex.GetWireBoxLogicHeight()->UpdateAbsolute();
 			}
		}
	}

	void CCmdSetVertexHeight::_UnExecute()
	{
		CTerrainMesh * pTerrain = NULL;
		pTerrain = CMapEditApp::GetInst()->GetEditingMesh();
		if ( pTerrain == NULL )
			return;

		COpearationLog::GetInst()->WriteOperationLog("UNDO网格面板下拉地形");

		for ( vector<SCmdVertex>::iterator iter = m_vecDiffs.begin(); iter != m_vecDiffs.end(); ++iter )
		{
			//////////////////////////////////////////////////////////////////////////

			sqr::SVertex & vertex = pTerrain->GetVertex(iter->dwVertexIndex);

			for ( int i = 0; i < 4; ++i )
			{
				SGrid & grid = pTerrain->GetGrid(vertex.dwGridsBelongto[i]);

				for ( size_t j = 0; j < grid.vecCenterObjects.size(); ++j )
				{
					CTObject * pObject = grid.vecCenterObjects[j];

					pObject->GetModelGroup()->SetIsChanged(true);
				}
			}

			//////////////////////////////////////////////////////////////////////////

			CTerrainAlgorithm::GetInst()->SetVertexHeight(iter->dwVertexIndex, iter->fHeight);
		}

		for ( vector<SCmdVertex>::iterator iter = m_vecDiffs.begin(); iter != m_vecDiffs.end(); ++iter )
		{
			CTerrainAlgorithm::GetInst()->CalculateVertexNormal(iter->dwVertexIndex);
		}

		for ( vector<SCmdVertex>::iterator iter = m_vecDiffs.begin(); iter != m_vecDiffs.end(); ++iter )
		{
			CTerrainAlgorithm::GetInst()->ReCalculateGridsHeight(iter->dwVertexIndex);
		}

		set<DWORD> bufferindexs;
		for ( size_t i = 0; i < m_vecGridIndices.size(); ++i )
		{
			SGrid &grid = pTerrain->GetGrid(m_vecGridIndices[i]);

			bufferindexs.insert(grid.dwBufferIndex);
		}

		if ( !bufferindexs.empty() && !pTerrain->IsWater() )
		{
			pTerrain->ReWriteTerrainBuffer( bufferindexs, 0 );
		}

		CTerrainMesh * pWater = CMapEditApp::GetInst()->GetWater();

		for ( size_t i = 0; i < m_vecGridIndices.size(); ++i )
		{
			pWater->AdjustWaterMeshVisibility(m_vecGridIndices[i]);
		}

		///update logicheight wirebox
		for ( vector<SCmdVertex>::iterator iter = m_vecDiffs.begin(); iter != m_vecDiffs.end(); ++iter )
		{
			SVertex &vertex = pTerrain->GetVertex(iter->dwVertexIndex);
			if ( vertex.GetWireBoxLogicHeight() && vertex.fLogicHeight > 1.0f || vertex.fLogicHeight < -1.0f )
			{
				CVector3f vCenter = vertex.vPosition;

				vertex.GetWireBoxLogicHeight()->SetRelativePos(vCenter);
				vertex.GetWireBoxLogicHeight()->UpdateAbsolute();
			}
		}
	}

	void CCmdSetVertexHeight::AddGrid(int nGridIndex)
	{
		m_vecGridIndices.push_back(nGridIndex);
	}

	void CCmdSetVertexHeight::AddVertex(DWORD dwVertexIndex)
	{
		CTerrainMesh * pTerrain = NULL;
		pTerrain = CMapEditApp::GetInst()->GetEditingMesh();
		if ( pTerrain == NULL )
			return;

		SCmdVertex s;
		s.dwVertexIndex = dwVertexIndex;

		if ( find_if(m_vecDiffs.begin(),m_vecDiffs.end(),IsEqual(s)) != m_vecDiffs.end() )
		{
			return;
		}

		sqr::SVertex & vertex = pTerrain->GetVertex(dwVertexIndex);
		s.fHeight = vertex.vPosition.y;
		m_vecDiffs.push_back(s);
		m_dwVertexCnt = m_vecDiffs.size();
	}

	void CCmdSetVertexHeight::SetProcessParams(CToolSetTileHeight::SProcessParams processParams)
	{
		m_processParams = processParams;
	}

	void CCmdSetVertexHeight::SetGridIndexHot(int n)
	{
		m_nGridIndexHot = n;
	}

	void CCmdSetVertexHeight::SetReferenceHeight(float fHeight)
	{
		m_fReferenceHeight = fHeight;
	}

	//------------------------------------------------------------------------------
	CCmdSetVertexHeight::CCmdSetVertexHeight()
			: m_dwVertexCnt(0)
	{
		m_nGridIndexHot = INVALID;
	}
}
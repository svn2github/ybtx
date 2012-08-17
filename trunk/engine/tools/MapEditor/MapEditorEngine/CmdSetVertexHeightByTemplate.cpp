#include "stdafx.h"
#include "CmdSetVertexHeightByTemplate.h"
#include "TerrainMesh.h"
#include "CMapEditApp.h"
#include "TerrainAlgorithm.h"
#include "Operator.h"
#include "ModelGroup.h"

namespace sqr
{
	void CCmdSetVertexHeightByTemplate::Execute()
	{
		CCmd::Execute();

		CTerrainMesh * pTerrain = NULL;
		pTerrain = CMapEditApp::GetInst()->GetEditingMesh();
		if ( pTerrain == NULL )
			return;

		SGrid & grid = pTerrain->GetGrid(m_nGridIndexHot);
		int nVertexIndexHot = grid.dwVertexIndex[0];

		for ( vector<SVertex>::iterator iter = m_vecDiffs.begin(); iter != m_vecDiffs.end(); ++iter )
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

			switch ( m_processParams.eTemplateType )
			{
				case sqr::EHTT_GAUSSCUT :
				{
					CTerrainAlgorithm::GetInst()->TemplateIncCutVertexHeightByGauss(iter->dwVertexIndex, m_processParams.dwBrushSizeInX, m_processParams.dwBrushSizeInZ, nVertexIndexHot);
				}
				break;

				case sqr::EHTT_LINEARCUT :
				{
					CTerrainAlgorithm::GetInst()->TemplateIncCutVertexHeightByLinear(iter->dwVertexIndex, m_processParams.dwBrushSizeInX, m_processParams.dwBrushSizeInZ, nVertexIndexHot);
				}
				break;
			}
		}

		for ( vector<SVertex>::iterator iter = m_vecDiffs.begin(); iter != m_vecDiffs.end(); ++iter )
		{
			pTerrain->CalculateVertexNormal(iter->dwVertexIndex);
		}

		for ( vector<SVertex>::iterator iter = m_vecDiffs.begin(); iter != m_vecDiffs.end(); ++iter )
		{
			pTerrain->ReCalculateGridsHeight(iter->dwVertexIndex);
		}

		CTerrainMesh * pWater = CMapEditApp::GetInst()->GetWater();

		for ( vector<SVertex>::iterator iter = m_vecDiffs.begin(); iter != m_vecDiffs.end(); ++iter )
		{
			pWater->AdjustWaterMeshVisibility(iter->dwVertexIndex);
		}
	}

	void CCmdSetVertexHeightByTemplate::UnExecute()
	{
		CCmd::UnExecute();

		CTerrainMesh * pTerrain = NULL;
		pTerrain = CMapEditApp::GetInst()->GetEditingMesh();
		if ( pTerrain == NULL )
			return;

		for ( vector<SVertex>::iterator iter = m_vecDiffs.begin(); iter != m_vecDiffs.end(); ++iter )
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

		for ( vector<SVertex>::iterator iter = m_vecDiffs.begin(); iter != m_vecDiffs.end(); ++iter )
		{
			pTerrain->CalculateVertexNormal(iter->dwVertexIndex);
		}

		for ( vector<SVertex>::iterator iter = m_vecDiffs.begin(); iter != m_vecDiffs.end(); ++iter )
		{
			pTerrain->ReCalculateGridsHeight(iter->dwVertexIndex);
		}

		CTerrainMesh * pWater = CMapEditApp::GetInst()->GetWater();

		for ( vector<SVertex>::iterator iter = m_vecDiffs.begin(); iter != m_vecDiffs.end(); ++iter )
		{
			pWater->AdjustWaterMeshVisibility(iter->dwVertexIndex);
		}
	}

	void CCmdSetVertexHeightByTemplate::AddVertex(DWORD dwVertexIndex)
	{
		CTerrainMesh * pTerrain = NULL;
		pTerrain = CMapEditApp::GetInst()->GetEditingMesh();
		if ( pTerrain == NULL )
			return;

		SVertex s;
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

	void CCmdSetVertexHeightByTemplate::SetProcessParams(CToolSetTileHeight::SProcessParams processParams)
	{
		m_processParams = processParams;
	}

	void CCmdSetVertexHeightByTemplate::SetGridIndexHot(int n)
	{
		m_nGridIndexHot = n;
	}

	//------------------------------------------------------------------------------
	CCmdSetVertexHeightByTemplate::CCmdSetVertexHeightByTemplate() : m_dwVertexCnt(0)
	{
		::ZeroMemory(&m_processParams,sizeof(COperator::SProcessParams));
		m_nGridIndexHot = INVALID;
	}
}
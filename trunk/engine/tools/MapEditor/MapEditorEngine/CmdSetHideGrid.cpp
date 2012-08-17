#include "stdafx.h"
#include "CmdSetHideGrid.h"
#include "ToolRegion.h"
#include "TerrainMesh.h"
#include "CMapEditApp.h"
#include "OperationLog.h"

namespace sqr
{
	void CCmdSetHideGrid::_Execute()
	{
		CTerrainMesh * pTerrain = NULL;
		pTerrain = CMapEditApp::GetInst()->GetEditingMesh();
		if ( pTerrain == NULL )
			return;

		

		char szHideGridBuf[64];
		sprintf(szHideGridBuf, "DO 是否隐藏格子：%d", bHide);
		COpearationLog::GetInst()->WriteOperationLog(szHideGridBuf);

		CToolRegion::GetInst()->SetTerrainGridIsHide(m_vecGridIndices, bHide);

		set<DWORD> bufferindexs;
		for ( size_t i = 0; i < m_vecGridIndices.size(); ++i )
		{

			SGrid & grid = pTerrain->GetGrid(m_vecGridIndices[i]);
			bufferindexs.insert(grid.dwBufferIndex);
		}

		if ( !bufferindexs.empty() && !pTerrain->IsWater() )
		{
			pTerrain->ReWriteTerrainBuffer( bufferindexs, 0 );
		}
	}

	void CCmdSetHideGrid::_UnExecute()
	{
		CTerrainMesh * pTerrain = NULL;
		pTerrain = CMapEditApp::GetInst()->GetEditingMesh();
		if ( pTerrain == NULL )
			return;

		

		char szHideGridBuf[64];
		sprintf(szHideGridBuf, "UNDO 是否隐藏格子：%d", bHide);

		COpearationLog::GetInst()->WriteOperationLog(szHideGridBuf);
		CToolRegion::GetInst()->SetTerrainGridIsHide(m_vecGridIndices, !bHide);

		set<DWORD> bufferindexs;
		for ( size_t i = 0; i < m_vecGridIndices.size(); ++i )
		{

			SGrid & grid = pTerrain->GetGrid(m_vecGridIndices[i]);
			bufferindexs.insert(grid.dwBufferIndex);
		}

		if ( !bufferindexs.empty() && !pTerrain->IsWater() )
		{
			pTerrain->ReWriteTerrainBuffer( bufferindexs, 0 );
		}
	}


	void CCmdSetHideGrid::AddGrid(int nGridIndex)
	{
		m_vecGridIndices.push_back(nGridIndex);
	}

	void CCmdSetHideGrid::AddGrids( vector<int> & gridindexs )
	{
		m_vecGridIndices = gridindexs;
	}

	void CCmdSetHideGrid::SetGridIsHide( bool b )
	{
		bHide = b;
	}
}
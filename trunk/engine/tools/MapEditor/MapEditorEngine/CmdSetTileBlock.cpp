#include "stdafx.h"
#include "CmdSetTileBlock.h"
#include "TerrainMesh.h"
#include "CMapEditApp.h"
#include "TerrainAlgorithm.h"
#include "OperationLog.h"

namespace sqr
{
	void CCmdSetTileBlock::_Execute()
	{
		

		CTerrainMesh * pTerrainMesh = NULL;
		pTerrainMesh = CMapEditApp::GetInst()->GetEditingMesh();
		if ( pTerrainMesh == NULL )
			return;

		if( m_processParams.eEditBrushType == CToolSetTileBlock::EBT_NORMAL )
		{
			char szBlockBuf[64];
			sprintf(szBlockBuf, "DO ×èµ²£º×èµ²ÀàÐÍ = %d", m_processParams.eEditType);
			COpearationLog::GetInst()->WriteOperationLog(szBlockBuf);
			set<DWORD> bufferindexs;

			for ( vector<SDiff>::iterator iter = m_vecDiffs.begin(); iter != m_vecDiffs.end(); ++iter )
			{
				if ( m_processParams.eEditType == CToolSetTileBlock::EET_WRITE )
				{
					if ( m_processParams.eBlockTypeHot == CToolSetTileBlock::EBT_NONE )
					{
						CTerrainAlgorithm::GetInst()->SetTileBlock(iter->dwGridIndex, m_processParams.eBlockTypeHot);
					}
					else
					{
						CTerrainAlgorithm::GetInst()->SetTileBlock(iter->dwGridIndex, m_processParams.eBlockTypeHot);
					}
				}
				else if ( m_processParams.eEditType == CToolSetTileBlock::EET_ERASE )
				{
					CTerrainAlgorithm::GetInst()->SetTileBlock(iter->dwGridIndex, 0);
				}

				SGrid &grid = pTerrainMesh->GetGrid(iter->dwGridIndex);
				bufferindexs.insert(grid.dwBufferIndex);
			}

			if ( !bufferindexs.empty() && !pTerrainMesh->IsWater() )
			{
				pTerrainMesh->ReWriteTerrainBuffer( bufferindexs, 9 );
			}
		}
		else
		{
			int nGridIndex = 0;
			vector<int> subindexs;
			int nBlockType = 0;
			size_t size = 0;
			bool bSetExactBlock = false;

			if ( m_processParams.eEditType == CToolSetTileBlock::EET_WRITE )
			{
				nBlockType = m_processParams.eBlockTypeHot;
				bSetExactBlock = true;
			}
			else
			{
				nBlockType = 0;
				bSetExactBlock = false;
			}

			if( m_processParams.eEditBrushType == CToolSetTileBlock::EBT_EXCAT )
			{
				for ( vector<SDiff>::iterator iter = m_vecDiffs.begin(); iter != m_vecDiffs.end(); ++iter )
				{
					int nGridIndex = iter->dwGridIndex;
					subindexs = iter->subindexs;
					SGrid &grid = pTerrainMesh->GetGrid(nGridIndex);
					grid.m_bSetExactBlock = true;
					size = subindexs.size();

					for ( size_t i = 0; i < size; ++i )
					{
						grid.subGridTypes[subindexs[i]] = nBlockType;
					}
				}
			}
		}

	}

	void CCmdSetTileBlock::_UnExecute()
	{
		

		CTerrainMesh * pTerrainMesh = NULL;
		pTerrainMesh = CMapEditApp::GetInst()->GetEditingMesh();
		if ( pTerrainMesh == NULL )
			return;

		if( m_processParams.eEditBrushType == CToolSetTileBlock::EBT_NORMAL )
		{
			char szBlockBuf[64];
			sprintf(szBlockBuf, "UNDO ×èµ²£º×èµ²ÀàÐÍ = %d", m_processParams.eEditType);
			COpearationLog::GetInst()->WriteOperationLog(szBlockBuf);
			set<DWORD> bufferindexs;

			for ( vector<SDiff>::iterator iter = m_vecDiffs.begin(); iter != m_vecDiffs.end(); ++iter )
			{
				CTerrainAlgorithm::GetInst()->SetTileBlock(iter->dwGridIndex, iter->nBlockType);

				SGrid &grid = pTerrainMesh->GetGrid(iter->dwGridIndex);
				bufferindexs.insert(grid.dwBufferIndex);
			}

			if ( !bufferindexs.empty() && !pTerrainMesh->IsWater() )
			{
				pTerrainMesh->ReWriteTerrainBuffer( bufferindexs, 9 );
			}
		}
		else
		{
			if( m_processParams.eEditBrushType == CToolSetTileBlock::EBT_EXCAT )
			{
				int nGridIndex = 0;
				vector<int> subindexs;
				int nBlockType = m_processParams.eBlockTypeHot;
				size_t size = 0;

				if( m_processParams.eEditBrushType == CToolSetTileBlock::EBT_EXCAT )
				{
					for ( vector<SDiff>::iterator iter = m_vecDiffs.begin(); iter != m_vecDiffs.end(); ++iter )
					{
						int nGridIndex = iter->dwGridIndex;
						subindexs = iter->subindexs;
						SGrid &grid = pTerrainMesh->GetGrid(nGridIndex);
						grid.m_bSetExactBlock = false;
						size = subindexs.size();

						for ( size_t i = 0; i < size; ++i )
							grid.subGridTypes[i] = nBlockType;
					}
				}
			}
		}
	}

	void CCmdSetTileBlock::AddGrid(DWORD dwGridIndex)
	{
		CTerrainMesh * pTerrainMesh = NULL;
		pTerrainMesh = CMapEditApp::GetInst()->GetEditingMesh();
		if ( pTerrainMesh == NULL )
			return;
		SGrid & grid = pTerrainMesh->GetGrid(dwGridIndex);

		SDiff s;
		s.dwGridIndex = dwGridIndex;
		s.nBlockType = grid.nBlockType;
		m_vecDiffs.push_back(s);
	}

	void CCmdSetTileBlock::AddSubGrid(DWORD dwGridIndex, vector<int>& subindexs)
	{
		CTerrainMesh * pTerrainMesh = NULL;
		pTerrainMesh = CMapEditApp::GetInst()->GetEditingMesh();
		if ( pTerrainMesh == NULL )
			return;
		SGrid & grid = pTerrainMesh->GetGrid(dwGridIndex);

		SDiff s;
		s.dwGridIndex = dwGridIndex;
		s.nBlockType = grid.nBlockType;
		s.subindexs = subindexs;
		m_vecDiffs.push_back(s);
	}

	void CCmdSetTileBlock::SetProcessParams(CToolSetTileBlock::SProcessParams processParams)
	{
		m_processParams = processParams;
	}
}
#include "stdafx.h"
#include "CmdSetSerialTexture.h"
#include "TerrainMesh.h"
#include "CMapEditApp.h"
#include "MEMath.h"
#include "TerrainAlgorithm.h"
#include "OperationLog.h"
#include "TraceHelper.h"

void CCmdSetSerialTexture::_Execute()
{
	

	CTerrainMesh *pTerrainMesh = CMapEditApp::GetInst()->GetEditingMesh();
	if ( !pTerrainMesh->IsWater() )
	{
		if ( m_processParams.eWayType == sqr::EWT_CUT || m_processParams.eWayType == sqr::EWT_DEC
			||  m_processParams.eWayType == EWT_SMOOTH )
			return;

		if ( !pTerrainMesh->CreateTerrainTexture( m_processParams.vecTextureNames ) )
			return;
	}
	bool bCliffLayer = CToolSetTileTexture::GetInst()->GetCliffLayerOne();

	char szTexBuf[256];
	sprintf(szTexBuf, "DO 刷第 %d 层 纹理，纹理名 %s", m_processParams.nLayerHot, m_processParams.vecTextureNames[0].c_str());
	COpearationLog::GetInst()->WriteOperationLog(szTexBuf);

	CToolSetTileTexture::GetInst()->CmdSequenceGridTex(this);

	int nGridIndex = 0;
	set<DWORD> bufferindexs;
	size_t size = m_vecDiffs.size();
	for ( size_t i = 0; i !=size; ++i)
	{
		nGridIndex = m_vecDiffs[i].nGridIndex;

		if ( pTerrainMesh->IsWater() )
		{
			pTerrainMesh->SetGridRiver(nGridIndex,m_processParams.pRiver);
		}
		else
		{
			SGrid & grid = pTerrainMesh->GetGrid(nGridIndex);
			bufferindexs.insert(grid.dwBufferIndex);
		}
	}

	if ( !pTerrainMesh->IsWater() )
	{
		//////////////////////////////////////////////////////////////////////////
		if ( !bufferindexs.empty() )
		{
			if ( bCliffLayer )
				pTerrainMesh->ReWriteTerrainBuffer( bufferindexs, 6 );
			else
			{
				switch (m_processParams.nLayerHot)
				{
				case 0:
					{
						pTerrainMesh->ReWriteTerrainBuffer( bufferindexs, 3 );
						break;
					}
				case 1:
					{
						pTerrainMesh->ReWriteTerrainBuffer( bufferindexs, 4 );
						break;
					}
				default:
					break;
				}
			}

		}

		//////////////////////////////////////////////////////////////////////////
	}
}

void CCmdSetSerialTexture::_UnExecute()
{
	

	CTerrainMesh * pTerrainMesh = NULL;
	pTerrainMesh = CMapEditApp::GetInst()->GetEditingMesh();
	if ( pTerrainMesh == NULL )
		return;

	char szTexBuf[64];
	sprintf(szTexBuf, "UNDO 刷第 %d 层 纹理", m_processParams.nLayerHot);
	COpearationLog::GetInst()->WriteOperationLog(szTexBuf);

	set<DWORD> bufferindexs;
	bool bCliffLayer = CToolSetTileTexture::GetInst()->GetCliffLayerOne();

	for ( vector<SDiffGridTextureInfo>::iterator iter = m_vecDiffs.begin(); iter != m_vecDiffs.end(); ++iter )
	{
		if ( pTerrainMesh->IsWater() )
		{
			pTerrainMesh->SetGridRiver(iter->nodeInfo.dwGridIndex,NULL);
		}
		else
		{
			SGrid & grid = pTerrainMesh->GetGrid(iter->nodeInfo.dwGridIndex);
			bufferindexs.insert(grid.dwBufferIndex);
			string str = iter->nodeInfo.strTextureNames[m_processParams.nLayerHot];
			DebugOut("Undo gird = %d, %s\n", grid.dwGridIndex, str.c_str());
			pTerrainMesh->SetGridTextureHandle(m_processParams.nLayerHot,iter->nodeInfo.dwGridIndex,iter->nodeInfo.strTextureNames[m_processParams.nLayerHot], bCliffLayer);
		}
	}

	if ( !pTerrainMesh->IsWater() )
	{
		size_t gridcount = m_vecDiffs.size();

		if ( m_processParams.nLayerHot == 0 )
		{
			for ( size_t i = 0; i < gridcount; ++i )
			{
				DWORD gridindex = m_vecDiffs[i].nodeInfo.dwGridIndex;

				SGrid & grid = pTerrainMesh->GetGrid(gridindex);

				grid.bCliffLayerOne = m_vecDiffs[i].bChiffLayerOne;

				if ( !grid.bCliffLayerOne && !bCliffLayer )
					CTerrainAlgorithm::GetInst()->ReBackGridLayerUV(gridindex, 0);
			}
		}

		if ( !bufferindexs.empty() )
		{
			switch (m_processParams.nLayerHot)
			{
			case 0:
				{
					pTerrainMesh->ReWriteTerrainBuffer( bufferindexs, 3 );
					break;
				}
			case 1:
				{
					pTerrainMesh->ReWriteTerrainBuffer( bufferindexs, 4 );
					break;
				}
			default:
				break;
			}
		}
	}
}

void CCmdSetSerialTexture::AddGrid(DWORD dwGridIndex)
{
	CTerrainMesh * pTerrain = NULL;
	pTerrain = CMapEditApp::GetInst()->GetEditingMesh();
	if ( pTerrain == NULL )
		return;

	SDiffGridTextureInfo s;
	SGrid &grid = pTerrain->GetGrid(dwGridIndex);
	s.nodeInfo = grid.nodeInfo;
	s.bChiffLayerOne = grid.bCliffLayerOne;
	s.nGridIndex = dwGridIndex;
	memcpy(s.uv,grid.uv,sizeof(s.uv));
	m_vecDiffs.push_back(s);
	++m_dwGridCnt;
}

void CCmdSetSerialTexture::SetProcessParams(CToolSetTileTexture::SProcessParams processParams)
{
	m_processParams = processParams;
}


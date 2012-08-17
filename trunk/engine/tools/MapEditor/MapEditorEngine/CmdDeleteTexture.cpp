#include "stdafx.h"
#include "CmdDeleteTexture.h"
#include "TerrainMesh.h"
#include "CMapEditApp.h"
#include "OperationLog.h"
#include "OperationLog.h"

namespace sqr
{
	void CCmdDeleteTexture::_Execute()
	{
		

		CTerrainMesh * pTerrain = NULL;
		pTerrain = CMapEditApp::GetInst()->GetEditingMesh();
		if ( pTerrain == NULL )
			return;

		int layerHot = m_processParams.nLayerHot;

		char szDeleteTexBuf[64];
		sprintf(szDeleteTexBuf, "DO 删除第 %d 层 纹理信息", layerHot);
		COpearationLog::GetInst()->WriteOperationLog(szDeleteTexBuf);

		for ( size_t i = 0; i < m_vecDiffs.size(); ++i)
		{
			SGrid & grid = pTerrain->GetGrid(m_vecDiffs[i].dwGridIndex);

			grid.nodeInfo.strTextureNames[layerHot] = "";
		}
	}

	void CCmdDeleteTexture::_UnExecute()
	{
		

		CTerrainMesh * pTerrain = NULL;
		pTerrain = CMapEditApp::GetInst()->GetEditingMesh();
		if ( pTerrain == NULL )
			return;

		int layerHot = m_processParams.nLayerHot;
		char szDeleteTexBuf[64];
		sprintf(szDeleteTexBuf, "UNDO 删除第 %d 层 纹理信息", layerHot);
		COpearationLog::GetInst()->WriteOperationLog(szDeleteTexBuf);

		for (size_t i = 0; i < m_vecDiffs.size(); ++i )
		{
			SGrid & grid = pTerrain->GetGrid(m_vecDiffs[i].dwGridIndex);

			grid.nodeInfo.strTextureNames[layerHot] = m_vecDiffs[i].strTextureNames[layerHot];
		}
	}

	void CCmdDeleteTexture::AddGrid(DWORD dwGridIndex)
	{
		STextureNodeInfo s;

		CTerrainMesh * pTerrain = NULL;
		pTerrain = CMapEditApp::GetInst()->GetEditingMesh();
		if ( pTerrain == NULL )
			return;

		SGrid & grid = pTerrain->GetGrid(dwGridIndex);

		s.dwGridIndex = grid.nodeInfo.dwGridIndex;
		s.strTextureNames[0] = grid.nodeInfo.strTextureNames[0];
		s.strTextureNames[1] = grid.nodeInfo.strTextureNames[1];
		s.strTextureNames[2] = grid.nodeInfo.strTextureNames[2];
		s.strTextureNames[3] = grid.nodeInfo.strTextureNames[3];

		m_vecDiffs.push_back(s);

		m_dwGridCnt++;
	}

	void CCmdDeleteTexture::SetProcessParams(CToolSetTileTexture::SProcessParams processParams)
	{
		m_processParams = processParams;
	}
}
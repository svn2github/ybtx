//////////////////////////////////////////////////////////////////////////
//  	
//	Copyright (C) 2003-2004
//	All Rights Reserved
//  In memory of bluely
//  
//////////////////////////////////////////////////////////////////////////

#pragma once

#include "Cmd.h"
#include "TerrainMesh.h"
#include "Operator.h"
#include "XFile.h"
#include "CmdSetTexture.h"

#if defined(_DEBUG) && defined(_WIN32)
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#endif

namespace bg
{
namespace sc
{

void CCmdSetTexture::Execute()
{
	CCmd::Execute();

	COperator * pOperator = CXFile::GetInstance()->GetOperator();
	CTerrainMesh * pTerrainMesh = CXFile::GetInstance()->GetTerrainMesh();

	DWORD dwTextureHandleHot;
	DWORD dwLayerHot;

	if ( m_processParams.dwLayerHot == INVALID )
	{
		memcpy(&m_processParams,pOperator->GetProcessParams(),sizeof(COperator::SProcessParams));
		m_processParams.pObjectHot.grab();
		dwTextureHandleHot = pOperator->GetTextureHandleHot();
		dwLayerHot = pOperator->GetLayerHot();
	}
	else	// use old params for redo
	{
		dwTextureHandleHot = m_processParams.dwTextureHandleHot;
		dwLayerHot = m_processParams.dwLayerHot;
	}
	
	for ( std::vector<SDiffGridTextureInfo>::iterator iter = m_vecDiffs.begin(); iter != m_vecDiffs.end(); ++iter )
	{
		pTerrainMesh->SetGridTextureHandle(dwLayerHot,iter->nodeInfo.dwGridIndex, dwTextureHandleHot);
	}
}

void CCmdSetTexture::UnExecute()
{
	CCmd::UnExecute();

	CTerrainMesh * pTerrainMesh = CXFile::GetInstance()->GetTerrainMesh();

	DWORD dwLayerHot = m_processParams.dwLayerHot;
	
	for ( std::vector<SDiffGridTextureInfo>::iterator iter = m_vecDiffs.begin(); iter != m_vecDiffs.end(); ++iter )
	{
		pTerrainMesh->SetGridTextureHandle(dwLayerHot,iter->nodeInfo.dwGridIndex,iter->nodeInfo.dwTextureHandles[dwLayerHot]);
	}
}

void CCmdSetTexture::AddGrid(DWORD dwGridIndex)
{
	SDiffGridTextureInfo s;
	CTerrainMesh::SGrid & grid = CXFile::GetInstance()->GetTerrainMesh()->GetGrid(dwGridIndex);
	s.nodeInfo = grid.nodeInfo;
	memcpy(s.uv,grid.uv,sizeof(s.uv));
	m_vecDiffs.push_back(s);
	m_dwGridCnt++;
}

}
}
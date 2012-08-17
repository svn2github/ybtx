//////////////////////////////////////////////////////////////////////////
//	
//	Copyright (C) 2003-2004
//  All Rights Reserved
//  In memory of bluely
//  
//////////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "VisibleSet.h"
#include "TerrainMesh.h"

#if defined(_DEBUG) && defined(_WIN32)
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#endif

namespace bg
{
namespace sc
{

CVisibleSet::CVisibleSet()
{
//	m_dwCurFlagValue = 0;
	m_dwVisibleGridCnt = 0;
}

CVisibleSet::~CVisibleSet()
{
}

void CVisibleSet::Create(CTerrainMesh * pMap)
{
	m_dwMapWidth  = pMap->GetWidth();
	m_dwMapDepth  = pMap->GetDepth();
	m_dwGridCnt =  m_dwMapWidth * m_dwMapDepth;
	m_pGridFlags = new bool[m_dwGridCnt];
	ZeroMemory(m_pGridFlags, sizeof(bool) * m_dwGridCnt);
	m_pGridIndices = new DWORD[m_dwGridCnt];
}

void CVisibleSet::Destroy()
{
	SAFE_DELETE_ARRAY(m_pGridIndices);
	SAFE_DELETE_ARRAY(m_pGridFlags)
}

void CVisibleSet::SetVisible(int iGridIndex)
{
	if ( !m_pGridFlags[iGridIndex] ) 
	{
		m_pGridIndices[m_dwVisibleGridCnt++] = iGridIndex;
		m_pGridFlags[iGridIndex] = true;
	}
}

void CVisibleSet::Reset()
{
	m_dwVisibleGridCnt = 0;
	ZeroMemory(m_pGridFlags, sizeof(bool) * m_dwGridCnt);
}

}
}
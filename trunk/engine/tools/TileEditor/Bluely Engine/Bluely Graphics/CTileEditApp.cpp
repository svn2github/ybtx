//////////////////////////////////////////////////////////////////////////
//  	
//	Copyright (C) 2003-2004
//	All Rights Reserved
//  In memory of bluely
//  
//////////////////////////////////////////////////////////////////////////

#pragma once

#include "extradefine.h"
#include "CTileEditApp.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

namespace bg
{
namespace sc
{

CTileEditApp * CTileEditApp::one = NULL;

void CTileEditApp::SetTerrainMesh(CTerrainMesh * pTerrainMesh)
{ 
	if ( m_pTerrainMesh )
		m_pTerrainMesh->Remove();
	
	m_pTerrainMesh = pTerrainMesh; 
}

void CTileEditApp::SetOperator(COperator * pOperator)
{
	if ( m_pOperator )
		m_pOperator->Remove();
    
	m_pOperator = pOperator;
}

}
}
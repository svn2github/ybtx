//////////////////////////////////////////////////////////////////////////
//  	
//	Copyright (C) 2003-2004
//	All Rights Reserved
//  In memory of bluely
//  
//////////////////////////////////////////////////////////////////////////

#pragma once

#include "extradefine.h"
#include "XFile.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

namespace bg
{
namespace sc
{

CXFile * CXFile::one = NULL;

void CXFile::SetTerrainMesh(CTerrainMesh * pTerrainMesh)
{ 
	if ( m_pTerrainMesh )
		m_pTerrainMesh->Remove();
	
	m_pTerrainMesh = pTerrainMesh; 
}

void CXFile::SetOperator(COperator * pOperator)
{
	if ( m_pOperator )
		m_pOperator->Remove();
    
	m_pOperator = pOperator;
}

}
}
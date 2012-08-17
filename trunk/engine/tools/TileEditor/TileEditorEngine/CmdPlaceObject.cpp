//////////////////////////////////////////////////////////////////////////
//  
//	Copyright (C) 2003-2004
//	All Rights Reserved
//  In memory of bluely
//  
//////////////////////////////////////////////////////////////////////////

#pragma once
#include "stdafx.h"
#include "Cmd.h"
#include "TerrainMesh.h"
#include "Operator.h"
#include "XFile.h"
#include "CmdPlaceObject.h"
#include "Picker.h"
#include "Face.h"

#if defined(_DEBUG) && defined(_WIN32)
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#endif

namespace bg
{
namespace sc
{
void CCmdPlaceObject::Execute()
{
	CCmd::Execute();
	
	CTerrainMesh * pTerrainMesh = CXFile::GetInstance()->GetTerrainMesh();

	m_diff.pObject->GetFace()->CleanBlockGridIndices();

	std::vector<int>::iterator iter = m_diff.vecBlockGridIndices.begin();
	
	CModel * pModel = m_diff.pObject->GetFace()->GetSelfPresentation(0);
	
	for ( int i = 0, int j = 0, k = 0; iter != m_diff.vecBlockGridIndices.end(); ++i, ++iter )
	{
		if ( (*iter) == INVALID )
		{
			if ( i != m_diff.vecBlockGridIndices.size()-1 ) 
			{
				j = 0;
				++k;
				pModel = m_diff.pObject->GetFace()->GetSelfPresentation(k);
			}
			continue;
		}
		

		pModel->AddBlockGridIndex(*iter);
	
		j++;
	}
}

void CCmdPlaceObject::UnExecute()
{
	CCmd::UnExecute();
	
	CTerrainMesh * pTerrainMesh = CXFile::GetInstance()->GetTerrainMesh();
	
	std::vector<int>::iterator iter = m_diff.vecBlockGridIndices.begin(); 
	
	CModel * pModel = m_diff.pObject->GetFace()->GetSelfPresentation(0);
	
	for ( int i = 0, int j = 0, k = 0; iter != m_diff.vecBlockGridIndices.end(); ++i, ++iter )
	{
		if ( (*iter) == INVALID )
		{
			if ( i != m_diff.vecBlockGridIndices.size()-1 ) 
			{
				j = 0;
				++k;
				pModel = m_diff.pObject->GetFace()->GetSelfPresentation(k);
			}
			continue;
		}
	
		CTerrainMesh::SGrid & grid = pTerrainMesh->GetGrid(*iter);
		
		grid.vecObjects.erase(
			remove(grid.vecObjects.begin(), grid.vecObjects.end(), m_diff.pObject), grid.vecObjects.end() );
		
		//std::vector< std::pair<float,CTerrainMesh::CObject*> >::iterator iter = grid.vecLogicHeights.begin();
		//
		//for( ; iter != grid.vecLogicHeights.end(); ++iter )
		//{
		//	if ( iter->second == m_diff.pObject.ptr() )
		//	{
		//		grid.vecLogicHeights.erase(iter);
		//		break;
		//	}
		//}
	}
	
	CTerrainMesh::SGrid & grid = pTerrainMesh->GetGrid(m_diff.nHotGridIndex);
	grid.vecCenterObjects.erase(
		remove(grid.vecCenterObjects.begin(), grid.vecCenterObjects.end(), m_diff.pObject), grid.vecCenterObjects.end() );

	m_diff.pObject->GetFace()->CleanBlockGridIndices();
	m_diff.pObject->GetFace()->SetEditState(CModel::EES_MOVING);
	m_diff.pObject->SetCenterGridIndex(INVALID);
	
	// update hot
	CTerrainMesh::CObject * p = NULL;
	int nHotGridIndex = sc::CPicker::GetInstance()->GetPickPos(&p);
	CXFile::GetInstance()->GetOperator()->SetGridIndexHot(nHotGridIndex);
	CXFile::GetInstance()->GetOperator()->SetObjectHot(p);
}

}
}
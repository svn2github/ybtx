//////////////////////////////////////////////////////////////////////////
//  	
//	Copyright (C) 2003-2004
//	All Rights Reserved
//  In memory of bluely
//  
//////////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "TerrainMesh.h"
#include "Operator.h"
#include "XFile.h"
#include "CmdRotateObject.h"
#include "Scene.h"

#if defined(_DEBUG) && defined(_WIN32)
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#endif

namespace bg
{
namespace sc
{

void CCmdRotateObject::Execute()
{
	CCmd::Execute();

	CTerrainMesh * pTerrainMesh = CXFile::GetInstance()->GetTerrainMesh();

	// Caution : no need to change the center flag for rotation!!!
	
	for ( int nModelIndex = 0; nModelIndex < m_diff.pObject->GetFace()->GetSelfPresentationCount(); nModelIndex++ )
	{
		CModel * pModel = m_diff.pObject->GetFace()->GetSelfPresentation(nModelIndex);
		
		// remove block flag
		for ( int i = 0; i < pModel->GetBlockSize(); i++ )
		{
			int nBlockGridIndex = pModel->GetBlockGridIndex(i);
		
			CTerrainMesh::SGrid & grid = CXFile::GetInstance()->GetTerrainMesh()->GetGrid(nBlockGridIndex);
		
			grid.vecObjects.erase(
				remove(grid.vecObjects.begin(), grid.vecObjects.end(), m_diff.pObject), grid.vecObjects.end() );
		}
		
		pModel->UpdateRotationDiscrete();
		
		for ( int i = 0; i < pModel->GetBlockSize(); i++ )
		{
			int nBlockGridIndex = pModel->GetBlockGridIndex(i);
		
			CTerrainMesh::SGrid & grid = CXFile::GetInstance()->GetTerrainMesh()->GetGrid(nBlockGridIndex);
		
			grid.vecObjects.push_back(m_diff.pObject);
		}
	}
}

void CCmdRotateObject::UnExecute()
{
	CCmd::UnExecute();
	CTerrainMesh * pTerrainMesh = CXFile::GetInstance()->GetTerrainMesh();

	// Caution : no need to change the center flag for rotation!!!
	for ( int nModelIndex = 0; nModelIndex < m_diff.pObject->GetFace()->GetSelfPresentationCount(); nModelIndex++ )
	{
		CModel * pModel = m_diff.pObject->GetFace()->GetSelfPresentation(nModelIndex);

		// remove block flag
		for ( int i = 0; i < pModel->GetBlockSize(); i++ )
		{
			int nBlockGridIndex = pModel->GetBlockGridIndex(i);

			CTerrainMesh::SGrid & grid = CXFile::GetInstance()->GetTerrainMesh()->GetGrid(nBlockGridIndex);

			grid.vecObjects.erase(
				remove(grid.vecObjects.begin(), grid.vecObjects.end(), m_diff.pObject), grid.vecObjects.end() );
		}

		pModel->UpdateRotationDiscrete();
		pModel->UpdateRotationDiscrete();
		pModel->UpdateRotationDiscrete();

		for ( int i = 0; i < pModel->GetBlockSize(); i++ )
		{
			int nBlockGridIndex = pModel->GetBlockGridIndex(i);

			CTerrainMesh::SGrid & grid = CXFile::GetInstance()->GetTerrainMesh()->GetGrid(nBlockGridIndex);

			grid.vecObjects.push_back(m_diff.pObject);
		}
	}
}

}
}
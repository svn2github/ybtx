//////////////////////////////////////////////////////////////////////////
//  	
//	Copyright (C) 2003-2004
//	All Rights Reserved
//  In memory of bluely
//  
//////////////////////////////////////////////////////////////////////////

#include "TerrainMesh.h"
#include "Operator.h"
#include "XFile.h"
#include "CmdDeleteObject.h"
#include "Scene.h"
#include "Picker.h"

#if defined(_DEBUG) && defined(_WIN32)
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#endif

namespace bg
{
namespace sc
{

void CCmdDeleteObject::Execute()
{
	// pull up
	CCmd::Execute();
	
	// update hot
	CTerrainMesh::CObject * p = NULL;
	DWORD dwGridIndexHot = sc::CPicker::GetInstance()->GetPickPos(&p);
	CXFile::GetInstance()->GetOperator()->SetGridIndexHot(dwGridIndexHot);
	CXFile::GetInstance()->GetOperator()->SetObjectHot(p);
}

void CCmdDeleteObject::UnExecute()
{
	// back to stationary state
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
	
		grid.vecObjects.push_back(m_diff.pObject);
	
		/* [ 7131059 every piece a center ]
		if ( *iter == pModel->GetLogicalCenterGridIndex() )
		{
		grid.vecCenterObjects.push_back(m_diff.pObject);
		}
		*/
	
		//float f = pModel->GetGridLogicHeight(j);
		//grid.vecLogicHeights.push_back(make_pair(f,m_diff.pObject.ptr()));
		//
		pModel->AddBlockGridIndex(*iter);
		
		j++;
	}

	// [ 7131059 every Face a center
	CTerrainMesh::SGrid & grid = pTerrainMesh->GetGrid(m_diff.nCenterGridIndex);
	grid.vecCenterObjects.push_back(m_diff.pObject);
	// ]

	// assumption: no pre active object exists
	CXFile::GetInstance()->GetOperator()->SetActiveObject(NULL);
	m_diff.pObject->SetCenterGridIndex(m_diff.nCenterGridIndex);
	m_diff.pObject->GetFace()->SetEditState(CModel::EES_PLACED);
}

}
}
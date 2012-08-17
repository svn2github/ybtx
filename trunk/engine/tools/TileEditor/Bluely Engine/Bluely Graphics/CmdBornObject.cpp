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
#include "CmdBornObject.h"
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

void CCmdBornObject::Execute()
{
	CCmd::Execute();
	
	if ( m_diff.pObject.ptr() == NULL )
	{
		m_diff.pObject = new CTerrainMesh::CObject;           
		CFace * pFace = new CFace;
				
		std::vector<sc::CModel*> vec;

		for ( std::vector<std::string>::iterator iter = m_diff.vecModelFileNames.begin(); iter != m_diff.vecModelFileNames.end(); ++iter )
		{
			sc::CModel * p = sc::CSceneManager::GetInstance()->AddNodeModel(*iter, m_diff.bSubUnit);
			vec.push_back(p);
		}
		
		pFace->AddContainer(vec,m_diff.bSubUnit,m_diff.bBlockType,0,0);
		
		pFace->SetModelFileNames(m_diff.vecModelFileNames);
		
		m_diff.pObject->SetFace(pFace);
		m_diff.pPreActiveObject = CXFile::GetInstance()->GetOperator()->GetActiveObject();
	}
	
	m_diff.pObject->GetFace()->SetEditState(CModel::EES_MOVING);
	
	if ( m_diff.pPreActiveObject.ptr() != NULL )
	{
		m_diff.pPreActiveObject->GetFace()->SetEditState(CModel::EES_DANGLING);
	}
	
	CXFile::GetInstance()->GetOperator()->SetActiveObject(m_diff.pObject);
}

void CCmdBornObject::UnExecute()
{
	CCmd::UnExecute();
	m_diff.pObject->GetFace()->SetEditState(CModel::EES_DANGLING);
	CXFile::GetInstance()->GetOperator()->SetActiveObject(m_diff.pPreActiveObject);
	if ( m_diff.pPreActiveObject.ptr() != NULL )
	{
		m_diff.pPreActiveObject->GetFace()->SetEditState(CModel::EES_MOVING);
	}
}

}
}
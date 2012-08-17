#include "stdafx.h"
#include "CmdRotateObject.h"
#include "TerrainMesh.h"
#include "CMapEditApp.h"
#include "CMapEditObject.h"
#include "Operator.h"
#include "Operator.inl"
#include "ModelGroup.h"

namespace sqr
{
	void CCmdRotateObject::_Execute()
	{
		

		CTerrainMesh * pTerrainMesh = CMapEditApp::GetInst()->GetEditingMesh();
		COperator * pOperator = CMapEditApp::GetInst()->GetOperator();

		vector<DWORD> vecHeightVertexIndices;

		for ( int nModelIndex = 0; nModelIndex < m_diff.pObject->GetModelGroup()->GetSelfPresentationCount(); nModelIndex++ )
		{
			vecHeightVertexIndices.clear();

			CMapEditObject * pModel = m_diff.pObject->GetModelGroup()->GetSelfPresentation(nModelIndex);
		}

		// erase center image info
		CMapEditObject * pModel = m_diff.pObject->GetModelGroup()->GetSelfPresentation(0);
		int nCenterGridIndex = pModel->GetUserOffsetAdjustedGridIndex(m_diff.pObject->GetCenterGridIndex());
		SGrid & gridCenter = pTerrainMesh->GetGrid(nCenterGridIndex);
		gridCenter.vecCenterObjects.erase(
			remove(gridCenter.vecCenterObjects.begin(), gridCenter.vecCenterObjects.end(), m_diff.pObject), gridCenter.vecCenterObjects.end() );

		for ( int nModelIndex = 0; nModelIndex < m_diff.pObject->GetModelGroup()->GetSelfPresentationCount(); nModelIndex++ )
		{
			CMapEditObject * pModel = m_diff.pObject->GetModelGroup()->GetSelfPresentation(nModelIndex);

			pModel->UpdateRotationDiscrete();
		}

		//////////////////////////////////////////////////////////////////////////

		for ( int nModelIndex = 0; nModelIndex < m_diff.pObject->GetModelGroup()->GetSelfPresentationCount(); nModelIndex++ )
		{
			CMapEditObject * pModel = m_diff.pObject->GetModelGroup()->GetSelfPresentation(nModelIndex);

			vecHeightVertexIndices.clear();
		}

		// write center image info
		pModel = m_diff.pObject->GetModelGroup()->GetSelfPresentation(0);
		nCenterGridIndex = pModel->GetUserOffsetAdjustedGridIndex(m_diff.pObject->GetCenterGridIndex());
		SGrid & gridCenter2 = pTerrainMesh->GetGrid(nCenterGridIndex);
		gridCenter2.vecCenterObjects.push_back(m_diff.pObject);

	}

	void CCmdRotateObject::_UnExecute()
	{
		
		CTerrainMesh * pTerrainMesh = CMapEditApp::GetInst()->GetEditingMesh();
		vector<DWORD> vecHeightVertexIndices;
		COperator * pOperator = CMapEditApp::GetInst()->GetOperator();

		for ( int nModelIndex = 0; nModelIndex < m_diff.pObject->GetModelGroup()->GetSelfPresentationCount(); nModelIndex++ )
		{
			CMapEditObject * pModel = m_diff.pObject->GetModelGroup()->GetSelfPresentation(nModelIndex);

			vecHeightVertexIndices.clear();
		}

		// erase center image info
		CMapEditObject * pModel = m_diff.pObject->GetModelGroup()->GetSelfPresentation(0);
		int nCenterGridIndex = pModel->GetUserOffsetAdjustedGridIndex(m_diff.pObject->GetCenterGridIndex());
		SGrid & gridCenter = pTerrainMesh->GetGrid(nCenterGridIndex);
		gridCenter.vecCenterObjects.erase(
			remove(gridCenter.vecCenterObjects.begin(), gridCenter.vecCenterObjects.end(), m_diff.pObject), gridCenter.vecCenterObjects.end() );

		//////////////////////////////////////////////////////////////////////////
		for ( int nModelIndex = 0; nModelIndex < m_diff.pObject->GetModelGroup()->GetSelfPresentationCount(); nModelIndex++ )
		{
			CMapEditObject * pModel = m_diff.pObject->GetModelGroup()->GetSelfPresentation(nModelIndex);

			pModel->UpdateRotationDiscrete();
			pModel->UpdateRotationDiscrete();
			pModel->UpdateRotationDiscrete();
		}

		//////////////////////////////////////////////////////////////////////////

		for ( int nModelIndex = 0; nModelIndex < m_diff.pObject->GetModelGroup()->GetSelfPresentationCount(); nModelIndex++ )
		{
			CMapEditObject * pModel = m_diff.pObject->GetModelGroup()->GetSelfPresentation(nModelIndex);

			vecHeightVertexIndices.clear();
		}

		// write center image info
		pModel = m_diff.pObject->GetModelGroup()->GetSelfPresentation(0);
		nCenterGridIndex = pModel->GetUserOffsetAdjustedGridIndex(m_diff.pObject->GetCenterGridIndex());
		SGrid & gridCenter2 = pTerrainMesh->GetGrid(nCenterGridIndex);
		gridCenter2.vecCenterObjects.push_back(m_diff.pObject);
	}
}
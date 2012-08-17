#include "stdafx.h"
#include "CmdMoveObject.h"
#include "TerrainMesh.h"
#include "CMapEditApp.h"
#include "CMapEditObject.h"
#include "Picker.h"
#include "ModelGroup.h"
#include "Operator.h"
#include "Operator.inl"
#include "OperationLog.h"
#include "ToolSetModel.h"
#include "EditStateOpertiaonMgr.h"

namespace sqr
{
	void CCmdMoveObject::_Execute()
	{
		COpearationLog::GetInst()->WriteOperationLog("DO LShift Or C move object");
		

		CTerrainMesh * pTerrainMesh = CMapEditApp::GetInst()->GetEditingMesh();

		COperator * pOperator = CMapEditApp::GetInst()->GetOperator();

		CMapEditObject * pModel = m_diff.pObject->GetModelGroup()->GetSelfPresentation(0);

		pModel->SetGroupOffset(m_diff.vGroupOffset);

		// erase center image info
		int nCenterGridIndex = pModel->GetUserOffsetAdjustedGridIndex(m_diff.nCenterGridIndex);
		SGrid & gridCenter = pTerrainMesh->GetGrid(nCenterGridIndex);
		gridCenter.vecCenterObjects.erase(
			remove(gridCenter.vecCenterObjects.begin(), gridCenter.vecCenterObjects.end(), m_diff.pObject), gridCenter.vecCenterObjects.end() );

		CToolSetModel::GetInst()->AddActiveObject(m_diff.pObject);
		m_diff.pObject->SetCenterGridIndex(INVALID);
		m_diff.pObject->GetModelGroup()->SetEditState(sqr::EES_MOVING);

		// update hot
		CTObject * p = NULL;
		CVector3f v;
		DWORD dwGridIndexHot = CPicker::GetInst()->GetPickPos(&p,&v);
		pOperator->SetGridIndexHot(dwGridIndexHot);
		pOperator->SetHitPoint(*((CVector3f*)&v));
		pOperator->SetObjectHot(p);
	}

	void CCmdMoveObject::_UnExecute()
	{
		// back to stationary state
		COpearationLog::GetInst()->WriteOperationLog("UNDO LShift Or C move object");

		

		CTerrainMesh * pTerrainMesh = CMapEditApp::GetInst()->GetEditingMesh();

		CMapEditObject * pModel = m_diff.pObject->GetModelGroup()->GetSelfPresentation(0);

		pModel->SetGroupOffset(CVector3f(0.0f,0.0f,0.0f));

		// write center image info
		int nCenterGridIndex = pModel->GetUserOffsetAdjustedGridIndex(m_diff.nCenterGridIndex);
		SGrid & gridCenter = pTerrainMesh->GetGrid(nCenterGridIndex);
		gridCenter.vecCenterObjects.push_back(m_diff.pObject);

		// assumption: no pre active object exists
		CToolSetModel::GetInst()->RemoveActiveObject(m_diff.pObject);
		//CEditStateOpertiaonMgr::GetInst()->SetIsCalObjectWorOffset(true);
		m_diff.pObject->SetCenterGridIndex(m_diff.nCenterGridIndex);
		m_diff.pObject->GetModelGroup()->SetEditState(sqr::EES_PLACED);
		//CEditStateOpertiaonMgr::GetInst()->SetIsCalObjectWorOffset(false);
	}
}
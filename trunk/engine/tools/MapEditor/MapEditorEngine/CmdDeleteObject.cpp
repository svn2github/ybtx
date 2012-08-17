#include "stdafx.h"
#include "CmdDeleteObject.h"
#include "TerrainMesh.h"
#include "CMapEditApp.h"
#include "Picker.h"
#include "CMapEditObject.h"
#include "ModelGroup.h"
#include "Operator.h"
#include "Operator.inl"
#include "OffLineBaker/OfflineRender.h"
#include "OperationLog.h"
#include "ToolSetModel.h"
#include "SceneAudioMgr.h"

namespace sqr
{
	void CCmdDeleteObject::_Execute()
	{
		

		CTerrainMesh * pTerrainMesh = CMapEditApp::GetInst()->GetEditingMesh();
		COperator * pOperator = CMapEditApp::GetInst()->GetOperator();

		vector<int>::iterator iterBlockGridIndex = m_diff.vecBlockGridIndices.begin();

		CMapEditObject * pModel = m_diff.pObject->GetModelGroup()->GetSelfPresentation(0);
		string modelname = "";
		modelname = pModel->GetName();

		char szObjectBuf[1024];
		sprintf(szObjectBuf, "DO delete %s", modelname.c_str());
		COpearationLog::GetInst()->WriteOperationLog(szObjectBuf);

		// erase center image info
		int nCenterGridIndex = pModel->GetUserOffsetAdjustedGridIndex(m_diff.nCenterGridIndex);

		if ( m_diff.pObject->GetModelGroup()->m_SL.m_bIAmbScenePointLight || m_diff.pObject->GetModelGroup()->m_SpotLight.m_bIAmbSceneSpotLight )
		{
			if ( pTerrainMesh->m_DynamicLightGrids.find(nCenterGridIndex) != pTerrainMesh->m_DynamicLightGrids.end() )
				pTerrainMesh->m_DynamicLightGrids.erase(pTerrainMesh->m_DynamicLightGrids.find(nCenterGridIndex));
			if ( pTerrainMesh->m_VisibleDynamicLightGrids.find(nCenterGridIndex) != pTerrainMesh->m_VisibleDynamicLightGrids.end() )
				pTerrainMesh->m_VisibleDynamicLightGrids.erase(pTerrainMesh->m_VisibleDynamicLightGrids.find(nCenterGridIndex));
		}

		SGrid & gridCenter = pTerrainMesh->GetGrid(nCenterGridIndex);
		
		gridCenter.vecCenterObjects.erase(
			remove(gridCenter.vecCenterObjects.begin(), gridCenter.vecCenterObjects.end(), m_diff.pObject), gridCenter.vecCenterObjects.end() );

		int nDeleteGridIndex = 0;
		size_t nModelGroupCnt = gridCenter.vecCenterObjects.size();
		for ( size_t m = 0; m < nModelGroupCnt; ++m )
		{
			if( gridCenter.vecCenterObjects[m] == m_diff.pObject )
			{
				nDeleteGridIndex = m;
				m = nModelGroupCnt;
			}
		}

		OfflineRender::GetInst()->UpdateGridLights(nCenterGridIndex);

		//////////////////////////////////////////////////////////////////////////

		pModel = m_diff.pObject->GetModelGroup()->GetSelfPresentation(0);

		//////////////////////////////////////////////////////////////////////////
		m_diff.pObject->GetModelGroup()->SetEditState(sqr::EES_DANGLING);
		m_diff.pObject->SetCenterGridIndex(INVALID);

		///effect
		if ( m_diff.pObject->GetModelGroup()->GetIsEffectBased() && m_diff.pObject->GetModelGroup()->GetIsCameraOriented() == false )
		{
			pOperator->RemoveIndividualEffectObject(m_diff.pObject);
		}

		///sound
		if( m_diff.pObject->GetModelGroup()->m_pSceneAudio )
		{
			CSceneAudioMgr::GetInst()->RemoveIndividualSoundObject(m_diff.pObject);
		}

 		// update hot
 		CTObject * p = NULL;
 		CVector3f v;
 		DWORD dwGridIndexHot = CPicker::GetInst()->GetPickPos(&p,&v);
 		pOperator->SetGridIndexHot(dwGridIndexHot);
 		pOperator->SetHitPoint(*((CVector3f*)&v));
 		pOperator->SetObjectHot(p);
 		m_diff.pObject->GetModelGroup()->SetEditState(sqr::EES_DELETE);

 		///删除模型名记录
 		set<string>::iterator iter = pTerrainMesh->m_SceneModelList.find(modelname);
 		if ( iter != pTerrainMesh->m_SceneModelList.end() )
 			pTerrainMesh->m_SceneModelList.erase(iter);
	}

	void CCmdDeleteObject::_UnExecute()
	{
		// back to stationary state
		

		CTerrainMesh * pTerrainMesh = CMapEditApp::GetInst()->GetEditingMesh();
		COperator * pOperator = CMapEditApp::GetInst()->GetOperator();

		vector<int>::iterator iterBlockGridIndex = m_diff.vecBlockGridIndices.begin();

		CMapEditObject * pModel = m_diff.pObject->GetModelGroup()->GetSelfPresentation(0);

		// write center image info
		int nCenterGridIndex = pModel->GetUserOffsetAdjustedGridIndex(m_diff.nCenterGridIndex);
		SGrid & gridCenter = pTerrainMesh->GetGrid(nCenterGridIndex);

		gridCenter.vecCenterObjects.push_back(m_diff.pObject);

		if ( m_diff.pObject->GetModelGroup()->m_SL.m_bIAmbScenePointLight || m_diff.pObject->GetModelGroup()->m_SpotLight.m_bIAmbSceneSpotLight )
		{
			pTerrainMesh->m_DynamicLightGrids.insert(nCenterGridIndex);
			pTerrainMesh->m_VisibleDynamicLightGrids.insert(nCenterGridIndex);
		}

		OfflineRender::GetInst()->UpdateGridLights(nCenterGridIndex);

		//////////////////////////////////////////////////////////////////////////

		pModel = m_diff.pObject->GetModelGroup()->GetSelfPresentation(0);
		//////////////////////////////////////////////////////////////////////////
		
		///effect
		if ( m_diff.pObject->GetModelGroup()->GetIsEffectBased() && m_diff.pObject->GetModelGroup()->GetIsCameraOriented() == false )
		{
			pOperator->AddIndividualEffectObject(m_diff.pObject);
		}

		///sound
		if( m_diff.pObject->GetModelGroup()->m_pSceneAudio )
		{
			CSceneAudioMgr::GetInst()->AddIndividualSoundObject(m_diff.pObject);
		}

		// assumption: no pre active object exists
		CToolSetModel::GetInst()->RemoveActiveObject(m_diff.pObject);
		m_diff.pObject->SetCenterGridIndex(m_diff.nCenterGridIndex);
		m_diff.pObject->GetModelGroup()->SetEditState(sqr::EES_PLACED);

 		///加入模型名记录
 		string modelname = "";
 		modelname = pModel->GetName();
 		pTerrainMesh->m_SceneModelList.insert(modelname);

		char szObjectBuf[1024];
		sprintf(szObjectBuf, "DO delete %s", modelname.c_str());
		COpearationLog::GetInst()->WriteOperationLog(szObjectBuf);
	}
}
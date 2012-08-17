#include "stdafx.h"
#include "CmdPlaceObject.h"
#include "CMapEditApp.h"
#include "Picker.h"
#include "TerrainMesh.h"
#include "CMapEditObject.h"
#include "ModelGroup.h"
#include "Operator.h"
#include "Operator.inl"
#include "OffLineBaker/OfflineRender.h"
#include "OperationLog.h"
#include "ToolSetModel.h"

namespace sqr
{
	void CCmdPlaceObject::_Execute()
	{
		CTerrainMesh * pTerrainMesh = CMapEditApp::GetInst()->GetEditingMesh();

		CMapEditObject * pModel = m_diff.pObject->GetModelGroup()->GetSelfPresentation(0);

		pModel->SetGroupOffset(CVector3f(0.0f,0.0f,0.0f));

		int nHotGridIndex = pModel->GetUserOffsetAdjustedGridIndex(m_diff.nHotGridIndex);
		if ( m_diff.pObject->GetModelGroup()->m_SL.m_bIAmbScenePointLight || m_diff.pObject->GetModelGroup()->m_SpotLight.m_bIAmbSceneSpotLight )
		{
			pTerrainMesh->m_DynamicLightGrids.insert(nHotGridIndex);
			pTerrainMesh->m_VisibleDynamicLightGrids.insert(nHotGridIndex);
			m_diff.pObject->GetModelGroup()->SetIsLightEffect(true);
		}
		
		SGrid & gridCenter = pTerrainMesh->GetGrid(nHotGridIndex);

		gridCenter.vecCenterObjects.push_back(m_diff.pObject);

		///初始聚光灯的目标点位置，目标点位置的初始值也即为物体当前格子中心位置
		CVector3f targetPos = gridCenter.vCenter;
		size_t size = gridCenter.vecCenterObjects.size();
		for ( size_t j  = 0; j < size; ++j)
		{
			CEditModelGroup *pModelGroup = gridCenter.vecCenterObjects[j]->GetModelGroup();

			if (pModelGroup->m_SpotLight.m_bIAmbSceneSpotLight )
			{
				if ( !pModelGroup->m_bMoveDelete )
					pModelGroup->m_SpotLight.m_TargetPos = targetPos;

				pModelGroup->m_SpotLightPos = targetPos;

			}

		}
		OfflineRender::GetInst()->UpdateGridLights(nHotGridIndex);

		//////////////////////////////////////////////////////////////////////////

		pModel = m_diff.pObject->GetModelGroup()->GetSelfPresentation(0);

		//////////////////////////////////////////////////////////////////////////

		CToolSetModel::GetInst()->RemoveActiveObject(m_diff.pObject);

		// adjust the global allocation accordingly
		m_diff.pObject->SetCenterGridIndex(m_diff.nHotGridIndex);
		m_diff.pObject->GetModelGroup()->SetPreCenterGridIndex(m_diff.nHotGridIndex);
		m_diff.pObject->GetModelGroup()->SetEditState(sqr::EES_PLACED);

		m_diff.pObject->GetModelGroup()->UpdateOverallAmbientRatio();

 		///加入模型名记录
 		string modelname = "";
 		modelname = pModel->GetName();
 		pTerrainMesh->m_SceneModelList.insert(modelname);

		size_t npos = modelname.rfind("\\");
		modelname = modelname.substr(npos+1, modelname.length());

		char szObjectBuf[1024];
		sprintf(szObjectBuf, "DO 放置 %s", modelname.c_str());
		COpearationLog::GetInst()->WriteOperationLog(szObjectBuf);
	}

	void CCmdPlaceObject::_UnExecute()
	{
		CTerrainMesh * pTerrainMesh = CMapEditApp::GetInst()->GetEditingMesh();

		COperator * pOperator = CMapEditApp::GetInst()->GetOperator();

		CMapEditObject * pModel = m_diff.pObject->GetModelGroup()->GetSelfPresentation(0);

		pModel->SetGroupOffset(m_diff.vGroupOffset);

		pModel->SetVisible(false);

		int nHotGridIndex = pModel->GetUserOffsetAdjustedGridIndex(m_diff.nHotGridIndex);
		if ( m_diff.pObject->GetModelGroup()->m_SL.m_bIAmbScenePointLight || m_diff.pObject->GetModelGroup()->m_SpotLight.m_bIAmbSceneSpotLight )
		{
			if ( pTerrainMesh->m_DynamicLightGrids.find(nHotGridIndex) != pTerrainMesh->m_DynamicLightGrids.end() )
				pTerrainMesh->m_DynamicLightGrids.erase(pTerrainMesh->m_DynamicLightGrids.find(nHotGridIndex));
			if ( pTerrainMesh->m_VisibleDynamicLightGrids.find(nHotGridIndex) != pTerrainMesh->m_VisibleDynamicLightGrids.end() )
				pTerrainMesh->m_VisibleDynamicLightGrids.erase(pTerrainMesh->m_VisibleDynamicLightGrids.find(nHotGridIndex));

			m_diff.pObject->GetModelGroup()->SetIsLightEffect(false);
		}

		// erase center image info
		SGrid & gridCenter = pTerrainMesh->GetGrid(nHotGridIndex);

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

		gridCenter.vecCenterObjects.erase(
			remove(gridCenter.vecCenterObjects.begin(), gridCenter.vecCenterObjects.end(), m_diff.pObject), gridCenter.vecCenterObjects.end() );

		OfflineRender::GetInst()->UpdateGridLights(nHotGridIndex);

		//////////////////////////////////////////////////////////////////////////

		pModel = m_diff.pObject->GetModelGroup()->GetSelfPresentation(0);

		//////////////////////////////////////////////////////////////////////////
		CToolSetModel::GetInst()->AddActiveObject(m_diff.pObject);

		m_diff.pObject->GetModelGroup()->SetEditState(sqr::EES_MOVING);

		m_diff.pObject->SetCenterGridIndex(INVALID);

		//////////////////////////////////////////////////////////////////////////

		// update hot
		CTObject * p = NULL;
		CVector3f v;
		nHotGridIndex = CPicker::GetInst()->GetPickPos(&p,&v);
		pOperator->SetGridIndexHot(nHotGridIndex);
		pOperator->SetHitPoint(*((CVector3f*)&v));
		pOperator->SetObjectHot(p);

		///删除模型名记录
		string modelname = "";
		modelname = pModel->GetName();

 		set<string>::iterator iter = pTerrainMesh->m_SceneModelList.find(modelname);
 		if ( iter != pTerrainMesh->m_SceneModelList.end() )
 			pTerrainMesh->m_SceneModelList.erase(iter);

		size_t npos = modelname.rfind("\\");
		modelname = modelname.substr(npos+1, modelname.length());
		char szObjectBuf[1024];
		sprintf(szObjectBuf, "UNDO 放置 %s", modelname.c_str());
		COpearationLog::GetInst()->WriteOperationLog(szObjectBuf);
	}
}
#include "stdafx.h"
#include "CMapEditEffectObj.h"
#include "Engine.h"
#include "CMapEditCamera.inl"
#include "CCameraManager.h"
#include "TerrainMesh.h"
#include "CMapEditApp.h"
#include "WireBox.h"
#include "Picker.h"
#include "Operator.h"
#include "Operator.inl"
#include "ModelGroup.h"
#include "TerrainAdhereRenderer.h"
#include "CEditModel.h"
#include "SceneEnvMgr.h"
#include "SceneEnvMgr.inl"
#include "CRenderDebugFun.h"
#include "CWindowTarget.h"
#include "CRenderGroup.h"

const float MatRGBWeight = 128.0f / 255.0f;

namespace sqr
{
	CMapEditEffectObj::CMapEditEffectObj() 
	{
		m_bIsPointLight = m_bIsSpotLight = false;
	}

	CMapEditEffectObj::~CMapEditEffectObj()
	{
		this->Release();
	}

	void CMapEditEffectObj::Render()
	{
		DWORD dwCurrentTime = (uint32)GetProcessTime();
		DWORD pretime = ::timeGetTime();

		static DWORD dwCurrentTimeForBestMinimap = dwCurrentTime;

		dwCurrentTimeForBestMinimap = dwCurrentTime;

		CTerrainMesh *pTerrain  = CMapEditApp::GetInst()->GetEditingMesh();
		COperator	 *pOperator = CMapEditApp::GetInst()->GetOperator();

		if ( EES_MOVING == sModelCommonPro.eEditState )
		{
			int nGridIndexBlockCenter;

			if ( sModelCommonPro.bCameraOriented )
			{
				CVector3f v;

				if ( !CPicker::GetInst()->GetIntersected(v,&nGridIndexBlockCenter) )
				{
					nGridIndexBlockCenter = pOperator->GetGridIndexHot();
				}
			}
			else
			{
				nGridIndexBlockCenter = pOperator->GetGridIndexHot();
			}

			nGridIndexBlockCenter += ( sModelCommonPro.nXGroupOffset + sModelCommonPro.nZGroupOffset*pTerrain->GetWidth() );

			if ( !pTerrain->IsValid(nGridIndexBlockCenter) )
			{
				return;
			}

			SGrid &gridBlockCenter = pTerrain->GetGrid(nGridIndexBlockCenter);

			sModelCommonPro.vBlockCenter = gridBlockCenter.vCenter;

			int nImageCenterGridIndex = this->GetUserOffsetAdjustedGridIndex(gridBlockCenter.dwGridIndex);
			SGrid &gridImageCenter = pTerrain->GetGrid(nImageCenterGridIndex);
			sModelCommonPro.vImageCenter = gridImageCenter.vCenter;

			sModelCommonPro.matWorldOff.SetTranslate(sModelCommonPro.vBlockCenter.x, sModelCommonPro.vImageCenter.y, sModelCommonPro.vBlockCenter.z);

			sModelCommonPro.mat = sModelCommonPro.matSelfOff*sModelCommonPro.matUserOffset*sModelCommonPro.matWorldOff;

			if( m_pWireBox )
				m_pWireBox->SetAbsolute(sModelCommonPro.WireBoxMat);
		}

		CColor color;
		color.A = 255;

		///如果是场景灯的话，呈显黄色
		if( sModelCommonPro.bEffectLight )
		{
			color.R = 255;
			color.G = 255;
			color.B = 0;
		}
		else
		{
			color.R = 125;
			color.G = 125;
			color.B = 125;
		}

		CEditModel::SetAmbient(color);
		CEditModel::SetDiffuse(color);

		CEditModel::SetRenderStyle(ROS_LIGHTMAT1);

		
		//CEditModel::SetVisible(CMainWindowTarget::GetInst()->GetGroup()->GetCamera() != 0);

		SetPosition(sModelCommonPro.mat.GetLoc());
		if( !CSceneEnvMgr::GetInst()->GetIsGenerateMinMap() )
			CEditModel::OnPreRender(dwCurrentTime,CMainWindowTarget::GetInst()->GetGroup()->GetCamera());
		else
			CEditModel::OnPreRender(dwCurrentTime,NULL);
		//CEditModel::SetFixMatrix(sModelCommonPro.mat);
		
	
		CEditModel::Render(/*dwCurrentTime, &rs*/);

		if ( !CCameraManager::GetInst()->GetMapEditCamera()->GetIsOrthogonal() &&
			this->GetLocked() || this->GetSelected() && (!sModelCommonPro.bEffectBase) )
		{
			if( m_pWireBox )
			{
				m_pWireBox->SetAbsolute(sModelCommonPro.WireBoxMat);
				CTerrainAdhereRenderer::GetInst()->AddRenderWireBox(m_pWireBox);
			}
			//RenderBigBoxDebug(this);
		}

		DWORD postime = ::timeGetTime();
		DWORD diff	  = postime - pretime;

		//DebugOut("%lx, dif %u, pre %u, pos %u, %s\n", this, diff, pretime, postime, m_strSceneCueName.c_str());
	}
}
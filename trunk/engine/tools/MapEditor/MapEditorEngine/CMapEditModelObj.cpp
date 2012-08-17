#include "stdafx.h"
#include "CMapEditModelObj.h"
#include "Engine.h"
#include "CMapEditCamera.inl"
#include "CCameraManager.h"
#include "TerrainMesh.h"
#include "CMapEditApp.h"
#include "Picker.h"
#include "Operator.h"
#include "Operator.inl"
#include "ModelGroup.h"
#include "TerrainAdhereRenderer.h"
#include "CEditModel.h"
#include "SceneEnvMgr.h"
#include "SceneEnvMgr.inl"
#include "TraceHelper.h"
#include "CRenderDebugFun.h"
#include "WireBox.h"
#include "CWindowTarget.h"
#include "CRenderGroup.h"

const float MatRGBWeight = 128.0f / 255.0f;

namespace sqr
{
	CMapEditModelObj::CMapEditModelObj()
	{
		sModelSavePro.nMirrorType = 0;

		sModelSavePro.bTranslucent = false;
		sModelSavePro.bTransState = 0;
		sModelSavePro.bPlayAnimate = false;

		sModelCommonPro.bHideModelVertexColor = false;
		sModelCommonPro.bResGroupMark = 0;
	}

	CMapEditModelObj::~CMapEditModelObj()
	{
		this->Release();
	}

	void CMapEditModelObj::OnPreRender( uint32 uCurTime,CCamera* pCamera )
	{
		DWORD dwCurrentTime = (uint32)GetProcessTime();

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

			switch ( sModelCommonPro.eCenterType )
			{
			case ECT_CENTER:
				break;

			case ECT_CORNER:
				sModelCommonPro.vBlockCenter.x -= GRID_SPACE/2.0f;
				sModelCommonPro.vBlockCenter.z -= GRID_SPACE/2.0f;
				break;

			case ECT_XMIDWAY:
				sModelCommonPro.vBlockCenter.z -= GRID_SPACE/2.0f;
				break;

			case ECT_ZMIDWAY:
				sModelCommonPro.vBlockCenter.x -= GRID_SPACE/2.0f;
				break;
			}

			int nImageCenterGridIndex = this->GetUserOffsetAdjustedGridIndex(gridBlockCenter.dwGridIndex);
			SGrid &gridImageCenter = pTerrain->GetGrid(nImageCenterGridIndex);
			sModelCommonPro.vImageCenter = gridImageCenter.vCenter;

			sModelCommonPro.matWorldOff.SetTranslate(sModelCommonPro.vBlockCenter.x, sModelCommonPro.vImageCenter.y, sModelCommonPro.vBlockCenter.z);

			sModelCommonPro.mat = sModelCommonPro.matSelfOff*sModelCommonPro.matUserOffset*sModelCommonPro.matWorldOff;

			SetPosition(sModelCommonPro.mat.GetLoc());
			if( m_pWireBox )
				m_pWireBox->SetAbsolute(sModelCommonPro.matPieceOff*sModelCommonPro.matUserOffset*sModelCommonPro.matRotY*sModelCommonPro.matScale*sModelCommonPro.matWorldOff);
		}

		//CEditModel::SetVisible(true);

		CColor color;
		color.A = 255;

		if ( sModelCommonPro.bHideModelVertexColor )
		{
			color.R = 0x80;
			color.G = 0x80;
			color.B = 0x80;
		}
		else
		{
			color.R = uint8(sModelCommonPro.fRedOverallAmbientRatio * 255.0f);
			color.G = uint8(sModelCommonPro.fGreenOverallAmbientRatio * 255.0f);
			color.B = uint8(sModelCommonPro.fBlueOverallAmbientRatio * 255.0f);
		}

		if ( sModelCommonPro.bResGroupMark == 1/*RSG_NPC==1*/ )
		{
			CEditModel::SetRenderStyle(ROS_LIGHTMAT2);
		}
		else
		{
			if ( sModelSavePro.bShadow )
			{
				if ( sModelSavePro.bReceiveShadow )
				{
					CEditModel::SetRenderStyle(ROS_SHADOWRECEIVE_LIGHT1_SHADOW);
				}
				else
				{
					CEditModel::SetRenderStyle(ROS_SHADOW_LIGHT1);
				}
			}
			else
			{
				if ( sModelSavePro.bReceiveShadow )
				{
					CEditModel::SetRenderStyle(ROS_SHADOWRECEIVE_LIGHT1);
				}
				else
				{
					CEditModel::SetRenderStyle(ROS_LIGHTMAT1);
				}
			}
		}
		
 		CEditModel::SetAmbient(color);
 		CEditModel::SetDiffuse(color);
		
		if( !CSceneEnvMgr::GetInst()->GetIsGenerateMinMap() )
			CEditModel::OnPreRender(dwCurrentTime,pCamera);
		else
		{
			CEditModel::OnPreRender(dwCurrentTime,NULL);
			string str = sModelCommonPro.strFileName;
			//DebugOut("TopPic grid = %d, %s\n", sModelSavePro.nCenterGridIndex, str.c_str());
		}

		//CEditModel::Render();

		if ( !CCameraManager::GetInst()->GetMapEditCamera()->GetIsOrthogonal() &&
			this->GetLocked() || this->GetSelected() && (!sModelCommonPro.bEffectBase) )
		{
			if( m_pWireBox )
			{
				m_pWireBox->SetAbsolute(sModelCommonPro.WireBoxMat);
				CTerrainAdhereRenderer::GetInst()->AddRenderWireBox(m_pWireBox);
			}
		//	RenderBigBoxDebug(this);
		}
	}

	void CMapEditModelObj::UseNextRenderStyle()
	{
		for ( GMap< GString, CRenderPiece >::iterator it = CEditModel::GetRenderPieces().begin(); it != CEditModel::GetRenderPieces().end(); it++ )
		{
			uint32 uMaxRSNum = 0;

			CRenderPiece & RenderPiece = it->second;
			GString strPieceName = it->first;

			for ( size_t i = 0; i < RenderPiece.m_Pieces.size(); ++i )
			{
				::CRenderPiece::PieceData * pPieceData = &RenderPiece.m_Pieces[i];
				CPiece * pPiece = pPieceData->m_pPiece;
				uMaxRSNum = max(uMaxRSNum, pPiece->GetRSNum());
			}

			RenderPiece.m_RenderStyle = (RenderPiece.m_RenderStyle+1)%uMaxRSNum;

			CEditModel::SetPieceRSIndex(strPieceName.c_str(), RenderPiece.m_RenderStyle);
		}
	}
}
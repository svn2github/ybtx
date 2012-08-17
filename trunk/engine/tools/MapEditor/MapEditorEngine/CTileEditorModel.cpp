//////////////////////////////////////////////////////////////////////////
//
//	Copyright (C) 2003-2004
//	All Rights Reserved
//  In memory of bluely
//
//////////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "CTileEditorModel.h"
#include "Render.h"
#include "CMapEditCamera.inl"
#include "TerrainMesh.h"
#include "CMapEditApp.h"
#include "CGraphic.h"
#include "CPieceGroup.h"
#include "CEditModel.h"
#include "IEffect.h"
#include "CCameraManager.h"
#include "CSkeletalsMgr.h"
#include "CRenderGroup.h"
#include "CEditContext.h"
namespace sqr
{
	CTileEditorModel::CTileEditorModel()
	{
		m_pModel = NULL;
		m_eCenterType = ECT_NONE;
		m_bSubUnit = false;

		m_pEffect = NULL;
		m_pGroup = NULL;
	}

	CTileEditorModel::~CTileEditorModel()
	{
	}

	void CTileEditorModel::SetCenterTypeOrigin(ECenter_Type eCenterType)
	{
		m_eCenterType = eCenterType;
	}

	void CTileEditorModel::Create(string strFileName, bool bSubUnit, float fLift, float fShiftX, float fShiftZ, int &BlockInfoNum, int &LogicHeightInfoNum)
	{
		m_strFileName = strFileName;
		m_bSubUnit = bSubUnit;

		CGraphic * pCoreGraphic = CGraphic::GetInst();
		//m_pModel = new CEditModel;
		/*CSceneManager::GetInst()->AddRenderObject(m_pModel);*/
		m_pModel = CMapEditApp::GetInst()->GetRenderScene()->CreateEditModel(NULL);
		m_pModel->AddRef();

		CPieceGroup * pPieceGroup = NULL;

		SQR_TRY
		{
			if ( pCoreGraphic->CreatePieceGroup(strFileName.c_str(), &pPieceGroup) == INVALID )
			{
				string log = strFileName;
				log = log + "模型文件没有找到";

				MessageBox(NULL, log.c_str(), "错误", MB_OK);

				return;
			}
		}
		SQR_CATCH(exp)
		{
			MessageBox(NULL, exp.ErrorMsg(), "错误", MB_OK);
		}
		SQR_TRY_END;

		if (pPieceGroup)
		{
			for ( size_t i = 0; pPieceGroup && i < pPieceGroup->GetPieceClassNum() ; i++)
			{
				m_pModel->AddMeshPiece(pPieceGroup,pPieceGroup->GetPieceClassName(i),0);
			}
			pPieceGroup->Release();
		}
	}

	void CTileEditorModel::Render()
	{
		DWORD dwCurrentTime = ::timeGetTime();

		CMatrix mat;
		mat.Identity();
		switch ( m_eCenterType )
		{
			case ECT_CENTER:
				break;

			case ECT_CORNER:
				mat._41 -= GRID_SPACE * 0.5f;
				mat._42 -= GRID_SPACE * 0.5f;
				break;

			case ECT_XMIDWAY:
				mat._42 -= GRID_SPACE * 0.5f;
				break;

			case ECT_ZMIDWAY:
				mat._41 -= GRID_SPACE * 0.5f;
				break;
		}

		CColor color;
		color.A = 255;
		color.R = 0x80;
		color.G = 0x80;
		color.B = 0x80;

		m_pModel->SetAmbient(color);
		m_pModel->SetDiffuse(color);

		m_pModel->SetPosition(mat.GetLoc());
		m_pModel->OnPreRender(dwCurrentTime,CGraphic::GetInst()->GetActiveRenderTarget()->GetGroup()->GetCamera());
		m_pModel->Render();
	}

	void CTileEditorModel::UnRender()
	{
		m_pModel->SetVisible(false);
	}

	bool CTileEditorModel::SetAnimationByAgpFileName(string strAgpFileName)
	{
		if( strAgpFileName.empty() )
			return false;

		CSkeletalFrame* pSke;
		CAnimationGroup* pAnimationGroup;
		string sAgpFile = strAgpFileName.substr(0,strAgpFileName.size()-4)+".agp";
		SQR_TRY
		{
			if ( CGraphic::GetInst()->CreateAnimationGroup(sAgpFile.c_str(),&pAnimationGroup)==INVALID||CSkeletalsMgr::GetInst()->GetSkeInPool(pAnimationGroup->GetSkeletalFrameName(),&pSke) == INVALID  ) //=== (void**)&pAnimationGroup
			{
				string log = strAgpFileName;
				log = log + "骨架或者动画列表文件没有找到";

				MessageBox(NULL, log.c_str(), "错误", MB_OK);
				return false;
			}
			else
			{
				m_pModel->SetSkeletalFrame(pSke);
				m_pModel->SetAnimateGroup(pAnimationGroup);
				string strAniName = pAnimationGroup->GetAnimationName(0);
				m_pModel->SetNextAnimation(strAniName.c_str(),FramePair(0,-1),true, 1);

				pAnimationGroup->Release();
			}
		}
		SQR_CATCH(exp)
		{
			MessageBox(NULL, exp.ErrorMsg(), "错误", MB_OK);
			return false;
		}
		SQR_TRY_END;

		return true;
	}

	void CTileEditorModel::LinkEffect(IEffect * pEffect, IEffectGroup * pGroup)
	{
		m_pGroup = pGroup;
		m_pEffect = pEffect;
		m_pModel->AddEffectPtr(pEffect,CEffectPlayer::PLAY_LOOP);
	}

	void CTileEditorModel::UnlinkEffect()
	{
		m_pModel->ClearAllEfx();
	}
}

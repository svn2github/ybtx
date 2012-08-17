#include "stdafx.h"
#include "GTRenderMgr.h"
#include "CAppConfigClient.h"
#include "CDataCache.h"
#include "CGraphic.h"
#include "CCamera.h"
#include "BaseHelper.h"
#include "CEditorConfig.h"
#include "ScriptMgr.h"
#include "CPathMgr.h"
#include "CLangCfg.h"
#include "CFontMgr.h"
#include "CodeCvs.h"
#include "CEditModel.h"


CGTRenderMgr::~CGTRenderMgr(void)
{
	DestroyObj();
	SafeRelease(m_pFont);
	if ( CLangCfg::HasInst() )
		CLangCfg::Destroy();
}

void CGTRenderMgr::DestroyObj(void)
{
	if(m_pRenderObj)
	{
		m_MiniScene.DestroyObject(m_pRenderObj);
		m_MiniScene.DestroyObject(m_pDummyRenderObj);
		m_MiniScene.DestroyObject(m_pReferenceRenderObj);

		m_pRenderObj = NULL;
		m_pDummyRenderObj = NULL;
		m_pReferenceRenderObj = NULL;
	}
}


CRenderObject*  CGTRenderMgr::CreateCharacter(string name, EModelType type)
{
	if(m_pRenderObj)
	{
		m_MiniScene.DestroyObject(m_pRenderObj);
		//pRenderScene->DestroyRenderObject(pDummyRenderObj);
		//pRenderScene->DestroyRenderObject(pReferenceRenderObj);
	}
	
	m_pRenderObj = m_MiniScene.CreateObject();
	gs_pCahce->DisPlayCharacter(name,m_pRenderObj,type);
	m_ModelFileName = gs_pCahce->GetModelFileName();
	
	if(m_pDummyRenderObj == NULL)
	{
		m_pDummyRenderObj = m_MiniScene.CreateObject();
		m_pDummyRenderObj->AddEffect( "fx/setting/mubiaoxuanze.efx",  "选中目标/create", CEffectPlayer::PLAY_LOOP, NULL );
		//m_pDummyRenderObj->AddPiece( RFT_ARE, "fx/setting/mubiaoxuanze.efx", "", "选中目标/create");
	}

	if(m_pReferenceRenderObj == NULL)
	{
		m_pReferenceRenderObj = m_MiniScene.CreateObject();
		gs_pCahce->DisPlayCharacter("佣兵工会登记人",m_pReferenceRenderObj,eMT_Character);
		m_pReferenceRenderObj->SetPosition(CVector3f(64*2.0f, 0.0f, 0.0f)); //差2米
		m_pReferenceRenderObj->AddEffect("fx/setting/mubiaoxuanze.efx", "选中目标/create", CEffectPlayer::PLAY_LOOP,NULL);
	}
	
	//pRenderObj->ShowPieceClass("body",true);
	//pRenderObj->ShowPieceClassBuffer("body",true);
	//pRenderObj->ShowPieceBuffer("body01",false);
	//pRenderObj->ShowPiece("body01",false);
	//pRenderObj->ShowPieceBuffer("body01",false);//隐藏所有的piece

	if (!m_bDisplayAperture)
	{
		m_pReferenceRenderObj->SetAlphaValue(0);
		m_pDummyRenderObj->SetAlphaValue(0);
	}

	return m_pRenderObj;
}

void CGTRenderMgr::DisplayCharacterAperture(float fScale)
{
	if(!m_pRenderObj)
		return;
	//pRenderObj->AddNoneActEffect("fx/setting/mubiaoxuanze.efx", "选中目标/create",-1,0,0);
		m_pDummyRenderObj->SetScale(fScale);
}

void CGTRenderMgr::HideApertureAndRefer()
{
	m_bDisplayAperture = !m_bDisplayAperture;
	if(m_pReferenceRenderObj)
	{
		int curAlpha = m_pReferenceRenderObj->GetAlphaValue();
		curAlpha = !m_bDisplayAperture ? 0 : 255;
		m_pReferenceRenderObj->SetAlphaValue(curAlpha);
	}
	if (m_pDummyRenderObj)
	{
		int curAlpha = m_pDummyRenderObj->GetAlphaValue();
		curAlpha = !m_bDisplayAperture ? 0 : 255;
		m_pDummyRenderObj->SetAlphaValue(curAlpha);
	}
}

void CGTRenderMgr::OnRender()
{
	if ( GetTickCount() - tStart >= 1000 )
	{
		tStart = GetTickCount();
		nFPS = tFrameCnt;
		tFrameCnt = 0;
	}
	++tFrameCnt;
	CString strBuf;
	strBuf.Format("Current Fps：%d", nFPS);

	sqr::RenderTextParam param(12);
	param.SetText(strBuf);
	param.SetRect(CFRect( 0, 10, 800, 600 ));
	param.SetColor(0xffff0000);
	if(m_pFont)
		m_pFont->DrawText(param);
	else
		CGraphic::GetInst()->CreateFont(CFontMgr::DISTANCE_FONT_ID, false, 0.0f, &m_pFont );

	m_MiniScene.Render();
}

CGTRenderMgr::CGTRenderMgr( CRenderGroup* pRenderGroup )
: m_MiniScene(pRenderGroup)
,m_bDisplayAperture(false)
{
	m_pRenderObj = NULL;
	m_pDummyRenderObj = NULL;
	m_pReferenceRenderObj = NULL;
	m_pFont = NULL;
	m_IsShow = false;

	m_CModelFaceNum = 0;
	m_CModelVertexNum = 0;

	tStart = 0;
	nFPS = 0;
	tFrameCnt = 0;

	CPkgFile::SearchPathList pathList = CPkgFile::GetSearchList(L"");
	wstring resPath = pathList.front() + wstring(L"/../gui/tgt/");

	//CLangCfg::Create();
	//CLangCfg::GetInst()->LoadConfig("./LangConfig.xml");
	//CFontMgr::GetInst()->SetLangIndex(0);
	//CFontMgr::GetInst()->SetLangFolder(CLangCfg::GetInst()->GetLanFolder(0));
	CGraphic::GetInst()->CreateFont( CFontMgr::DISTANCE_FONT_ID, false, 0.0f, &m_pFont );

	m_pCamera = pRenderGroup->GetCamera();
	m_pCamera->setNearDist(50.0f);
	m_pCamera->setFarDist(3200.0f);

	m_MiniScene.RegisterCameraCtrl(&m_TPSCtrller);
	m_TPSCtrller.SetXRotateDegree(CMath::qtr_pi);
	m_TPSCtrller.SetYRotateDegree(CMath::qtr_pi + CMath::half_pi);
	m_TPSCtrller.SetTargetDist(640.0f);
	m_TPSCtrller.SetResetCtrlTime(0);
	m_IsShow = true;
}


/*
local config = Npc_Client[NpcName].BoyAniFile
if  config ~= nil then
local fileName = string.gsub(config, "character/ani/npc/", "fx/setting/npcfx/")
fileName = string.gsub(fileName, ".ara", ".are")
print(fileName)
Character:SetAttackFxFile(fileName)
Character:SetMoveFxFile(fileName)
Character:SetActionFxFile(fileName)
		  end
g_MainPlayer:NpcIntoVision(Character)
			 end
*/
void CGTRenderMgr::DoAnimate(std::string name)
{
	if (!m_pRenderObj)
		return;
	SQR_TRY{
		
		string NpcFxFileName = GetFxFileName();
		m_pRenderObj->ClearAllEfx();
		m_LastNpcFxName = name + "/create";
		if (m_pRenderObj->CheckEffect(NpcFxFileName.c_str(),m_LastNpcFxName.c_str() )!=eECR_NoExist)
		{
			m_pRenderObj->AddEffect( NpcFxFileName.c_str(), m_LastNpcFxName.c_str(), CEffectPlayer::PLAY_LOOP,NULL );
		}		
		m_pRenderObj->DoAni(name.c_str(),true);
		
	}
	SQR_CATCH(exp)
	{
		cout<<exp.ErrorMsg()<<endl;
	}
	SQR_TRY_END;
		
}

string CGTRenderMgr::GetFxFileName()
{
	string modelFileName = gs_pCahce->GetModelFileName();

	size_t NamePos = modelFileName.rfind("//");
	string NpcName = modelFileName.erase(0,NamePos+1);

	int TypeNamePos = NpcName.rfind('.');
	NpcName = NpcName.erase(TypeNamePos,4);

	string NpcFxFileName = "fx/setting/npcfx/"+NpcName+".efx";
	return NpcFxFileName;
}

CRenderObject* CGTRenderMgr::GetDummyCharacter()
{
		return m_pDummyRenderObj;
}

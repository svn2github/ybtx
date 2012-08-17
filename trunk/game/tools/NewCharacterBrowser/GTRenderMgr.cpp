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
#include "CEditModel.h"
#include "CRenderObject.h"
#include "CRenderFont.h"
#include "StringHelper.h"
#include "CSkeletalsMgr.h"
#include "CTPSCameraCtrller.h"
#include "CCameraManager.h"
#include "CEditContext.h"

CGTRenderMgr::CGTRenderMgr()
: m_bDisplayAperture(false)
{
	m_pRenderSceneContext	= NULL;
	m_pRenderObj			= NULL;
	m_pDummyRenderObj		= NULL;
	m_pReferenceRenderObj	= NULL;
	m_pFont					= NULL;
	m_IsShow				= false;

	m_CModelFaceNum			= 0;
	m_CModelVertexNum		= 0;

	tStart					= 0;
	nFPS					= 0;
	tFrameCnt				= 0;

	CPkgFile::SearchPathList pathList = CPkgFile::GetSearchList(L"");
	wstring resPath = pathList.front() + wstring(L"/../gui/tgt/");

	CGraphic::GetInst()->CreateFont( CFontMgr::DISTANCE_FONT_ID, false, 0.0f, &m_pFont );

	if( !CCameraManager::HasInst() )
		CCameraManager::Create();


	m_IsShow = true;

	m_pMainPlayer = NULL;
}

CGTRenderMgr::~CGTRenderMgr(void)
{
	DestroyObj();
	DestroyMainPlayer();

	SafeRelease(m_pFont);
	if ( CLangCfg::HasInst() )
		CLangCfg::Destroy();
}



void CGTRenderMgr::SetRenderSceneContext( CEditContext* context )
{
	m_pRenderSceneContext = context;
}

void CGTRenderMgr::DestroyMainPlayer(void)
{
	if(m_pMainPlayer)
	{
		m_pRenderSceneContext->DestroyRenderObject(m_pMainPlayer);

		m_pMainPlayer = NULL;
	}
}

void CGTRenderMgr::DestroyObj(void)
{
	if(m_pRenderObj)
	{
		m_pRenderSceneContext->DestroyRenderObject(m_pRenderObj);
		m_pRenderSceneContext->DestroyRenderObject(m_pDummyRenderObj);
		m_pRenderSceneContext->DestroyRenderObject(m_pReferenceRenderObj);

		m_pRenderObj = NULL;
		m_pDummyRenderObj = NULL;
		m_pReferenceRenderObj = NULL;
	}
}


CRenderObject*  CGTRenderMgr::CreateCharacter(string name, EModelType type)
{
	if(m_pRenderObj)
	{
		m_pRenderSceneContext->DestroyRenderObject(m_pRenderObj);
		//pRenderScene->DestroyRenderObject(pDummyRenderObj);
		//pRenderScene->DestroyRenderObject(pReferenceRenderObj);
	}

	m_pRenderObj = m_pRenderSceneContext->CreateEditModel(NULL);
	gs_pCahce->DisPlayCharacter(name,m_pRenderObj,type);
	m_ModelFileName = gs_pCahce->GetModelFileName();

	if(m_pDummyRenderObj == NULL)
	{
		m_pDummyRenderObj = m_pRenderSceneContext->CreateEditModel(NULL);
		m_pDummyRenderObj->AddEffect( "fx/setting/mubiaoxuanze.efx",  "选中目标/create", CEffectPlayer::PLAY_LOOP, NULL );
		//m_pDummyRenderObj->AddPiece( RFT_ARE, "fx/setting/mubiaoxuanze.efx", "", "选中目标/create");
	}

	if(m_pReferenceRenderObj == NULL)
	{
		m_pReferenceRenderObj = m_pRenderSceneContext->CreateEditModel(NULL);
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
 	string strBuf = format("Current Fps：%d", nFPS);
 
 	sqr::RenderTextParam param(12);
 	param.SetText(strBuf);
 	param.SetRect(CFRect( 0, 10, 800, 600 ));
 	param.SetColor(0xffff0000);

	//m_MiniScene.Render();
}

void CGTRenderMgr::DoAnimate(std::string name)
{
	if (!m_pRenderObj)
		return;
	
	SQR_TRY
	{
		string NpcFxFileName = GetFxFileName();
		m_LastNpcFxName = name + "/create";
		if (m_pRenderObj->CheckEffect(NpcFxFileName.c_str(),m_LastNpcFxName.c_str() )!=eECR_NoExist)
		{
			m_pRenderObj->AddPiece( RFT_ARE, NpcFxFileName.c_str(), "", m_LastNpcFxName.c_str() );
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

void CGTRenderMgr::CreateMainPlayer()
{
	if(m_pMainPlayer == NULL)
		m_pMainPlayer = m_pRenderSceneContext->CreateEditModel(NULL);
}

void CGTRenderMgr::SetMainPlayerIsShow( const bool show )
{
	if( m_pMainPlayer )
		m_pMainPlayer->ShowPiece("", show);
}

void CGTRenderMgr::SetMainPlayerAni(const string& aniname, const float fSpeed)
{
	if( m_pMainPlayer )
		m_pMainPlayer->DoAni(aniname.c_str(), -1, true, 0, 0, -1, fSpeed);
}

void CGTRenderMgr::MainPlayerAddPiece(const string& modelname, const string& piecename, const string& rsname )
{
	if( m_pMainPlayer )
		m_pMainPlayer->AddPiece(RFT_ARP, modelname.c_str(), piecename.c_str(), rsname.c_str());
}

void CGTRenderMgr::HideMainPlayerPiece(const vector<string> hidePieceNames)
{
	size_t size = hidePieceNames.size();
	for( size_t i = 0; i < size; ++i )
		m_pMainPlayer->ShowPiece((hidePieceNames[i]).c_str(),false);
}

void CGTRenderMgr::UpdatePlayerAnimationGroup(const string& name)
{
	if( m_pMainPlayer )
	{
		m_pMainPlayer->Delframework();
		m_pMainPlayer->AddPiece( RFT_ARA, name.c_str(), NULL, NULL );
	}
}

bool CGTRenderMgr::GetAnimationGroupAniNames( const string& agpName, vector<string>& aninames )
{
	bool isAsyc = CPkgFile::IsEnableAsyncRead();
	if( isAsyc )
		CGraphic::GetInst()->SetAsynSwitch(false);

	CSkeletalFrame* pSke = NULL;
	CAnimationGroup* pAniGroup = NULL;

	if( 0==CGraphic::GetInst()->CreateAnimationGroup(agpName.c_str(),&pAniGroup)&&0 == CSkeletalsMgr::GetInst()->GetSkeInPool(pAniGroup->GetSkeletalFrameName(),&pSke) )
	{
		int nAniNum = pAniGroup->GetAnimationNum();
		for(int i =0; i < nAniNum; ++i )
		{
			const TCHAR* pAniName = pAniGroup->GetAnimationName(i);
			aninames.push_back(pAniName);
		}
		pSke = NULL;
		pAniGroup->Release();

		return true;
	}

	return false;
}

void CGTRenderMgr::ReleaseMainPlayer()
{
	if( m_pMainPlayer )
		m_pMainPlayer->Release();
}

void CGTRenderMgr::GetPlayerEffectNames( const string strEffectPath, vector<string>& effectNames )
{
	IEffectGroup* pEffectGroup = NULL;
	IEffectManager::GetInst()->CreateEffectGroup(strEffectPath.c_str(),&pEffectGroup);
	if( pEffectGroup != NULL )
	{
		for( uint32 i = 0; i< pEffectGroup->GetEffectNum(); ++i )
		{
			string temp = pEffectGroup->GetEffectName(i);
			//if( temp.find(strPieceName + "\\") != size_t(-1) )
				effectNames.push_back(temp);
		}
		pEffectGroup->Release();
	}
}

void CGTRenderMgr::AttachPlayerEffect( const string& strEffectPath, const string& strEffectName )
{
	//if( m_pMainPlayer )
	//	m_pMainPlayer->AddNoneActEffect(strEffectPath.c_str(), strEffectName.c_str(), -1, NULL, NULL);
}

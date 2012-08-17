#include "stdafx.h"
#include "CCoreSceneMgrClient.h"
#include "CCoreSceneClient.h"
#include "TCoreSceneMgr.inl"
#include "CMetaSceneClient.h"
#include "CMetaSceneMgrClient.h"
#include "TWhole.inl"
#include "IAppClientHandler.h"
#include "PrtlGas2GacObjectDef.h"
#include "PrtlGac2GasGenericDef.h"
#include "BaseHelper.h"
#include "CAppClient.h"
#include "CRenderSystemClient.h"
#include "CConnClient.h"
#include "CoreCommon.h"


template class TCoreSceneMgr<CTraitsClient>;
template class TWhole<CCoreSceneMgrClient,CCoreSceneClient>;

CCoreSceneMgrClient::CCoreSceneMgrClient()
{
}

CCoreSceneMgrClient::~CCoreSceneMgrClient()
{
}



bool CCoreSceneMgrClient::CreateMainScene(const char* szSceneName)
{
	CCoreSceneClient* pMainScene=CCoreSceneClient::MainScene();

	if( pMainScene )
	{
		string sCurSceneName = pMainScene->GetMetaScene()->GetSceneName();
		if( sCurSceneName != szSceneName )
		{
			ostringstream strm;
			strm<<"cur:\""<<szSceneName<<"\"\tcreate:\""<<szSceneName<<"\"";
			GenErr("Can't create a scene different from the current main scene ", strm.str());
		}

		if( pMainScene -> m_bBeingUsedByLogic )
			GenErr("Can't create a scene twice.");
	}
	else
	{
		CreateMainScene( szSceneName,true );

		return true;
	}

	CCoreSceneClient::MainScene()->m_bBeingUsedByLogic=true;

	return false;
}


bool CCoreSceneMgrClient::DestroyMainScene()
{
	if( !CCoreSceneClient::MainScene()->m_bBeingUsedByLogic )
		return false;

	if( CCoreSceneClient::MainScene()->m_bBeingUsedByEngine )
		return false;

	IntDestroyMainScene();

	return true;
}

void CCoreSceneMgrClient::CreateMainScene(const char* szSceneName,bool bCreateByLogic)
{
	CMetaSceneMgrClient* pMetaSceneMgr=CMetaSceneMgrClient::Inst();
	
	CMetaSceneClient* pMetaScene=pMetaSceneMgr->CoreCreateMetaScene(szSceneName,0);

	CCoreSceneClient* pMainScene=NULL;

	SQR_TRY
	{
		pMainScene=new CCoreSceneClient(pMetaScene,this,bCreateByLogic);
	}
	SQR_CATCH(exp)
	{
		GfkLogExp(exp);
		ShowErrorMsgAndExit("CCoreSceneClient create fail, the client will be shut");
	}
	SQR_TRY_END

	IAppClientHandler* pHandler=CAppClient::Inst()->GetHandler();
	if(pHandler)	
		pHandler->OnMainSceneCreated(pMainScene);
}

void CCoreSceneMgrClient::IntDestroyMainScene()
{
	CCoreSceneClient* pMainScene=CCoreSceneClient::MainScene();
	if ( !pMainScene )
		return;

	IAppClientHandler* pHandler;
	
	pHandler=CAppClient::Inst()->GetHandler();

	if( pHandler )
		pHandler->OnDestroyMainScene();
	
	pMainScene->DestroyAllObject();
	CMetaSceneMgrClient::Inst()->CoreDestroyMetaScene(pMainScene->GetMetaScene());

	pHandler=CAppClient::Inst()->GetHandler();

	if( pHandler )
		pHandler->OnMainSceneDestroyed();
}

void CCoreSceneMgrClient::DestroyMainSceneIfNotBeingUsedByLogic()
{
	CCoreSceneClient* pMainScene=CCoreSceneClient::MainScene();
	if( !pMainScene )
		return;

	if( pMainScene->m_bBeingUsedByLogic )
	{
		pMainScene->DestroyAllObject();
		pMainScene->m_bBeingUsedByEngine = false;
		return;
	}
	
	IntDestroyMainScene();
}

void CCoreSceneMgrClient::OnServerCommand(const CGas2GacOC_Set_Main_Scene* pCmd)
{
	CCoreSceneClient* pMainScene=CCoreSceneClient::MainScene();

	string sSceneName(reinterpret_cast<const char*>(pCmd+1),pCmd->usbSceneNameLen);

	CMetaSceneMgrClient* pMetaSceneMgr=CMetaSceneMgrClient::Inst();

	uint32 SceneVer = pCmd->uqbSceneVer;

	IAppClientHandler* pHandler;

	if(pMainScene)
	{
		if( pMainScene->GetMetaScene()->GetSceneName() == sSceneName )
		{
			//如果已有的MainScene与要求创建的MainScene是同一个场景资源文件，这里不会重新创建这个场景，而是沿用旧的

			pHandler=CAppClient::Inst()->GetHandler();

			if(pHandler)
				pHandler->OnDestroyMainScene();

			pMainScene->DestroyAllObject();
			CCoreSceneClient::MainScene() = NULL;

			pHandler=CAppClient::Inst()->GetHandler();

			if(pHandler)
				pHandler->OnMainSceneDestroyed();

			CCoreSceneClient::MainScene() = pMainScene;
			pMainScene->m_bBeingUsedByEngine=true;
			pMainScene->m_bBeingUsedByLogic=false;	//只要引擎要用场景，立即归还。
			pMainScene->m_bDestroyDirector = false;

			pHandler=CAppClient::Inst()->GetHandler();
			
			CConnClient::Inst()->OnMainSceneSet();

			if(pHandler)
				pHandler->OnMainSceneCreated(pMainScene);

			CCoreSceneClient::MainScene()->SetTimeDistortedRatio(pCmd);
			return;
		}
		pHandler=CAppClient::Inst()->GetHandler();

		if(pHandler)
			pHandler->OnDestroyMainScene();

		pMainScene->DestroyAllObject();
		pMetaSceneMgr->CoreDestroyMetaScene(pMainScene->GetMetaScene());

		pHandler=CAppClient::Inst()->GetHandler();

		if(pHandler)
			pHandler->OnMainSceneDestroyed();
	}

	CMetaSceneClient* pMetaScene=pMetaSceneMgr->CoreCreateMetaScene(sSceneName.c_str(), SceneVer);
	SQR_TRY
	{
		pMainScene=new CCoreSceneClient(pMetaScene,this,false);
		CCoreSceneClient::MainScene()->SetTimeDistortedRatio(pCmd);
	}
	SQR_CATCH(exp)
	{
		GfkLogExp(exp);
		ShowErrorMsgAndExit("CCoreSceneClient create fail, the client will be shut");
	}
	SQR_TRY_END

	pHandler=CAppClient::Inst()->GetHandler();

	CConnClient::Inst()->OnMainSceneSet();

	if(pHandler)
		pHandler->OnMainSceneCreated(pMainScene);
}

void CCoreSceneMgrClient::RefreshCameraDest(float speed, float xpos, float ypos)
{
	CRenderScene* pRenderScene=CClientRenderScene::GetInst();
	//CCameraTick* pInst = CCameraTick::GetInst();
	float fRatio = static_cast<float>(eGridSpanForObj / eGridSpan);
	pRenderScene->SetCameraMaxSpeed(speed);
	pRenderScene->SetCameraDest(xpos / fRatio,ypos  / fRatio);
}
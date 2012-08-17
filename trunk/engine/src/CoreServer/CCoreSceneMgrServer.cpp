#include "stdafx.h"
#include "CCoreSceneMgrServer.h"
#include "CSyncCoreSceneMgrServer.h"
#include "CCoreSceneServer.h"
#include "CMetaSceneServer.h"
#include "CCoreServerSceneJob.h"
#include "CCoreServerJob.inl"

CCoreSceneMgrServer::CCoreSceneMgrServer(void)
{
	Ast( !IntInst() );
	IntInst() = this;
}

CCoreSceneMgrServer::~CCoreSceneMgrServer(void)
{
	IntInst() = NULL;
}

CCoreSceneServer* CCoreSceneMgrServer::CreateCoreScene(CMetaSceneServer* pMetaScene, const char* szSyncAoiFileName, const char* szSceneName, CDimensionInfo* DimInfo)
{
	CCoreSceneServer* pCoreScene = new CCoreSceneServer(pMetaScene);
	pCoreScene->CppSetSceneName(szSceneName);
	uint32 uSlot = pCoreScene->CreateQuadScene(pMetaScene);
	(new CCoreCreateCoreSceneJob(pCoreScene->GetGlobalID(), pMetaScene->m_pSyncMetaScene, 
		szSyncAoiFileName, DimInfo, uSlot))->Add();
	return pCoreScene;
}

bool CCoreSceneMgrServer::DestroyCoreScene(CCoreSceneServer* pScene)
{
	uint32 uSceneID = pScene->GetGlobalID();
	bool bRet = pScene->Destroy();
	if(bRet)
		(new CCoreDestroyCoreSceneJob(uSceneID))->Add();
	return bRet;
}

CCoreSceneMgrServer*& CCoreSceneMgrServer::IntInst()
{
	static CCoreSceneMgrServer* ls_pCoreSceneMgr = NULL;
	return ls_pCoreSceneMgr;
}


CCoreSceneMgrServer* CCoreSceneMgrServer::Inst()
{
	return IntInst();
}

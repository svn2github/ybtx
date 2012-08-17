#include "stdafx.h"
#include "CCoreServerSceneResult.h"
#include "CCoreSceneServer.h"


CCoreEndDestroySceneResult::CCoreEndDestroySceneResult(uint32 uSceneID)
:m_uSceneID(uSceneID)
{

}


CCoreEndDestroySceneResult::~CCoreEndDestroySceneResult()
{
	CCoreSceneServer* pScene = CCoreSceneServer::GetCoreSceneServer(m_uSceneID);
	if (pScene)
	{
		pScene->EndDestroy();
	}
}

CCoreEndCreateSyncCoreSceneResult::CCoreEndCreateSyncCoreSceneResult(uint32 uSceneID)
:m_uSceneID(uSceneID)
{

}


CCoreEndCreateSyncCoreSceneResult::~CCoreEndCreateSyncCoreSceneResult()
{
	CCoreSceneServer* pScene = CCoreSceneServer::GetCoreSceneServer(m_uSceneID);
	if (pScene)
	{
		pScene->SetCoreSceneState(ECoreSceneState_Running);
	}
}


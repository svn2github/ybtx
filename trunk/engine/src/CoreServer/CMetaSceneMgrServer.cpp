#include "stdafx.h"
#include "CMetaSceneServer.h"
#include "CMetaSceneMgrServer.h"
#include "TMetaSceneMgr.inl"
#include "TWhole.inl"
#include "CAppServer.h"
#include "CSyncMetaSceneMgrServer.h"
#include "CSyncMetaSceneServer.h"

template class TMetaSceneMgr<CTraitsServer>;
template class TWhole<CMetaSceneMgrServer,CMetaSceneServer>;

CMetaSceneMgrServer::CMetaSceneMgrServer(void)
{
}

CMetaSceneMgrServer::~CMetaSceneMgrServer(void)
{
}

CMetaSceneServer* CMetaSceneMgrServer::CreateMetaScene(const char* szSceneName)
{
	if( CAppServer::Inst()->DoingQuit() )
		GenErr("Can't create MetaScene while application is doing quit.");

	CMetaSceneServer* pMetaScene = Parent_t::CreateMetaScene(szSceneName,0);
	pMetaScene->m_pSyncMetaScene = 	CSyncMetaSceneMgrServer::Inst()->CreateMetaScene(pMetaScene);
	return pMetaScene;
}

void CMetaSceneMgrServer::DestroyMetaScene(CMetaSceneServer* pScene)
{
	CSyncMetaSceneMgrServer::Inst()->DestroyMetaScene(pScene?pScene->m_pSyncMetaScene:NULL);
	Parent_t::DestroyMetaScene(pScene);
}

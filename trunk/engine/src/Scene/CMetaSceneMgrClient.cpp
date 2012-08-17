#include "stdafx.h"
#include "CMetaSceneClient.h"
#include "CMetaSceneMgrClient.h"
#include "TMetaSceneMgr.inl"
#include "TWhole.inl"
#include "BaseHelper.h"
#include "ErrLogHelper.h"
#include "CAppClient.h"

template class TMetaSceneMgr<CTraitsClient>;
template class TWhole<CMetaSceneMgrClient,CMetaSceneClient>;

CMetaSceneMgrClient::CMetaSceneMgrClient()
{
}

CMetaSceneMgrClient::~CMetaSceneMgrClient()
{
}

CMetaSceneClient* CMetaSceneMgrClient::IntCreateMetaScene(const char* szSceneName, uint32 SceneVer)
{
	CMetaSceneClient* pScene =NULL;
	SQR_TRY
	{
		pScene=ParentMgr_t::CreateMetaScene(szSceneName, SceneVer);
		Ver( m_mapLoadedScene.insert(make_pair(szSceneName,pScene)).second );
		return pScene;
	}
	SQR_CATCH(exp)
	{
		GfkLogExp(exp);
		ShowErrorMsgAndExit("CMetaSceneClient create fail, the client will be shut");
	}
	SQR_TRY_END;
	return pScene;
}

void CMetaSceneMgrClient::IntDestroyMetaScene(CMetaSceneClient* pScene)
{
	SQR_TRY
	{
		VerEq(m_mapLoadedScene.erase(pScene->GetSceneName()),1);
		ParentMgr_t::DestroyMetaScene(pScene);
	}
	SQR_CATCH(exp)
	{
		GfkLogExp(exp);
		ShowErrorMsgAndExit("CMetaSceneClient delete fail, the client will be shut");
	}
	SQR_TRY_END;
}

CMetaSceneClient* CMetaSceneMgrClient::CoreCreateMetaScene(const char* szSceneName, uint32 SceneVer)
{
	map<string,CMetaSceneClient*>::iterator it=m_mapLoadedScene.find(szSceneName);

	CMetaSceneClient* pScene
		=(it!=m_mapLoadedScene.end())?it->second:IntCreateMetaScene(szSceneName, SceneVer);

	Ast(!pScene->m_bUsedByCore);
	pScene->m_bUsedByCore=true;
	return pScene;
}

void CMetaSceneMgrClient::CoreDestroyMetaScene(CMetaSceneClient* pScene)
{
	Ast(pScene->m_bUsedByCore);
	pScene->m_bUsedByCore=false;

	if(pScene->m_uUsedCountByShell)
		return;

	IntDestroyMetaScene(pScene);
}

CMetaSceneClient* CMetaSceneMgrClient::CreateMetaScene(const char* szSceneName)
{
	map<string,CMetaSceneClient*>::iterator it=m_mapLoadedScene.find(szSceneName);

	CMetaSceneClient* pScene
		=(it!=m_mapLoadedScene.end())?it->second:IntCreateMetaScene(szSceneName, 0);

	++pScene->m_uUsedCountByShell;

	return pScene;
}

void CMetaSceneMgrClient::DestroyMetaScene(CMetaSceneClient* pScene)
{
	Ast(pScene->m_uUsedCountByShell);

	if(--pScene->m_uUsedCountByShell || pScene->m_bUsedByCore)
		return;

	IntDestroyMetaScene(pScene);
}
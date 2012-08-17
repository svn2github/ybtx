#include "stdafx.h"
#include "CoreCommon.h"
#include "CMetaSceneServer.h"
#include "CCoreSceneServer.h"
#include "CMetaSceneMgrServer.h"
#include "TMetaScene.inl"
#include "TWhole.inl"
#include "TPart.inl"
#include "ErrLogHelper.h"
#include "TBaseMetaSceneServer.inl"


template class TPart<CMetaSceneServer, CMetaSceneMgrServer>;
template class TMetaScene<CTraitsServer>;
template class TBaseMetaSceneServer<CTraitsServer>;

CMetaSceneServer::CMetaSceneServer( const char* szSceneName, const uint32 sceneId, CMetaSceneMgrServer* pMgr )
:Parent_t(szSceneName, sceneId, pMgr)
,m_pSyncMetaScene(NULL)
{
	SQR_TRY
	{
		Parent_t::LoadAll();
	}
	SQR_CATCH(exp)
	{
		LogExp(exp);
	}
	SQR_TRY_END;
}

CMetaSceneServer::~CMetaSceneServer()
{
}

#include "stdafx.h"
#include "CoreCommon.h"
#include "CSyncMetaSceneServer.h"
#include "CSyncCoreSceneServer.h"
#include "CSyncMetaSceneMgrServer.h"
#include "TMetaScene.inl"
#include "TWhole.inl"
#include "TPart.inl"
#include "ErrLogHelper.h"
#include "TBaseMetaSceneServer.inl"

template class TPart<CSyncMetaSceneServer, CSyncMetaSceneMgrServer>;
template class TMetaScene<CSyncTraitsServer>;
template class TBaseMetaSceneServer<CSyncTraitsServer>;

CSyncMetaSceneServer::CSyncMetaSceneServer(const char* szSceneName, const uint32 sceneId, CSyncMetaSceneMgrServer* pMgr)
:Parent_t(szSceneName, sceneId, pMgr)
{

}

CSyncMetaSceneServer::~CSyncMetaSceneServer()
{
}

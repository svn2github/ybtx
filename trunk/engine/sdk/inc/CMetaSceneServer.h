#pragma once
#include "CTraitsServer.h"
#include "TBaseMetaSceneServer.h"


namespace sqr
{
	class CSyncMetaSceneServer;
	class CSyncMetaSceneMgrServer;
	class CCoreSceneMgrServer;

	class CMetaSceneServer
		:public TBaseMetaSceneServer<CTraitsServer>
	{
		typedef TBaseMetaSceneServer<CTraitsServer> Parent_t;
		friend class TMetaSceneMgr<CTraitsServer>;
		friend class CMetaSceneMgrServer;
		friend class CCoreSceneMgrServer;
		friend class TPart<CMetaSceneServer,CMetaSceneMgrServer>;
		friend class CSyncMetaSceneMgrServer;

	private:
		CMetaSceneServer( const char* szSceneName, const uint32 sceneId, CMetaSceneMgrServer* pMgr );
		~CMetaSceneServer();

		CSyncMetaSceneServer* m_pSyncMetaScene;
	};
}

#pragma once

#include "TMetaSceneMgr.h"
#include "CTraitsClient.h"

namespace sqr
{
	class CTestSuiteCoreObjectClient;
	class CTestSuiteMetaSceneClient;
	
	class CMetaSceneMgrClient
		:public TMetaSceneMgr<CTraitsClient>
	{
	private:
		typedef TMetaSceneMgr<CTraitsClient> ParentMgr_t;
		friend class TMetaSceneMgr<CTraitsClient>;
		friend class CMetaSceneClient;
		friend class CCoreSceneMgrClient;
		friend class CTestSuiteCoreObjectClient;
		friend class CTestSuiteMetaSceneClient;
		friend class TCoreApp<CTraitsClient>;

		map<string,CMetaSceneClient*>	m_mapLoadedScene;
		
		CMetaSceneClient* IntCreateMetaScene(const char* szSceneName, uint32 SceneVer);
		void IntDestroyMetaScene(CMetaSceneClient* pScene);

		CMetaSceneClient* CoreCreateMetaScene(const char* szSceneName, uint32 SceneVer);
		void CoreDestroyMetaScene(CMetaSceneClient* pScene);
	public:
		CMetaSceneMgrClient();
		~CMetaSceneMgrClient();

		CMetaSceneClient* CreateMetaScene(const char* szSceneName);
		void DestroyMetaScene(CMetaSceneClient* pScene);
	};
}
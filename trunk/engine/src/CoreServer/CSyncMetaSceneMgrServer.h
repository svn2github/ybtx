#pragma once
#include "TMetaSceneMgr.h"
#include "CSyncTraitsServer.h"

namespace sqr
{
	class CMetaSceneMgrServer;
	class CMetaSceneServer;

	class CSyncMetaSceneMgrServer
		:public TMetaSceneMgr<CSyncTraitsServer>
	{
		friend 	class CMetaSceneMgrServer;
	private:
		typedef TMetaSceneMgr<CSyncTraitsServer>	Parent_t;
	public:
		CSyncMetaSceneMgrServer(void);
		~CSyncMetaSceneMgrServer(void);

		CSyncMetaSceneServer* CreateMetaScene(const CMetaSceneServer* pMetaScene);
		void DestroyMetaScene(CSyncMetaSceneServer* pScene);
	};
}

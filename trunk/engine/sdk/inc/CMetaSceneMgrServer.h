#pragma once

#include "TMetaSceneMgr.h"
#include "CTraitsServer.h"

namespace sqr
{
	class CMetaSceneMgrServer
				:public TMetaSceneMgr<CTraitsServer>
	{
		private:
			typedef TMetaSceneMgr<CTraitsServer>	Parent_t;
		public:
			CMetaSceneMgrServer(void);
			~CMetaSceneMgrServer(void);

			CMetaSceneServer* CreateMetaScene(const char* szSceneName);
			void DestroyMetaScene(CMetaSceneServer* pScene);
	};
}


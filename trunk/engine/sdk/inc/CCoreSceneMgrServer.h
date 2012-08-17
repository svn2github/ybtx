#pragma once
#include "CDynamicObject.h"
#include "CCoreSceneMallocObject.h"

namespace sqr
{
	class CCoreSceneServer;
	class CMetaSceneServer;
	class CSyncCoreSceneMgrServer;
	class CAppServer;
	class CDimensionInfo;

	class CCoreSceneMgrServer
		:public virtual CDynamicObject
		,public CCoreSceneMallocObject
	{
		friend class CAppServer;
	public:
		CCoreSceneServer* CreateCoreScene(CMetaSceneServer* pMetaScene
			,const char* szSyncAoiFileName,const char* szSceneName, CDimensionInfo* DimInfo);
		bool DestroyCoreScene(CCoreSceneServer* pScene);

		static CCoreSceneMgrServer* Inst();

	private:
		static CCoreSceneMgrServer*& IntInst();

		CCoreSceneMgrServer(void);
		~CCoreSceneMgrServer(void);
	};
}

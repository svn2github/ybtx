#pragma once
#include "TCoreSceneMgr.h"
#include "CSyncTraitsServer.h"
#include "ISend.h"

namespace sqr
{
	class CCoreSceneServer;
	class CDimensionInfo;

	class CSyncCoreSceneMgrServer
		:public TCoreSceneMgr<CSyncTraitsServer>
		,public ISend
	{
	friend class CCoreCreateCoreSceneJob;
	public:
		CSyncCoreSceneMgrServer();
		~CSyncCoreSceneMgrServer();
		CSyncCoreSceneServer* CreateCoreScene(uint32 uScenID, CSyncMetaSceneServer* pMetaScene, const char* szSyncAoiFileName, CDimensionInfo* DimInfo, uint32 uSolt);
		bool DestroyCoreScene(CSyncCoreSceneServer* pScene);
	private:
		typedef TCoreSceneMgr<CSyncTraitsServer>	ParentMgr_t;
		friend class TCoreSceneMgr<CSyncTraitsServer>;
		friend class CSyncAppServer;
		friend class CSyncCoreSceneServer;

		bool	m_bHalting;

		bool Halt();
		bool Halting()const;

		void OnACoreSceneDestroyed();

		uint32 Send(const void* pData,uint32 uSize);
	};
}

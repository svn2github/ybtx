#pragma once
#include "TCoreSceneMgr.h"
#include "CTraitsClient.h"

namespace sqr
{
	class CTestSuiteCoreObjectClient;
	class CCoreSceneClient;
	class CConnClient;

	class CGas2GacOC_Set_Main_Scene;

	class CCoreSceneMgrClient
		:public TCoreSceneMgr<CTraitsClient>
	{
	public:
		//return false表示同名场景已经被创建了
		bool CreateMainScene(const char* szSceneName);
		//return false表示CoreObjectDirector正在使用该场景，无法被删除
		bool DestroyMainScene();

		void RefreshCameraDest(float speed, float xpos, float ypos);
		
	private:
		typedef TCoreSceneMgr<CTraitsClient>	ParentMgr_t;
		friend class TCoreSceneMgr<CTraitsClient>;
		friend class CTestSuiteCoreObjectClient;
		friend class CCoreSceneClient;
		friend class CConnMgrClient;
		friend class CConnClient;
		friend class TCoreApp<CTraitsClient>;
		friend class CAppClient;

		void CreateMainScene(const char* szSceneName,bool bCreateByLogic);
		void IntDestroyMainScene();

		void DestroyMainSceneIfNotBeingUsedByLogic();

		CCoreSceneMgrClient();
		~CCoreSceneMgrClient();

		void OnServerCommand(const CGas2GacOC_Set_Main_Scene* pCmd);
	};

}

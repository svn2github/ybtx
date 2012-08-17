#pragma once
#include "TCoreScene.h"
#include "CTraitsClient.h"

namespace sqr
{
	class CClientRenderScene;
	class CRenderScene;
	class CMetaSceneClient;
	class IModelHandler;

	class CCoreObjectClient;
	class CCoreObjectPerformer;
	class CCoreObjectFollower;
	class CCoreObjectDirector;

	class ICoreObjectDirectorHandler;
	class ICoreObjectFollowerHandler;
	class ICoreObjectPerformerHandler;
	class ICoreObjectClientHandler;

	class ICoreSceneClientHandler;

	class CGas2GacOC_Create_Moving_Object;
	class CGas2GacOC_Create_Still_Object;
	class CGas2GacOC_Destroy_Object;
	class CGas2GacOC_Set_Time_Distorted_Ratio;
	class CGas2GacOC_Set_Main_Scene;

	class CDistortedTimeSceneClient;
	class CDistortedTimeObjClient;
	class CDistortedTimeObj;

	class CCoreSceneClient
		:public TCoreScene<CTraitsClient>
	{
	public:
		void SetHandler(ICoreSceneClientHandler* pHandler);
		ICoreSceneClientHandler* GetHandler()const;

		CCoreObjectPerformer* CreateObjectPerformer(const CFPos& Pos,ICoreObjectPerformerHandler* pHandler);
		void DestroyObjectPerformer(CCoreObjectPerformer* pObject);
		
		CDistortedTimeObjClient* CreateDistortedTimeObj();
		void DestroyDistortedTimeObj(CDistortedTimeObj* pObj);
		void SetTimeDistortedRatio(const CGas2GacOC_Set_Main_Scene* pCmd);

		CRenderScene* GetRenderScene()const;


		static CCoreSceneClient*& MainScene();
		static CCoreSceneClient*  Inst();
	private:
		typedef TCoreScene<CTraitsClient>	ParentScene_t;
		friend class CCoreObjectFollower;
		friend class TCoreSceneMgr<CTraitsClient>;
		friend class CCoreSceneMgrClient;
		friend class CConnClient;
		friend class TPart<ImpCoreScene_t,ImpCoreSceneMgr_t>;
		friend class TPart<ImpCoreScene_t,ImpMetaScene_t>;
		friend class TCoreScene<CTraitsClient>;
		friend class CTestSuiteCoreObjectClient;
		
		void OnDeleteObjectInClearDerived(CCoreObjectClient* pObj);

		void OnServerCommand(const CGas2GacOC_Create_Moving_Object* pCmd);
		void OnServerCommand(const CGas2GacOC_Create_Still_Object* pCmd);
		void OnServerCommand(const CGas2GacOC_Destroy_Object* pCmd);
		void OnServerCommand(const CGas2GacOC_Set_Time_Distorted_Ratio* pCmd);

		template<typename ObjectClass,typename CmdClass>
		void CreateObject(CmdClass* pCmd);
		template<typename CmdClass>
		void CreateObject(CmdClass* pCmd);

		void DestroyAllObject();
		void SetDynamicBarrierInfo(uint32 uOwnerID, const DynamicBarrierArea& dynamicBarrier, EBarrierType eBarrierType);

		CCoreSceneClient(CMetaSceneClient* pSceneClient,CCoreSceneMgrClient* pMgr,bool bCreateByLogic);
		~CCoreSceneClient(void);

		CClientRenderScene*					m_pRenderScene;
		ICoreSceneClientHandler*			m_pHandler;
		bool								m_bBeingUsedByLogic;
		bool								m_bBeingUsedByEngine;
		bool								m_bDestroyDirector;
		CDistortedTimeSceneClient*			m_pDistortedTimeScene;
	};
}
#pragma once
#include "TCoreScene.h"
#include "CSyncTraitsServer.h"
#include "ISend.h"
#include "TIDPtrMap.h"


namespace sqr
{
	template<class T> struct TRect;
	typedef TRect<int>		CIRect;

	class IMulticast;
	class CSyncMetaSceneServer;

	class ICoreObjectMediatorHandler;
	class ICoreObjectDictatorHandler;
	class ICoreObjectCalculatorHandler;

	class CCoreGridServer;
	class CConnServer;

	class CSyncCoreObjectServer;
	class CSyncCoreObjectServer;
	class CSyncCoreObjectMediator;
	class CSyncCoreObjectDictator;
	class CCoreObjectCalculator;

	class CObjVarDef;

	class CAsyncContext;

	class CAsyAoiViewObj;
	class CAsyAoiScene;
	class ICoreGridServerHandler;

	class CSyncCoreQuadScene;

	class CCoreSendSceneCmdJob;

	class CSyncDistortedTimeSceneServer;
	class CSyncDistortedTimeObjServer;
	class CDistortedTimeObj;

	class CDimensionInfo;

	class CSyncCoreSceneServer
		:public TCoreScene<CSyncTraitsServer>
		,public ISend
	{
		typedef vector<CSyncCoreSceneServer*,TCoreSceneAllocator<CSyncCoreSceneServer*> > IDVecSynCoreSceneServer;
	public:
		ISend* GetIS()const;

		void CreateObjectMediator(uint32 uObjID, const CFPos& PixelPos,uint32 uVarDefID, uint32 uSlot, float fDirectorMaxSpeed);
		void CreateObjectDictator(uint32 uObjID, const CFPos& PixelPos,uint32 uVarDefID, uint32 uSlot);
		void CreateObjectCalculator(uint32 uObjID, const CFPos& PixelPos, uint32 uSlot);

		CSyncDistortedTimeObjServer* CreateDistortedTimeObj();
		void DestroyDistortedTimeObj(CDistortedTimeObj* pObj);
		void SetTimeDistortedRatio(float fTimeDistortedRatio);
		float GetTimeDistortedRatio()const;

		//return false表示对象已经处于正在删除状态。
		bool DestroyObject(CSyncCoreObjectServer* pObject);

		CSyncMetaSceneServer* GetMetaScene()const;

		bool TransferObject( CSyncCoreObjectServer* pObj, CSyncCoreSceneServer* pScene, const CFPos& PixelPos );

		bool Destroying()const;
		bool IsPixelValid(const CFPos& PixelPos)const;

		uint32 GetGlobalID()const {return m_uID;}
		static CSyncCoreSceneServer* GetSynCoreSceneServer(uint32 uID);

	private:
		typedef TCoreScene<CSyncTraitsServer>	ParentScene_t;
		friend class CSyncCoreObjectMediator;
		friend class CSyncCoreObjectDictator;
		friend class CSyncCoreObjectServer;
		friend class TCoreSceneMgr<CSyncTraitsServer>;
		friend class CSyncCoreSceneMgrServer;
		friend class TPart<ImpCoreScene_t,ImpCoreSceneMgr_t>;
		friend class TPart<ImpCoreScene_t,ImpMetaScene_t>;
		friend class TCoreScene<CSyncTraitsServer>;
		friend class CSyncMetaSceneServer;
		friend class CCoreSendSceneCmdJob;
		friend class CAoiSceneHandler;


		IMulticast*				m_pMulticast;

		uint32 Send(const void* pData,uint32 uSize);

		CSyncCoreSceneServer(uint uID, CSyncMetaSceneServer* pSceneServer, CSyncCoreSceneMgrServer* pMgr
			, const char* szSyncAoiFileName, CDimensionInfo* DimInfo);
		~CSyncCoreSceneServer(void);

		//uint32	m_uViewAoiRadius;

		CAsyAoiScene *m_pAoiScene;

		CAsyAoiScene* GetAoiScene() const;

		bool HaltAoiScene();

		void CreateAoiObj(CSyncCoreObjectServer* pObj);
		void DestroyAoiObj(CAsyAoiViewObj* pAoiObj);

		void MulticastSendCmd(IMulticast*pMulticast, const void* pData, uint32 uSize);
		void AddToMulticast(CSynConnServer*);
		void DelFromMulticast(CSynConnServer*);

		//return false表示场景已经处于删除状态
		bool BeginDestroyScene();
		//这个函数会被Aoi Scene OnHalted消息所调用
		void EndDestroyScene();

		//对象自己删除自己了以后，通知场景对象。如果场景对象此时正在析构中，则检查是否所有对象都删除干净了，如果是则场景自身也可以开始析构了。
		void OnObjectDestroyed();
		void SetDynamicBarrierInfo(uint32 uOwnerID, const DynamicBarrierArea& dynamicBarrier, EBarrierType eBarrierType);


		void ConfirmObjectCreateCondition(const CFPos& PixelPos);
		void ConfirmObjectCreateSceneState(CSyncCoreObjectServer* pObj);

		void ClearObjectInDestroyingSceneList();

		void CreateQuadScene(uint32 uSlot);
		void ReleaseQuadScene();

		static IDVecSynCoreSceneServer& GetIDVecCCoreSCeneServer();

	private:
		list<CSyncCoreObjectServer*,TCoreSceneAllocator<CSyncCoreObjectServer*> >	m_listObjectInDestroyingScene;

		CSyncCoreQuadScene*						m_pQuadScene;

		uint32									m_uID;
		bool									m_bDestroying;
		CSyncDistortedTimeSceneServer*			m_pDistortedTimeScene;

	};
}

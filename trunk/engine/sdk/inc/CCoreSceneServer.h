#pragma once
#include "CDynamicObject.h"
#include "FindPathDefs.h"
#include "TIDPtrMap.h"
#include "TPart.h"
#include "ISend.h"
#include "TCoreSceneAllocator.h"

namespace sqr
{
	template<class T> struct TPos;
	typedef TPos<int32> CPos;
	typedef TPos<float> CFPos;

	template<class T> struct TRect;
	typedef TRect<float> CFRect;

	class CSyncCoreSceneServer;
	class CSyncCoreSceneMgrServer;
	class CMetaSceneServer;
	class CPixelPath;
	class ISend;

	class CCoreObjectServer;
	class CCoreObjectMediator;
	class CCoreObjectDictator;
	class CCoreObjectCalculator;
	class ICoreObjectMediatorHandler;
	class ICoreObjectDictatorHandler;
	class ICoreObjectCalculatorHandler;

	class ICoreSceneServerHandler;

	class CAsynCoreQuadScene;
	class CDynamicBarrierInfo;

	class CAsyncContext;

	class CAsynDistortedTimeSceneServer;
	class CAsynDistortedTimeObjServer;
	class CDistortedTimeObj;

	enum ECoreSceneState
	{
		ECoreSceneState_Construct,
		ECoreSceneState_Running,
		ECoreSceneState_Desconstruct,
	};


	class CCoreSceneServer
		:public virtual CDynamicObject
		,public TPart<CCoreSceneServer, CMetaSceneServer>
		,public ISend
		,public CCoreSceneMallocObject
	{
		friend class CCoreObjectServer;
		friend class CCoreSceneMgrServer;
		friend class CCoreObjectDictator;
		friend class CCoreEndCreateSyncCoreSceneResult;
		friend class CCoreEndDestroySceneResult;
		friend class CCoreObjOnTransferedResult;

		typedef TIDPtrMap<CCoreSceneServer>	MapCoreScene;
		typedef vector<CPos> DynamicBarrierArea;
		typedef TPart<CCoreSceneServer, CMetaSceneServer> ParentMetaPart_t;
	public:
		CCoreSceneServer(CMetaSceneServer* pMetaScene);
		~CCoreSceneServer(void);

		void SetHandler(ICoreSceneServerHandler* pHandler);
		ICoreSceneServerHandler* GetHandler()const;

		CMetaSceneServer* GetMetaScene()const;

		CPixelPath* CreatePath(const CFPos& PixelPosBegin, const CFPos& PixelPosEnd, EFindPathType eFindPathType, EBarrierType eBarrierType, int32 nMaxStep);
		CPixelPath* CreatePath(const CFPos& PixelPosBegin, const CFPos& PixelPosEnd, EFindPathType eFindPathType, EBarrierType eBarrierType);
		CPixelPath* CreatePath(const int16* pPathData, uint16 uPathDataLen, const CFPos& PixelPosBegin, const CFPos& PixelPosEnd);
		CPixelPath* CreatePath(const vector<CFPos>& vecPos);

		bool IsGridValid(const CPos& GridPos) const;
		bool IsPixelValid(const CFPos& PixelPos)const;
		
		//暂时添加两个接口用于调试使用
		EBarrierType GetStaticBarrier( const CPos& GridPos )const;
		EBarrierType GetDynamicBarrier( const CPos& GridPos )const;
		//
		EBarrierType GetBarrier( const CPos& GridPos )const;

		CCoreObjectMediator* CreateObjectMediator(const CFPos& PixelPos,ICoreObjectMediatorHandler* pHandler,uint32 uVarDefID, float fDirectorMaxSpeed);
		CCoreObjectDictator* CreateObjectDictator(const CFPos& PixelPos,ICoreObjectDictatorHandler* pHandler,uint32 uVarDefID);
		CCoreObjectCalculator* CreateObjectCalculator(const CFPos& PixelPos,ICoreObjectCalculatorHandler* pHandler);

		//return false表示对象已经处于正在删除状态。
		bool DestroyObject(CCoreObjectServer* pObject);

		CAsynDistortedTimeObjServer* CreateDistortedTimeObj();
		void DestroyDistortedTimeObj(CDistortedTimeObj* pObj);
		void SetTimeDistortedRatio(float fTimeDistortedRatio);

		void QueryObject(vector<CCoreObjectServer*>& vecObject,const CFPos& Pos,float fGridRadius)const;

		bool TransferObject( CCoreObjectServer* pObj, CCoreSceneServer* pScene, const CFPos& PixelPos );

		bool HasObject()const;

		uint32 GetGlobalID()const {return m_uID;}

		bool Destroy();
		bool Destroying()const;

		virtual uint32 Send(const void* pData,uint32 uSize);

		static CCoreSceneServer* GetCoreSceneServer(uint32 uSceneID);
		const string& GetSceneName()const {return m_sSceneName;}
		uint32 GetWidthInPixel() const;
		uint32 GetHeightInPixel() const;

	private:
		void AddObject(CCoreObjectServer* pObj);
		void DelObject(CCoreObjectServer* pObj);
		
		void EndDestroy();		

		void QueryObject(vector<CCoreObjectServer*>& vecObject, CCoreObjectServer* pObj, 
						 const CFPos& OffsetPos,float fRadius)const;

		uint32 CreateQuadScene(CMetaSceneServer* pMetaScene);
		void ReleaseQuadScene();

		void ConfirmObjectCreateCondition(const CFPos& PixelPos);

		void SetDynamicBarrierInfo(uint32 uOwnerID, const DynamicBarrierArea& dynamicBarrier, EBarrierType eBarrierType);
		static void QueryFunc(void* pVecObject, uint32 uId);

		void SetCoreSceneState(ECoreSceneState eSceneState);

		static MapCoreScene& GetMapCoreScene();

		void CppSetSceneName(const char* sSceneName) {m_sSceneName = sSceneName;};
		void DestroyAllObj();

	private:
		typedef list<CCoreObjectServer*,TCoreSceneAllocator<CCoreObjectServer*> >				ListObject;
		CAsynCoreQuadScene*										m_pQuadScene;
		uint32													m_uID;
		CDynamicBarrierInfo*									m_pBarrierInfo;
		ListObject												m_lstObject;
		ECoreSceneState											m_eCoreSceneState;
		ICoreSceneServerHandler*								m_pHandler;
		CAsyncContext*											m_pDestroyContext;
		string													m_sSceneName;
		CAsynDistortedTimeSceneServer*							m_pDistortedTimeScene;
	};
}

#pragma once  

#include "CPos.h" 
#include "AoiDef.h"
#include "CBaseAoiObj.h"
#include "TIDPtrMap.h"
#include "CQuadMallocObject.h"
#include "TQuadAllocator.h"
#include "PatternRef.h"

namespace sqr
{ 
	class CAsyAoiSyncerObj;
	class CAsyAoiSynceeObj;

	class CAsyAoiScene;
	class CTestSuiteAoi;

	class CAoiViewObjHandler;

	class CAsyAoiDimObj;

	class CSynAoiViewObj;
	class CSyncCoreObjectServer;

	class CAsyAoiViewObj : public CQuadMallocObject
	{ 
		friend class CAsyAoiScene; 
		friend class CTestSuiteAoi;

		friend class CAoiViewChangedResult;
		friend class CAoiSyncChangedResult;
		friend class CAoiMyRectChangedResult;
		friend class CAoiOtRectChangedResult;
		friend class CAoiSizeChangedResult;
		friend class CAoiEyeSightChangedResult;
		friend class CAoiStealthChangedResult;
		friend class CAoiKeennessChangedResult;
		friend class CAoiViewGroupChangedResult;
		friend class CAoiPosChangedResult;
		friend class CAoiSyncSightEnabledResult;
		friend class CAoiMsgFromViewerHandledResult;
		friend class CAoiMsgFromVieweeHandledResult;
		friend class CAoiMsgToVieweeHandledEndedResult;
		friend class CAoiMsgToViewerHandledEndedResult;
		friend class CAoiMsgToSelfHandledResult;	
		friend class CAoiSetLastSyncJobIDResult;
		friend class CAoiObjHaltedResult;

		friend class CAoiJob;

		friend class CAsyAoiDimObj;
		friend class CSyncCoreSceneServer;

	public: 
		bool CanBeSync()const;
		virtual EAoiObjType GetType()const
		{
			return eAOT_ViewObj;
		}

		const CAsyAoiSyncerObj* CastToSyncerObj()const;
		CAsyAoiSyncerObj* CastToSyncerObj();
		const CAsyAoiSynceeObj* CastToSynceeObj()const;
		CAsyAoiSynceeObj* CastToSynceeObj();

		void SetSize( float fSize, uint32 uDimension = 0);
		void SetEyeSight( float fEyeSight, uint32 uDimension = 0);
		void SetStealth( float fStealth, uint32 uDimension = 0);
		void SetKeenness(float Keenness, uint32 uDimension = 0);
		void SetViewGroup(uint32 uGroupID, uint32 uDimension = 0);
		float GetSize(uint32 uDimension = 0) const; 
		float GetEyeSight(uint32 uDimension = 0) const; 
		float GetStealth(uint32 uDimension = 0) const; 
		float GetKeenness(uint32 uDimension = 0) const; 
		uint32 GetViewGroup(uint32 uDimension = 0) const; 
		float GetDim0Size() const; 
		float GetDim0Stealth() const; 
		uint32 GetDim0ViewGroup() const; 

		void GetDimensions(vector<uint32>& vecDim) const;

		void SetPos(const CFPos& pos);
		void ForceSetPos(const CFPos& pos);//强制更新四叉树
		const CFPos& GetPos() const;

		CAoiViewObjHandler* GetHandler() const;

		static CAsyAoiViewObj* GetAoiObject(uint32 id);

		bool Halt();
		bool Running()const;
		bool Halting()const;
		bool Halted()const;

		void PostMsgToViewee(const void* pContext, uint32 uDimension);
		void PostMsgToViewer(const void* pContext, uint32 uDimension);
		void PostMsgToSelf(const void* pContext);	

	protected: 
		CAsyAoiViewObj(CAsyAoiScene *pScene, const CFPos& pos,CAoiViewObjHandler *pObj); 
		virtual ~CAsyAoiViewObj();

		uint32 GetGlobalID() const;

		void OnViewRelationChanged(CAsyAoiViewObj* pViewObj, bool bSee, uint32 uDimension = 0);

		void OnMsgFromViewer(const void* pContext);
		void OnMsgFromViewee(const void* pContext);
		void OnMsgToVieweeHandled(const void* pContext);
		void OnMsgToViewerHandled(const void* pContext);
		void OnMsgFromSelf(const void* pContext);
		void OnHalted();

		CAsyAoiScene* GetScene() const
		{
			return m_pScene;
		}

		CAsyAoiDimObj* GetAoiDimObj(uint32 uDimensionId) const;

	private:

		void CreateAoiObj(const CFPos& pos, EAoiObjType type);

		//一下对同步对象操作
		static CSynAoiViewObj* GetSynAoiObject(uint32 id);
		static void AddSynAoiObject(uint32 uGlobalId, CSynAoiViewObj* pObj);
		static void DelSynAoiObject(uint32 uGlobalId);

	private:
		typedef TIDPtrMap<CAsyAoiViewObj>	IDMapAsyAoiObject_t;
		static IDMapAsyAoiObject_t		ms_IDMapAsyObject;

		typedef vector<CSynAoiViewObj*, TQuadAllocator<CSynAoiViewObj*> >	IDVecSynAoiObject_t;
		static IDVecSynAoiObject_t		ms_vecSynObject;

		CAsyAoiScene*				m_pScene;
		TPtRefer<CAsyAoiViewObj, CAoiViewObjHandler>	m_Handler;
		uint32						m_uId;
		CFPos						m_Pos;

		enum EAsynAoiObjState
		{
			eAAOS_Running,
			eAAOS_Halting,
			eAAOS_Halted
		};

		EAsynAoiObjState	m_eState;


#ifdef _WIN32
		typedef hash_map<uint32, CAsyAoiDimObj*, hash_compare<uint32, less<uint32> >, 
			TQuadAllocator<pair<uint32, CAsyAoiDimObj*> > >	MapDimObj_t;
#else
		typedef hash_map<uint32, CAsyAoiDimObj*, hash<uint32>, equal_to<uint32>, 
			TQuadAllocator<pair<uint32, CAsyAoiDimObj*> > > MapDimObj_t;
#endif

		MapDimObj_t			m_mapAoiDimObj;

		TPtRefee<CAsyAoiViewObj, CSyncCoreObjectServer>	m_RefsByCoreObj;
	}; 
} 

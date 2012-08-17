#pragma once 
#include "CQuadMallocObject.h"
#include "TQuadAllocator.h"

namespace sqr
{
	class CAsyAoiGrid;
	class CAsyAoiViewObj;
	class CAsyAoiSyncerObj;
	class CAsyAoiSynceeObj;

	class CAoiViewObjHandler;
	class CAoiSyncerObjHandler;
	class CAoiSynceeObjHandler;

	class CAsyAoiDimScene;

	class CTestSuiteAoi;

	class CSynAoiScene;

	template<typename T> struct TPos;
	typedef TPos<float>	CFPos;
	typedef TPos<int32>	CPos;

	class CAoiSceneHandler;

	class CDimensionInfo;
	class CAsyAoiScene : public CQuadMallocObject
	{
		friend class CAsyAoiViewObj;
		friend class CTestSuiteAoi;
		friend class CAoiJob;
		friend class CAoiSceneHaltedResult;

	public:
		CAsyAoiScene(const char* szSyncAoiFileName,uint32 uScale,  
					  uint32 uPixelWidth, uint32 uPixelHeight, CAoiSceneHandler* pHandler, CDimensionInfo* DimInfo);
		
		CAsyAoiViewObj* CreateViewObj(const CFPos& pos,CAoiViewObjHandler *pHandler);
		CAsyAoiSyncerObj* CreateSyncerObj(const CFPos& pos,CAoiSyncerObjHandler *pHandler);
		CAsyAoiSynceeObj* CreateSynceeObj(const CFPos& pos,CAoiSynceeObjHandler *pHandler);
		void DestroyObj(CAsyAoiViewObj* pObj);

		bool IsInSyncAoi(const CPos& OffsetGridPos)const;

		void Release();

		static CAsyAoiScene* GetAoiScene(uint32 uId);

		CAoiSceneHandler* GetHandler();

		bool Halt();
		bool Running()const;
		bool Halting()const;
		bool Halted()const;

	private:
		~CAsyAoiScene();

		static uint32 GetNextId();

		//一下为同步对象的操作
		uint32 GetSynSceneId() const;
		static CSynAoiScene* GetSynAoiScene(uint32 uId);
		static bool AddSynAoiScene(uint32 uGlobalId, CSynAoiScene* pScene);
		static bool DelSynAoiScene(uint32 uGlobalId);

		void OnHalted();

	private:

#ifdef _WIN32
		typedef hash_map<uint32, CSynAoiScene*, hash_compare<uint32, less<uint32> >, 
			TQuadAllocator<pair<uint32, CSynAoiScene*> > >	MapSynAoiScene_t;
#else
		typedef hash_map<uint32, CSynAoiScene*, hash<uint32>, equal_to<uint32>, 
			TQuadAllocator<pair<uint32, CSynAoiScene*> > > MapSynAoiScene_t;
#endif
	
		static MapSynAoiScene_t ms_mapIdToSynScene;

		uint32 m_uId;

		CAoiSceneHandler*	m_pHandler;

#ifdef _WIN32
		typedef hash_map<uint32, CAsyAoiScene*, hash_compare<uint32, less<uint32> >, 
			TQuadAllocator<pair<uint32, CAsyAoiScene*> > >	MapAsyAoiScene_t;
#else
		typedef hash_map<uint32, CAsyAoiScene*, hash<uint32>, equal_to<uint32>, 
			TQuadAllocator<pair<uint32, CAsyAoiScene*> > > MapAsyAoiScene_t;
#endif

		static MapAsyAoiScene_t	ms_mapIdToAsyScene;

		enum EAsynAoiSceneState
		{
			eAASS_Running,
			eAASS_Halting,
			eAASS_Halted
		};

		EAsynAoiSceneState	m_eState;

#ifdef _WIN32
		typedef hash_map<uint32, CAsyAoiDimScene*, hash_compare<uint32, less<uint32> >, 
			TQuadAllocator<pair<uint32, CAsyAoiDimScene*> > >	MapAsyAoiDimScene_t;
#else
		typedef hash_map<uint32, CAsyAoiDimScene*, hash<uint32>, equal_to<uint32>, 
			TQuadAllocator<pair<uint32, CAsyAoiDimScene*> > > MapAsyAoiDimScene_t;
#endif

		MapAsyAoiDimScene_t		m_mapAoiDimScene;

	};
}


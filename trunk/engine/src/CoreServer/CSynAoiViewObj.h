#pragma once  
 
#include "CPos.h" 
#include "AoiDef.h"
#include "CBaseAoiObj.h"
#include "CQuadMallocObject.h"
#include "TQuadAllocator.h"

namespace sqr
{ 

	class CSynAoiSyncerObj;
	class CSynAoiSynceeObj;

	class CSynAoiScene;
	class CTestSuiteAoi;

	
	class CSynAoiDimObj;

	class CSynAoiViewObj : public CQuadMallocObject
	{ 
		friend class CSynAoiScene; 
		friend class CAoiGrid; 
		friend class CTestSuiteAoi;
		
		friend class CSynAoiDimObj;
		friend class CAoiDestroyObjJob;

	public: 
		bool CanBeSync()const;
		virtual EAoiObjType GetType()const;

		const CSynAoiSyncerObj* CastToSyncerObj()const;
		CSynAoiSyncerObj* CastToSyncerObj();
		const CSynAoiSynceeObj* CastToSynceeObj()const;
		CSynAoiSynceeObj* CastToSynceeObj();

		void SetSize( float fSize, uint32 uDimension = 0 );
		void SetEyeSight( float fEyeSight, uint32 uDimension = 0 );
		void SetStealth( float fStealth, uint32 uDimension = 0 );
		void SetKeenness(float Keenness, uint32 uDimension = 0 );
		void SetViewGroup(uint32 uGroupID, uint32 uDimension = 0 );
		float GetSize(uint32 uDimension = 0) const; 
		float GetEyeSight(uint32 uDimension = 0) const; 
		float GetStealth(uint32 uDimension = 0) const; 
		float GetKeenness(uint32 uDimension = 0) const; 
		uint32 GetViewGroup(uint32 uDimension = 0) const; 
		
		void GetDimensions(vector<uint32>& vecDim) const;

		void SetPos(const CFPos& PixelPos);
		void ForceSetPos(const CFPos& PixelPos);
		const CFPos& GetPos() const;

		bool CanSee(CSynAoiViewObj* pObj, uint32 uDimension = 0);

		void PostMsgToViewee(const void* pContext, uint32 uDimension);
		void PostMsgToViewer(const void* pContext, uint32 uDimension);
		void PostMsgToSelf(const void* pContext);

	protected: 
		CSynAoiViewObj(CSynAoiScene *pScene, const CFPos& pos, uint32 uId, uint32 uCoreObjId); 
		virtual ~CSynAoiViewObj();

		inline CSynAoiScene* GetScene() const
		{
			return m_pScene;
		}

		inline uint32 GetGlobalID() const
		{
			return m_uId;
		}

		inline uint32 GetCoreObjID() const
		{
			return m_uCoreObjId;
		}

		const CFPos& GetPosition(uint32 uDimension = 0) const;

		virtual void OnViewRelationChanged(CSynAoiViewObj* pViewObj,
			const CFPos& MyGridPos,const CFPos& OtGridPos,bool bSee, uint32 uDimension = 0);
		virtual void OnViewRelationKept(CSynAoiViewObj* pOtObj,
			const CFPos& MyOldGridPos,const CFPos& MyGridPos,
			const CFPos& OtOldGridPos,const CFPos& OtGridPos, uint32 uDimension = 0);

		void HandleViewRelationChanged(CSynAoiViewObj* pObj,bool bSee, uint32 uDimension = 0);
		
		void OnMsgFromViewer(const void* pContext);
		void OnMsgFromViewee(const void* pContext);

		void OnMsgToVieweeHandled(const void* pContext);
		void OnMsgToViewerHandled(const void* pContext);
	
		void OnMsgFromSyncer(const void* pContext);
		void OnMsgFromSyncee(const void* pContext);

		void OnMsgToSynceeHandled(const void* pContext);
		void OnMsgToSyncerHandled(const void* pContext);

	protected:
		void InsertToScene();
		void RemoveFrScene();

		CSynAoiDimObj* GetAoiDimObj(uint32 uId) const;

		CSynAoiScene*				m_pScene;

		CFPos						m_Pos;

		uint32						m_uId;
		uint32						m_uCoreObjId;

#ifdef _WIN32
		typedef hash_map<uint32, CSynAoiDimObj*, hash_compare<uint32, less<uint32> >, 
			TQuadAllocator<pair<uint32, CSynAoiDimObj*> > >	MapDimObj_t;
#else
		typedef hash_map<uint32, CSynAoiDimObj*, hash<uint32>, equal_to<uint32>, 
			TQuadAllocator<pair<uint32, CSynAoiDimObj*> > > MapDimObj_t;
#endif

		MapDimObj_t					m_mapAoiDimObj;

	}; 
} 

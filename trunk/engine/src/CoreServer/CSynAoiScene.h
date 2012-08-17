#pragma once 

#include "CPos.h"
#include "TQuadAllocator.h"

namespace sqr
{
	template<class T> struct TRect;
	typedef TRect<int>		CIRect;

	class CTestSuiteAoi;
	class CSynAoiViewObj;
	class CSynAoiSyncerObj;
	class CSynAoiSynceeObj;

	class CSynAoiDimScene;
	class CDimensionInfo;

	struct CAoiShape;

	class CSynAoiScene
		: public CQuadMallocObject
	{
		friend class CSynAoiViewObj;
		friend class CTestSuiteAoi;
	
	public:
		CSynAoiScene( const char* szSyncAoiFileName,uint32 uScale,  
					  uint32 uPixelWidth, uint32 uPixelHeight, CDimensionInfo* DimInfo);
		~CSynAoiScene();

		CSynAoiViewObj* CreateViewObj(const CFPos& pos, uint32 uId, uint32 uCoreObjId);
		CSynAoiSyncerObj* CreateSyncerObj(const CFPos& pos, uint32 uId, uint32 uCoreObjId);
		CSynAoiSynceeObj* CreateSynceeObj(const CFPos& pos, uint32 uId, uint32 uCoreObjId);
		void DestroyObj(CSynAoiViewObj* pObj);

		bool IsInSyncAoi(const CPos& OffsetGridPos)const;
		bool IsInSyncAoi(const CFPos& OffsetGridPos)const;

		void Release();

		bool DimensionExist(uint32 uDimensionId);
	private:
	
		uint32  m_uPixelWidth;
		uint32  m_uPixelHeight;
		uint32	m_uScale;

		CAoiShape*	m_pAoiShape;

#ifdef _WIN32
		typedef hash_map<uint32, CSynAoiDimScene*, hash_compare<uint32, less<uint32> >, 
			TQuadAllocator<pair<uint32, CSynAoiDimScene*> > >	MapDimScene_t;
#else
		typedef hash_map<uint32, CSynAoiDimScene*, hash<uint32>, equal_to<uint32>, 
			TQuadAllocator<pair<uint32, CSynAoiDimScene*> > > MapDimScene_t;
#endif

		MapDimScene_t	m_mapDimScene;

	};
}

#pragma once
#include "TCoreSceneAllocator.h"
#include "IDynamicBarrierInfo.h"
#include "CPos.h"

namespace sqr
{
#define DynamicBarrierUsingHashMap

	class CDynamicBarrierInfo
		: public IDynamicBarrierInfo
		, public CCoreSceneMallocObject
	{

#ifdef DynamicBarrierUsingHashMap
#ifdef _WIN32
		typedef hash_map<uint32,uint8,hash_compare<uint32,less<uint32> >,TCoreSceneAllocator<pair<uint32,uint8> > > DynamicBarrierMatrix;
#else
		typedef hash_map<uint32,uint8,hash<uint32>,equal_to<uint32>,TCoreSceneAllocator<pair<uint32,uint8> > > DynamicBarrierMatrix;
#endif
#else
		typedef vector<uint8, TCoreSceneAllocator<uint8> > DynamicBarrierMatrix;
#endif

	public:
		CDynamicBarrierInfo(uint32 uWidth, uint32 uHeight);
		~CDynamicBarrierInfo();
		void SetBarrier(uint32 uOwnerID, const vector<CPos>& vecDBArea, EBarrierType eBarrierType);
		EBarrierType GetBarrier(const CPos& pos)const;
		EBarrierType GetBarrier(uint32 uIndex)const;

	private:
		typedef map<uint32, uint8, less<uint32>, TCoreSceneAllocator<pair<uint32,uint8> > >		MapGridDBInfo;

		class DynamicBarrierInfo
			:public
#ifdef DynamicBarrierUsingHashMap
#ifdef _WIN32
			hash_map<uint32, MapGridDBInfo,hash_compare<uint32,less<uint32> >,TCoreSceneAllocator<pair<uint32,MapGridDBInfo> > >
#else
			hash_map<uint32, MapGridDBInfo,hash<uint32>,equal_to<uint32>,TCoreSceneAllocator<pair<uint32,MapGridDBInfo> > >
#endif
#else
			vector<MapGridDBInfo, TCoreSceneAllocator<MapGridDBInfo> >
#endif
		{};

		void ClearBarrier(uint32 uOwnerID, const vector<CPos>& vecDBArea);
		void SetBarrierMatrix(uint32 x, uint32 y, uint8 bVal);
		void SetBarrierMatrix(uint32 uIndex, uint8 bVal);
		uint32 GetIndex(uint32 x, uint32 y)const;

		uint32 m_uWidthInGrid;
		uint32 m_uHeightInGrid;

		DynamicBarrierMatrix	m_pBarrierMatrix;
		DynamicBarrierInfo		m_BarrierInfo;

	};

}


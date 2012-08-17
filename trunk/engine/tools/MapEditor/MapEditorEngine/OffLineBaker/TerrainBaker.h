#pragma once
#include "lightmapBaker.h"


namespace sqr
{
	SmartPointer(CTerrainBaker);
	class RegionShadowMapBaker;
	class CTerrainBaker :public CLightMapBaker
	{
	public:
		CTerrainBaker(void);
		~CTerrainBaker(void);
		void Setup();
		void Discard();
		void TilePrepare(index_t x,index_t z);
		void OnProcess(index_t x,index_t z);
		void RenderReceive();

		vector<int> tileIndexList;

	protected:
		RegionShadowMapBaker* m_RegionBakeHelp;
	};
//!namespace
}

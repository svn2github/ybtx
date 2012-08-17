#pragma once
#include "CTerrain.h"

namespace sqr_tools
{
	class CEditTerrainMgr;
	class CEditTerrainNode : public TerrainNode
	{
		friend class CEditTerrainMgr;
	public:
		CEditTerrainNode(CEditTerrainMgr* pParent , URegionID id);
		~CEditTerrainNode();

		void CreateBuffer();
		void DestroyBuffer();

	public:
		vector<bool>		m_TerrainTileVisibles;
	};
}
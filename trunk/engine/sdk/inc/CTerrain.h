#pragma once
#include "CRenderOperation.h"
#include "CAxisAlignedBox.h"
#include "CElementManager.h"

namespace sqr
{
	class ITexture;
	class CIndexHardwareBuffer;
	class CVertexHardwareBuffer;
	class CTerrainTile : public CSceneMallocObject 
	{
	public:
		CTerrainTile();
		~CTerrainTile();
	public:
		CRenderOperation	op;
		CAxisAlignedBox		aab;
	};

	class TerrainNode : public CElementNode
	{
	public:
		TerrainNode(CElementManager* pParent , URegionID id);
		~TerrainNode();

	public:
		typedef SVector<CTerrainTile> TilePool;
		TilePool			m_TerrainTiles;
		CAxisAlignedBox		m_TAAB;
		ITexture*			text;
	};
}
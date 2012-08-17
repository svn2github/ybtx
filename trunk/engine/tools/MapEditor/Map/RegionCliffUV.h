#pragma once
#include "../IO/VersionMapChunk.h"

namespace bly
{
class CTerrainMesh;
}

namespace Map
{

/// 悬崖贴图的UV.
class RegionCliffUV : public IO::DataChunk
{
	DeclareChunk(RegionCliffUV);
public:
	RegionCliffUV();
	virtual ~RegionCliffUV();

	/// 写入流
	void Write();
	/// 从流中读取
	void Read();

	/// 加载数据.
	/// @param	CTerrainMesh * terrain
	/// @param	uint index	Region索引
	/// @return	void
	void LoadFromTerrain(CTerrainMesh* terrain, uint index);

	enum
	{
		GRID_VERTEX_NUM = 4,
		REGION_WIDTH_IN_GRID = 16,
		REGION_GRID_NUM = REGION_WIDTH_IN_GRID * REGION_WIDTH_IN_GRID,
		REGION_UV_NUM = REGION_GRID_NUM * GRID_VERTEX_NUM
	};

private:
	D3DXVECTOR2 cliffuv[REGION_UV_NUM];
};

}// namespace Map

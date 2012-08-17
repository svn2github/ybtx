#pragma once
#include "../IO/VersionMapChunk.h"

namespace bly
{
class CTerrainMesh;
}

namespace Map
{

/// 场景信息.
class RegionVertexHeight : public IO::DataChunk
{
	DeclareChunk(RegionVertexHeight);
public:
	RegionVertexHeight();
	virtual ~RegionVertexHeight();

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
		REGION_WIDTH_IN_GRID = 16,
		REGION_WIDTH_IN_VERTEX = REGION_WIDTH_IN_GRID + 1,
		REGION_VERTEX_NUM = REGION_WIDTH_IN_VERTEX*REGION_WIDTH_IN_VERTEX
	};

private:
	short height[REGION_VERTEX_NUM];
};

}// namespace Map

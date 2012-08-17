#pragma once
#include "../IO/VersionMapChunk.h"

namespace bly
{
class CTerrainMesh;
}

namespace Map
{

/// 阻挡信息.
class RegionBlockInfo : public IO::DataChunk
{
	DeclareChunk(RegionBlockInfo);
public:
	RegionBlockInfo();
	virtual ~RegionBlockInfo();

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
		REGION_GRID_NUM = REGION_WIDTH_IN_GRID*REGION_WIDTH_IN_GRID
	};
private:
	byte block[REGION_GRID_NUM];
};

}// namespace Map

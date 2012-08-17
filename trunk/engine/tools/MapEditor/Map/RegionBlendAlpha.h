#pragma once
#include "../IO/VersionMapChunk.h"

namespace bly
{
class CTerrainMesh;
}

namespace Map
{

/// 纹理混合权重.
class RegionBlendAlpha : public IO::DataChunk
{
	DeclareChunk(RegionBlendAlpha);
public:
	RegionBlendAlpha();
	virtual ~RegionBlendAlpha();

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
	byte alpha1[REGION_VERTEX_NUM];	///< 第二层权重值
	byte alpha2[REGION_VERTEX_NUM];	///< 第三层权重值
};

}// namespace Map

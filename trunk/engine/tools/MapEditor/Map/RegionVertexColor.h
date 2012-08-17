#pragma once
#include "../IO/VersionMapChunk.h"

namespace bly
{
class CTerrainMesh;
}

namespace Map
{

/// 顶点色.
class RegionVertexColor : public IO::DataChunk
{
	DeclareChunk(RegionVertexColor);
public:
	RegionVertexColor();
	virtual ~RegionVertexColor();

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
	/// 32位色转16位色.
	static uint16 X888To565(uint32 c);

private:
	uint16 color[REGION_VERTEX_NUM];
};

}// namespace Map

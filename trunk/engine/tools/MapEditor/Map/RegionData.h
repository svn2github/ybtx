#pragma once
#include "../IO/VersionMapChunk.h"

namespace bly
{
class CTerrainMesh;
}

namespace Map
{
class RegionHeader;
class RegionVertexHeight;
class RegionVertexColor;
class RegionBlendAlpha;
class RegionCliffUV;
class RegionLogicalHeight;
class RegionWater;
class RegionLightInfo;
class RegionBlockInfo;

/// Region数据段.
class RegionData : public IO::DataChunk
{
	DeclareChunk(RegionData);
public:
	RegionData();
	virtual ~RegionData();

	/// 写入流
	void Write();
	/// 从流中读取
	void Read();

	/// 加载数据.
	/// @param	CTerrainMesh * terrain
	/// @param	uint index	Region索引
	/// @return	void
	void LoadFromTerrain(CTerrainMesh* terrain, uint index);

private:
	RegionHeader*		header;
	RegionVertexHeight* height;
	RegionVertexColor*	color;
	RegionBlendAlpha*	alpha;
	RegionCliffUV*		cliffuv;
	RegionLogicalHeight* logicH;
	RegionWater*		water;
	RegionLightInfo*	light;
	RegionBlockInfo*	block;

	std::vector<DataChunk*> chunkList;
};

}// namespace Map

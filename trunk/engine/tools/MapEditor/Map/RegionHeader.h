#pragma once
#include "../IO/VersionMapChunk.h"

namespace bly
{
class CTerrainMesh;
}

namespace Map
{

/// 区域文件头.
class RegionHeader : public IO::DataChunk
{
	DeclareChunk(RegionHeader);
public:
	RegionHeader();
	virtual ~RegionHeader();

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
	uint flag;	///< reserved
	uint16 x;	///< Region列索引
	uint16 z;	///< Region行索引
};

}// namespace Map

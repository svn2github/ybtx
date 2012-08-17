#pragma once
#include "../IO/VersionMapChunk.h"
namespace bly
{
class CTerrainMesh;
}
namespace Map
{

/// 地图文件头.
class MapHeader : public IO::DataChunk
{
	DeclareChunk(MapHeader);
public:
	/// 构造函数初始化数据
	MapHeader();
	/// 析构函数
	virtual ~MapHeader();

	/// 写入流
	void Write();
	/// 从流中读取
	void Read();

	/// 加载
	void LoadFromTerrainMesh(CTerrainMesh* terrain);

private:
	uint flag;
	uint16 width;	///< 地图宽(Region单位)
	uint16 height;	///< 地图高(Region单位)
};

}// namespace Map

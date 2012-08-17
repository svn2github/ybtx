#pragma once
#include "../IO/VersionMapChunk.h"

namespace bly
{
class CTerrainMesh;
}

namespace Map
{

/// 区域环境光.
class RegionLightInfo : public IO::DataChunk
{
	DeclareChunk(RegionLightInfo);
public:
	RegionLightInfo();
	virtual ~RegionLightInfo();

	/// 写入流
	void Write();
	/// 从流中读取
	void Read();

private:

};

}// namespace Map

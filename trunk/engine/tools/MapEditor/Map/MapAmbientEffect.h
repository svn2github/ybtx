#pragma once
#include "../IO/VersionMapChunk.h"

namespace bly
{
class CTerrainMesh;
}

namespace Map
{

/// 环境特效.
class MapAmbientEffect : public IO::DataChunk
{
	DeclareChunk(MapAmbientEffect);
public:
	MapAmbientEffect();
	virtual ~MapAmbientEffect();

	/// 写入流
	void Write();
	/// 从流中读取
	void Read();

private:

};

}// namespace Map

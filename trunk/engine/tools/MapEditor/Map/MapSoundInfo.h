#pragma once
#include "../IO/VersionMapChunk.h"

namespace bly
{
class COperator;
}

namespace Map
{

/// 音乐信息.
class MapSoundInfo : public IO::DataChunk
{
	DeclareChunk(MapSoundInfo);
public:
	MapSoundInfo();
	virtual ~MapSoundInfo();

	/// 写入流
	void Write();
	/// 从流中读取
	void Read();

	/// 加载
	void LoadFromOperator(COperator* o);

private:
	std::string backMusic;
	int interval;
};

}// namespace Map

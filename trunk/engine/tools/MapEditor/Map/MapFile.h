#pragma once
#include "../IO/BinFileBase.h"

namespace IO
{
class VersionMapChunk;
}

namespace Map
{
class MapHeader;
class MapSceneInfo;
class MapSoundInfo;
class MapAmbientEffect;
class MapHDRInfo;

/// 地图文件.
class MapFile : public IO::BinFileBase
{
public:
	MapFile();
	virtual ~MapFile();

private:
	/// 初始化响应
	void OnInit();

private:
	IO::VersionMapChunk* versions;
	MapHeader* mapHeader;
	MapSceneInfo* sceneInfo;
	MapSoundInfo* soundInfo;
	MapAmbientEffect* ambientFx;
	MapHDRInfo* hdrInfo;
};

}// namespace Map


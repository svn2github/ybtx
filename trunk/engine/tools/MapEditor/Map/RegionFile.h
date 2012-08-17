#pragma once
#include "../IO/BinFileBase.h"

namespace Map
{
class RegionIndex;
class RegionData;

/// 地图文件.
class RegionFile : public IO::BinFileBase
{
public:
	RegionFile();
	virtual ~RegionFile();

	/// 从流中加载文件
	void Load();
	/// 保存到流
	void Save();

private:
	/// 初始化响应
	void OnInit();

private:
	RegionIndex* indices;
	std::vector<RegionData*> regionList;
};

}// namespace Map


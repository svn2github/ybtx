#pragma once
#include "../IO/VersionMapChunk.h"

namespace Map
{

/// 区域文件索引.
/// 保存每个区域在文件中的偏移值
class RegionIndex : public IO::DataChunk
{
	DeclareChunk(RegionIndex);
public:
	RegionIndex();
	virtual ~RegionIndex();

	/// 写入流
	void Write();
	/// 从流中读取
	void Read();

	/// 初始化region数目
	void Resize(uint regionNum);

	/// 设置Region的文件偏移.
	/// @param	uint index	region索引
	/// @param	uint offset	文件偏移
	/// @return	void
	void SetOffset(uint index, uint offset);

private:
	std::vector<uint> indexList;
};

}// namespace Map

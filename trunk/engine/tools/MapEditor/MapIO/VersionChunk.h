#pragma once
#include "DataChunk.h"

namespace IO
{

/// 版本号.
class VersionChunk : public DataChunk
{
public:
	/// 构造函数
	VersionChunk();
	/// 析构函数
	virtual ~VersionChunk();
	/// 写入流
	void Write();
	/// 从流中读取
	void Read();
	/// 设置版本号
	void SetVersion(uint v);
	/// 获得版本号
	uint GetVersion() const;

private:
	uint version;
};

}// namespace IO


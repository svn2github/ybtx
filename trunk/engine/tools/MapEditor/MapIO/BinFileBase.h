#pragma once
#include "DataChunk.h"

namespace IO
{
SmartPointer(Stream);
SmartPointer(BinarySerializer);
class VersionMapChunk;
typedef std::vector<DataChunk*> ChunkList;

/// 二进制文件基类.
/// 管理了文件的版本号和各Chunk的版本号
class BinFileBase
{
public:
	BinFileBase(uint version);
	virtual ~BinFileBase();

	/// 打开流
	bool Open(const StreamPtr& stream);
	/// 关闭流
	void Close();

	uint GetVersion() const;

	/// 从流中加载文件
	virtual void Load();
	/// 保存到流
	virtual void Save();

	/// 添加一个Chunk
	void AddChunk(DataChunk* chunk);
	/// 查找指定Chunk
	DataChunk* FindChunk(const Util::FourCC& fourcc);

protected:
	/// 打开响应
	virtual void OnOpen();
	/// 关闭响应
	virtual void OnClose();
	/// 初始化响应
	virtual void OnInit();

protected:
	ChunkList chunkList;
	BinarySerializerPtr serializer;
};

}// namespace IO


#pragma once
#include "../Util/FourCC.h"
#include "TRefPointer.h"

namespace IO
{
SmartPointer(BinarySerializer);

/// 数据块.
class DataChunk
{
public:
	/// 构造函数
	DataChunk(const Util::FourCC& id);
	/// 析构函数
	virtual ~DataChunk();
	/// 取得Chunk id
	const Util::FourCC& GetID() const;
	/// Chunk数据长度(不包括FourCC和size本身)
	uint Size();
	/// 设置序列化器
	void SetSerializer(const BinarySerializerPtr& s);
	/// 返回序列化器
	const BinarySerializerPtr& GetSerializer() const;

	/// 写入流
	virtual void Write();
	/// 从流中读取
	/// @note 不读取FourCC
	virtual void Read();

protected:
	Util::FourCC fourcc;
	uint size;
	BinarySerializerPtr serializer;
};

}// namespace IO

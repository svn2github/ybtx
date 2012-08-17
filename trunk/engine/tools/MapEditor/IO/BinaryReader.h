#pragma once
#include "Serializer.h"

namespace sqr
{

SmartPointer(BinaryReader);

/// 二进制序列化.
class BinaryReader : public Serializer
{
public:
	/// 8位字符
	char ReadChar();
	/// 8位无符号字符
	byte ReadByte();
	/// 16位短整数
	int16 ReadInt16();
	/// 16位无符号短整数
	uint16 ReadUInt16();
	/// 32位整数
	int32 ReadInt32();
	/// 32位无符号整数
	uint32 ReadUInt32();
	/// 单精度浮点数
	float ReadFloat();
	/// 布尔值
	bool ReadBool();
	/// 字符串
	string ReadString();
	/// 数据段
	void ReadData(void* data, size_t size);

	/// 跳过数据段
	void AbortData(size_t size);
};

}// namespace sqr

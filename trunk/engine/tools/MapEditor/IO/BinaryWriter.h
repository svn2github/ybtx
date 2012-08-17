#pragma once
#include "Serializer.h"

namespace sqr
{

SmartPointer(BinaryWriter);

/// 二进制序列化.
class BinaryWriter : public Serializer
{
public:
	/// 8位字符
	void WriteChar(char c);
	/// 8位无符号字符
	void WriteByte(byte c);
	/// 16位短整数
	void WriteInt16(int16 s);
	/// 16位无符号短整数
	void WriteUInt16(uint16 s);
	/// 32位整数
	void WriteInt32(int32 i);
	/// 32位无符号整数
	void WriteUInt32(uint32 i);
	/// 单精度浮点数
	void WriteFloat(float f);
	/// 布尔值
	void WriteBool(bool b);
	/// 字符串
	void WriteString(const string& s);
	/// 数据段
	void WriteData(void* data, size_t size);
};

}// namespace sqr

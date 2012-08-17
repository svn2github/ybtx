#pragma once
#include "Stream.h"

namespace sqr
{
SmartPointer(FileStream);

/// 文件流
class FileStream :  public Stream
{
public:
	/// 构造函数
	FileStream();
	/// 析构函数
	virtual ~FileStream();
	/// 是否可读
	bool CanRead() const;
	/// 是否可写
	bool CanWrite() const;
	/// 是否可以查找
	bool CanSeek() const;
	/// 返回当前位置
	index_t GetPosition() const;
	/// 返回流长度
	size_t GetSize() const;
	/// 打开流
	bool Open();
	/// 关闭流
	void Close();
	/// 读取一定字节的数据
	size_t Read(void* data, size_t size);
	/// 写入一定字节的数据
	void Write(void* data, size_t size);
	/// 寻找到指定位置
	void Seek(int offset, SeekOrigin dir);
	/// 是否已经到达流末尾
	bool Eof() const;
	/// 映射到内存
	void* Map();
	/// 取消映射
	void UnMap();

private:
	HANDLE handle;
	void* mappedData;
};


//------------------------------------------------------------------------------
inline bool
FileStream::CanRead() const
{
	return (this->mode != WriteAccess);
}

//------------------------------------------------------------------------------
inline bool
FileStream::CanWrite() const
{
	return (this->mode != ReadAccess);
}

//------------------------------------------------------------------------------
inline bool
FileStream::CanSeek() const
{
	return true;
}

}// namespace sqr

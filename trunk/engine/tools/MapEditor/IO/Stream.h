#pragma once
#include "CRefObject.h"

namespace sqr
{

SmartPointer(Stream);

/// 流基类.
/// 暂时只提供一些基本的接口
/// 可以派生文件流, 内存流, 文件包流等
class Stream : public CRefObject
{
public:
	/// 访问模式.
	enum AccessMode
	{
		ReadAccess,		///< 可读
		WriteAccess,	///< 可写
		ReadWriteAccess	///< 可读可写
	};
	/// Seek时的相对位置.
	enum SeekOrigin
	{
		Begin,		///< 流的开始位置
		Current,	///< 当前位置
		End			///< 流末尾
	};
public:
	/// 构造函数
	Stream();
	/// 析构函数
	virtual ~Stream();
	/// 设置资源位置
	void SetPath(const string& u);
	/// 返回资源位置
	const string& GetPath() const;
	/// 设置访问模式
	void SetAccessMode(AccessMode m);
	/// 返回访问模式
	AccessMode GetAccessMode() const;
	/// 流是否已经打开
	bool IsOpen() const;

	/// 打开流
	virtual bool Open();
	/// 关闭流
	virtual void Close();
	/// 是否可读
	virtual bool CanRead() const;
	/// 是否可写
	virtual bool CanWrite() const;
	/// 是否可以寻找
	virtual bool CanSeek() const;
	/// 返回当前位置
	virtual index_t GetPosition() const;
	/// 返回流长度
	virtual size_t GetSize() const;
	/// 读取一定字节的数据
	virtual size_t Read(void* data, size_t size);
	/// 写入一定字节的数据
	virtual void Write(void* data, size_t size);
	/// 寻找到指定位置
	virtual void Seek(int offset, SeekOrigin dir);
	/// 是否已经到达流末尾
	virtual bool Eof() const;
	/// 映射到内存
	virtual void* Map();
	/// 取消映射
	virtual void UnMap();
	/// 是否已经映射
	bool IsMapped() const;

protected:
	string path;			///< 资源位置
	bool isOpen;		///< 是否打开
	bool isMapped;
	AccessMode mode;	///< 访问模式
	//@todo 线程安全相关
};

//------------------------------------------------------------------------------
inline bool
Stream::IsOpen() const
{
	return this->isOpen;
}

//------------------------------------------------------------------------------
inline bool
Stream::IsMapped() const
{
	return this->isMapped;
}

//------------------------------------------------------------------------------
inline void
Stream::SetPath( const string& u )
{
	Ast(!this->IsOpen());
	this->path = u;
}

//------------------------------------------------------------------------------
inline const string&
Stream::GetPath() const
{
	return this->path;
}

//------------------------------------------------------------------------------
inline void
Stream::SetAccessMode( AccessMode m )
{
	this->mode = m;
}

//------------------------------------------------------------------------------
inline Stream::AccessMode
Stream::GetAccessMode() const
{
	return this->mode;
}

}// namespace sqr


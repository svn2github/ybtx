#pragma once
#include "CRefObject.h"
#include "Stream.h"

namespace sqr
{

SmartPointer(Serializer);

/// 序列化接口.
/// 可以实现二进制序列化, 文本序列化, XML序列化, 等等
/// @todo 映射到内存Map()/UnMap()
class Serializer : public CRefObject
{
public:
	/// 构造函数
	Serializer();
	/// 析构函数
	virtual ~Serializer();
	/// 设置序列化相关的流
	void SetStream(const StreamPtr& s);
	/// 返回相关联的流
	const StreamPtr& GetStream() const;
	/// 是否设置了流
	bool HasStream();
	/// 流是否已经到达末尾
	bool Eof();
	/// 打开流开始序列化
	virtual bool Open();
	/// 结束序列化, 关闭流
	virtual void Close();
	/// 是否已经开始序列化
	bool IsOpen();

protected:
	StreamPtr stream;
	bool isOpen;
};

//------------------------------------------------------------------------------
inline void
Serializer::SetStream( const StreamPtr& s )
{
	Ast(s.isvalid());
	this->stream = s;
}

//------------------------------------------------------------------------------
inline const StreamPtr&
Serializer::GetStream() const
{
	return this->stream;
}

//------------------------------------------------------------------------------
inline bool
Serializer::HasStream()
{
	return this->stream.isvalid();
}

//------------------------------------------------------------------------------
inline bool
Serializer::Eof()
{
	Ast(this->HasStream());
	return this->stream->Eof();
}

//------------------------------------------------------------------------------
inline bool
Serializer::IsOpen()
{
	return this->isOpen;
}

}// namespace sqr


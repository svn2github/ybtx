#include "StdAfx.h"
#include "Stream.h"

namespace sqr
{

//------------------------------------------------------------------------------
Stream::Stream()
		: isOpen(false)
		, isMapped(false)
		, mode(ReadAccess)
{
	// empty
}

//------------------------------------------------------------------------------
Stream::~Stream()
{
	Ast(!this->IsOpen());
}

//------------------------------------------------------------------------------
bool Stream::CanRead() const
{
	return false;
}

//------------------------------------------------------------------------------
bool Stream::CanWrite() const
{
	return false;
}

//------------------------------------------------------------------------------
bool Stream::CanSeek() const
{
	return false;
}

//------------------------------------------------------------------------------
index_t Stream::GetPosition() const
{
	Ast(this->IsOpen());
	return 0;
}

//------------------------------------------------------------------------------
bool Stream::Open()
{
	Ast(!this->IsOpen());
	this->isOpen = true;
	return true;
}

//------------------------------------------------------------------------------
void Stream::Close()
{
	Ast(this->IsOpen());
	if (this->IsMapped())
	{
		this->UnMap();
	}
	this->isOpen = false;
}

//------------------------------------------------------------------------------
size_t Stream::Read( void* data, size_t size )
{
	Ast(this->IsOpen() && NULL != data);
	Ast(this->CanRead());
	return 0;
}

//------------------------------------------------------------------------------
void Stream::Write( void* data, size_t size )
{
	Ast(this->IsOpen() && NULL != data);
	Ast(this->CanWrite());
}

//------------------------------------------------------------------------------
void Stream::Seek( int offset, SeekOrigin dir )
{
	Ast(this->IsOpen());
	Ast(this->CanSeek());
}

//------------------------------------------------------------------------------
bool Stream::Eof() const
{
	Ast(this->IsOpen());
	return true;
}

//------------------------------------------------------------------------------
size_t
Stream::GetSize() const
{
	Ast(this->IsOpen());
	return 0;
}

//------------------------------------------------------------------------------
void*
Stream::Map()
{
	Ast(this->IsOpen());
	Ast(!this->IsMapped());
	this->isMapped = true;
	return NULL;
}

//------------------------------------------------------------------------------
void
Stream::UnMap()
{
	Ast(this->IsOpen());
	Ast(this->IsMapped());
	this->isMapped = false;
}
}// namespace sqr

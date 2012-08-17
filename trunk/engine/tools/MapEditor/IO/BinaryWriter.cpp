#include "StdAfx.h"
#include "BinaryWriter.h"

namespace sqr
{

//------------------------------------------------------------------------------
void BinaryWriter::WriteChar( char c )
{
	this->stream->Write(&c, sizeof(c));
}

//------------------------------------------------------------------------------
void BinaryWriter::WriteByte( byte c )
{
	this->stream->Write(&c, sizeof(c));
}

//------------------------------------------------------------------------------
void BinaryWriter::WriteInt16( int16 s )
{
	this->stream->Write(&s, sizeof(s));
}

//------------------------------------------------------------------------------
void BinaryWriter::WriteUInt16( uint16 s )
{
	this->stream->Write(&s, sizeof(s));
}

//------------------------------------------------------------------------------
void BinaryWriter::WriteInt32( int32 i )
{
	this->stream->Write(&i, sizeof(i));
}

//------------------------------------------------------------------------------
void BinaryWriter::WriteUInt32( uint32 i )
{
	this->stream->Write(&i, sizeof(i));
}

//------------------------------------------------------------------------------
void BinaryWriter::WriteFloat( float f )
{
	this->stream->Write(&f, sizeof(f));
}

//------------------------------------------------------------------------------
void BinaryWriter::WriteBool( bool b )
{
	this->stream->Write(&b, sizeof(b));
}

//------------------------------------------------------------------------------
void BinaryWriter::WriteString( const string& s )
{
	Ast(s.length() < USHRT_MAX);
	this->WriteUInt16(uint16(s.length()));
	if (s.length() > 0)
	{
		this->stream->Write((void*)s.c_str(), s.length());
	}
}

//------------------------------------------------------------------------------
void BinaryWriter::WriteData( void* data, size_t size )
{
	Ast(NULL != data);
	this->stream->Write(data, size);
}

}// namespace sqr

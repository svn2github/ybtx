#include "stdafx.h"
#include "CDataChunk.h"
#include "MemoryHelper.h"

CDataChunk::CDataChunk()
	:m_pBuffer(NULL),m_pPos(NULL),m_pEnd(NULL)
{}

CDataChunk::CDataChunk( void *pData,size_t fSize )
{
	m_pBuffer = reinterpret_cast< char* >( pData );
	m_pEnd = m_pBuffer + fSize;
	m_pPos = m_pBuffer;
	m_Header.stSize = fSize;
	m_Header.dwName = 0;
}

CDataChunk::~CDataChunk()
{}

//char*  CDataChunk::allocate( size_t size,const char*  ptr )
//{
//	if( m_pBuffer )
//		ChunkDel(m_pBuffer);
//
//	m_pBuffer = (char*)SqrChunkNew(size);
//	m_Header.stSize = size;
//	m_pPos = m_pBuffer;
//	m_pEnd = m_pBuffer + size;
//
//	if( ptr )
//		memcpy( m_pBuffer, ptr, size );
//
//	return m_pBuffer;
//}

char*  CDataChunk::SetBuffer( char* ptr,size_t size )
{
	//if( m_pBuffer )
	//	ChunkDel(m_pBuffer);

	m_pBuffer = ptr;
	m_Header.stSize = size;
	m_pPos = m_pBuffer;
	m_pEnd = m_pBuffer + size;

	return m_pBuffer;
}

//void CDataChunk::free()
//{
//	if ( m_pBuffer )
//	{
//		ChunkDel(m_pBuffer);
//		m_pBuffer = 0;
//		m_Header.stSize = 0;
//	}
//	return;
//}

char* CDataChunk::GetBuffer( void )
{
	return m_pBuffer;
}

const char* CDataChunk::GetBuffer( void ) const
{
	return m_pBuffer;
}

size_t CDataChunk::GetSize( void ) const
{
	return m_Header.stSize;
}

FourCC CDataChunk::GetName( void ) const
{
	return m_Header.dwName;
}

DWORD CDataChunk::SetName( FourCC fName )
{
	m_Header.dwName = fName;
	return m_Header.dwName.AsDWORD();
}

DWORD CDataChunk::SetName( const std::string& s )
{
	m_Header.dwName.FromString(s);
	return m_Header.dwName.AsDWORD();
}

CDataChunk::ChunkHdr&	CDataChunk::GetHdr(void) const
{ return m_Header; }

uint16		CDataChunk::GetVersion(void) const
{ return m_Header.ver; }

void		CDataChunk::SetVersion(uint16 ver)
{ m_Header.ver = ver; }

void CDataChunk::SetType(uint16 type)
{
	m_Header.type = type;
}

uint16 CDataChunk::GetType() const
{
	return m_Header.type;
}

CDataChunk& CDataChunk::operator = ( CDataChunk& fData )
{
	if( fData.GetBuffer() != m_pBuffer )
	{
		m_pBuffer = fData.GetBuffer();
		m_Header.dwName = fData.GetName();
		m_Header.stSize = fData.GetSize();
		m_Header.ver	= fData.GetVersion();
		m_pPos = m_pBuffer;
		m_pEnd = m_pBuffer + m_Header.stSize;
	}
	return *this;
}

#include "stdafx.h"
#include "CAsynPipeInBuffer.h"
#include "TSqrAllocator.inl"
#include "ExpHelper.h"

CAsynPipeInBuffer::CAsynPipeInBuffer(size_t stBufferSize)
:m_stMaxBufferSize( stBufferSize )
{
	m_sBuffer.reserve(128);
}

void CAsynPipeInBuffer::InBufferClear()
{
	m_sBuffer.clear();
}


void CAsynPipeInBuffer::InBufferPushData(const char* pData, size_t stSize)
{
	m_sBuffer.append( pData,stSize );
}

bool CAsynPipeInBuffer::IsFull()const
{
	return m_sBuffer.size() > m_stMaxBufferSize;
}


char* CAsynPipeInBuffer::InBufferGetData()const
{
	return const_cast<char*>( m_sBuffer.data() );
}

size_t CAsynPipeInBuffer::InBufferGetDataSize()const
{
	return m_sBuffer.size();
}

void CAsynPipeInBuffer::InBufferPopData(size_t stPopDataSize)
{
	Ast( stPopDataSize <= m_sBuffer.size() );
	m_sBuffer.erase( 0, stPopDataSize );

	if( !m_sBuffer.empty() )
		return;

	if( m_sBuffer.capacity() <= m_stMaxBufferSize )
		return;

	String_t sEmpty;
	m_sBuffer.swap(sEmpty);
}


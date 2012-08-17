#pragma once
#include "CMemData.h"
#include "CSqrAllocPool.h"
#include "ExpHelper.h"

inline CMemData::CMemData(CSqrAllocPool* pPool)
:m_pPool(pPool)
{
	Ast( pPool );	
	m_stSize = 0;
	m_stMemSize = 0;
	m_pBuffer = NULL;
}

inline CMemData::CMemData(const CMemData& Buffer)
:m_pPool(Buffer.m_pPool)
{
	m_stSize = 0;
	m_stMemSize = 0;
	m_pBuffer = NULL;
	Assign( Buffer.Data(),Buffer.Size() );
}

inline CMemData::~CMemData(void)
{
	m_pPool->Dealloc( m_pBuffer );
}

inline size_t CMemData::Size()const
{
	return m_stSize;
}

inline bool CMemData::Empty()const
{
	return Size()==0;
}

inline void CMemData::Reserve(size_t stSize)
{
	if( m_stMemSize>=stSize )
		return;

	m_pBuffer= m_pPool->Realloc( m_pBuffer, stSize );
	m_stMemSize = stSize;
}

inline size_t CMemData::Capacity()const
{
	return m_stMemSize;
}

inline void CMemData::ReSize(size_t stSize)
{
	Reserve( stSize );
	m_stSize=stSize;
}

inline char* CMemData::Data()
{
	return reinterpret_cast<char*>(m_pBuffer);
}

inline const char* CMemData::Data()const
{
	return const_cast<CMemData*>(this)->Data();
}


inline void CMemData::Assign( const void* pData , size_t stSize )
{
	Reserve( stSize );

	memcpy( m_pBuffer, pData , stSize );
	m_stSize = stSize;
}

inline void CMemData::Assign( size_t stOffset, const void* pData , size_t stSize )
{
	Reserve( stSize + stOffset );

	memcpy( Data()+stOffset, pData , stSize );
	m_stSize = max(stSize+stOffset,m_stSize);
}

inline char* CMemData::AppendPos()
{
	return Data()+Size();
}

inline void CMemData::Append( const void* pData , size_t stSize )
{
	Reserve( stSize+ m_stMemSize );

	memcpy( Data()+m_stSize, pData , stSize );
	m_stSize += stSize;
}

inline void CMemData::Erase( size_t stOffset, size_t stSize )
{
	if( stOffset >= m_stSize )
		return;

	const size_t stDataToMove = stOffset + stSize;

	if( stDataToMove >= m_stSize )
	{
		m_stSize = stOffset;
		return;
	}

	memmove( Data()+stOffset ,Data()+stDataToMove, m_stSize - stDataToMove );
	m_stSize -= stSize;
}

inline void CMemData::Trim()
{
	if( m_stMemSize <= m_stSize )
		return;

	m_pBuffer=m_pPool->Realloc( m_pBuffer, m_stSize );

	m_stMemSize = m_stSize;
}

template<typename DataType>
inline DataType* CMemData::AsPtr(size_t stOffset)
{
	Ast( stOffset + sizeof( DataType ) <= m_stSize );

	return reinterpret_cast< DataType* > ( Data() + stOffset );
}

template<typename DataType>
inline const DataType* CMemData::AsPtr(size_t stOffset)const
{
	return const_cast<CMemData*>(this)->AsPtr<DataType>( stOffset );
}

template<typename DataType>
inline DataType& CMemData::Value(size_t stDataTypeOffset)
{
	Ast( (stDataTypeOffset+1) * sizeof(DataType) <= m_stSize );

	return *AsPtr<DataType>( stDataTypeOffset * sizeof(DataType) );
}

template<typename DataType>
inline const DataType& CMemData::Value(size_t stDataTypeOffset)const
{
	return const_cast<CMemData*>(this)->Value<DataType>( stDataTypeOffset );
}

void CMemData::Clear()
{
	m_pPool->Dealloc( m_pBuffer );
	m_pBuffer = NULL;
	m_stSize = 0;
	m_stMemSize = 0;
}

#include "stdafx.h"
#include "CSqrAllocPool.h"
#include "CSqrAllocPoolMgr.h"
#include "ThreadHelper.inl"
#include "CThreadAllocPool.h"
#include "CMemoryCookie.h"

CSqrAllocPool::CSqrAllocPool(const char* szPoolName)
{
	m_nMemUsage = 0;
	m_bFindLeakage=false;
	
	strncpy(m_szPoolName, szPoolName, sizeof(m_szPoolName) - 1);
	m_szPoolName[sizeof(m_szPoolName) - 1] = '\0';

	TLS_CreateKey( &m_Key );
	CreateLock( &m_lkDeqPool );
	
	CSqrAllocPoolMgr::Inst().AddPool(this);	
}


CSqrAllocPool::~CSqrAllocPool()
{
	CSqrAllocPoolMgr::Inst().DelPool(this);

	Lock( &m_lkDeqPool );

	while( !m_deqPool.empty() )
	{
		delete m_deqPool.front();
		m_deqPool.pop_front();
	}
	
	Unlock( &m_lkDeqPool );

	DestroyLock( &m_lkDeqPool );
	TLS_DestroyKey( m_Key );
}

CThreadAllocPool* CSqrAllocPool::GetPool()
{
	CThreadAllocPool* pPool=static_cast<CThreadAllocPool*>( TLS_GetValue(m_Key) );

	if( !pPool )
	{
		pPool = new CThreadAllocPool( this );
		TLS_SetValue( m_Key, pPool );
		
		Lock( &m_lkDeqPool );
		m_deqPool.push_back( pPool );
		Unlock( &m_lkDeqPool );
	}
	return pPool;
}

void CSqrAllocPool::FreeFromMemThread( void* pMem )
{
	GetPool()->FreeFromMemThread( pMem );
}

const char* CSqrAllocPool::GetPoolName()const
{
	return m_szPoolName;
}

bool CSqrAllocPool::CheckLeakageEnabled()const
{
	return true;
}

void CSqrAllocPool::EnableFindLeakage()
{
	m_bFindLeakage = true;
}

void* CSqrAllocPool::Alloc(size_t stSize)
{
	return GetPool()->Alloc( stSize );
}


void CSqrAllocPool::Dealloc(void* pMem)
{
	return GetPool()->Dealloc( pMem );
}

void* CSqrAllocPool::Realloc(void* pMem,size_t stNewSize)
{
	return GetPool()->Realloc( pMem, stNewSize );
}


size_t CSqrAllocPool::GetMemSize(void* pMem)
{
	return CThreadAllocPool::GetMemSize( pMem );
}

size_t CSqrAllocPool::GetMemUsage() const
{
	size_t stMemUsage = 0;
	
	Lock( &m_lkDeqPool );

	PoolDeq_t::const_iterator it = m_deqPool.begin();
	PoolDeq_t::const_iterator itEnd = m_deqPool.end();

	for(; it != itEnd; ++it )
		stMemUsage+=(*it)->GetMemUsage();
	
	Unlock( &m_lkDeqPool );

	return stMemUsage;
}

size_t CSqrAllocPool::GetCapacity() const
{
	size_t stCapacity = 0;
	
	Lock( &m_lkDeqPool );

	PoolDeq_t::const_iterator it = m_deqPool.begin();
	PoolDeq_t::const_iterator itEnd = m_deqPool.end();

	for(; it != itEnd; ++it )
		stCapacity+=(*it)->GetCapacity();
	
	Unlock( &m_lkDeqPool );

	return stCapacity;
}

void CSqrAllocPool::Trim()
{
	Lock( &m_lkDeqPool );

	PoolDeq_t::const_iterator it = m_deqPool.begin();
	PoolDeq_t::const_iterator itEnd = m_deqPool.end();

	for(; it != itEnd; ++it )
		(*it)->DelayTrim();
	
	Unlock( &m_lkDeqPool );
}

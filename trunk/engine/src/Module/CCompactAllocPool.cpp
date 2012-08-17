#include "stdafx.h"
#include "CCompactAllocPool.h"
#include "CSqrAllocPoolMgr.h"
#include "dlmalloc.h"

CCompactAllocPool::CCompactAllocPool(size_t stCapacity, const char* szPoolName)
{
	m_stMemUsage = 0;
	
	strncpy(m_szPoolName, szPoolName, sizeof(m_szPoolName) - 1);
	m_szPoolName[sizeof(m_szPoolName) - 1] = '\0';

	CSqrAllocPoolMgr::Inst().AddPool(this);

	m_bDelayTrim=false;

	m_pSpace=create_mspace( 0, 0 );
}


CCompactAllocPool::~CCompactAllocPool()
{
	destroy_mspace( m_pSpace );
	m_pSpace=NULL;

	CSqrAllocPoolMgr::Inst().DelPool(this);
}

const char* CCompactAllocPool::GetPoolName()const
{
	return m_szPoolName;
}

bool CCompactAllocPool::CheckLeakageEnabled()const
{
	return false;
}

void* CCompactAllocPool::Alloc(size_t stSize)
{
	m_stMemUsage += stSize;
	return mspace_malloc( m_pSpace, stSize );
}


void CCompactAllocPool::Dealloc(void* pMem,size_t stSize)
{
	if( m_bDelayTrim)
	{
		m_bDelayTrim = false;
		mspace_trim( m_pSpace, 0 );
	}
	m_stMemUsage -= stSize;
	return mspace_free( m_pSpace , pMem );
}

void* CCompactAllocPool::Realloc(void* pMem,size_t stOldSize,size_t stNewSize)
{
	if( m_bDelayTrim)
	{
		m_bDelayTrim = false;
		mspace_trim( m_pSpace, 0 );
	}
	m_stMemUsage = m_stMemUsage + stNewSize - stOldSize;
	return mspace_realloc( m_pSpace, pMem, stNewSize );
}

size_t CCompactAllocPool::GetMemUsage() const
{
	return m_stMemUsage;
}

size_t CCompactAllocPool::GetCapacity() const
{
	return mspace_footprint( m_pSpace );
}

void CCompactAllocPool::Trim()
{
	m_bDelayTrim=true;
}


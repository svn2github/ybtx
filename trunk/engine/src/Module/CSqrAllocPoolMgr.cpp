#include "stdafx.h"
#include "CSqrAllocPoolMgr.h"
#include "ISqrAllocPool.h"
#include "CSqrAllocPool.h"
#include "ExpHelper.h"
#include "ThreadHelper.h"
#include "MemoryHelper.h"

#ifdef _WIN32
#include "dlmalloc.h"
#endif

namespace sqr
{
	size_t GetAllMemUsage()
	{
		return CSqrAllocPoolMgr::Inst().GetAllMemUsage();
	}

	size_t GetAllCapacity()
	{
		return CSqrAllocPoolMgr::Inst().GetAllCapacity();
	}
}

CSqrAllocPoolMgr::CSqrAllocPoolMgr()
{
	sqr::CreateLock( & m_lkAllocPoolMap );
}

CSqrAllocPoolMgr::~CSqrAllocPoolMgr()
{
	sqr::DestroyLock( & m_lkAllocPoolMap );
}

CSqrAllocPoolMgr& CSqrAllocPoolMgr::Inst()
{
	//因为pool mgr可能会管理静态的pool，为了保证mgr的声明周期长于CMemThread，我们不删除mgr
	static CSqrAllocPoolMgr* ls_pPoolMgr=new CSqrAllocPoolMgr;
	return *ls_pPoolMgr;
}

void CSqrAllocPoolMgr::EnableFindLeakageOnPool(const char* szPoolName)
{
	m_setFindLeakagePoolName.insert( szPoolName );
}

void CSqrAllocPoolMgr::AddPool(ISqrAllocPool* pPool)
{	
	sqr::Lock( &m_lkAllocPoolMap );
	bool bResult = m_mapAllocPool.insert( make_pair( pPool,pPool->GetPoolName() ) ).second;
	sqr::Unlock( &m_lkAllocPoolMap );

	if( !bResult )
	{
		ostringstream strm;
		strm<<"alloc pool \""<<pPool->GetPoolName()<<"\" already exist, please do a rename.";
		GenErr( strm.str() );
	}

	if( !m_setFindLeakagePoolName.count( pPool->GetPoolName() ) )
		return;

	CSqrAllocPool* pRealPool = dynamic_cast<CSqrAllocPool*>( pPool );
	if( !pRealPool )
	{
		ostringstream strm;
		
		strm<<"Only CSqrAllocPool or it's derivied class can find memory leakage,\""
			<<pPool->GetPoolName()<<"\" is not a CSqrAllocPool.";
		GenErr( strm.str() );
	}

	pRealPool->EnableFindLeakage();
}

void CSqrAllocPoolMgr::DelPool(ISqrAllocPool* pPool)
{
	sqr::Lock( &m_lkAllocPoolMap );
	bool bResult = m_mapAllocPool.erase( pPool )>0;
	sqr::Unlock( &m_lkAllocPoolMap );

	if ( bResult )
		return;

	ostringstream strm;
	strm<<"alloc pool \""<<pPool->GetPoolName()<<"\" not exist.";
	GenErr( strm.str() );
}

size_t CSqrAllocPoolMgr::GetAllMemUsage()
{
	size_t stSize = 0;
	
	sqr::Lock( &m_lkAllocPoolMap );
	for( PoolMap_t::iterator it = m_mapAllocPool.begin(); it != m_mapAllocPool.end(); ++it )
		stSize += it->first->GetMemUsage();
	sqr::Unlock( &m_lkAllocPoolMap );
	
	return stSize;
}


size_t CSqrAllocPoolMgr::GetAllCapacity()
{
	size_t stSize = 0;
	
	sqr::Lock( &m_lkAllocPoolMap );
	for( PoolMap_t::iterator it = m_mapAllocPool.begin(); it != m_mapAllocPool.end(); ++it )
		stSize += it->first->GetCapacity();
	sqr::Unlock( &m_lkAllocPoolMap );
	
	return stSize;
}

static void StreamMemInfo(ostream& strm, const char* name, size_t stMemUsage, size_t stCapacity)
{
	char buf[1024];

	const char format[] = "%-35s  \t%12.6f MB \t%12.6f MB\n";

	const double mb = 1024 * 1024;
	const double MemUsage = double(stMemUsage) / mb;
	const double Capacity = double(stCapacity) / mb;
	sprintf(buf, format, name, MemUsage, Capacity);

	strm << buf;
}

void CSqrAllocPoolMgr::GetAllMemInfo(ostream& strm)
{
	StreamMemInfo(strm, "CppMemTotal: ", GetAllMemUsage(), GetAllCapacity());

	sqr::Lock( &m_lkAllocPoolMap );
	for( PoolMap_t::iterator it = m_mapAllocPool.begin(); it != m_mapAllocPool.end(); ++it )
	{
		ISqrAllocPool* pPool = it->first;

		if(pPool == NULL)
			continue;

		const char* szName = pPool->GetPoolName();
		size_t stMemUsage = pPool->GetMemUsage();
		size_t stCapacity = pPool->GetCapacity();

		StreamMemInfo(strm, szName, stMemUsage, stCapacity);
	}
	sqr::Unlock( &m_lkAllocPoolMap );
}


void CSqrAllocPoolMgr::GetMemLeakage(ostream& strm)
{
	char buf[1024];

#ifdef _WIN32
#ifdef _WIN64
	char format[] = "%-35s  \t%12I64u bytes\n";
#else
	char format[] = "%-35s  \t%12I32u bytes\n";
#endif
#else
	char format[] = "%-35s  \t%12zu bytes\n";
#endif

	sqr::Lock( &m_lkAllocPoolMap );
	for( PoolMap_t::iterator it = m_mapAllocPool.begin(); it != m_mapAllocPool.end(); ++it )
	{
		ISqrAllocPool* pPool = it->first;

		if(pPool == NULL)
			continue;

		if (pPool->CheckLeakageEnabled())
		{
			const char* szName = pPool->GetPoolName();
			size_t stSize = pPool->GetMemUsage();

			if (stSize > 0)
			{
				sprintf(buf, format, szName, stSize);
				strm << buf;
			}
		}

	}
	sqr::Unlock( &m_lkAllocPoolMap );
}

void CSqrAllocPoolMgr::Trim()
{
	sqr::Lock( &m_lkAllocPoolMap );
	for( PoolMap_t::iterator it = m_mapAllocPool.begin(); it != m_mapAllocPool.end(); ++it )
	{
		ISqrAllocPool* pPool = it->first;
		pPool->Trim();
	}
	sqr::Unlock( &m_lkAllocPoolMap );
}

#include "stdafx.h"
#include "CMemoryLeakageMgr.h"
#include "CMemThreadMgr.h"
#include "ExpHelper.h"
#include "ErrLogHelper.h"
#include "CSqrAllocPoolMgr.h"
#include "UtfCvs.h"
#include "ThreadHelper.h"

#ifndef _WIN32
#include <malloc.h>
#define _countof(array) (sizeof(array)/sizeof(array[0]))
#endif

//请查看CMemoryLeakageMgr.h里面的说明内容

CMemoryLeakageMgr::CMemoryLeakageMgr(int nCheckLevel)
{
	m_pThreadMgr = NULL;

	//必须已最快速度new出m_pThreadMgr,否则后续任何new操作都会导致内存分配出现指针错误
	if( nCheckLevel>1 )
		m_pThreadMgr = new CMemThreadMgr;

	if( CSqrAllocPoolMgr::Inst().GetAllMemUsage() > 0 )
		GenErr( "Memory leaked before watch begin!" );
	
	m_fdLeakage = NULL;	

	if( !nCheckLevel )
		return;
	
	uint32 uVersion = GetCurrentRevision();

#ifdef _WIN32
	swprintf_s(m_szMemFileName, _countof(m_szMemFileName), L"../../var/mem%uv%u.log", GetCurPID(), uVersion);
#else
	swprintf(m_szMemFileName, _countof(m_szMemFileName), L"../../var/mem%uv%u.log", GetCurPID(), uVersion);
#endif

	m_szMemFileName[ _countof(m_szMemFileName) - 1 ] = L'\0';

#ifdef _WIN32
	m_fdLeakage = _wfopen(m_szMemFileName, L"w");
#else
	char* strMemFileName = U16_to_U8(m_szMemFileName);
	m_fdLeakage = fopen(strMemFileName, "w");
	FreeUtfCvsDest(strMemFileName);
#endif
	if( !m_fdLeakage )
	{
		wostringstream strm;
		strm<<"Open file "<<m_szMemFileName<<" failed.";
		GenErr( U16_to_U8(strm.str().c_str()) );
	}
}

CMemoryLeakageMgr::~CMemoryLeakageMgr()
{
	if ( m_pThreadMgr )
		DumpStack();
	
	DumpLeakageNum();

	delete m_pThreadMgr;
	m_pThreadMgr=NULL;
	
	if (m_fdLeakage)
	{
		fclose( m_fdLeakage );
		m_fdLeakage = NULL;
	}
}

static CMemoryLeakageMgr* gs_Inst=NULL;

void CMemoryLeakageMgr::Init( int nCheckLevel )
{
	gs_Inst = new CMemoryLeakageMgr(nCheckLevel);
}

void CMemoryLeakageMgr::Unit()
{
	delete gs_Inst;
	gs_Inst=NULL;
}

CMemoryLeakageMgr& CMemoryLeakageMgr::Inst()
{
	return *gs_Inst;
}


void CMemoryLeakageMgr::DumpLeakageNum()
{
	if( !m_fdLeakage )
		return;

	if (CSqrAllocPoolMgr::Inst().GetAllMemUsage() == 0)
		return;

	basic_ostringstream<char, char_traits<char>, TMallocAllocator<char> > strm;

	CSqrAllocPoolMgr::Inst().GetMemLeakage(strm);

	if ( strm.str().empty() )
		return;

	WriteMemLog(strm.str().c_str());

#ifndef _WIN32
	fprintf( stderr, "%s\n", strm.str().c_str() );
#else
	MessageBox( NULL , strm.str().c_str() , "内存泄漏" , MB_OK|MB_ICONERROR );
#endif
}

void CMemoryLeakageMgr::DumpStack()
{
	if( !m_pThreadMgr )
		return;
	m_pThreadMgr->AddDumpStackJob();
}


#ifndef _WIN32
void CMemoryLeakageMgr::FetchAllSymbol()
{
	if ( !m_pThreadMgr )
		return;

	CMemThreadMgr::Inst()->AddFetchAllSymbolJob();

	/*
	由于我们现在的fetch symbol这些都在另一个线程处理，所以在
	fetch symbol完全结束之前，我们是不能够把我们的GameGas或GameGac动态库给free掉的
	不然会导致找不到symbol的情况，所以我们这里有一个semaphore，
	当从线程把fetch all symbol完成之后，即把在GameGas或GameGac里面的内存泄露函数
	全部找到symbol之后，在让主线程运行
	*/
	CMemThreadMgr::Inst()->WaitFetchAllSymbolEnded();
}
#endif


void CMemoryLeakageMgr::WriteMemLog(const char* szLog)
{
	fputs( szLog,m_fdLeakage);
}



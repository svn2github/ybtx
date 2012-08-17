#include "stdafx.h"
#include "CThreadAllocPool.h"
#include "dlmalloc.h"
#include <iomanip>
#include "CMemoryCookie.h"
#include "CMemCallStackInfo.h"
#include "CMemThreadMgr.h"
#include "ExpHelper.h"
#include "ThreadHelper.inl"
#include "CSqrAllocPool.h"
#include "MemoryHelper.h"



CThreadAllocPool::CThreadAllocPool(CSqrAllocPool* pPool)
{
	m_dlmspace = create_mspace( 0, 0 );
	m_nMemUsage = 0;
	
	CMemoryCookie* pCookie=static_cast<CMemoryCookie*>( mspace_malloc( m_dlmspace, sizeof(CMemoryCookie) ) );
	pCookie->m_pNext=NULL;

	m_pFreeQueueIn = pCookie;
	m_pFreeQueueOut = pCookie;
	m_pParentPool = pPool;

	m_bDelayTrim = false;	
}

CThreadAllocPool::~CThreadAllocPool(void)
{
	destroy_mspace( m_dlmspace );
}

void* CThreadAllocPool::GetMSpace()const
{
	return m_dlmspace;
}


static void PrintMemoryToStream( ostringstream& strm,void* pMem,size_t stSize )
{
	//从有问题的内存的前32个字节开始打印

	strm<< "起始地址前的32个字节:" << endl;

	for( int i = -32 ;i< 0; ++i )
	{
		strm << hex <<setfill('0') << setw(2) << int( *( (uint8*)(pMem)+i ) )
			<< setw(1) << " ";
	}

	strm<< endl ;

	strm<< "起始地址后的" << dec << stSize << "个字节:" << endl;

	for( int i = 0 ;i< int(stSize); ++i )
	{
		strm << hex <<setfill('0') << setw(2) << int( *( (uint8*)(pMem)+i ) )
			<< setw(1) << " ";
	}
}





//分配的内存如果不以8字节对齐，会有一些神奇的底层api不能正常工作，例如WSASend函数

static const size_t gs_stAligment = 8;//8字节对齐
//返回的数据以8字节对齐
#define ROUND_UP(size) (size_t)((size + gs_stAligment - 1) & ~(gs_stAligment - 1));

static const size_t gs_stCookieSize = ROUND_UP(sizeof(CMemoryCookie));
static const size_t gs_stCheckSize = 1;
static const size_t gs_stExtraSize = gs_stCookieSize + gs_stCheckSize;

void* CThreadAllocPool::Alloc(size_t stSize)
{
	HandleFreeQueue();

	stSize += gs_stExtraSize;

	uint8* const p= reinterpret_cast< uint8* >( mspace_malloc( GetMSpace(), stSize ) );
	
	if ( !p )
	{
#ifdef _WIN32
		void* ptr = VirtualAlloc(0, stSize, MEM_RESERVE|MEM_COMMIT, PAGE_READWRITE);
		if  (ptr == 0)
		{
			ostringstream strm;
			strm << "分配大小: " << stSize<<" , "
				<< "内存池: " << m_pParentPool->GetPoolName()<<" , "
				<< "内存池使用: " << GetMemUsage()<<" , "
				<< "内存池大小: " << GetCapacity()<<" , "
				<< "所有内存池使用: " << GetAllMemUsage()<<" , "
				<< "所有内存池大小: " << GetAllCapacity()<<" , "
				<< "VirtualAlloc 分配失败 ErrorID: " <<	GetLastError()
				<< endl;
			GenErr("分配内存失败", strm.str().c_str());
		}
		else
		{
			VirtualFree(ptr, 0, MEM_RELEASE);
			ostringstream strm;
			strm << "分配大小: " << stSize<<" , "
				<< "内存池: " << m_pParentPool->GetPoolName()<<" , "
				<< "内存池使用: " << GetMemUsage()<<" , "
				<< "内存池大小: " << GetCapacity()<<" , "
				<< "所有内存池使用: " << GetAllMemUsage()<<" , "
				<< "所有内存池大小: " << GetAllCapacity()<<" , "
				<< "dlmalloc 分配出错"
				<< endl;
			GenErr("分配内存失败", strm.str().c_str());
		}
#else
		ostringstream strm;
		strm << "分配大小: " << stSize<<" , "
			<< "内存池: " << m_pParentPool->GetPoolName()<<" , "
			<< "内存池使用: " << GetMemUsage()<<" , "
			<< "内存池大小: " << GetCapacity()<<" , "
			<< "所有内存池使用: " << GetAllMemUsage()<<" , "
			<< "所有内存池大小: " << GetAllCapacity()<<" , "
			<< endl;
		GenErr("分配内存失败", strm.str().c_str());
#endif
	}

	Atomic_FetchAndAdd( &m_nMemUsage, (int32)stSize );

	CMemoryCookie *const pCookie = reinterpret_cast<CMemoryCookie*>(p);
	pCookie->m_stSize=stSize;
	pCookie->m_pPool=this;
	
	if ( m_pParentPool->m_bFindLeakage )
	{
		pCookie->m_pStackInfo = new CMemCallStackInfo();

		CMemThreadMgr::Inst()->AddMemMallocJob(pCookie);
	}
	else
	{
		pCookie->m_uCheckSum = pCookie->m_stSize ^ size_t(pCookie) ^ size_t(0xffffffffffffffff);
	}


	uint8* const pBottomCheck = p + stSize - gs_stCheckSize;

	*pBottomCheck = 0xbb;
	
	return p + gs_stCookieSize;
}


inline void CThreadAllocPool::ConfirmIsChildPool( CThreadAllocPool* pThreadPoolOfCookie )
{
	if( pThreadPoolOfCookie->m_pParentPool == m_pParentPool )
		return;

	ostringstream strm;
	strm<<"\""<<pThreadPoolOfCookie->m_pParentPool->GetPoolName()<<"\"中分配的内存被拿到\""
		<<m_pParentPool->GetPoolName()<<"\"中释放!";

	GenErr( strm.str() );
}

void CThreadAllocPool::Dealloc(void* pMem)
{
	if( !pMem )
		return;

	uint8*const p = reinterpret_cast<uint8*>(pMem) - gs_stCookieSize;

	CMemoryCookie * const pCookie= reinterpret_cast<CMemoryCookie*>( p );

	CThreadAllocPool * const pPoolOfCookie = pCookie->m_pPool;
	
	const size_t stSize=pCookie->m_stSize;
	
	if ( m_pParentPool->m_bFindLeakage )
	{
		ConfirmIsChildPool( pPoolOfCookie );

		Atomic_FetchAndAdd( &pPoolOfCookie->m_nMemUsage, -(int32)stSize );

		if( !CMemThreadMgr::Inst() )
			ShowErrorMsgAndExit("RunModule或者Module中存在直接或间接被构造出来的全局或者静态对象，这些对象进行了内存分配操作，请消除之！");

		CMemThreadMgr::Inst()->AddMemFreeJob(pCookie, m_pParentPool);
	}
	else
	{
		if( ( stSize ^ size_t(pCookie->m_uCheckSum) ^ size_t(pCookie) ) != size_t(0xffffffffffffffff) )
		{
			ostringstream strm;
			PrintMemoryToStream( strm , pMem , 128-32 );

			GenErr( "内存长度校验失败，可能是野指针或者内存被写坏，放弃释放该内存块。",strm.str() );
		}

		uint8* const pBottomCheck = p + stSize - gs_stCheckSize;

		bool const bWriteBelowBoundary = *pBottomCheck != 0xbb;

		ConfirmIsChildPool( pPoolOfCookie );
		
		Atomic_FetchAndAdd( &pPoolOfCookie->m_nMemUsage, -(int32)stSize );

		if( pPoolOfCookie == this )
		{
			mspace_free( GetMSpace(), pCookie );
		}
		else
		{
			pPoolOfCookie->PushToFreeQueue( pCookie );
		}

		if( bWriteBelowBoundary )
		{
			//在这里发现内存问题的时候,stSize的值是可信的（因为通过了校验）,所以这里将报错延迟到内存释放以后。

			size_t stPrintSize = stSize+32;
			if( stPrintSize > 512-32 )
				stPrintSize = 512-32;

			ostringstream strm;

			PrintMemoryToStream( strm , pMem , stPrintSize );

			DbgGenErr("内存向下写越界" ,strm.str());
		}
	}
}


void* CThreadAllocPool::Realloc(void* pMem,size_t stNewSize)
{
	if( !pMem )
		return Alloc( stNewSize );

	if( stNewSize == 0 )
	{
		Dealloc( pMem );
		return NULL;
	}

	uint8*const oldp = reinterpret_cast<uint8*>(pMem) - gs_stCookieSize;

	CMemoryCookie *const pOldCookie= reinterpret_cast<CMemoryCookie*>( oldp );

	CThreadAllocPool*const pOldPool = pOldCookie->m_pPool;
	
	const size_t stOldSize=pOldCookie->m_stSize;
	
	if( (pOldPool != this) || m_pParentPool->m_bFindLeakage )
	{
		void*pNewMem = Alloc( stNewSize );
		memcpy( pNewMem, pMem, min(stOldSize-gs_stExtraSize,stNewSize) );
		Dealloc( pMem );
		return pNewMem;
	}

	if( ( stOldSize ^ size_t(pOldCookie->m_uCheckSum) ^ size_t(pOldCookie) ) != size_t(0xffffffffffffffff) )
	{
		ostringstream strm;
		PrintMemoryToStream( strm , pMem , 128-32 );

		GenErr( "内存长度校验失败，可能是野指针或者内存被写坏，放弃释放该内存块。",strm.str() );
	}

	ConfirmIsChildPool( pOldPool );
	
	uint8* const pBottomCheck = oldp + stOldSize - gs_stCheckSize;

	bool const bWriteBelowBoundary = *pBottomCheck != 0xbb;

	stNewSize += gs_stExtraSize;

	uint8*const newp=reinterpret_cast<uint8*>( mspace_realloc( GetMSpace(), oldp, stNewSize ) );

	if( bWriteBelowBoundary )
	{
		//在这里发现内存问题的时候,stSize的值是可信的（因为通过了校验）,所以这里将报错延迟到内存释放以后。

		size_t stPrintSize = stOldSize+32;
		if( stPrintSize > 512-32 )
			stPrintSize = 512-32;

		ostringstream strm;

		PrintMemoryToStream( strm , pMem , stPrintSize );

		DbgGenErr("内存向下写越界" ,strm.str());
	}	

	if ( !newp )
	{
		ostringstream strm;
		strm << "重新分配大小: " << stNewSize<<" , "
			<< "内存池: " << m_pParentPool->GetPoolName()<<" , "
			<< "内存池使用: " << GetMemUsage()<<" , "
			<< "内存池大小: " << GetCapacity()<<" , "
			<< endl;
		GenErr("重新分配内存失败", strm.str().c_str());
	}

	Atomic_FetchAndAdd( &m_nMemUsage, int32(stNewSize)-int32(stOldSize) );
	
	CMemoryCookie *const pNewCookie = reinterpret_cast<CMemoryCookie*>(newp);
	
	pNewCookie->m_stSize=stNewSize;
	
	pNewCookie->m_uCheckSum = pNewCookie->m_stSize ^ size_t(pNewCookie) ^ size_t(0xffffffffffffffff);

	uint8* const pNewBottomCheck = newp + stNewSize - gs_stCheckSize;

	*pNewBottomCheck = 0xbb;

	return newp + gs_stCookieSize;
}


void CThreadAllocPool::FreeFromMemThread( void* pMem )
{
	uint8*const p =static_cast<uint8*>(  pMem );

	CMemoryCookie * const pCookie= reinterpret_cast<CMemoryCookie*>( p);

	CThreadAllocPool * const pPoolOfCookie = pCookie->m_pPool;

	const size_t stSize=pCookie->m_stSize;

	uint8* const pBottomCheck = p + stSize - gs_stCheckSize;

	bool const bWriteBelowBoundary = *pBottomCheck != 0xbb;

	ConfirmIsChildPool( pPoolOfCookie );
	
	if( pPoolOfCookie == this )
	{
		mspace_free( GetMSpace(), pCookie );			
	}
	else
	{
		pPoolOfCookie->PushToFreeQueue( pCookie );
	}

	if( bWriteBelowBoundary )
	{
		//在这里发现内存问题的时候,stSize的值是可信的（因为通过了校验）,所以这里将报错延迟到内存释放以后。

		size_t stPrintSize = stSize+32;
		if( stPrintSize > 512-32 )
			stPrintSize = 512-32;

		ostringstream strm;

		PrintMemoryToStream( strm , pMem , stPrintSize );

		DbgGenErr("内存向下写越界" ,strm.str());
	}
}


size_t CThreadAllocPool::GetMemSize(void* pMem)
{
	if( !pMem )
		return 0;

	uint8*const p = reinterpret_cast<uint8*>(pMem) - gs_stCookieSize;

	CMemoryCookie * const pCookie= reinterpret_cast<CMemoryCookie*>( p );

	return pCookie->m_stSize-gs_stExtraSize;
}

size_t CThreadAllocPool::GetMemUsage()const
{
	return m_nMemUsage;
}

size_t CThreadAllocPool::GetCapacity()const
{
	return mspace_footprint( GetMSpace() );
}


void CThreadAllocPool::PushToFreeQueue( CMemoryCookie* pCookie )
{
	pCookie->m_pNext=NULL;
	
	CMemoryCookie* pPrev=static_cast<CMemoryCookie*>
		( Atomic_SwapPtr( reinterpret_cast<void* volatile*>( &m_pFreeQueueIn ), pCookie ) );

	Ast( pPrev );

	pPrev->m_pNext = pCookie;
}

void CThreadAllocPool::HandleFreeQueue()
{
	if( m_bDelayTrim )
	{
		m_bDelayTrim=false;
		mspace_trim( m_dlmspace, 0 );

	}
	//永远都会预留最后一个MemoryCookie不删除
	
	CMemoryCookie* pCookie = m_pFreeQueueOut;
	CMemoryCookie* pNext = pCookie->m_pNext;

	if( !pNext )
		return;
	
	for(;;)
	{
		mspace_free( m_dlmspace, pCookie );

		pCookie = pNext;

		pNext = pCookie->m_pNext;

		if( !pNext )
		{
			m_pFreeQueueOut = pCookie;
			return;
		}		
	}
}

void CThreadAllocPool::DelayTrim()
{
	m_bDelayTrim = true;
}

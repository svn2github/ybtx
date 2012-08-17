#include "stdafx.h"
#include "CMemCallStackInfo.h"
#include <cassert>
#include "ExpHelper.h"
#include <iostream>
#include "CStackFrame.h"
#include "CStackFrameMgr.h"
#include "CMemCallStackInfoSet.h"
#include "MemoryHelper.h"
#include "StackWalkHelper.h"

CMemCallStackInfo::CMemCallStackInfo()
:m_uMemUsage(0),m_nCounter(0)
{
	GetStack(0, 10);
}

CMemCallStackInfo::CMemCallStackInfo(size_t uBegin, size_t uEnd)
:m_uMemUsage(0), m_nCounter(0)
{
	GetStack(uBegin, uEnd);
}

CMemCallStackInfo::~CMemCallStackInfo(void)
{
}

void CMemCallStackInfo::AddRef(size_t uMemUsage)
{
	++m_nCounter;
	m_uMemUsage += uMemUsage;
}

bool CMemCallStackInfo::DelRef(size_t uMemUsage)
{
	Ast( m_nCounter > 0 );
	--m_nCounter;
	m_uMemUsage -= uMemUsage;
	return m_nCounter==0;
}

size_t CMemCallStackInfo::GetMemUsage()const
{
	return m_uMemUsage;
}

size_t CMemCallStackInfo::GetCount()const
{
	return size_t(m_nCounter);
}

size_t CMemCallStackInfo::GetText(char* szBuffer,size_t uLength)const
{
	ostringstream oss;

	for(const_iterator it=begin();it!=end();++it)
		oss<<(*it)->GetText()<<endl;

	strncpy( szBuffer, oss.str().c_str(), uLength );
	return oss.str().size();
}

void CMemCallStackInfo::Print()const
{
	for(const_iterator it=begin();it!=end();++it)
	{
		WatchLeakage_MemLog((*it)->GetText());
		WatchLeakage_MemLog("\n");
	}
}

CMemCallStackAddrVector* CMemCallStackInfo::TakeAddrVec()
{
	CMemCallStackAddrVector* pVec=m_pVecAddr;
	m_pVecAddr=NULL;
	return pVec;
}

void CMemCallStackInfo::GetStack(size_t uBegin, size_t uEnd)
{
#ifdef _WIN32
	CONTEXT context;
	RtlCaptureContext( &context );
	
	STACKFRAME64 sf;
	memset(&sf,0,sizeof(sf));

	// 初始化stackframe结构
	sf.AddrPC.Offset		= context.Eip;
	sf.AddrPC.Mode			= AddrModeFlat;
	sf.AddrStack.Offset		= context.Esp;
	sf.AddrStack.Mode		= AddrModeFlat;
	sf.AddrFrame.Offset		= context.Ebp;
	sf.AddrFrame.Mode		= AddrModeFlat;

	uEnd++;
	HANDLE hThread = ::GetCurrentThread();	
	HANDLE hProcess=GetCurrentProcess();

	m_pVecAddr = new CMemCallStackAddrVector;
	for(size_t i=0;i<uEnd;++i)
	{
		// 获取下一个栈帧
		if(! SafeStackWalk64(IMAGE_FILE_MACHINE_I386,hProcess, hThread, &sf,NULL, 0, SymFunctionTableAccess64, SymGetModuleBase64, 0 ) )
			break;

		// 检查帧的正确性
		if(0 == sf.AddrFrame.Offset)
			break;

		if(i>=uBegin)
			m_pVecAddr->push_back((void*)(sf.AddrPC.Offset));
	}
#else
	void* aryStack[10];

	size_t stSize = ::backtrace ( aryStack, min(sizeof(aryStack) / sizeof(void*), uEnd) );

	if( stSize <= uBegin )
		return;

	const size_t uMin=min(stSize,uEnd)-1;

	m_pVecAddr = new CMemCallStackAddrVector;

	for(size_t i=uBegin;i<uMin;++i)
		m_pVecAddr->push_back( aryStack[i] );
#endif
}

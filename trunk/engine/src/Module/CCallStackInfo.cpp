#include "stdafx.h"
#include "CCallStackInfo.h"
#include "BaseHelper.h"
#include "CStackFrameMgr.h"
#include "CMemoryLeakageMgr.h"
#include "CStackFrame.h"
#include "CMapFileMgr.h"
#include "StackWalkHelper.h"
#include <iostream>

#ifndef _WIN32
#define UNW_LOCAL_ONLY
extern "C"
{
#include "unwind/libunwind.h"
}
#endif

static const size_t stMaxFuncNameCount = 512;

CCallStackInfo::CCallStackInfo(bool bWalk, size_t stBase, size_t stCount)
: m_stNextSymbolPos(0)
{
	m_stBegin = stBase;
	m_stEnd = stBase + stCount;

	m_ayStack = (char*)malloc(sizeof(char) * stCount * stMaxFuncNameCount);

	m_bNewStack = true;

	if(bWalk)
	{
		GetStack( m_stBegin, m_stEnd);
	}
}

CCallStackInfo::CCallStackInfo(void* buffer, size_t stBase, size_t stCount)
: m_stNextSymbolPos(0)
, m_bNewStack(false)
{
	m_stBegin = stBase;
	m_stEnd = stBase + stCount;

	m_ayStack = (char*)buffer;

	GetStack( m_stBegin, m_stEnd);
}

#ifdef _WIN32
CCallStackInfo::CCallStackInfo(PCONTEXT pContext, size_t stBase, size_t stCount)
: m_stNextSymbolPos(0)
{
	m_stBegin = stBase;
	m_stEnd = stBase + stCount;

	m_ayStack = (char*)malloc(sizeof(char) * stCount * stMaxFuncNameCount);

	m_bNewStack = true;

	GetStack( pContext, m_stBegin, m_stEnd);
}
#endif

CCallStackInfo::~CCallStackInfo(void)
{
	if (m_bNewStack)
	{
		free(m_ayStack);
	}
}

void CCallStackInfo::WalkStack()
{
	GetStack(m_stBegin, m_stEnd);
}

void CCallStackInfo::GetStack( size_t uBegin, size_t uEnd )
{
	m_stNextSymbolPos = 0;

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
	DWORD dwMachineType		= IMAGE_FILE_MACHINE_I386;


	//#ifdef _WIN32
	//unsigned _Eip,_Esp,_Ebp;
	//_asm
	//{
	//	call		popeax;
	//popeax:
	//	pop			EAX;
	//	mov			_Eip, EAX;
	//	mov			_Esp, ESP;
	//	mov			_Ebp, EBP;
	//}

	//STACKFRAME64 sf;
	//memset(&sf,0,sizeof(sf));

	//// 初始化stackframe结构
	//sf.AddrPC.Offset		= _Eip;
	//sf.AddrPC.Mode			= AddrModeFlat;
	//sf.AddrStack.Offset		= _Esp;
	//sf.AddrStack.Mode		= AddrModeFlat;
	//sf.AddrFrame.Offset		= _Ebp;
	//sf.AddrFrame.Mode		= AddrModeFlat;
	//DWORD dwMachineType		= IMAGE_FILE_MACHINE_I386;

	HANDLE hThread = ::GetCurrentThread();	
	HANDLE hProcess=GetCurrentProcess();

	for(size_t i=0;i<uEnd;++i)
	{
		// 获取下一个栈帧
		if(!SafeStackWalk64(dwMachineType,hProcess, hThread, &sf,
						NULL, 0, SymFunctionTableAccess64, SymGetModuleBase64, 0 ))
			break;

		// 检查帧的正确性
		if(0 == sf.AddrFrame.Offset)
			break;

		if(i>=uBegin)
			FetchSymbol((void*)(sf.AddrPC.Offset)) ;
	}

	m_ayStack[m_stNextSymbolPos] = '\0';
	m_stNextSymbolPos = 0;

#else

	unw_cursor_t cursor;

	unw_context_t uc;

	unw_getcontext(&uc);
	unw_init_local(&cursor, & uc);

	size_t i = 0;
	char name[stMaxFuncNameCount];

	while(unw_step(&cursor) > 0 && i < uEnd)
	{  
		name[0] = '\0';
		unw_word_t offp;
		int ret = unw_get_proc_name(&cursor, name, sizeof(name), &offp);
		if(ret == 0)
		{
			name[sizeof(name) - 1] = '\0';

			if(i>=uBegin)
				CopySymbol(name);
		}

		++i;
	}

	m_ayStack[m_stNextSymbolPos] = '\0';
	m_stNextSymbolPos = 0;

#endif
}

#ifdef _WIN32
void CCallStackInfo::GetStack(PCONTEXT pContext, uint32 uBegin, uint32 uEnd )
{
	m_stNextSymbolPos = 0; 

	HANDLE hThread = ::GetCurrentThread();
	STACKFRAME64 sf;
	memset(&sf,0,sizeof(sf));

	// 初始化stackframe结构
	sf.AddrPC.Offset		= pContext->Eip;
	sf.AddrPC.Mode			= AddrModeFlat;
	sf.AddrStack.Offset		= pContext->Esp;
	sf.AddrStack.Mode		= AddrModeFlat;
	sf.AddrFrame.Offset		= pContext->Ebp;
	sf.AddrFrame.Mode		= AddrModeFlat;
	DWORD dwMachineType		= IMAGE_FILE_MACHINE_I386;

	HANDLE hProcess=GetCurrentProcess();

	for(unsigned i=0;i<uEnd;++i)
	{
		// 获取下一个栈帧
		if(!SafeStackWalk64(dwMachineType,hProcess, hThread, &sf,
						NULL, 0, SymFunctionTableAccess64, SymGetModuleBase64, 0 ))
			break;

		// 检查帧的正确性
		if(0 == sf.AddrFrame.Offset)
			break;

		if(i>=uBegin)
			FetchSymbol((void*)(sf.AddrPC.Offset)) ;
	}

	m_ayStack[m_stNextSymbolPos] = '\0';
	m_stNextSymbolPos = 0;
}

#endif

const char* CCallStackInfo::GetText() const
{
	return m_ayStack;
}

#ifdef _WIN32
void CCallStackInfo::FetchSymbol(void* pAddr) 
{
	char buf[stMaxFuncNameCount];
	
	
	const char* szSymbol = CMapFileMgr::Inst().FindFunName((uint32)pAddr);
	if (szSymbol)
	{
		_snprintf(buf, sizeof(buf) - 1, "Addr: 0x%x\t%s", pAddr, szSymbol);
	}
	else
	{
		_snprintf(buf, sizeof(buf) - 1, "Addr: 0x%x\t", pAddr);
	}

	buf[sizeof(buf) - 1] = '\0';

	CopySymbol(buf);
}

#endif


void CCallStackInfo::CopySymbol(const char* szSymbol)
{
	char* szDest = m_ayStack + m_stNextSymbolPos;
	size_t uLen = strlen(szSymbol) + 1;

	strcpy(szDest, szSymbol);

	szDest[uLen - 1] = '\n';
	m_stNextSymbolPos += uLen;
}

#include "stdafx.h"
#ifndef _WIN32
#include "CCoreSigMsgMgr.h"
#include "ExpHelper.h"
#include "ErrLogHelper.h"
#include "CodeCvs.h"
#include <csignal>
#include <cstring>
#include "CJmpStack.h"
#include "CLog.h"
#include "UtfCvs.h"
#include "CError.h"
#include "ThreadHelper.h"
#include <ucontext.h>
#include <time.h>
#include "CCallStackInfo.h"
#include "ThreadHelper.inl"


CCoreSigMsgMgr::CCoreSigMsgMgr()
{
	m_pHandlerFun=NULL;
	m_nIndexOut=0;
	m_nIndexIn=0;
	m_bJustContinued=false;
	memset( m_arySignalQueue, 0, sizeof(m_arySignalQueue) );

	struct sigaction sa;
	sa.sa_sigaction=&CCoreSigMsgMgr::SigHandler;
	if(-1==sigemptyset(&sa.sa_mask))
		GenErrnoErr("sigemptyset failed.");

	sa.sa_flags=SA_SIGINFO  | SA_ONSTACK;

	if(-1==sigaction(SIGTERM,&sa,NULL))
		GenErrnoErr("sigaction SIGTERM failed.");
	if(-1==sigaction(SIGINT,&sa,NULL))
		GenErrnoErr("sigaction SIGINT failed.");
	if(-1==sigaction(SIGCONT,&sa,NULL))
		GenErrnoErr("sigaction SIGCONT failed.");

	if (CatchErrorEnabled())
	{
		if (-1 == sigaction(SIGSEGV, &sa, NULL))
			GenErrnoErr("sigaction SIGSEGV failed.");
		if (-1 == sigaction(SIGILL, &sa, NULL))
			GenErrnoErr("sigaction SIGILL failed.");
		if (-1 == sigaction(SIGFPE, &sa, NULL))
			GenErrnoErr("sigaction SIGFPE failed.");
		if (-1 == sigaction(SIGUSR1, &sa, NULL))
			GenErrnoErr("sigaction SIGUSR1 failed.");
		if (-1 == sigaction(SIGUSR2, &sa, NULL))
			GenErrnoErr("sigaction SIGUSR2 failed.");
	}

	//pick 10 realtime signal for us
	for( int i=40; i< 50; ++ i)
	{
		if(-1!=sigaction(i,&sa,NULL))
			continue;
		GenErrnoErr("sigaction ealtime signal failed.");

	}
}

CCoreSigMsgMgr::~CCoreSigMsgMgr()
{

}

CCoreSigMsgMgr& CCoreSigMsgMgr::Inst()
{
	static CCoreSigMsgMgr ls_Inst;
	return ls_Inst;
}

void CCoreSigMsgMgr::SetSigHandler(void (*pFun)(int32))
{
	m_pHandlerFun = pFun;
}


void CCoreSigMsgMgr::SigHandler(int signum, siginfo_t* info, void* context)
{
	if( CatchErrorEnabled() )
	{	
		switch(signum)
		{
		case SIGSEGV:
		case SIGILL:
		case SIGFPE:
		case SIGUSR1:
		case SIGUSR2:
			break;
		case 45:
			{
#ifndef _WIN32
			char szStack[sizeof(char)*30*1024];
			CCallStackInfo StackInfor(szStack, 0, 30);

			char szFile[256];
			sprintf(szFile, "%s/il_killed_%d.log", U16_to_U8( GetLogPath() ), GetCurPID());
			FILE* pFile = fopen(szFile, "a+");
			if(pFile)
			{
				fwrite(szStack, sizeof(char), strlen(szStack), pFile);
				fclose(pFile);
			}
#endif
			exit(1);
			}
		default:
			goto out;
		}


		const char* szFunc = "发生函数";
		char szStack[1024];
		CCallStackInfo StackInfor(szStack, 2, 2);

		//只取第一层有函数symbol的堆栈
		for (size_t i = 0; i < sizeof(szStack); i++)
		{
			if (szStack[i] == '\0')
				break;

			if (szStack[i] == '\n')
			{
				szStack[i] = '\0';
				break;
			}
		}

		char szType[1024];

		switch(signum)
		{
		case SIGSEGV:
			{
				void* pMemAddr = info->si_addr;
				ucontext_t* pContext = (ucontext_t*)context;
				size_t stIP = pContext->uc_mcontext.gregs[REG_RIP];

				char szError[128];
				sprintf(szError, "代码执行地址: 0x%llX\n访问内存地址: %p\n", (uint64)stIP, pMemAddr);

				//如果访问的内存地址大于这个值，我们认为是野指针，否则认为是非法内存访问
				if(stIP > 1024)
				{	
					snprintf(szType, sizeof(szType) - 1, "非法内存访问 %s: %s", szFunc, szStack);
				}
				else
				{	
					snprintf(szType, sizeof(szType) - 1, "无效内存访问 %s: %s", szFunc, szStack);
				}
				SQR_THROW(szType, szError);
			}
		case SIGILL:
			{
				snprintf(szType, sizeof(szType) - 1, "无效cpu指令 %s: %s", szFunc, szStack);
				SQR_THROW(szType);
			}
		case SIGFPE:
			{
				snprintf(szType, sizeof(szType) - 1, "浮点运算异常 %s: %s", szFunc, szStack);
				SQR_THROW(szType);
			}
		case SIGUSR1:
			{
				snprintf(szType, sizeof(szType) - 1, "死循环 %s: %s", szFunc, szStack);
				SQR_THROW(szType);
			}
		case SIGUSR2:
			{
				LogErr("警告:主循环偏慢，用户会感受到延迟。");
				return;
			}
		default:
			break;
		}
	}

out:

	CCoreSigMsgMgr::Inst().PushSigMsg( signum );

}

void CCoreSigMsgMgr::PushSigMsg(int nSigNum)
{
	Ast( nSigNum );
	printf("%d\n",nSigNum);

	if( nSigNum == SIGCONT )
	{
		m_bJustContinued = true;
		return;
	}

	int32 nIndexIn;

	const int nAySize=sizeof( m_arySignalQueue )/sizeof( int );

	for( ;; )
	{
		nIndexIn = m_nIndexIn;
		if( (nIndexIn+1) % nAySize  == m_nIndexOut )
		{
			char szMsg[32];
			snprintf( szMsg,sizeof(szMsg),"%d", nSigNum );
			LogErr("信号队列已满，无法再放入新的信号",szMsg);
			return;
		}
		int32 nNewIndex = ( nIndexIn+1 ) % nAySize;

		if( nIndexIn == Atomic_CompareAndSwap( &m_nIndexIn,nIndexIn,nNewIndex ) )
			break;
	}

	m_arySignalQueue[nIndexIn]=nSigNum;
}

void CCoreSigMsgMgr::FlushSigMsg()
{
	for( ;; )
	{
		if( m_arySignalQueue[ m_nIndexOut] ==0 || m_nIndexOut == m_nIndexIn )
			break;	//queue empty

		if( m_pHandlerFun )
			m_pHandlerFun( m_arySignalQueue[ m_nIndexOut ] );
		m_arySignalQueue[ m_nIndexOut++ ] = 0;

		if( m_nIndexOut >= sizeof( m_arySignalQueue )/sizeof(int) )
			m_nIndexOut = 0;
	}
}


#endif

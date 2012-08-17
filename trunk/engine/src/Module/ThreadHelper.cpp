#include "stdafx.h"
#include "ThreadHelper.h"
#include "TimeHelper.h"
#include "ExpHelper.h"
#include "ErrLogHelper.h"
#include "CThreadWatcherMap.h"
#include "TMallocAllocator.h"
#include  "SqrAlloc.h"

#if defined _WIN32
#include <process.h>
#else


#include <time.h>
#include <errno.h>
#include <sched.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/sysinfo.h>
#include <sys/resource.h>
#include <sys/syscall.h>
#include <linux/unistd.h>
#include <pthread.h>
#include <signal.h>

#define gettid() syscall(SYS_gettid)

#endif


namespace sqr
{
	bool Sleep(unsigned uMiliSecond)
	{
#ifdef _WIN32
		return WAIT_IO_COMPLETION != ::SleepEx(uMiliSecond,TRUE);
#else
		timespec ts;
		ts.tv_sec	=uMiliSecond/1000;
		ts.tv_nsec	=uMiliSecond%1000*1000000;
		if(-1==nanosleep(&ts,&ts))
		{
			//如果是信号，我们就不管
			if(errno == EINTR)
				return false;

			ostringstream strm;
			strm<<"nanosleep failed with error:"<<strerror(errno)<<ends;
			GenErr(strm.str());
		}
#endif
		return true;
	}

	//////////////////////////////////////////////////////////////////////////
	// Thread section

	struct CThreadArgHolder
	{
		THREADPROC	m_pThreadFunc;
		void*		m_pArg;
	};
#ifdef _WIN32
	static unsigned int __stdcall ThreadProcCaller(void * pArg)
#else
	static void* ThreadProcCaller(void* pArg)
#endif
	{
		CThreadArgHolder* pHolder = static_cast<CThreadArgHolder*>( pArg);

		THREADPROC proc=pHolder->m_pThreadFunc;		
		void* arg=pHolder->m_pArg;

		SqrFree(pHolder);

		//这个try_catch的用处不大，主要是用于为每个线程初始化容错系统的jmpstack
		SQR_TRY
		{
			proc(arg);
		}
		SQR_CATCH(exp)
		{
			LogExp(exp);
		}
		SQR_TRY_END;
#ifdef _WIN32
		return 0;
#else
		return NULL;
#endif
	}

	void CreateThread(HTHREAD* phThread,unsigned long nStackSize, THREADPROC pStartRoutine,void* pParam)
	{
		if ( pStartRoutine == NULL )
			GenErr("Thread routine is NULL.");
		if ( nStackSize < 0 )
			GenErr("Invalid stacksize.");

		Ast(phThread);
		
		//这里不能使用new和delete 因为CMemThreadMgr并没有初始化,会造成循环调用
		CThreadArgHolder* pArgHolder=reinterpret_cast<CThreadArgHolder*>(SqrMalloc(sizeof(CThreadArgHolder)));

		pArgHolder->m_pThreadFunc=pStartRoutine;
		pArgHolder->m_pArg=pParam;

#ifndef _WIN32		
		if( pthread_create(phThread, NULL,ThreadProcCaller,pArgHolder) )
			GenErrnoErr("pthread_create failed.");
#else
		unsigned ThreadID;
		//这里必须先SUSPENDED起来，否则*phThread可能都还没有完成赋值，线程已经执行了不少了。
		*phThread=(HANDLE)_beginthreadex(0,nStackSize,ThreadProcCaller,pArgHolder,CREATE_SUSPENDED ,&ThreadID);
		if (*phThread == NULL)
		{
			ostringstream strm;
			strm<<"_beginthreadex failed with error:"<<errno;
			GenErr( strm.str() );
		}

		//将创建线程的线程优先级传递给被创建线程
		int nPriority=GetThreadPriority( ::GetCurrentThread() );
		if( THREAD_PRIORITY_ERROR_RETURN == nPriority )
			GenErrnoErr("::GetThreadPriority failed.");

		if( !SetThreadPriority( *phThread,nPriority ) )
			GenErrnoErr( "::SetThreadPriority failed." );

		ResumeThread( *phThread );
#endif
	}

	bool LowerCurThreadPriority(void)
	{
#ifdef _WIN32
		HANDLE hThread = ::GetCurrentThread();
		int nPriority = ::GetThreadPriority( hThread );

		if( THREAD_PRIORITY_ERROR_RETURN == nPriority )
			GenErrnoErr("::GetThreadPriority failed.");
		
		if( nPriority <= -2 )
			return false;

		if( nPriority == THREAD_PRIORITY_TIME_CRITICAL )
		{
			nPriority = THREAD_PRIORITY_HIGHEST;
		}
		else
		{
			--nPriority;
		}
		
		if( !SetThreadPriority(hThread,nPriority ) )
			GenErrnoErr("SetThreadPriority failed.");
#else
		int nTid=gettid();

		errno = 0;
		int nNice=getpriority(PRIO_PROCESS, nTid );
		if( errno )
			GenErrnoErr( "getpriority failed." );

		if( nNice >= 19 )
			return false;
		
		if( setpriority(PRIO_PROCESS,nTid,nNice+1) )
			GenErrnoErr( "setpriority failed." );
#endif
		return true;
	}

	void DetachThread(HTHREAD* phThread)
	{
#ifndef _WIN32
		int r=pthread_detach(*phThread);
		if( r ) GenErrnoErr( "pthread_detach failed with error:", r );
#else
		if( !CloseHandle(*phThread) )
			GenErrnoErr("CloseHandle failed.");
#endif
	}

	void ExitThread(unsigned long dwExitCode)
	{
#ifndef _WIN32
		pthread_exit(&dwExitCode);
#else
		_endthreadex(dwExitCode);
#endif
	}

	void TerminateThread(HTHREAD* phThread, unsigned long dwExitCode)
	{
#ifndef _WIN32
		int r=pthread_cancel(*phThread);
		if( r ) GenErrnoErr( "pthread_cancel failed with error:", r );
#else
		if( !::TerminateThread(*phThread, dwExitCode) )
			GenErrnoErr("::TerminateThread failed.");
		if( !CloseHandle( *phThread ) )
			GenErrnoErr("::CloseHandle failed.");
#endif
	}

	void GetCurrentThread(HTHREAD* phThread)
	{
#ifndef _WIN32
		*phThread = pthread_self();
#else
		*phThread = ::GetCurrentThread();
#endif
	}

	void JoinThread(HTHREAD* phThread)
	{
#ifndef _WIN32
		int r=pthread_join(*phThread, NULL);
		if( r ) GenErrnoErr("pthread_join failed.",r);
#else
		switch ( WaitForSingleObject(*phThread, INFINITE) )
		{
		case WAIT_ABANDONED:
			GenErr("WaitForSingleObject return WAIT_ABADONED on a thread.");
		case WAIT_OBJECT_0:
			CloseHandle(*phThread);
			break;
		default:
			GenErrnoErr("WaitForSingleObject failed.");
		}
#endif
	}

	HTHREADID  GetCurTID()
	{
#ifndef _WIN32
		return gettid();
#else
		return ::GetCurrentThreadId();
#endif
	}

	HPID GetCurPID()
	{
#ifdef _WIN32
		return GetCurrentProcessId();
#else
		return getpid();
#endif
	}

	//////////////////////////////////////////////////////////////////////////
	// Lock section
	void CreateLock(HLOCK* pLock)
	{
#ifndef _WIN32
		pthread_mutexattr_t attr;
		int r;		

		r=pthread_mutexattr_init(&attr);
		if( r ) GenErrnoErr( "pthread_mutextattr_init failed with error:",r);

		r=pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE_NP);
		if( r ) GenErrnoErr( "pthread_mutextattr_init failed with error:",r);

		r=pthread_mutex_init(pLock, &attr);
		if( r ) GenErrnoErr( "pthread_mutex_init failed with error:",r);

		r=pthread_mutexattr_destroy(&attr);
		if( r ) GenErrnoErr( "pthread_mutexattr_destroy failed with error:",r);
#else
		InitializeCriticalSectionAndSpinCount(pLock, 4000);
#endif
	}

	void DestroyLock(HLOCK* pLock)
	{
#ifndef _WIN32
		int r=pthread_mutex_destroy(pLock);
		if( r ) GenErrnoErr( "pthread_mutex_destroy failed with error:",r);
#else
		DeleteCriticalSection(pLock);
#endif
	}

	void Lock(HLOCK* pLock)
	{
#ifndef _WIN32
		int r= pthread_mutex_lock(pLock);
		if( r ) GenErrnoErr( "pthread_mutex_lock failed with error:",r);
#else
		EnterCriticalSection(pLock);
#endif
	}

	void Unlock(HLOCK* pLock)
	{
#ifndef _WIN32
		int r=pthread_mutex_unlock(pLock);
		if( r ) GenErrnoErr( "pthread_mutex_unlock failed with error:",r);
#else
		LeaveCriticalSection(pLock);
#endif
	}

	// Semaphore section
	void CreateSemaphore(HSEMAPHORE* phSemaphore, 
		int32 nInitCount, int32 nMaxCount)
	{
#ifndef _WIN32
		if( sem_init(phSemaphore, 0, nInitCount) )
			GenErrnoErr( "sem_init failed." );
#else
		*phSemaphore = ::CreateSemaphoreA(NULL, nInitCount, nMaxCount, NULL);
		if(!*phSemaphore)
			GenErrnoErr( "::CreateSemaphore failed." );
#endif
	}

	void PutSemaphore(HSEMAPHORE* phSemaphore)
	{
#ifndef _WIN32
		if( sem_post(phSemaphore) )
			GenErrnoErr( "sem_post failed." );
#else
		if( !ReleaseSemaphore(*phSemaphore, 1, NULL) )
			GenErrnoErr( "ReleaseSemaphore failed." );
#endif
	}

	int32 GetSemaphore(HSEMAPHORE* phSemaphore)
	{
#ifndef _WIN32
		for(;;)
		{
			if(0==sem_wait(phSemaphore))
				return 0;

			switch(errno)
			{
			case EINTR:
				continue;
			default:
				return -1;
			}
		}
#else
		if (WAIT_FAILED ==WaitForSingleObjectEx(*phSemaphore, INFINITE, FALSE) )
			return -1;
		return 0;
#endif

	}

	int32 GetSemaphore(HSEMAPHORE* phSemaphore, unsigned uMilliSecs)
	{
#ifndef _WIN32
		struct timespec ts;
		if(clock_gettime(CLOCK_REALTIME,&ts))
		{
			ostringstream strm;
			strm<<"clock_gettime failed and last error is:"<<strerror(errno);
			GenErr(strm.str());
		}

		unsigned uDiffSec=uMilliSecs/1000;
		ts.tv_sec+=uDiffSec;
		ts.tv_nsec+=(uMilliSecs-uDiffSec*1000)*1000000;

		const unsigned max_nsec = 1000 * 1000000;
		if(ts.tv_nsec >= max_nsec)
		{
			//man文档上说，如果tv_nsec超过了1000000000，那么sem_timedwait会出EINVAL的错误
			//把nsec转换成sec
		
			unsigned uSec = ts.tv_nsec / max_nsec;
			ts.tv_sec += uSec;
			ts.tv_nsec -= uSec * max_nsec;
		}

		for(;;)
		{
			if(0==sem_timedwait(phSemaphore,&ts))
				return 0;

			switch(errno)
			{
			case ETIMEDOUT:
				return 1;
			case EINTR:
				continue;
			default:
				return -1;
			}
		}
#else
		switch (WaitForSingleObjectEx(*phSemaphore, uMilliSecs, FALSE) )
		{
		case WAIT_OBJECT_0:
			return 0;
		case WAIT_TIMEOUT:
			return 1;
		default:
			return -1;
		}
#endif
	}

	void DestroySemaphore(HSEMAPHORE* phSemaphore)
	{
#ifndef _WIN32
		if( sem_destroy(phSemaphore) )
			GenErrnoErr("sem_destroy failed.");
#else
		if( !CloseHandle(*phSemaphore) )
			GenErrnoErr("::CloseHandle on semaphore failed.");
#endif
	}


	//spin lock
	void CreateSpinLock(HSPINLOCK* phSpinLock)
	{
#ifndef _WIN32
		pthread_spin_init(phSpinLock, 0);
#else
		*phSpinLock = 0;
#endif
	}

	void SpinLock(HSPINLOCK* phSpinLock)
	{
#ifndef _WIN32
#if 0
		long oldvar = 0;                                                              
		long newvar = 1;                                                              
		while(__sync_val_compare_and_swap(phSpinLock, oldvar, newvar)){              
			usleep(0);                                                                
		}      
#endif
		pthread_spin_lock(phSpinLock);
#else
		LONG	lExchange = 1;
		LONG	lComparand = 0;

		while (InterlockedCompareExchange(phSpinLock, lExchange, lComparand))
		{
			Sleep(0);
		}
#endif
	}

	void SpinUnlock(HSPINLOCK* phSpinLock)
	{
#ifndef _WIN32
#if 0
		long oldvar = 1;                                                              
		long newvar = 0;                                                              
		__sync_val_compare_and_swap(phSpinLock, oldvar, newvar);
#endif
		pthread_spin_unlock(phSpinLock);
#else
		LONG	lExchange = 0;
		LONG	lComparand = 1;

		InterlockedCompareExchange(phSpinLock, lExchange, lComparand);
#endif
	}

	void DestroySpinLock(HSPINLOCK* phSpinLock)
	{
#ifndef _WIN32
		pthread_spin_destroy(phSpinLock);
#endif
	}

	//thread local storage
	void TLS_CreateKey(HTLSKEY* key)
	{
#ifdef _WIN32
		*key  = TlsAlloc();
		if (*key  == TLS_OUT_OF_INDEXES)
			GenErr("TLS out of indexes");
#else
		int r=pthread_key_create(key, NULL);
		if(r)GenErrnoErr("pthread_key_create failed:",r);
#endif
	}

	void TLS_DestroyKey(HTLSKEY key)
	{
#ifdef _WIN32
		TlsFree(key);
#else
		int r=pthread_key_delete(key);
		if(r)GenErrnoErr("pthread_key_delete failed:",r);
#endif
	}

	void TLS_SetValue(HTLSKEY key, void* pData)
	{
#ifdef _WIN32
		TlsSetValue(key, pData);
#else
		int r=pthread_setspecific(key, pData);
		if(r) GenErrnoErr("pthread_setspecific failed:",r);
#endif	
	}

	void* TLS_GetValue(HTLSKEY key)
	{
#ifdef _WIN32
		return TlsGetValue(key);
#else
		return pthread_getspecific(key);
#endif
	}

	void WatchThread(HTHREAD hThread,const char* szThreadName,volatile uint64* pTimeVar,uint64 uAllowedDelay,uint32 uAllowedTimes,uint64 uSlowValue)
	{
		if( CThreadWatcherMap::Inst() )
			CThreadWatcherMap::Inst()->AddThread(hThread, szThreadName,pTimeVar, uAllowedDelay, uAllowedTimes, uSlowValue);
	}

	void DoNotWatchThread(HTHREAD hThread)
	{
		if( CThreadWatcherMap::Inst() )
			CThreadWatcherMap::Inst()->DelThread(hThread);
	}

	void EnableWatchThread(HTHREAD hThread, bool bEnabled)
	{
		if( CThreadWatcherMap::Inst() )
			CThreadWatcherMap::Inst()->EnableWatch(hThread, bEnabled);
	}

	static HTLSKEY m_StackKey; 

	void InitAltSigStack()
	{
#ifndef _WIN32
		TLS_CreateKey(&m_StackKey);
#endif
	}

	void UnitAltSigStack()
	{
#ifndef _WIN32
		TLS_DestroyKey(m_StackKey);
#endif
	}

	void SetAltSigStackEnabled(bool bEnabled)
	{
#ifndef _WIN32
		if (bEnabled)
		{
			void* stack_buf = TLS_GetValue(m_StackKey);
			if (stack_buf)
			{
				return;
			}
			else
			{
				stack_t ss;

				const size_t stSize = 1 * 1024 * 1024;
				stack_buf = malloc(stSize);
				TLS_SetValue(m_StackKey, stack_buf);
				ss.ss_sp = stack_buf;
				ss.ss_size = stSize;
				ss.ss_flags = SS_ONSTACK;

				sigaltstack(&ss, NULL);	
			}
		}
		else
		{
			void* stack_buf = TLS_GetValue(m_StackKey);
			if (!stack_buf)
			{	
				return;
			}
			else
			{
				free(stack_buf);
				TLS_SetValue(m_StackKey, NULL);

				stack_t ss;
				ss.ss_flags = SS_DISABLE;
				sigaltstack(&ss, NULL);	
			}
		}
#endif

	}


	class CThreadNameMgr
	{
	public:
		CThreadNameMgr();
		~CThreadNameMgr();

		void SetName(const char* szName);
		const char* GetName();

	private:
		typedef basic_string<char,std::char_traits<char>,
			TMallocAllocator<char> >	String_t;

		HTLSKEY		m_Key;
	};

	CThreadNameMgr::CThreadNameMgr()
	{
		TLS_CreateKey(&m_Key);
	}

	CThreadNameMgr::~CThreadNameMgr()
	{
		TLS_DestroyKey(m_Key);
	}

	void CThreadNameMgr::SetName(const char* szName)
	{
		void* pValue = TLS_GetValue(m_Key);
		if (!pValue)
		{
			if (!szName)
			{
				return;
			}

			void* buffer = malloc(sizeof(String_t));
			String_t* pStr = new (buffer)String_t;
			pStr->assign(szName);
			TLS_SetValue(m_Key, pStr);
		}
		else
		{
			String_t* pStr = (String_t*)pValue;
			if (szName)
			{
				pStr->assign(szName);
			}
			else
			{
				pStr->~String_t();
				free(pStr);
				TLS_SetValue(m_Key, NULL);
			}
		}
	}

	const char* CThreadNameMgr::GetName()
	{
		void* pValue = TLS_GetValue(m_Key);	
		if (pValue)
		{
			String_t* pStr = (String_t*)pValue;
			return pStr->c_str();
		}
		else
		{
			return NULL;
		}
	}

	static CThreadNameMgr ls_ThreadMgr;

	void SetThreadName(const char* szName)
	{
		ls_ThreadMgr.SetName(szName);
	}

	const char* GetThreadName()
	{
		return ls_ThreadMgr.GetName();
	}
}	


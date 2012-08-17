#pragma once

#ifndef _WIN32
#include <semaphore.h>
#include <unistd.h>
#include <pthread.h>
#else
#define WIN32_LEAN_AND_MEAN		// 从 Windows 头中排除极少使用的资料
#include <windows.h>
#endif


namespace sqr
{
#define _THREAD_PRIORITY_LOWEST          -2
#define _THREAD_PRIORITY_BELOW_NORMAL    -1
#define _THREAD_PRIORITY_NORMAL          0
#define _THREAD_PRIORITY_ABOVE_NORMAL    1
#define _THREAD_PRIORITY_HIGHEST         2



#if defined(_WIN32)

	typedef	HANDLE				HTHREAD;
	typedef	CRITICAL_SECTION	HLOCK;
	typedef LONG				HSPINLOCK;
	typedef HANDLE				HSEMAPHORE;
	typedef DWORD				HTHREADID;
	typedef DWORD				HTLSKEY;
	typedef	DWORD				HPID;

#define INVALID_TLS_KEY		((DWORD)0xFFFFFFFF)

#else

	typedef	pthread_t			HTHREAD;
	typedef pthread_mutex_t		HLOCK;
	typedef sem_t				HSEMAPHORE;
	typedef pthread_spinlock_t	HSPINLOCK;
	typedef pid_t				HTHREADID;
	typedef pthread_key_t		HTLSKEY;
	typedef pid_t				HPID;

#define SQRSTDCALL
#define INVALID_TLS_KEY		0xFFFFFFFF

#endif

	typedef void (*THREADPROC )( void * );
}


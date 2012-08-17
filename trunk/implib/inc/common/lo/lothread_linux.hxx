#ifndef __lo_THREAD_LINUX_H__
#define __lo_THREAD_LINUX_H__


#include <sys/time.h>
#include <errno.h>

// criticalsection
typedef pthread_mutex_t THREAD_CRITICAL_SECTION,*LPTHREAD_CRITICAL_SECTION;

void initialize_critical_section(LPTHREAD_CRITICAL_SECTION pSec)
{
	if( NULL == pSec ){ 
		assert(false);
		return ;
	}
	pthread_mutexattr_t attr;
	pthread_mutexattr_init(&attr);
	pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE);
	pthread_mutex_init(pSec, &attr);
}
#define THREAD_INITIALIZE_SECTION(s)	initialize_critical_section(s)	
#define THREAD_DELETE_SECTION(s)	pthread_mutex_destroy(s);
#define THREAD_ENTER_SECTION(s)		pthread_mutex_lock(s);
#define THREAD_LEAVE_SECTION(s)		pthread_mutex_unlock(s);


// Event

/**
 * @brief 事件封装
 * @author welbon
 * @date   2010-09-14
 */
typedef struct THREAD_EVENT
{
	//为了防止竞争，条件变量的使用总是和一个互斥锁结合在一起。
	//Linux平台互斥结构体对象
	pthread_mutex_t		mutex_;
	
	//Linux条件变量结构体对象
	pthread_cond_t		cond_;
	
} THREAD_EVENT,*THREAD_EVENT_HANDLE;

//
typedef void* LPSERCURITY_ATTRIBUTES;
typedef char* LPTSTR;

#define  INFINITE -1



THREAD_EVENT_HANDLE	bb_CreateEvent(LPSERCURITY_ATTRIBUTES lpEventAttributes,BOOL bManualReset, BOOL bInitialState, LPTSTR lpName);
BOOL 			bb_CloseEvent(THREAD_EVENT_HANDLE hEvent);
BOOL 			bb_SetEvent(THREAD_EVENT_HANDLE hEvent);
BOOL 			bb_ResetEvent(THREAD_EVENT_HANDLE hEvent);
int  			bb_WaitEvent(THREAD_EVENT_HANDLE hEvent,const long lTime);


#define THREAD_CREATE_EVENT(e,lpEventAttributes,bManualReset,bInitialState,lpName)\
bb_CreateEvent(e,lpEventAttributes,bManualReset,bInitialState,lpName)

#define THREAD_DESTROY_EVENT(e)\
bb_CloseEvent(e)

#define THREAD_SET_EVENT(e)\
bb_SetEvent(e)

#define THREAD_RESET_EVENT(e)\
bb_ResetEvent(e)

#define THREAD_WAITING_EVENT(e,l)\
bb_WaitEvent(e,l)

void bb_CreateEvent(THREAD_EVENT_HANDLE hEvent,
					LPSERCURITY_ATTRIBUTES lpEventAttributes, 
					BOOL bManualReset, 
					BOOL bInitialState, 
					LPTSTR lpName)

{
	if( !hEvent ){
		hEvent = (THREAD_EVENT_HANDLE)malloc(sizeof(THREAD_EVENT)) ;
		memset(hEvent,0,sizeof(THREAD_EVENT));
	}
	
	pthread_mutexattr_t attr;
	pthread_mutexattr_init(&attr);
	pthread_mutex_init(&hEvent->mutex_, &attr);
	pthread_mutexattr_destroy(&attr);
	pthread_cond_init(&hEvent->cond_, NULL);
}

BOOL bb_CloseEvent(THREAD_EVENT_HANDLE hEvent)
{
	if( !hEvent )
		return FALSE;
	
	pthread_cond_destroy(&hEvent->cond_);
	pthread_mutex_destroy(&hEvent->mutex_);
	
	free(hEvent);
	
	return TRUE;
}

BOOL bb_SetEvent(THREAD_EVENT_HANDLE hEvent)
{
	if( !hEvent )
		return FALSE;
	
	
	pthread_mutex_lock(&hEvent->mutex_);
	//唤醒所有被阻塞在条件变量mhCond_t上的线程。
	pthread_cond_broadcast(&hEvent->cond_);
	pthread_mutex_unlock(&hEvent->mutex_);
	
	return TRUE;
}

BOOL bb_ResetEvent(THREAD_EVENT_HANDLE hEvent)
{
	if( !hEvent )
		return FALSE;
	
	//
	// UNIX 不支持
	return TRUE;
}

int bb_WaitEvent(THREAD_EVENT_HANDLE hEvent,const long dwTime)
{
	if( !hEvent )
		return 0;
	
	if (dwTime == (uint32)-1)
	{
		//使线程阻塞在一个条件变量的互斥锁上，无条件等待
		pthread_mutex_lock(&hEvent->mutex_);
		pthread_cond_wait(&hEvent->cond_, &hEvent->mutex_);
		pthread_mutex_unlock(&hEvent->mutex_);
		return 0;
	}
	
	struct timeval now;      /*time when we started waiting*/ 
	struct timespec timeout; /*timeout value for the wait function */ 
	
	pthread_mutex_lock(&hEvent->mutex_);		//Lock
	//取当前时间
	gettimeofday(&now, NULL); 
	//准备时间间隔值        
	timeout.tv_sec  = now.tv_sec + dwTime / 1000; 
	timeout.tv_nsec = ((now.tv_usec + dwTime) % 1000) * 1000;        
	
	//使线程阻塞在一个条件变量的互斥锁上，计时等待
	int ldwResult = pthread_cond_timedwait(&hEvent->cond_, &hEvent->mutex_, &timeout);
	pthread_mutex_unlock(&hEvent->mutex_);		//UnLock
	if(ldwResult == ETIMEDOUT)
	{
		return -1;
	}
	return 0;
}


#endif //__lo_THREAD_LINUX_H__
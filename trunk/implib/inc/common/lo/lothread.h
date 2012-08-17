#ifndef __lo_THREAD_H__
#define __lo_THREAD_H__

/**
* 
* @filebrief: 封装线程操作
* 
* @author: welbon
* 
* 
* @date: 2010-09-14
*/


#if defined(WIN32) || defined(_WIN32) || defined(WINCE)

#include <windows.h>

// criticalsection
typedef CRITICAL_SECTION THREAD_CRITICAL_SECTION, *LPTHREAD_CRITICAL_SECTION;

#define THREAD_INITIALIZE_SECTION(s) 	InitializeCriticalSection(s)
#define THREAD_DELETE_SECTION(s)	    DeleteCriticalSection(s)
#define THREAD_ENTER_SECTION(s)		    EnterCriticalSection(s)
#define THREAD_LEAVE_SECTION(s)		    LeaveCriticalSection(s)

typedef HANDLE THREAD_EVENT_HANDLE;

#define THREAD_CREATE_EVENT(e,lpEventAttributes,bManualReset,bInitialState,lpName)\
	if (!e) e = ::CreateEvent(lpEventAttributes,bManualReset,bInitialState,lpName);

#define THREAD_DESTROY_EVENT(e)\
	if (e){ \
	::CloseHandle(e);\
	e = 0;}

#define THREAD_SET_EVENT(e)\
	if( e ) ::SetEvent(e);

#define THREAD_RESET_EVENT(e)\
	if (e) ::ResetEvent(e);

#define THREAD_WAITING_EVENT(e,l)\
	if (e) WaitForSingleObject(e,l)

#else //!//defined(WIN32) || defined(_WIN32)

#include "lothread_linux.hxx"

#endif


#endif //__lo_THREAD_H__


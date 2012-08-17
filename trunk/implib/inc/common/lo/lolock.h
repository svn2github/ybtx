#ifndef __lo_LOCK_H__
#define __lo_LOCK_H__

#include <common/lo/lodefine.h>
#include <common/lo/lothread.h>

DEFINE_NAMESPACE(locom)

// Linux , UNIX下面不支持信标对象
#if defined(WIN32) || defined(_WIN32) || defined(WINCE)

class CloSemaphore
{
private:
	CloSemaphore& operator=(const CloSemaphore& r);
public:
	CloSemaphore(HANDLE* psem)
		:m_psem(psem)
	{
		*m_psem = ::CreateSemaphore(0,1,0XFFFF,0);
	}
	~CloSemaphore()
	{
		ReleaseSemaphore(*m_psem,1,0);
		CloseHandle(*m_psem);
	}
	void Lock()
	{
		::WaitForSingleObject(*m_psem,INFINITE);
	}
	void UnLock()
	{
		ReleaseSemaphore(*m_psem,1,0);
	}
private:
	HANDLE*   m_psem;
};

class CloMetuex
{
private:
	HANDLE *m_pHandle;

public:
	CloMetuex(HANDLE *pHandle)
		:m_pHandle(pHandle)
	{
		*m_pHandle = CreateMutex(NULL,FALSE,NULL);
	}

	~CloMetuex()
	{
		CloseHandle(*m_pHandle);
	}

	void Lock()
	{
		WaitForSingleObject(*m_pHandle,INFINITE);
	}

	void UnLock()
	{
		ReleaseMutex(*m_pHandle);
	}
};

#endif // 

class CloCriticalSection
{
private:
	LPTHREAD_CRITICAL_SECTION m_pSec;

public:
	CloCriticalSection( LPTHREAD_CRITICAL_SECTION pSec )
		:m_pSec(pSec)
	{
		if ( m_pSec )
		{
			::THREAD_INITIALIZE_SECTION(m_pSec);
		}

	}

	~CloCriticalSection()
	{
		if ( m_pSec )
		{
			::THREAD_DELETE_SECTION(m_pSec);
		}
	}

	void Lock()
	{
		if ( m_pSec )
		{
			::THREAD_ENTER_SECTION(m_pSec);
		}

	}

	void UnLock()
	{
		if ( m_pSec )
		{
			::THREAD_LEAVE_SECTION(m_pSec);
		}
	}
};

class CloEvent
{
protected:
	THREAD_EVENT_HANDLE m_hHandle;

public:
	CloEvent(THREAD_EVENT_HANDLE hHandle,BOOL bManual)
		:m_hHandle(hHandle)
	{
		THREAD_CREATE_EVENT(m_hHandle,NULL,bManual,FALSE,NULL);
	}

	~CloEvent()
	{
		THREAD_DESTROY_EVENT(m_hHandle);
	}
};

class CloManualEvent : public CloEvent
{
public:
	CloManualEvent(THREAD_EVENT_HANDLE hHandle)
		:CloEvent(hHandle,TRUE)
	{
	}

	void Lock()
	{
		//
		//WaitForSingleObject(*m_pHandle,INFINITE);
		//ResetEvent(*m_pHandle);
		//
		THREAD_WAITING_EVENT(m_hHandle,INFINITE);
		THREAD_RESET_EVENT(m_hHandle);
	}

	void UnLock()
	{
		//使其有信号
		//SetEvent(*m_pHandle);
		//
		THREAD_SET_EVENT(m_hHandle);
	}
};

class CloAutoEvent : public CloEvent
{
public:
	CloAutoEvent(THREAD_EVENT_HANDLE hHandle)
		:CloEvent(hHandle,FALSE)
	{

	}
	void Lock()
	{
		//
		//WaitForSingleObject(*m_pHandle,INFINITE);
		//
		THREAD_WAITING_EVENT(m_hHandle,INFINITE);		
	}
	void UnLock()
	{
		//使其有信号
		//SetEvent(*m_pHandle);
		//
		THREAD_SET_EVENT(m_hHandle);
	}
};

class CloSignalEvent : public CloEvent
{
private:
	THREAD_EVENT_HANDLE m_hThreadEvent;
public:

	CloSignalEvent()
		:CloEvent(m_hThreadEvent,FALSE)
	{

	}
	void Wait(int cms)
	{
		//
		//WaitForSingleObject(hHandle,cms);
		//
		THREAD_WAITING_EVENT(m_hThreadEvent,cms);
	}

	void WakeUp()
	{
		//使其有信号
		//SetEvent(hHandle);
		//
		//
		THREAD_SET_EVENT(m_hThreadEvent);
	}

	THREAD_EVENT_HANDLE Handle(void) const{ 
		return (const THREAD_EVENT_HANDLE)&m_hThreadEvent;
	}
};

template<typename Type, typename LockPolicy>
class CloSmartLock
{
private:
	Type		m_Type;
	LockPolicy	m_Policy;

public:
	CloSmartLock():m_Policy(&m_Type)
	{
	}

	void Lock()
	{
		m_Policy.Lock();
	}

	void UnLock()
	{
		m_Policy.UnLock();
	}
};

template<typename Type, typename LockPolicy>
class CloAutoLock
{
private:
	CloSmartLock<Type,LockPolicy> *m_pLock;
public:
	CloAutoLock(CloSmartLock<Type,LockPolicy> *pLock)
		:m_pLock(pLock)
	{
		m_pLock->Lock();
	}

	~CloAutoLock()
	{
		m_pLock->UnLock();
	}
};

END_NAMESPACE(locom)

#endif //__lo_LOCK_H__
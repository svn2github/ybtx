#include "stdafx.h"
#ifdef _WIN32
#include "CReadThread.h"
#include "ThreadHelper.h"
#include "ExpHelper.h"
#include "BaseHelper.h"
#include "CCpuCheckPoint.h"
#include "ErrLogHelper.h"

CReadThread*& CReadThread::Inst()
{
	static CReadThread*	ls_pInst;
	return ls_pInst;
}

void CReadThread::Init()
{
	if( Inst() )
		return;
	Ast(!Inst());
	Inst()=new CReadThread;
}

void CReadThread::Unit()
{	
	SafeDelete(Inst());	
	return;	
}

void CReadThread::Read(HANDLE hFile,void* pBuffer,uint32 uBufferSize,LPOVERLAPPED pOverlapped,
					   void (*pCallBack)(uint32 dwNumberOfBytesTransfered, LPOVERLAPPED lpOverlapped))
{
	CReadRequest* pRequest=new CReadRequest;
	pRequest->m_hFile=hFile;
	pRequest->m_pBuffer=pBuffer;
	pRequest->m_uBufferSize=uBufferSize;
	pRequest->m_pOverlapped=pOverlapped;
	pRequest->m_pCallBack=pCallBack;

	Lock(&m_lkCallBack);
	m_lstRequest.push_back(pRequest);
	Unlock(&m_lkCallBack);

	PutSemaphore( &m_semMsg );
}

CReadThread::CReadThread()
{
	CreateLock(&m_lkCallBack);
	CreateSemaphore(&m_semMsg,0,INT_MAX);
	sqr::CreateThread(&m_hThread,0,&ThreadProc,this);

#ifdef _DEBUG
	SetThreadPriority(&m_hThread,BELOW_NORMAL_PRIORITY_CLASS);
#else
	SetThreadPriority(&m_hThread,ABOVE_NORMAL_PRIORITY_CLASS);
#endif

}

CReadThread::~CReadThread()
{
	PutSemaphore( &m_semMsg );
	sqr::JoinThread(&m_hThread);
	DestroySemaphore( &m_semMsg );
	DestroyLock(&m_lkCallBack);

	while( ! m_lstRequest.empty() )
	{
		delete m_lstRequest.front();
		m_lstRequest.pop_front();
	}
}

void CReadThread::ReadThread()
{
	CCpuCheckPoint ccp;

	SQR_TRY
	{
		for(;;)
		{
			switch( GetSemaphore(&m_semMsg) )
			{
			case 0:
			case -1:
				break;
			default:
				GenErr("Invalid error code from GetSemaphore");
			}

			Lock(&m_lkCallBack);
			if( m_lstRequest.empty() )
			{
				Unlock(&m_lkCallBack);
				return;
			}
			CReadRequest* pData=m_lstRequest.front();
			m_lstRequest.pop_front();
			Unlock(&m_lkCallBack);

			DWORD uDataRead;
			SetFilePointer( pData->m_hFile, pData->m_pOverlapped->Offset,NULL, FILE_BEGIN);
			if (!ReadFile(pData->m_hFile,pData->m_pBuffer,pData->m_uBufferSize,&uDataRead,NULL))
			{
				ostringstream strm;
				strm<<"ReadFile failed with error code:"<<GetLastError();
			}


			pData->m_pCallBack(uDataRead,pData->m_pOverlapped);

			SafeDelete(pData);
		}
	}
	SQR_CATCH( exp )
	{
		LogExp( exp );
	}
	SQR_TRY_END;
}

void CReadThread::ThreadProc(void* pArg)
{
#ifdef _WIN32
	if ( _configthreadlocale(_ENABLE_PER_THREAD_LOCALE) == -1 )
		GenErr("_configthreadlocale failed.");
#endif

	SetThreadName("ReadThread");

	static_cast<CReadThread*>(pArg)->ReadThread();
}

#endif


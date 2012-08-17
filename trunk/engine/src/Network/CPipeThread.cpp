#include "stdafx.h"
#include "CPipeThread.h"
#include "ThreadHelper.h"
#include "CPipeThreadMgr.h"
#include "ExpHelper.h"
#include <time.h>
#include "CPipeModule.h"
#include "CSyncPipeReactor.h"
#include "CAsynPipeReactor.h"
#include "TimeHelper.h"

#include "CPipeJob.inl"
#include "CPipeResult.inl"

#include "TTrThreadMsgBuffer.inl"
#include "CPipeTrMsgBufferSwapper.h"
#include "TSqrAllocator.inl"

template class TTrThreadMsgBuffer<CPipeThreadTraits>;

CPipeThread::CPipeThread(CPipeThreadMgr* pMgr, CSyncPipeReactor* pReactor, uint32 uThreadId)
: Parent_t(1024, 1024 * 1024, CPipeTrMsgBufferSwapper::Inst())
{
	m_pThreadMgr=pMgr;
	m_pReactor=pReactor;
	m_uThreadId=uThreadId;
	m_bQuit=false;
	m_stPipeNum=0;

	CreateThread(&m_hThread, 1024, &CPipeThread::Execute, this);
}

CPipeThread::~CPipeThread()
{
	JoinThread(&m_hThread);
	m_pReactor->Release();
}

void CPipeThread::Quit()
{
	m_bQuit = true;
}

void CPipeThread::Execute(void* pParam)
{
	SetThreadName("PipeThread");

	return reinterpret_cast<CPipeThread*>(pParam)->DoExecute();
}

void CPipeThread::DoExecute()
{
	SetAltSigStackEnabled(true);

#ifdef _WIN32
	if(CatchErrorEnabled())
		_set_se_translator(TransESHFun);
#endif

	volatile uint64 uTime=GetProcessTime();
	
	WatchThread( m_hThread , GetThreadName(), &uTime, 30*1000 , 2, 1000 );

	LowerCurThreadPriority();


	SQR_TRY
	{
		for(;;)
		{
#ifdef _WIN32
in:
#endif
			uTime = GetProcessTime();

			uint32 uResult = m_pReactor->GetEvent( 200 );

#ifdef _WIN32
			for( ;; )
			{
				switch(uResult)
				{
				case eGER_TimedOut:
					goto in;
				case eGER_NetMsg:
					FlushRightMsg();
					break;
				case eGER_Iocp:
					break;
				case eGER_Canceled:
					{
						HandleAllLeftMsg();
						FlushRightMsg();
						if( m_bQuit )
							goto out;
						break;
					}
				case eGER_SysMsg:
					{
						MSG msg;
						while( PeekMessage(&msg,NULL,0,0,1) )
						{
							if( msg.message != WM_NULL )
								GenErr("Network thread receive unknown windows message");
						}
					}
					break;
				default:
					{
						ostringstream strm;
						strm<<"m_pPipeReactor->GetEvent return unknown value "<<uResult;
						GenErr(strm.str());
					}
				}

				uResult = m_pReactor->GetEvent( 0 );
			}
#else
			for( ;; )
			{
				if( uResult & eGER_NetMsg )
				{
					FlushRightMsg();
				}

				if( uResult & eGER_Canceled )
				{
					HandleAllLeftMsg();
					FlushRightMsg();
					if( m_bQuit )
						goto out;
				}

				if( ! (uResult & eGER_Signal) )
					break;

				uResult =m_pReactor->GetEvent( 0 );
			}
#endif
		}

out:

		(new (this) CPipeThreadEndedResult())->Add(this);

		while(true)
		{
			bool bSuc = FlushRightMsg();
			if(bSuc)
				break;

			m_pReactor->GetEvent(uint32(-1));

			HandleAllLeftMsg();
		}
	}
	SQR_CATCH(exp)
	{
		LogExp(exp);
	}
	SQR_TRY_END;

	DoNotWatchThread( m_hThread );

	SetAltSigStackEnabled(false);
}

void CPipeThread::OnWakeRight()
{
	m_pReactor->CancelBlock();
}

void CPipeThread::AddPipe(int32 nPipeId,CSyncPipe* pPipe)
{
	Ver( m_mapId2SyncPipe.insert(make_pair(nPipeId, pPipe)).second );
}

void CPipeThread::DelPipe(int32 nPipeId)
{
	Ver( m_mapId2SyncPipe.erase( nPipeId ) );
}

CSyncPipe* CPipeThread::GetPipe(int32 nPipeId)const
{
	MapSyncPipe::const_iterator it = m_mapId2SyncPipe.find(nPipeId);
	if( it ==  m_mapId2SyncPipe.end() )
		return NULL;
	return it->second;
}

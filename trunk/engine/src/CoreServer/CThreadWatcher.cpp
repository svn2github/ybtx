#include "stdafx.h"
#include "CThreadWatcher.h"
#include "ExpHelper.h"
#include "ErrLogHelper.h"
#include "ThreadHelper.h"
#include "CTickMgr.h"
#include "TimeHelper.h"
#include <csignal>
#include "CThreadWatcherMap.h"
#include "CCoreSigMsgMgr.h"
#include "CLock.inl"


typedef map< HTHREAD , CThreadLoopInfo*, 
	less<HTHREAD>, TMallocAllocator<pair<HTHREAD, CThreadLoopInfo*> > >	VarMap_t;

void CThreadWatcher::Begin()
{
	m_lkBeginEnd.Lock();

	if( m_uBeginCounter++ == 0)
	{	
		CreateThread( &m_hThread,512,&CThreadWatcher::EntryProc,this );
		CreateSemaphore( &m_semQuit, 0, 1 );

		printf("死循环检测开启\n");
	}

	m_lkBeginEnd.Unlock();
}

void CThreadWatcher::End()
{
	m_lkBeginEnd.Lock();

	if( --m_uBeginCounter == 0 )
	{
		Ast( m_uBeginCounter>= 0 );

		PutSemaphore( &m_semQuit );
		JoinThread( &m_hThread );
		DestroySemaphore( &m_semQuit );

		m_hThread =0;

		printf("死循环检测关闭\n");
	}

	m_lkBeginEnd.Unlock();
}

bool CThreadWatcher::Enabled()const
{
	return m_uBeginCounter > 0;
}

CThreadWatcher* CThreadWatcher::Inst()
{
	static CThreadWatcher Inst;
	return &Inst;
}

CThreadWatcher::CThreadWatcher(void)
{
	m_hThread = 0;
}

CThreadWatcher::~CThreadWatcher(void)
{
	if( m_hThread )
		GenErr("ThreadWatcher thread is still running!");
}

void CThreadWatcher::EntryProc( void* pArg)
{
	return static_cast<CThreadWatcher*>( pArg )->Execute();
}


void CThreadWatcher::CheckBurstCpuUsage()
{
	uint64 uRealTime=GetProcessTime();

	CThreadWatcherMap::Inst()->Lock();

	VarMap_t& mapVar = CThreadWatcherMap::Inst()->m_mapVar;

	VarMap_t::iterator itEnd = mapVar.end();


	for( VarMap_t::iterator it = mapVar.begin(); it!= itEnd; ++ it )
	{
		CThreadLoopInfo& Info = *it->second;

		if (!Info.m_bEnabled)
			continue;
		
		const uint64 uNewTime = *Info.m_pTimeVar;

		if( uRealTime <  uNewTime + Info.m_uSlowValue )
			continue;

		if( Info.m_uLogStackSignalCounter > 0 )
			continue;

#ifndef _WIN32
		if( CCoreSigMsgMgr::Inst().m_bJustContinued )
			break;
#endif

		fprintf( stderr, "线程\"%s\"循环过慢,在linux下将会记录堆栈 %d > %d\n" 
			,Info.m_szThreadName,uint32( uRealTime - uNewTime ), uint32(Info.m_uSlowValue) );

#ifndef _WIN32
		HTHREAD hThread = it->first;

		pthread_kill( hThread ,SIGUSR2 );
#endif

		++Info.m_uLogStackSignalCounter;
	}

	CThreadWatcherMap::Inst()->Unlock();
}


void CThreadWatcher::CheckInfiniteLoop()
{
	CThreadWatcherMap::Inst()->Lock();

	VarMap_t& mapVar = CThreadWatcherMap::Inst()->m_mapVar;

	VarMap_t::iterator itEnd = mapVar.end(); 		

	uint64 uRealTime=GetProcessTime();

	for( VarMap_t::iterator it = mapVar.begin(); it!= itEnd; ++ it )
	{
		CThreadLoopInfo& Info = *it->second;

		if (!Info.m_bEnabled)
			continue;

		const uint64 uNewTime = *Info.m_pTimeVar;

		if( uRealTime <  uNewTime + Info.m_uAllowedDelay )
			continue;

#ifndef _WIN32
		if( CCoreSigMsgMgr::Inst().m_bJustContinued )
			break;
#endif

		fprintf( stderr, "怀疑线程\"%s\"死循环,在linux下将会被唤醒 %d %d\n" 
			,Info.m_szThreadName
			,Info.m_uWakeSignalCounter
			,uint32( uRealTime - uNewTime ) );

#ifndef _WIN32
		HTHREAD hThread = it->first;

		if( Info.m_uWakeSignalCounter > Info.m_uAllowedTimes )
		{
			ostringstream strm;
			strm << Info.m_szThreadName << "死循环无法恢复，终止进程. ";
			strm << "秒数:" << (uRealTime - uNewTime);

			char sPath[256];
			getcwd(sPath, 255);
			char sFile[256];
			sprintf(sFile, "%s/ThreadWatch_%d.log", sPath, GetCurPID());
			FILE* pFile = fopen(sFile, "a+");
			if(pFile)
			{
				const char* pStr = strm.str().c_str();
				fwrite(pStr, sizeof(char), strlen(pStr), pFile);
				fclose(pFile);
			}

			pthread_kill( hThread , 45 );		//发送程序退出信号
			break;
		}
		pthread_kill( hThread ,SIGUSR1 );
		++Info.m_uWakeSignalCounter;
#endif
	}

	CThreadWatcherMap::Inst()->Unlock();
}



void CThreadWatcher::LowerSignalCounter()
{
	CThreadWatcherMap::Inst()->Lock();

	VarMap_t& mapVar = CThreadWatcherMap::Inst()->m_mapVar;

	VarMap_t::iterator itEnd = mapVar.end(); 		

	for( VarMap_t::iterator it = mapVar.begin(); it!= itEnd; ++ it )
	{
		CThreadLoopInfo& Info = *it->second;

		if( Info.m_uWakeSignalCounter > 0 )
			--Info.m_uWakeSignalCounter;

		if( Info.m_uLogStackSignalCounter > 0 )
			Info.m_uLogStackSignalCounter=0;
	}

	CThreadWatcherMap::Inst()->Unlock();
}


void CThreadWatcher::Execute()
{
	const uint32 uCheckLoopInterval=1000;
	const uint32 uCheckSlowInterval=100;

	const uint32 uTickInterval=uCheckSlowInterval;	

	const uint32 uTickSlowNum=uint32( uCheckLoopInterval/float(uCheckSlowInterval)+0.5f );

	CTickMgr TickMgr( uTickInterval, uTickSlowNum );
	
	TTickFunctionHandler<CThreadWatcher,&CThreadWatcher::CheckInfiniteLoop>		TickCheckLoop(this);
	TickMgr.Register( &TickCheckLoop, uCheckLoopInterval );

	TTickFunctionHandler<CThreadWatcher,&CThreadWatcher::CheckBurstCpuUsage>	TickCheckBurstCpuUsage(this);
	TickMgr.Register( &TickCheckBurstCpuUsage, uTickInterval );

	TTickFunctionHandler<CThreadWatcher,&CThreadWatcher::LowerSignalCounter>	TickLowerCounter(this);
	TickMgr.Register( &TickLowerCounter, uint32( CThreadWatcherMap::GetMaxAllowedDelay() ) );

	const uint32 uPauseTimeWhenContinue= 60 * 1000;

	for(;;)
	{
		int uInterval;

#ifndef _WIN32
		if( CCoreSigMsgMgr::Inst().m_bJustContinued )
		{
			printf("由于发生了进程暂停信号，死循环检测暂停%d毫秒。\n",int(uPauseTimeWhenContinue));
			uInterval = uPauseTimeWhenContinue;
			CCoreSigMsgMgr::Inst().m_bJustContinued = false;
		}
		else
		{
#endif
			uInterval = uTickInterval;
#ifndef _WIN32
		}
#endif

		switch(  GetSemaphore( &m_semQuit, uInterval ) )
		{
		case 0:
			goto out;
		case 1:
			if( uInterval == uPauseTimeWhenContinue )
				puts("死循环检测恢复。");
			break;
		default:
			GenErr("GetSemaphore return invalid message");
		}


		TickMgr.OnTick();
	}
out:

	TickMgr.UnRegister( &TickCheckBurstCpuUsage );
	TickMgr.UnRegister( &TickCheckLoop );
	TickMgr.UnRegister( &TickLowerCounter );
}

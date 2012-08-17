#include "stdafx.h"
#include "SyncSystem.h"
#include "CGraphicExp.h"
#include "CPkgFile.h"
#include "CResourceContainer.h"
#include "TraceHelper.h"
#include "mmsystem.h"
#include "ErrLogHelper.h"
#include "TSqrAllocator.inl"
#include "CWindowTarget.h"

#define  MaxSyncLoad 10

CSyncSystem::SyncList	CSyncSystem::st_listSyncCoders[SPRI_COUNT];
SyncCoder*		CSyncSystem::st_pActiveCoder= NULL;
uint64			CSyncSystem::st_SyncTime	= 0;
PVOID			CSyncSystem::st_pSyncFiber	= NULL;
PVOID			CSyncSystem::st_pCallThread	= NULL;
CTimeCheckPoint CSyncSystem::st_CheckPoint;
bool			CSyncSystem::st_isStopImme	= false;
//bool			CSyncSystem::st_isBusy		= false;
bool			CSyncSystem::st_isIdle		= true;
GVector<void*>	CSyncSystem::st_mapContext2Stack;
#define			_CODER_STOP_TIME_ 0xFFFFFFFFFFFFFFFE
void CSyncSystem::Create(void)
{
	ConvertThreadToFiber(NULL);

	st_pSyncFiber	= NULL;

	st_pSyncFiber	= CreateFiber(0,(LPFIBER_START_ROUTINE)(&CSyncSystem::SyncFun),NULL);
	st_SyncTime		= 0;
	st_pActiveCoder = NULL;
	st_pCallThread	= NULL;
	st_isStopImme	= false;
	//st_isBusy		= false;
	st_isIdle		= false;

	st_mapContext2Stack.resize(2);
	st_mapContext2Stack[0] = NULL;
	st_mapContext2Stack[1] = NULL;
}

//void CSyncSystem::Reset(void)
//{
//	//if(NULL!=st_pSyncFiber)
//	//	DeleteFiber(st_pSyncFiber);
//
//	
//}

void CSyncSystem::Destroy(void)
{
	//st_SyncTime = 0xFFFFFFFFFFFFFFFE;
	//st_isStopImme = true;
	//SwitchToFiber(st_pSyncFiber);
	//
	st_mapContext2Stack[0] = NULL;
	st_mapContext2Stack[1] = NULL;
	DeleteFiber(st_pSyncFiber);
	for( int i = 0; i< SPRI_COUNT; ++i )
	{
		SyncList::iterator it,eit = st_listSyncCoders[i].end();
		for( it = st_listSyncCoders[i].begin(); it!=eit; ++it )
			(*it)->Release();
		st_listSyncCoders[i].clear();
	}
}

void CSyncSystem::PushCoder(SyncCoder* pCoder)
{
	Ast(pCoder&&"Coder不能为空");
	SyncPRI fPri = pCoder->GetPRI();
	if(fPri == SPRI_NONE)
		return;
	st_listSyncCoders[fPri].push_back(pCoder);
	pCoder->PushIterator(SyncListIterator( fPri,--st_listSyncCoders[fPri].end() ));
	pCoder->AddRef();
}

void CSyncSystem::DelCoder(SyncCoder* pCoder)
{	
	if( pCoder->IsFree() )
	{
		pCoder->m_eSpri = SPRI_NONE;
		return;
	}
	
	SyncListIterator It;
	pCoder->PopIterator(It);
	st_listSyncCoders[It.m_eSpri].erase(It.m_SyncIt);
	pCoder->Release();
}

void CSyncSystem::SafeDelCoder(SyncCoder* pCoder)
{
	if(st_pActiveCoder==pCoder)
	{
		uint64  TempTime = st_SyncTime;
		st_SyncTime = _CODER_STOP_TIME_;
		st_isStopImme = true;
		ResumeFiber(st_pSyncFiber);
		st_SyncTime = TempTime;
	}	
	else
	{
		DelCoder(pCoder);
	}
}

//void CSyncSystem::Clear(void)
//{
//	for( int i = 0; i< SPRI_COUNT; ++i )
//	{
//		SyncList::iterator it,eit = st_listSyncCoders[i].end();
//		for( it = st_listSyncCoders[i].begin(); it!=eit;  )
//		{
//			if((*it)->GetRefCount() == 1)
//				SafeDelCoder(*(it++));
//			else
//				++it;
//		}
//	}
//}

void SyncCoder::SetPRI(SyncPRI pri)
{
	if(pri == m_eSpri)
		return;

	m_eSpri = pri;
	CSyncSystem::RefreshCoderPRI(this);
}

void CSyncSystem::RefreshCoderPRI(SyncCoder* pCoder)
{
	if(pCoder == st_pActiveCoder)
		return;
	
	int Num = 0;
	SyncListIterator It;
	while( !pCoder->IsFree() )
	{
		++Num;	
		pCoder->PopIterator(It);
		st_listSyncCoders[It.m_eSpri].erase(It.m_SyncIt);
	}
	
	for( int i = 0; i<Num; ++i )
	{
		PushCoder(pCoder);
		pCoder->Release();
	}
}

bool CSyncSystem::BeginSyncFun(uint64 time)
{	
	if( CMainWindowTarget::GetInst()->IsRenderActive() != S_OK )
		return true;

	st_SyncTime = time;	
	if(st_pCallThread==NULL)
	{
		st_CheckPoint.SetCheckPoint();
		//if( !(CPkgFile::ReadEmpty() && st_listSyncCoders.empty() && st_isIdle) )
		ResumeFiber(st_pSyncFiber);
	}
	return	st_isIdle;
}

bool CSyncSystem::StopSynFun(void)
{
	if(st_pCallThread)
	{
		PVOID pCalThread = st_pCallThread;
		st_pCallThread = NULL;
		ResumeFiber(pCalThread);
	}
	return st_isStopImme;
}

bool CSyncSystem::TryToStop(void)
{
	if(st_pCallThread)
	{
		if( st_CheckPoint.GetHDElapse() > st_SyncTime || (st_isStopImme && st_pActiveCoder != (SyncCoder*)0x1) )
			return StopSynFun();
	}
	return false;
}

void CSyncSystem::SyncFun(void)
{
	if(CatchErrorEnabled())
		_set_se_translator(TransESHFun);
	bool isFlushFull = true;
	while(true)
	{	
		SQR_TRY	
		{
			st_isIdle = false;
			
			SQR_TRY	
			{
				CResMgr::GetInst()->OptimizeRecycle();
				CoderStop();
			}
			SQR_CATCH(exp)
			{
				exp.AppendMsg("资源文件释放失败");
				GfkLogExp(exp);
			}
			SQR_TRY_END;


			SQR_TRY	
			{
				st_isStopImme = false;
				st_pActiveCoder = (SyncCoder*)0x1;
				isFlushFull = CPkgFile::Flush( st_SyncTime );
		
				st_pActiveCoder = NULL;
				if( st_isStopImme )
				{
					st_isStopImme = false;
					StopSynFun();
				}

				if(isFlushFull && st_listSyncCoders[SPRI_High].empty())
					continue;
			}
			SQR_CATCH(exp)
			{
				exp.AppendMsg("资源文件加载失败");
				GfkLogExp(exp);
			}
			SQR_TRY_END;
				
		
			SyncList* pCurSyncList = NULL;
			for( int i = 0; i< SPRI_COUNT; ++i )
			{
				if(!st_listSyncCoders[i].empty())
				{
					pCurSyncList = &(st_listSyncCoders[i]);
					break;
				}
			}
			
			if(pCurSyncList==NULL)
			{
				st_isIdle = true;
				StopSynFun();
				continue;
			}

			SyncList::iterator it = pCurSyncList->begin();
			st_pActiveCoder = *it;
			
			SQR_TRY	
			{
				if( !(*it)->AutoDeCode() )
					PushCoder( *it );				
			}
			SQR_CATCH(exp)
			{
				exp.AppendMsg("同步系统:解码失败:");
				GfkLogExp(exp);
			}
			SQR_TRY_END;	
	
			DelCoder( st_pActiveCoder );
			st_pActiveCoder = NULL;		
			CoderStop();
		}
		SQR_CATCH(exp)
		{
			GfkLogExp(exp);
		}
		SQR_TRY_END;
	}
}

void CSyncSystem::BeginSyncFun(IProcessCallBack* pSpro)
{
	uint32 lpCount = 0;
	HRESULT hr;
	while( (hr = CMainWindowTarget::GetInst()->IsRenderActive()) != S_OK )
	{
		if(lpCount > 100)
			return;

		if( hr == GRESET )
		{
			++lpCount;
			CMainWindowTarget::GetInst()->RestoreSize(false);
		}
		else
			Sleep(100);
	}

	st_SyncTime = _CODER_STOP_TIME_;
	if(NULL!=pSpro)
		pSpro->Clear();

	if(NULL!=st_pActiveCoder)
	{
		if(st_pCallThread!=NULL)
			GfkLogErr("CSyncSystem::出现异常");
		st_CheckPoint.SetCheckPoint();
		ResumeFiber(st_pSyncFiber);
	}

	SyncList TempCoder;
	

	uint32 TotalSize = st_listSyncCoders[SPRI_High].size()+st_listSyncCoders[SPRI_Middle].size()+st_listSyncCoders[SPRI_Low].size();
	
	for( int i = 0; i< SPRI_COUNT; ++i )
	{
		CPkgFile::Flush( st_SyncTime );
		while(!st_listSyncCoders[i].empty())
		{		
			SyncList::iterator it = st_listSyncCoders[i].begin();
			st_isStopImme = false;
			SQR_TRY	
			{
				bool isComplate = true;
				for(int i = 0; i<MaxSyncLoad; ++i )
				{
					if( isComplate = (*it)->AutoDeCode() )
					{
						break;
					}
				}

				if(!isComplate)
				{
					TempCoder.push_back( *it );
					(*it)->AddRef();
				}
			}
			SQR_CATCH(exp)
			{
				exp.AppendMsg("解码失败");
				GfkLogExp(exp);
			}
			SQR_TRY_END;

			if( (*it)->IsFree() )
			{
				st_listSyncCoders[i].erase(it);
				GfkLogErr("强行删除空置Coder");
			}
			else
				DelCoder( *it );
			
			uint32 CurSize = st_listSyncCoders[SPRI_High].size()+st_listSyncCoders[SPRI_Middle].size()+st_listSyncCoders[SPRI_Low].size();;
			if(CurSize>TotalSize)TotalSize=CurSize;
			
			if(NULL!=pSpro)
				pSpro->DoProcess( float(TotalSize - CurSize)/float(TotalSize) );

			if(st_listSyncCoders[SPRI_Middle].size()!=0)
				i = SPRI_Middle;

			if(st_listSyncCoders[SPRI_High].size()!=0)
				i = SPRI_High;
		}
	}
	SyncList::iterator it,eit = TempCoder.end();
	for( it = TempCoder.begin(); it!=eit; ++it )
	{
		PushCoder( *it );
		(*it)->Release();
	}
}

void CSyncSystem::CoderStop(void)
{
	if(st_SyncTime != _CODER_STOP_TIME_ )
		TryToStop();
	else
		StopSynFun();
}

void SyncCoder::PopIterator( SyncListIterator& Iter )
{
	Iter = m_SyncIters.front();
	m_SyncIters.pop();
}

void SyncCoder::PushIterator( SyncListIterator& Iter)
{
	m_SyncIters.push(Iter);
}

void CSyncSystem::ResumeFiber(void* pFiber)
{
	/*
		我们在切换一个fiber的时候，同时要处理容错系统，使得根据不同的fiber对应不同的stack
		以防止，在一个fiber发生错误，在另一个fiber里面接住，导致堆栈错乱
	*/
	if(pFiber == st_pSyncFiber)
		st_pCallThread = GetCurrentFiber();

	//得到切换fiber之前的context stack
	void* pSaveStack = GetCurrentContextStack();

	//保存context stack
	void* pCurFiber = GetCurrentFiber();
	st_mapContext2Stack[pCurFiber!=st_pSyncFiber] = pSaveStack;

	//得到切换fiber处的context stack，并将其设置为当前的
	void* pStack = st_mapContext2Stack[pCurFiber==st_pSyncFiber];
	SetCurrentContextStack(pStack);

	SwitchToFiber(pFiber);
}


#include "stdafx.h"
#include "CAoiThreadMgr.h"
#include "CAoiThread.h"
#include "ThreadHelper.h"
#include "CAoiJob.inl"
#include "ExpHelper.h"
#include "BaseHelper.h"
#include "CSyncAppServer.h"
#include "CAoiResult.inl"
#include "TimeHelper.h"
#include "ErrLogHelper.h"
#include "TTrThreadMsgBuffer.inl"
#include "CSyncServerTrMsgBufferMgrSwapper.h"
#include "TSqrAllocator.inl"
#include "CAoi2LogicThreadMsgBuffer.h"


template class TTrThreadMsgBuffer<CAoiThreadTraits>;
template class TTrThreadMsgBuffer<CAoi2LogicThreadTraits>;

CAoiThreadMgr::CAoiThreadMgr(uint32 num)
: Parent_t(1024, 1024 * 100, CSyncServerTrMsgBufferMgrSwapper::Inst())
, m_bEndThreads(false)
{
    MgrInit();

	CAoi2LogicThreadMsgBuffer::Init();

    CreateAoiThreads(num);

    Inst() = this;
}

CAoiThreadMgr::~CAoiThreadMgr(void)
{ 	
    ReleaseThreads();
    
    for( uint8 uIndex=0; uIndex < m_vecThreads.size(); uIndex++ )
		delete m_vecThreads[uIndex];
    
	CAoi2LogicThreadMsgBuffer::Unit();

    DestroySemaphore(&m_smJobQueue);
}

void CAoiThreadMgr::MgrInit()
{
    CreateSemaphore(&m_smJobQueue,0,INT_MAX);
}

void CAoiThreadMgr::CreateAoiThreads(uint32 uNum)
{
    m_vecThreads.resize(uNum);
    for( uint8 uIndex=0; uIndex < uNum; uIndex++ )
    {
		m_vecThreads[uIndex] = new CAoiThread(this);
    }
}

void CAoiThreadMgr::ReleaseThreads()
{
    for(size_t Index=0; Index < m_vecThreads.size(); Index++)
    {
		PutSemaphore(&m_smJobQueue);
    }
}

void CAoiThreadMgr::Init(uint32 num)
{
    new CAoiThreadMgr(num);
}

void CAoiThreadMgr::Unit()
{
    delete Inst();
    Inst() = NULL;
}

CAoiThreadMgr*& CAoiThreadMgr::Inst() 
{
    static CAoiThreadMgr* s_pInst = NULL;
    return s_pInst;
}

void CAoiThreadMgr::OnWakeRight()
{
	PutSemaphore(&m_smJobQueue);
}

void CAoiThreadMgr::OnLeftException(CError& error)
{

}

void CAoiThreadMgr::OnRightException(CError& error)
{
	ostringstream strm;
	strm << "DestroyedObjNum " << CSyncAppServer::GetCounter() << endl;
	error.AppendMsg(strm.str());
}

#include "stdafx.h"
#include "CCoreMsgMgr.h"
#include "CCoreMsg.h"
#include "CCoreThreadMsg.h"
#include "ThreadHelper.h"
#include "ErrLogHelper.h"
#include "ExpHelper.h"
#include "TSqrAllocator.inl"

CCoreMsgMgr::CCoreMsgMgr(void)
:m_MemPool(1024*10, CCoreMsgMallocObject::GetPool())
{
	CreateLock(&m_slkThreadCoreMsg);
}

CCoreMsgMgr::~CCoreMsgMgr(void)
{
	FlushMsg();
	DestroyLock(&m_slkThreadCoreMsg);
}


void CCoreMsgMgr::FlushMsg(void)
{
	do 
	{
		SQR_TRY
		{
			while( ! m_queCoreMsg.empty() )
			{
				CCoreMsg* pMsg=m_queCoreMsg.front();
				m_queCoreMsg.pop();

				pMsg->~CCoreMsg();
			}
			break;
		}
		SQR_CATCH(exp)
		{
			LogExp(exp);
		}
		SQR_TRY_END;
	} while(true);

	m_MemPool.Clear();
}


void CCoreMsgMgr::PostMsg(CCoreMsg* pMsg)
{
	m_queCoreMsg.push(pMsg);
}


void CCoreMsgMgr::FlushThreadMsg(void)
{
	Lock( &m_slkThreadCoreMsg );

	m_lstDoingThreadMsg.swap( m_lstThreadCoreMsg );

	Unlock( &m_slkThreadCoreMsg );

	ListThreadMsg_t::iterator it = m_lstDoingThreadMsg.begin(); 
	ListThreadMsg_t::iterator itEnd = m_lstDoingThreadMsg.end();

	for( ;; ) 
	{
		SQR_TRY
		{
			while( it!=itEnd )
			{
				CCoreThreadMsg* pMsg=*it++;

				delete pMsg;
			}
			break;
		}
		SQR_CATCH(exp)
		{
			LogExp(exp);
		}
		SQR_TRY_END;
	}

	m_lstDoingThreadMsg.clear();
}


bool CCoreMsgMgr::PostThreadMsg(CCoreThreadMsg* pMsg)
{
	//用spinlock比较安全，这样在信号处理函数调用这个函数也没什么问题。
	//信号处理函数里面能否使用mutex一直没有找到确定的文档。
	Lock(&m_slkThreadCoreMsg);
	bool bWasEmpty=m_lstThreadCoreMsg.empty();
	m_lstThreadCoreMsg.push_back(pMsg);
	Unlock(&m_slkThreadCoreMsg);
	return bWasEmpty;
}


CCoreMsgMgr* CCoreMsgMgr::ms_pInst = NULL;


void CCoreMsgMgr::Init()
{
	if(ms_pInst)
		GenErr("CCoreMsgMgr is already initial");

	ms_pInst = new CCoreMsgMgr;
}

void CCoreMsgMgr::Unit()
{
	if(!ms_pInst)
		GenErr("CCoreMsgMgr is already uninitial");

	delete ms_pInst;
	ms_pInst = NULL;
}

CCoreMsgMgr& CCoreMsgMgr::Inst()
{
	return *ms_pInst;
}

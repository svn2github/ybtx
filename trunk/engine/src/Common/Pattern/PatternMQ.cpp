#include "stdafx.h"
#include "PatternMQ.h"
#include "ExpHelper.h"
#include <iostream>
#include "ErrLogHelper.h"
#include "CMemPool.h"
#include "TSqrAllocObject.inl"
#include "TSqrAllocator.inl"


/*
Pattern MQ消息处理机制，先前的实现是能够保证同一个owner的消息时序，但是
不能保证所有消息的时序，这套机制现在必须得支持所有消息时序正常，所以先重新设计如下
为了不改动外部程序，所以以前的接口都不变
在CPtMQMsgMgr里面，现在保存一个消息队列，消息直接加入到消息队列里面，这样就能保证所有
消息的时序。
还要注意一个问题，就是owner删除的时候owner对应的消息是不能派发了，先前的做法是owner删除的
时候对应的消息会删除的，但是现在是不能这么做了。而我们也不能在owner里面保存每一个消息
的iterator指针，这样太耗内存了。
由于我们提供一个中间代理类proxy，这个类与owner双相引用，而同时被消息引用。
当owner加入一个消息的时候，如果proxy不存在，则首先创建一个proxy，把proxy绑定到消息上面
同时增加proxy的引用计数。
当owner删除的时候，把对应proxy上面对应的owner指针设为null，这样在派发消息的时候，首先判断
该消息上面proxy的owner是否为null，如果不为，则派发
没派发一个消息，对应的proxy的引用计数减一，如果proxy的引用计数为0并且对应的owner不存在
那么就把这个proxy删除
*/

DefineAllocObject(CPtMQMallocObject);

namespace sqr
{
	class COMMON_API CPtMQMsgOwnerProxy
		:public CPtMQMallocObject
	{
	public:
		friend class CPtMQMsgOwner;
		friend class CPtMQMsgMgr;
		friend class CPtMQMsg;

		CPtMQMsgOwnerProxy(CPtMQMsgOwner* pOwner);

		void AddRef();
		void DelRef();
		void Release();

	private:
		~CPtMQMsgOwnerProxy();

	private:
		CPtMQMsgOwner*	m_pOwner;
		uint32			m_uCounter;
	};
}

CPtMQMsgOwnerProxy::CPtMQMsgOwnerProxy(CPtMQMsgOwner* pOwner)
: m_pOwner(pOwner)
, m_uCounter(0)
{
}

CPtMQMsgOwnerProxy::~CPtMQMsgOwnerProxy()
{
}

inline void CPtMQMsgOwnerProxy::AddRef()
{
	++m_uCounter;
}

inline void CPtMQMsgOwnerProxy::DelRef()
{
	--m_uCounter;
	if ( !m_pOwner && (m_uCounter == 0) )
		delete this;
}

CPtMQMsg::CPtMQMsg( CPtMQMsgOwner* pOwner )
{
	m_pOwnerProxy = pOwner->m_pProxy;
	m_pOwnerProxy->AddRef();
	pOwner->m_pMgr->m_queMsg.push(this);
}

CPtMQMsgOwner* CPtMQMsg::GetOwner()
{
	return m_pOwnerProxy->m_pOwner;
}

CPtMQMsg::~CPtMQMsg()
{
}

void *CPtMQMsg::operator new( size_t stSize, CPtMQMsgOwner* pOwner )throw()
{
	return pOwner->GetMgr()->m_pMemPool->Alloc(stSize);
}

CPtMQMsgOwner::CPtMQMsgOwner()
{
	m_pMgr = NULL;
	m_pProxy = new CPtMQMsgOwnerProxy( this );
}

CPtMQMsgOwner::~CPtMQMsgOwner()
{
	ClearMsg();

	delete m_pProxy;

	if( m_pMgr )
		--m_pMgr->m_uCounter;
}

void CPtMQMsgOwner::SetMgr(CPtMQMsgMgr* pMgr)
{
	m_pMgr = pMgr;

	Ast( m_pMgr );

	++m_pMgr->m_uCounter;
}

void CPtMQMsgOwner::ClearMsg()
{
	if( m_pProxy->m_uCounter == 0 )
		return;

	m_pProxy->m_pOwner = NULL;
	m_pProxy = new CPtMQMsgOwnerProxy( this );
}

CPtMQMsgMgr::CPtMQMsgMgr(size_t stMemPoolPageSize)
{
	m_uCounter = 0;
	m_pMemPool= new CMemPool( stMemPoolPageSize, CPtMQMallocObject::GetPool() );
}


CPtMQMsgMgr::~CPtMQMsgMgr()
{
	delete m_pMemPool;
}


bool CPtMQMsgMgr::FlushMsg()
{
	if (m_queMsg.empty())
		return false;

	for( ;; ) 
	{
		SQR_TRY
		{
			while( !m_queMsg.empty() )
			{
				CPtMQMsg* pMsg = m_queMsg.front();
				m_queMsg.pop();

				CPtMQMsgOwnerProxy* const pOwnerProxy = pMsg->m_pOwnerProxy;
				CPtMQMsgOwner* const pOwner = pOwnerProxy->m_pOwner;

				if (pOwner)
					pMsg->Dispatch(pOwner);

				pOwnerProxy->DelRef();
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

	m_pMemPool->Clear();
	return true;
}

bool CPtMQMsgMgr::Release()
{
	if ( m_uCounter > 0 )
		return false;

	delete this;
	return true;
}

#pragma once
#include "CommonDefs.h"
#include "TSqrAllocObject.h"
#include "TSqrAllocator.h"
#include <queue>
#include <list>

//Pattern "Message Queue"

#ifdef _WIN32
#pragma warning(push)
#pragma warning(disable:4251)
#endif

namespace sqr
{
	DeclareAllocObject(CPtMQMallocObject);

	DeclarePoolAllocator(CPtMQMallocAllocator, CPtMQMallocObject);
	
	class CPtMQMsg;
	class CPtMQMsgOwner;
	class CPtMQMsgMgr;
	class CPtMQMsgOwnerProxy;

	class COMMON_API CPtMQMsg
	{
		friend class CPtMQMsgMgr;
		friend class CPtMQMsgOwner;
	public:
		explicit CPtMQMsg(CPtMQMsgOwner* pOwner);
		
		void *operator new( size_t stSize, CPtMQMsgOwner* pOwner )throw();
		inline void operator delete( void* , CPtMQMsgOwner* pOwner )throw(){}
		inline void operator delete( void* )throw(){}
	protected:
		virtual ~CPtMQMsg();
		CPtMQMsgOwnerProxy*		m_pOwnerProxy;
		CPtMQMsgOwner* GetOwner();
	private:
		virtual void Dispatch(CPtMQMsgOwner* pOwner)=0;
	};

	class COMMON_API CPtMQMsgOwner
	{
		friend class CPtMQMsgMgr;
		friend class CPtMQMsg;
	public:
		explicit CPtMQMsgOwner();
		void SetMgr(CPtMQMsgMgr* pMgr);
		virtual ~CPtMQMsgOwner();

		void ClearMsg();
		CPtMQMsgMgr* GetMgr()const;
	protected:
		CPtMQMsgMgr*			m_pMgr;
	private:
		CPtMQMsgOwnerProxy*		m_pProxy;
	};

	inline CPtMQMsgMgr* CPtMQMsgOwner::GetMgr()const
	{
		return m_pMgr;
	}
	

	class CMemPool;

	class COMMON_API CPtMQMsgMgr
		:public CPtMQMallocObject
	{
		friend class CPtMQMsgOwner;
		friend class CPtMQMsg;
	public:
		bool FlushMsg();
		bool Release();
		explicit CPtMQMsgMgr(size_t stMemPoolPageSize);
		CMemPool* GetMemPool()const;
	private:
		~CPtMQMsgMgr();
		uint32							m_uCounter;
		typedef std::deque<CPtMQMsg*,CPtMQMallocAllocator<CPtMQMsg*> >	MsgDeque_t;
		typedef std::queue<CPtMQMsg*, MsgDeque_t >		MsgQueue_t;
		MsgQueue_t	m_queMsg;

		CMemPool*	m_pMemPool;
	};

	inline CMemPool* CPtMQMsgMgr::GetMemPool()const
	{
		return m_pMemPool;
	}
}

#ifdef _WIN32
#pragma warning(pop)
#endif

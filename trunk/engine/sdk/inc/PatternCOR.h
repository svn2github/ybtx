#pragma once
#include "CommonDefs.h"
#include "PatternRef.inl"
#include "TSqrAllocObject.h"
#include "TSqrAllocator.h"
#include <queue>
//Pattern "Chain Of Responsibility"


#ifdef _WIN32
#pragma warning(push)
#pragma warning(disable:4251)
#endif

namespace sqr
{
	class CPtCORSubject;
	class CPtCORHandler;
	class CSyncPtCORSubject;
	
	template<typename,template<typename>class>
	class TObjectBank;

	DeclareAllocObject(CPtCORMallocObject);

	DeclarePoolAllocator(CPtCORMallocAllocator, CPtCORMallocObject);

	class CCOREvent
		:public CPtCORMallocObject
	{
	public:
		CCOREvent();
		void Revert();
		void Reset(CPtCORSubject* pSubject, CPtCORHandler* pHandler, uint32 uEventType, void* pArg);
		CPtCORSubject*	GetSubject()const;
		CPtCORHandler*	GetHandler()const;
		uint32			m_uEventType;
		void*			m_pArg;
	private:
		TPtRefer<CCOREvent, CPtCORHandler> m_Handler;
		TPtRefer<CCOREvent, CPtCORSubject> m_Subject;
	};

	class CCOREventMgr
	{
		typedef deque< CCOREvent*, CPtCORMallocAllocator<CCOREvent*> >				EventDeque_t;
		typedef queue< CCOREvent*, EventDeque_t >									EventQueue_t;
		typedef TObjectBank<CCOREvent, CPtCORMallocAllocator>						EventBank_t;
	public:
		CCOREventMgr();
		~CCOREventMgr();

		static CCOREventMgr* Inst()
		{
			static CCOREventMgr ls_Inst;
			return &ls_Inst;
		}
		void FlushEvent();
		void AddEvent(CPtCORSubject * pSubject,uint32 uType,void * pArg,CPtCORHandler * pHandler);
	private:
		EventQueue_t					m_queEvent;
		EventBank_t*					m_pEventBank;
	};

	class COMMON_API CPtCORHandler
	{
		friend class CPtCORSubject;
	public:
		CPtCORHandler();
		virtual ~CPtCORHandler();
		virtual void OnCOREvent(CPtCORSubject* pSubject,uint32 uEvent,void* pArg)=0;
		void DetachAll();

		typedef set< uint32 ,less<uint32>, CPtCORMallocAllocator<uint32> >			TypeSet_t;
		typedef	pair<CPtCORSubject*,TypeSet_t>										SubjectPair_t;
		typedef map< CPtCORSubject*, TypeSet_t ,less< CPtCORSubject* >, CPtCORMallocAllocator< SubjectPair_t > >		SubjectMap_t;
		typedef TPtRefee<CPtCORHandler,CCOREvent>	RefeeByEvent_t;

		RefeeByEvent_t& GetRefeeByEvent(){return m_RefeeByEvent;}

	private:
		SubjectMap_t	m_mapSubject;
		RefeeByEvent_t m_RefeeByEvent;
	};

	class COMMON_API CPtCORSubject
	{
		friend class CPtCORHandler;
		friend class CSyncPtCORSubject;
	public:
		bool Attach(CPtCORHandler*,uint32 uEvent);
		bool Detach(CPtCORHandler*,uint32 uEvent);
		void DetachAll();
		virtual bool Notify(uint32 uEventType,void* pArg);
		bool Notify(uint32 uEventType);
		bool Attached(CPtCORHandler*,uint32 uEventType)const;
		bool Attached(uint32 uEventType)const;

		CPtCORSubject(uint32 uHighFreqEvent);
		virtual ~CPtCORSubject();

		typedef set< CPtCORHandler*,less< CPtCORHandler*> ,CPtCORMallocAllocator< CPtCORHandler* > >	HandlerSet_t;
		typedef pair< uint32, HandlerSet_t >			TypePair_t;
		
		typedef vector< HandlerSet_t, CPtCORMallocAllocator<HandlerSet_t> >							TypeVec_t;
		typedef map< uint32, HandlerSet_t ,less<uint32>, CPtCORMallocAllocator<TypePair_t> >			TypeMap_t;
		
		typedef TPtRefee<CPtCORSubject,CCOREvent>	RefeeByEvent_t;

		RefeeByEvent_t& GetRefeeByEvent(){return m_RefeeByEvent;}

	private:
		uint32				m_uHighFreqEvent;
		TypeVec_t			m_VecType;
		TypeMap_t			m_MapType;
		RefeeByEvent_t		m_RefeeByEvent;
	};

}

#ifdef _WIN32
#pragma warning(pop)
#endif



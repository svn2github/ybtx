#include "stdafx.h"
#include "TObjectBank.inl"

#include "PatternCOR.h"
#include "TMallocAllocator.h"
#include "TSqrAllocObject.inl"
#include "ExpHelper.h"
#include "TSqrAllocator.inl"
#include "ErrLogHelper.h"
#include "TObjectBank.inl"

DefineAllocObject(CPtCORMallocObject);

CPtCORSubject*	CCOREvent::GetSubject()const
{
	return m_Subject.Get();
}

CPtCORHandler*	CCOREvent::GetHandler()const
{
	return m_Handler.Get();
}

CCOREvent::CCOREvent()
:m_uEventType(0)
,m_pArg(NULL)
{
	m_Subject.SetHolder(this);
	m_Handler.SetHolder(this);
}

void CCOREvent::Revert()
{
	m_uEventType	= 0;
	m_pArg			= NULL;
	m_Subject.Detach();
	m_Handler.Detach();
}

void CCOREvent::Reset(CPtCORSubject* pSubject, CPtCORHandler* pHandler, uint32 uEventType, void* pArg)
{
	m_uEventType = uEventType;
	m_pArg = pArg;
	m_Subject.Attach(pSubject->GetRefeeByEvent());
	m_Handler.Attach(pHandler->GetRefeeByEvent());
}

CCOREventMgr::CCOREventMgr()
{
	m_pEventBank = new EventBank_t;
}

CCOREventMgr::~CCOREventMgr()
{
	delete m_pEventBank;
}

void CCOREventMgr::FlushEvent()
{
	do 
	{
		SQR_TRY
		{
			while(!m_queEvent.empty())
			{
				CCOREvent* pEvent			= m_queEvent.front();
				m_queEvent.pop();

				CPtCORHandler * pHandler	= pEvent->GetHandler();
				CPtCORSubject * pSubject	= pEvent->GetSubject();
				uint32 uType				= pEvent->m_uEventType;
				void* pArg					= pEvent->m_pArg;
				
				pEvent->Revert();
				m_pEventBank->Push(pEvent);

				if(pHandler && pSubject)
					pHandler->OnCOREvent(pSubject, uType, pArg);
			}
			break;
		}
		SQR_CATCH(exp)
		{
			LogExp(exp);
		}
		SQR_TRY_END;
	} while (true);
}

void CCOREventMgr::AddEvent(CPtCORSubject * pSubject,uint32 uType, void* pArg, CPtCORHandler * pHandler)
{
	CCOREvent* pEvent = m_pEventBank->PopOrNew();

	pEvent->Reset(pSubject, pHandler, uType, pArg);
	m_queEvent.push( pEvent);
}

CPtCORHandler::CPtCORHandler()
{
	m_RefeeByEvent.SetHolder(this);
	m_mapSubject.clear();
}

CPtCORHandler::~CPtCORHandler(void)
{
	DetachAll();
}


void CPtCORHandler::DetachAll()
{
	while( !m_mapSubject.empty() )
	{
		SubjectMap_t::iterator it=m_mapSubject.begin();
		CPtCORSubject* pSubject=it->first;
		uint32 uEventType=(uint32)(*it->second.begin());
		Ver(pSubject->Detach(this,uEventType));
	}
}





bool CPtCORSubject::Attach (CPtCORHandler* pHandler,uint32 uEventType)
{
	if( !pHandler )
		GenErr("CPtCORSubject::Attach pHandler==NULL");
	
	HandlerSet_t* pHandlerSet = NULL;
	if (uEventType < m_uHighFreqEvent)
		pHandlerSet = &m_VecType[uEventType];
	else
	{
		TypeMap_t::iterator mapIter = m_MapType.find(uEventType);
		if (mapIter == m_MapType.end())
		{
			pair<TypeMap_t::iterator,bool> pairTypeMap = m_MapType.insert(make_pair(uEventType, HandlerSet_t()));
			Ast (pairTypeMap.second);
			pHandlerSet = &pairTypeMap.first->second;
		}
		else
			pHandlerSet = &mapIter->second;
	} 

	HandlerSet_t& setHandler = *pHandlerSet;
	pair<HandlerSet_t::iterator,bool> pairHandlerSet=setHandler.insert(pHandler);
	if(!pairHandlerSet.second)
		return false;

	pair<CPtCORHandler::SubjectMap_t::iterator,bool> pairSubjectMap;

	CPtCORHandler::TypeSet_t setTemp;
	pairSubjectMap=pHandler->m_mapSubject.insert( make_pair( this , setTemp ) );

	Ver(pairSubjectMap.first->second.insert(uEventType).second);
	
	return true;
}


bool CPtCORSubject::Detach (CPtCORHandler* pHandler,uint32 uEventType)
{
	if( !pHandler )
		GenErr("CPtCORSubject::Detach pHandler==NULL");
	
	if ( uEventType < m_uHighFreqEvent )
	{
		HandlerSet_t& setHandler = m_VecType[uEventType];
		if( 0 == setHandler.erase( pHandler ) )
			return false;
	}
	else
	{
		TypeMap_t::iterator mapIter = m_MapType.find(uEventType);
		if (mapIter == m_MapType.end())
			return false;
		else
		{
			HandlerSet_t& setHandler = mapIter->second;
			if( 0 == setHandler.erase( pHandler ) )
				return false;
			if( setHandler.empty() )
				m_MapType.erase(mapIter);
		}
	}

	CPtCORHandler::SubjectMap_t::iterator itSubject=pHandler->m_mapSubject.find(this);
	Ast( itSubject != pHandler->m_mapSubject.end() );

	CPtCORHandler::TypeSet_t& setType=itSubject->second;

	Ver( setType.erase(uEventType) );

	if(setType.empty())
		pHandler->m_mapSubject.erase(itSubject);

	return true;
}


bool CPtCORSubject::Attached(uint32 uEventType)const
{
	if ( uEventType < m_uHighFreqEvent )
	{
		const HandlerSet_t& setHandler = m_VecType[uEventType];
		return setHandler.size() != 0;
	}
	else
	{
		TypeMap_t::const_iterator mapIter = m_MapType.find(uEventType);
		return mapIter != m_MapType.end();
	}
}


bool CPtCORSubject::Attached(CPtCORHandler* pHandler,uint32 uEventType)const
{
	if ( uEventType < m_uHighFreqEvent )
	{
		const HandlerSet_t& setHandler = m_VecType[uEventType];
		return setHandler.find(pHandler) != setHandler.end();
	}
	else
	{
		TypeMap_t::const_iterator mapIter = m_MapType.find(uEventType);
		if (mapIter != m_MapType.end())
		{
			const HandlerSet_t& setHandler = mapIter->second;
			if (setHandler.find(pHandler) != setHandler.end())
				return true;
		}
		return false;
	}
}

void CPtCORSubject::DetachAll()
{
	for (uint32 i = 0; i < m_VecType.size(); i++)
	{
		HandlerSet_t& setHandler = m_VecType[i];
		while(!setHandler.empty())
		{
			CPtCORHandler* pHandler = *setHandler.begin();
			Detach(pHandler, i);
		}
	}
	while( !m_MapType.empty() )
	{
		uint32 uEventType=m_MapType.begin()->first;
		CPtCORHandler* pHandler=*m_MapType.begin()->second.begin();
		Detach(pHandler,uEventType);
	}
}


bool CPtCORSubject::Notify(uint32 uEventType,void* pArg)
{
	HandlerSet_t* pHandlerSet = NULL;
	if (uEventType < m_uHighFreqEvent)
		pHandlerSet = &m_VecType[uEventType];
	else
	{
		TypeMap_t::iterator mapIter = m_MapType.find(uEventType);
		if (mapIter == m_MapType.end())
			return true;
		else
			pHandlerSet = &mapIter->second;
	}

	HandlerSet_t& setHandler = *pHandlerSet;

	//只有存在attach自己的handler时才会FlushEvent
	HandlerSet_t::iterator iter = setHandler.begin();
	if (iter == setHandler.end())
		return true;

	for( ; iter != setHandler.end() ; ++iter )
		CCOREventMgr::Inst()->AddEvent(this,uEventType,pArg,*iter);

	CCOREventMgr::Inst()->FlushEvent();

	return true;
}

bool CPtCORSubject::Notify(uint32 uEventType)
{
	return Notify( uEventType, NULL );
}

CPtCORSubject::CPtCORSubject(uint32 uHighFreqEvent)
:m_uHighFreqEvent(uHighFreqEvent)
{
	m_RefeeByEvent.SetHolder(this);
	m_VecType.assign(m_uHighFreqEvent, HandlerSet_t());
}

CPtCORSubject::~CPtCORSubject()
{
	DetachAll();
}

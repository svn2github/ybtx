#include "stdafx.h"
#include "SyncPatternCOR.h"

bool CSyncPtCORSubject::Notify(uint32 uEventType)
{
	return Notify( uEventType, NULL );
}

bool CSyncPtCORSubject::Notify(uint32 uEventType,void* pArg)
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
		CSyncCOREventMgr::Inst()->AddEvent(this, uEventType, pArg, *iter);

	CSyncCOREventMgr::Inst()->FlushEvent();

	return true;
}

CSyncPtCORSubject::CSyncPtCORSubject(uint32 uHighFreqEvent)
:CPtCORSubject(uHighFreqEvent)
{
}

CSyncPtCORSubject::~CSyncPtCORSubject()
{
}

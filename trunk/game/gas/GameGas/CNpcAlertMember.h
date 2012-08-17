#pragma once

#include "IObjPosObserverHandler.h"
#include "CNpcAIMallocObject.h"

class CNpcAlertMgr;

class CNpcAlertMember
	: public IWatchHandler
	, public CNpcAIMallocObject
{
public:
	CNpcAlertMember(CNpcAlertMgr* pAlertMgr, uint32 uAlertTarget);
	~CNpcAlertMember();

	uint32 GetAlertTargetID();
private:
	virtual void OnObserveeEntered();
	virtual void OnObserveeLeft();
	virtual void OnWatched(EWatchResult eResult);
private:
	CNpcAlertMgr*				m_pAlertMgr;
	uint32						m_uAlertTarget;
};

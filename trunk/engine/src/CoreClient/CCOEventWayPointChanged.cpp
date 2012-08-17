#include "stdafx.h"
#include "CCOEventWayPointChanged.h"
#include "CCoreObjectClient.h"
#include "ICoreObjectClientHandler.h"


CCOEventWayPointChanged::CCOEventWayPointChanged(CCoreObjectClient* pObj)
:BaseEvent_t(pObj)
{
}


CCOEventWayPointChanged::~CCOEventWayPointChanged()
{
	CCoreObjectClient* pObj = GetCoreObj();
	if( !pObj )
		return;
	ICoreObjectClientHandler* pHandler=pObj->GetHandler();
	if( !pHandler )
		return;
	pHandler->OnWayPointChanged();
}

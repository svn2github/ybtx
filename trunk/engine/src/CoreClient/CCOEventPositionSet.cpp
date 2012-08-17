#include "stdafx.h"
#include "CCOEventPositionSet.h"
#include "CCoreObjectFollower.h"
#include "ICoreObjectFollowerHandler.h"


CCOEventPositionSet::CCOEventPositionSet(CCoreObjectFollower* pObj)
:BaseEvent_t(pObj)
{
}

CCOEventPositionSet::~CCOEventPositionSet()
{
	CCoreObjectClient* pObj = GetCoreObj();
	if( !pObj )
		return;
	ICoreObjectFollowerHandler* pHandler=static_cast<CCoreObjectFollower*>(pObj)->GetHandler();
	if( !pHandler )
		return;
	pHandler->OnPositionSet();
}

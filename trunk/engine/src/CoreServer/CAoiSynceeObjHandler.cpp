#include "stdafx.h"
#include "CAoiSynceeObjHandler.h"
#include "CSyncCoreObjectMediator.h"

CAoiSynceeObjHandler::CAoiSynceeObjHandler(CSyncCoreObjectMediator* pObj)
: CAoiSyncerObjHandler(pObj)
{
}

CSyncCoreObjectMediator* CAoiSynceeObjHandler::GetCoreObj()const
{
	return static_cast<CSyncCoreObjectMediator*>(CAoiViewObjHandler::GetCoreObj());
}

void CAoiSynceeObjHandler::OnMsgFromSyncer(const void* pContext)
{
	GetCoreObj()->OnMsgFromSyncer(pContext);
}

void CAoiSynceeObjHandler::OnMsgToSyncerHandled(const void* pContext)
{
	GetCoreObj()->OnMsgToSyncerHandled(pContext);
}

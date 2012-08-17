#include "stdafx.h"
#include "CAoiViewObjHandler.h"
#include "CAoiSyncerObjHandler.h"
#include "CAoiSynceeObjHandler.h"
#include "CSyncCoreObjectDictator.h"
#include "CAsyAoiSynceeObj.h"

CAoiSyncerObjHandler::CAoiSyncerObjHandler(CSyncCoreObjectDictator* pCoreObject)
: CAoiViewObjHandler(pCoreObject)
{

}

void CAoiSyncerObjHandler::OnSyncRelationChanged(CAsyAoiSynceeObj* pObj,bool bSee, uint32 uDimension)
{
	CAoiSynceeObjHandler* pAdapter=static_cast<CAoiSynceeObjHandler*>(pObj->GetHandler());
	GetCoreObj()->OnSyncRelationChanged(pAdapter->GetCoreObj(),bSee, uDimension);
}


CSyncCoreObjectDictator* CAoiSyncerObjHandler::GetCoreObj()const
{
	return static_cast<CSyncCoreObjectDictator*>(CAoiViewObjHandler::GetCoreObj());
}

void CAoiSyncerObjHandler::OnMsgFromSyncee(const void* pContext)
{
	GetCoreObj()->OnMsgFromSyncee(pContext);
}

void CAoiSyncerObjHandler::OnMsgToSynceeHandled(const void* pContext)
{
	GetCoreObj()->OnMsgToSynceeHandled(pContext);
}

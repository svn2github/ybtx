#include "stdafx.h"
#include "CAoiViewObjHandler.h"
#include "CAoiSyncerObjHandler.h"
#include "CAoiSynceeObjHandler.h"
#include "CSyncCoreObjectDictator.h"
#include "CSyncCoreObjectMediator.h"
#include "CAsyAoiViewObj.h"
#include "TSqrAllocator.inl"
#include "PatternRef.inl"
//template class TPtRefer<CAoiViewObjHandler, CSyncCoreObjectServer>;
CAoiViewObjHandler::CAoiViewObjHandler(CSyncCoreObjectServer* pCoreObject)
{
	m_RefsByAoiObj.SetHolder(this);
	m_CoreObj.SetHolder(this);
	if(pCoreObject)
		m_CoreObj.Attach(pCoreObject->m_RefsByAoiObjHandler);
}

CAoiViewObjHandler::~CAoiViewObjHandler()
{
	m_CoreObj.Detach();
}

void CAoiViewObjHandler::OnViewRelationChanged(CAsyAoiViewObj* pObj,bool bSee, uint32 uDimension)
{
	CAoiViewObjHandler* pHandler = pObj->GetHandler();
	GetCoreObj()->OnViewRelationChanged(pHandler->GetCoreObj(),bSee, uDimension);
}

CSyncCoreObjectServer* CAoiViewObjHandler::GetCoreObj()const
{
	return m_CoreObj.Get();
}

uint32 CAoiViewObjHandler::GetCoreObjID()
{
	return GetCoreObj()->GetGlobalID();
}

void CAoiViewObjHandler::OnMsgFromViewer(const void* pContext)
{
	GetCoreObj()->OnMsgFromViewer(pContext);
}

void CAoiViewObjHandler::OnMsgFromViewee(const void* pContext)
{
	GetCoreObj()->OnMsgFromViewee(pContext);
}

void CAoiViewObjHandler::OnMsgToVieweeHandled(const void* pContext)
{
	GetCoreObj()->OnMsgToVieweeHandled(pContext);
}

void CAoiViewObjHandler::OnMsgToViewerHandled(const void* pContext)
{
	GetCoreObj()->OnMsgToViewerHandled(pContext);
}

void CAoiViewObjHandler::OnMsgFromSelf(const void* pContext)
{
	GetCoreObj()->OnMsgFromSelf(pContext);
}

void CAoiViewObjHandler::OnHalted()
{
	GetCoreObj()->OnAoiObjectHalted();
}



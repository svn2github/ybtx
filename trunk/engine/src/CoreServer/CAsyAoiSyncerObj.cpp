#include "stdafx.h"
#include "CAsyAoiSyncerObj.h"
#include "CAsyAoiScene.h"
#include "CAsyAoiSynceeObj.h"
#include "CAoiSyncerObjHandler.h"

#include "ThreadHelper.h"
#include "CAoiThreadMgr.h"
#include "CAoiJob.inl"

CAsyAoiSyncerObj::CAsyAoiSyncerObj(CAsyAoiScene *pScene, const CFPos& pos,CAoiSyncerObjHandler *pHandler)
:CAsyAoiViewObj(pScene,pos,pHandler)
{
	
}

EAoiObjType CAsyAoiSyncerObj::GetType()const
{
	return eAOT_SyncerObj;
}


void CAsyAoiSyncerObj::OnSyncRelationChanged(CAsyAoiSynceeObj* pSynceeObj,bool bSee, uint32 uDimension)
{
	//调用外部回调
	CAoiSyncerObjHandler *pHandler = static_cast<CAoiSyncerObjHandler*>(GetHandler());
	pHandler->OnSyncRelationChanged(pSynceeObj, bSee, uDimension);
}

void CAsyAoiSyncerObj::PostMsgToSyncee(const void* pContext)
{
	if ( !Running() )
		return;

	if (!GetAoiDimObj(0))
		return;

	(new CAoiPostMsgToSynceeJob(GetGlobalID(), pContext))->Add();
}

void CAsyAoiSyncerObj::OnMsgFromSyncee(const void* pContext)
{
	CAoiSyncerObjHandler *pHandler = static_cast<CAoiSyncerObjHandler*>(GetHandler());
	pHandler->OnMsgFromSyncee(pContext);
}

void CAsyAoiSyncerObj::OnMsgToSynceeHandled(const void* pContext)
{
	CAoiSyncerObjHandler *pHandler = static_cast<CAoiSyncerObjHandler*>(GetHandler());
	pHandler->OnMsgToSynceeHandled(pContext);
}


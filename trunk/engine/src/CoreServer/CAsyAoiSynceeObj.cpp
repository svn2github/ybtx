#include "stdafx.h"
#include "CAsyAoiViewObj.h"
#include "CAsyAoiSynceeObj.h"
#include "CAsyAoiScene.h"
#include "CAoiSynceeObjHandler.h"
#include "CAoiThreadMgr.h"
#include "CAoiJob.inl"

uint32 CAsyAoiSynceeObj::ms_uSyncJobID = 0;

CAsyAoiSynceeObj::CAsyAoiSynceeObj(CAsyAoiScene *pScene, const CFPos& pos,CAoiSynceeObjHandler *pHandler)
:CAsyAoiSyncerObj(pScene,pos,pHandler)
{
}

EAoiObjType CAsyAoiSynceeObj::GetType()const
{
	return eAOT_SynceeObj;
}

void CAsyAoiSynceeObj::PostMsgToSyncer(const void* pContext)
{
	if ( !Running() )
		return;

	if (!GetAoiDimObj(0))
		return;

	(new CAoiPostMsgToSyncerJob(GetGlobalID(), pContext))->Add();
}

void CAsyAoiSynceeObj::OnMsgFromSyncer(const void* pContext)
{
	CAoiSynceeObjHandler *pHandler = static_cast<CAoiSynceeObjHandler*>(GetHandler());
	pHandler->OnMsgFromSyncer(pContext);
}

void CAsyAoiSynceeObj::OnMsgToSyncerHandled(const void* pContext)
{
	CAoiSynceeObjHandler *pHandler = static_cast<CAoiSynceeObjHandler*>(GetHandler());
	pHandler->OnMsgToSyncerHandled(pContext);
}


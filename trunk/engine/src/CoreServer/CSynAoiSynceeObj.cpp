#include "stdafx.h"
#include "CSynAoiSynceeObj.h"
#include "CSynAoiScene.h"
#include "CSynAoiDimObj.h"
#include "CAoiThreadMgr.h"

CSynAoiSynceeObj::CSynAoiSynceeObj(CSynAoiScene *pScene, const CFPos& pos, uint32 uId, uint32 uCoreObjId)
:CSynAoiSyncerObj(pScene,pos,uId, uCoreObjId)
{
}


EAoiObjType CSynAoiSynceeObj::GetType()const
{
	return eAOT_SynceeObj;
}



void CSynAoiSynceeObj::
OnViewRelationKept(CSynAoiViewObj* pOtObj,
				   const CFPos& MyOldGridPos,const CFPos& MyGridPos,
				   const CFPos& OtOldGridPos,const CFPos& OtGridPos, uint32 uDimension)
{
	CSynAoiSyncerObj* pSyncerObj=pOtObj->CastToSyncerObj();
	if( !pSyncerObj )
		return;

	bool ISawOt=GetScene()->IsInSyncAoi(OtOldGridPos-MyOldGridPos);
	bool ISeeOt=GetScene()->IsInSyncAoi(OtGridPos-MyGridPos);

	if( ISawOt != ISeeOt )
		pSyncerObj->OnSyncRelationChanged(this,ISeeOt, uDimension);
}


void CSynAoiSynceeObj::PostMsgToSyncer(const void* pContext)
{
	CSynAoiDimObj* pDimObj = GetAoiDimObj(0);
	if(!pDimObj)
		return;

	pDimObj->PostMsgToSyncer(pContext);
}

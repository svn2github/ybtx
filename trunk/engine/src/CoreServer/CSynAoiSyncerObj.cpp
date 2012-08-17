#include "stdafx.h"
#include "CSynAoiSyncerObj.h"
#include "CSynAoiScene.h"
#include "CSynAoiSynceeObj.h"
#include "CSynAoiDimObj.h"
#include "CAoiThreadMgr.h"
#include "CAoiResult.inl"

CSynAoiSyncerObj::CSynAoiSyncerObj(CSynAoiScene *pScene, const CFPos& pos,uint32 uId, uint32 uCoreObjId)
: CSynAoiViewObj(pScene,pos,uId, uCoreObjId)
{
}

void CSynAoiSyncerObj::OnViewRelationChanged(CSynAoiViewObj* pViewObj,const CFPos& MyGridPos,
											 const CFPos& OtGridPos,bool bSee, uint32 uDimension)
{
	CSynAoiSynceeObj* pSynceeObj = pViewObj->CastToSynceeObj();

	if( pSynceeObj )
	{
		//bSee在true和false两种情况下，调用sync和view回调的顺序要调换
		if(bSee)
		{
			HandleViewRelationChanged(pViewObj,bSee, uDimension);

			if( GetScene()->IsInSyncAoi( MyGridPos - OtGridPos ) )
				OnSyncRelationChanged( pSynceeObj,bSee, uDimension );
		}
		else
		{
			if( GetScene()->IsInSyncAoi( MyGridPos - OtGridPos ) )
				OnSyncRelationChanged( pSynceeObj,bSee, uDimension );

			HandleViewRelationChanged(pViewObj,bSee, uDimension);
		}
	}
	else
	{
		HandleViewRelationChanged(pViewObj,bSee, uDimension);
	}
}

EAoiObjType CSynAoiSyncerObj::GetType()const
{
	return eAOT_SyncerObj;
}


void CSynAoiSyncerObj::OnSyncRelationChanged(CSynAoiSynceeObj* pSynceeObj,bool bSee, uint32 uDimension)
{
	(new CAoiSyncChangedResult(GetGlobalID(), pSynceeObj->GetGlobalID(), bSee, uDimension))->Add();
}

void CSynAoiSyncerObj::PostMsgToSyncee(const void* pContext)
{
	CSynAoiDimObj* pDimObj = GetAoiDimObj(0);
	if(!pDimObj)
		return;

	pDimObj->PostMsgToSyncee(pContext);
}


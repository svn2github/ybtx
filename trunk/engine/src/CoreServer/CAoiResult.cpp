#include "stdafx.h"
#include "CAoiResult.h"
#include "CAoiResult.inl"
#include "CAsyAoiScene.h"
#include "CAsyAoiViewObj.h"
#include "CAsyAoiSyncerObj.h"
#include "CAsyAoiSynceeObj.h"
#include "ErrLogHelper.h"
#include "CAoiThreadMgr.h"


CAoiViewChangedResult::CAoiViewChangedResult(uint32 uId, uint32 uOtId, bool bSee, uint32 uDimension)
: CAoiRelationChangedResult(uId, uOtId, bSee, uDimension)
{
}

CAoiViewChangedResult::~CAoiViewChangedResult()
{

	CAsyAoiViewObj *pObj = CAsyAoiViewObj::GetAoiObject(m_uId);
	CAsyAoiViewObj *pOtObj = CAsyAoiViewObj::GetAoiObject(m_uOtId);
	//如果两个对象都没有删除，我们就可以处理回调了
	if (pObj && pOtObj)
	{
		pObj->OnViewRelationChanged(pOtObj, m_bSee, m_uDimension);
	}

}

CAoiSyncChangedResult::CAoiSyncChangedResult(uint32 uId, uint32 uOtId, bool bSee, uint32 uDimension)
: CAoiRelationChangedResult(uId, uOtId, bSee, uDimension)
{
}

CAoiSyncChangedResult::~CAoiSyncChangedResult()
{
	CAsyAoiViewObj *pObj = CAsyAoiViewObj::GetAoiObject(m_uId);
	CAsyAoiViewObj *pOtObj = CAsyAoiViewObj::GetAoiObject(m_uOtId);

	//如果两个对象都没有删除，我们就可以处理回调了

	if (pObj && pOtObj)
	{
		CAsyAoiSyncerObj *pSyncerObj = pObj->CastToSyncerObj();
		CAsyAoiSynceeObj *pSynceeOtObj = pOtObj->CastToSynceeObj();

		pSyncerObj->OnSyncRelationChanged(pSynceeOtObj, m_bSee, m_uDimension);
	}
}


/*
Destroy Aoi Obj
*/
CAoiObjHaltedResult::CAoiObjHaltedResult(uint32 uId)
: m_uId(uId)
{
}

CAoiObjHaltedResult::~CAoiObjHaltedResult()
{
	CAsyAoiViewObj* pObj = CAsyAoiViewObj::GetAoiObject(m_uId);
	if (!pObj)
		return;

	pObj->OnHalted();
}

CAoiMsgFromViewerHandledResult::CAoiMsgFromViewerHandledResult(uint32 uId, const void* pContext)
: m_uId(uId)
, m_pContext(pContext)
{
}

CAoiMsgFromViewerHandledResult::~CAoiMsgFromViewerHandledResult()
{
	CAsyAoiViewObj *pObj = CAsyAoiViewObj::GetAoiObject(m_uId);

	if (pObj)
	{
		pObj->OnMsgFromViewer(m_pContext);
	}
}

CAoiMsgFromVieweeHandledResult::CAoiMsgFromVieweeHandledResult(uint32 uId, const void* pContext)
: m_uId(uId)
, m_pContext(pContext)
{
}

CAoiMsgFromVieweeHandledResult::~CAoiMsgFromVieweeHandledResult()
{
	CAsyAoiViewObj *pObj = CAsyAoiViewObj::GetAoiObject(m_uId);

	if (pObj)
	{
		pObj->OnMsgFromViewee(m_pContext);
	}
}

CAoiMsgToVieweeHandledEndedResult::CAoiMsgToVieweeHandledEndedResult(uint32 uId, const void* pContext)
: m_uId(uId)
, m_pContext(pContext)
{
}

CAoiMsgToVieweeHandledEndedResult::~CAoiMsgToVieweeHandledEndedResult()
{
	CAsyAoiViewObj* pObj = CAsyAoiViewObj::GetAoiObject(m_uId);

	if (pObj)
	{
		pObj->OnMsgToVieweeHandled(m_pContext);
	}
}

CAoiMsgToViewerHandledEndedResult::CAoiMsgToViewerHandledEndedResult(uint32 uId, const void* pContext)
: m_uId(uId)
, m_pContext(pContext)
{
}

CAoiMsgToViewerHandledEndedResult::~CAoiMsgToViewerHandledEndedResult()
{
	CAsyAoiViewObj* pObj = CAsyAoiViewObj::GetAoiObject(m_uId);

	if (pObj)
	{
		pObj->OnMsgToViewerHandled(m_pContext);
	}
}

CAoiMsgToSelfHandledResult::CAoiMsgToSelfHandledResult(uint32 uId, const void* pContext)
: m_uId(uId)
, m_pContext(pContext)
{
}

CAoiMsgToSelfHandledResult::~CAoiMsgToSelfHandledResult()
{
	CAsyAoiViewObj* pObj = CAsyAoiViewObj::GetAoiObject(m_uId);

	if (pObj)
	{
		pObj->OnMsgFromSelf(m_pContext);
	}
}

CAoiMsgFromSynceeHandledResult::CAoiMsgFromSynceeHandledResult(uint32 uId, const void* pContext)
: m_uId(uId)
, m_pContext(pContext)
{
}

CAoiMsgFromSynceeHandledResult::~CAoiMsgFromSynceeHandledResult()
{
	CAsyAoiViewObj *pObj = CAsyAoiViewObj::GetAoiObject(m_uId);
	
	if (pObj)
	{
		pObj->CastToSyncerObj()->OnMsgFromSyncee(m_pContext);
	}
}

CAoiMsgFromSyncerHandledResult::CAoiMsgFromSyncerHandledResult(uint32 uId, const void* pContext)
: m_uId(uId)
, m_pContext(pContext)
{
}

CAoiMsgFromSyncerHandledResult::~CAoiMsgFromSyncerHandledResult()
{
	CAsyAoiViewObj *pObj = CAsyAoiViewObj::GetAoiObject(m_uId);
	
	if (pObj)
	{
		pObj->CastToSynceeObj()->OnMsgFromSyncer(m_pContext);
	}
}

CAoiMsgToSynceeHandledEndedResult::CAoiMsgToSynceeHandledEndedResult(uint32 uId, const void* pContext)
: m_uId(uId)
, m_pContext(pContext)
{
}

CAoiMsgToSynceeHandledEndedResult::~CAoiMsgToSynceeHandledEndedResult()
{
	CAsyAoiViewObj* pObj = CAsyAoiViewObj::GetAoiObject(m_uId);

	if (pObj)
	{
		pObj->CastToSyncerObj()->OnMsgToSynceeHandled(m_pContext);
	}
}

CAoiMsgToSyncerHandledEndedResult::CAoiMsgToSyncerHandledEndedResult(uint32 uId, const void* pContext)
: m_uId(uId)
, m_pContext(pContext)
{
}

CAoiMsgToSyncerHandledEndedResult::~CAoiMsgToSyncerHandledEndedResult()
{
	CAsyAoiViewObj* pObj = CAsyAoiViewObj::GetAoiObject(m_uId);

	if (pObj)
	{
		pObj->CastToSynceeObj()->OnMsgToSyncerHandled(m_pContext);
	}
}

CAoiTellErrMsgResult::CAoiTellErrMsgResult(const char* szError)
{
	m_sErrorMsg = CloneString(szError);
}

CAoiTellErrMsgResult::~CAoiTellErrMsgResult()
{
	CallLogCallBackFunc(m_sErrorMsg, NULL);
}

CAoiSceneHaltedResult::CAoiSceneHaltedResult(uint32 uId)
: m_uId(uId)
{
}

CAoiSceneHaltedResult::~CAoiSceneHaltedResult()
{
	CAsyAoiScene* pScene = CAsyAoiScene::GetAoiScene(m_uId);
	if (!pScene)
		return;

	pScene->OnHalted();
}


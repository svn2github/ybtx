#include "stdafx.h"
#include "CAOICmd.h"
#include "CSyncCoreObjectMediator.h"
#include "ExpHelper.h"
#include "CCoreServerObjResult.h"
#include "CCoreServerResult.inl"

void CAoiDisbindConnCmd::HandleCmd(CSyncCoreObjectServer* pObj)
{
	CSyncCoreObjectMediator* pMedObj = static_cast<CSyncCoreObjectMediator*>( pObj );
	pMedObj->DisbindConnection();
}

CAoiPostShellMsgCmd::CAoiPostShellMsgCmd(const void* pContext)
: m_pContext(pContext)
{

}

void CAoiPostShellMsgCmd::HandleCmd(CSyncCoreObjectServer* pObj)
{
	(new CCoreObjOnAoiMsgFromSelfResult(pObj->GetGlobalID(), m_pContext))->Add();
}


void CAoiDecBlockCountCmd::HandleCmd(CSyncCoreObjectServer* pObj)
{
	CSyncCoreObjectMediator* pMedObj = static_cast<CSyncCoreObjectMediator*>( pObj );
	VerGe( --pMedObj->m_uAoiBlockCount , 0 );
}


void CAoiBindToCmd::HandleCmd(CSyncCoreObjectServer* pCoreObj)
{
	if(!pCoreObj)
		return;
	CSyncCoreObjectDictator* pDicObj = pCoreObj->CastToObjDictator();
	if(!pDicObj)
		return;
	pDicObj->OnBindToAoiDone(m_uParentID, m_bRet);
}

CAoiBindToCmd::CAoiBindToCmd(uint32 uParentID, bool bRet)
: m_uParentID(uParentID)
, m_bRet(bRet)
{

}



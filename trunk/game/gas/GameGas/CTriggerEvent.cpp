#include "stdafx.h"
#include "CTriggerEvent.h"
#include "CFighterDictator.h"
#include "CAllStateMgr.h"
#include "CEntityServerManager.h"
#include "CTempVarServer.h"

void CTriggerEvent::MessageEvent(ETriggerStateType eType, CFighterDictator* pFrom, CFighterDictator* pTo, EHurtResult eArg1, bool bArg2, ESkillType eArg3, EAttackType eArg4)
{
	if(pFrom && pFrom->GetTempVarMgr()->DisTriggerEvent() || pTo && pTo->GetTempVarMgr()->DisTriggerEvent()) return;

	if(pFrom/* && pFrom->GetAllStateMgr()*/)
	{
		CAllStateMgrServer* pMgr = pFrom->GetAllStateMgr();
		pMgr->MessageEvent(GetID(eArg1, bArg2, eArg3, eArg4), pTo, eType);
		if(eArg4 != eATT_None) pMgr->MessageEvent(GetID(eArg1, bArg2, eArg3, eATT_None), pTo, eType);
		if(eArg3 != eST_None) pMgr->MessageEvent(GetID(eArg1, bArg2, eST_None, eATT_None), pTo, eType);
	}
	if(pTo/* && pTo->GetAllStateMgr()*/)
	{
		CAllStateMgrServer* pMgr = pTo->GetAllStateMgr();
		pMgr->MessageEvent(GetID(eArg1, !bArg2, eArg3, eArg4), pFrom, eType);
		if(eArg4 != eATT_None) pMgr->MessageEvent(GetID(eArg1, !bArg2, eArg3, eATT_None), pFrom, eType);
		if(eArg3 != eST_None) pMgr->MessageEvent(GetID(eArg1, !bArg2, eST_None, eATT_None), pFrom, eType);
	}
}

void CTriggerEvent::MessageEvent(ETriggerStateType eType, CFighterDictator* pFrom, const CFPos& pTo, EHurtResult eArg1, bool bArg2, ESkillType eArg3, EAttackType eArg4)
{
	if(pFrom && pFrom->GetTempVarMgr()->DisTriggerEvent()) return;

	if(bArg2)
	{
		GenErr("目标为地点的触发事件不能为被动");
		return;
	}
	if(pFrom/* && pFrom->GetAllStateMgr()*/)
	{
		CAllStateMgrServer* pMgr = pFrom->GetAllStateMgr();
		pMgr->MessageEvent(GetID(eArg1, bArg2, eArg3, eArg4), pTo, eType);
		if(eArg4 != eATT_None) pMgr->MessageEvent(GetID(eArg1, bArg2, eArg3, eATT_None), pTo, eType);
		if(eArg3 != eST_None) pMgr->MessageEvent(GetID(eArg1, bArg2, eST_None, eATT_None), pTo, eType);
	}
}

void CTriggerEvent::MessageEvent(ETriggerStateType eType, CFighterDictator* pFrom, CFighterDictator* pTo, ESpecialEventType eArg1, bool bArg2)
{
	if(pFrom && pFrom->GetTempVarMgr()->DisTriggerEvent() || pTo && pTo->GetTempVarMgr()->DisTriggerEvent()) return;

	if(pFrom/* && pFrom->GetAllStateMgr()*/)
	{
		pFrom->GetAllStateMgr()->MessageEvent(GetID(eArg1, bArg2), pTo, eType);
	}
	if(pTo/* && pTo->GetAllStateMgr()*/)
	{
		pTo->GetAllStateMgr()->MessageEvent(GetID(eArg1, !bArg2), pFrom, eType);
	}
}

void CTriggerEvent::MessageEvent(ETriggerStateType eType, CFighterDictator* pFrom, const CFPos& pTo, ESpecialEventType eArg1, bool bArg2)
{
	if(pFrom && pFrom->GetTempVarMgr()->DisTriggerEvent()) return;

	if(bArg2)
	{
		GenErr("目标为地点的触发事件不能为被动");
		return;
	}
	if(pFrom/* && pFrom->GetAllStateMgr()*/)
	{
		pFrom->GetAllStateMgr()->MessageEvent(GetID(eArg1, bArg2), pTo, eType);
	}
}

void CTriggerEvent::MessageEvent(ETriggerStateType eType, uint32 uFromID, uint32 uToID, EHurtResult eArg1, bool bArg2, ESkillType eArg3, EAttackType eArg4)
{
	CEntityServer* pEntityFrom = CEntityServerManager::GetEntityByID(uFromID);
	CFighterDictator* pFrom = pEntityFrom ? pEntityFrom->GetFighter() : NULL;
	CEntityServer* pEntityTo = CEntityServerManager::GetEntityByID(uToID);
	CFighterDictator* pTo = pEntityTo ? pEntityTo->GetFighter() : NULL;
	MessageEvent(eType, pFrom, pTo, eArg1, bArg2, eArg3, eArg4);
}

void CTriggerEvent::MessageEvent(ETriggerStateType eType, uint32 uFromID, const CFPos& pTo, EHurtResult eArg1, bool bArg2, ESkillType eArg3, EAttackType eArg4)
{
	CEntityServer* pEntityFrom = CEntityServerManager::GetEntityByID(uFromID);
	CFighterDictator* pFrom = pEntityFrom ? pEntityFrom->GetFighter() : NULL;
	MessageEvent(eType, pFrom, pTo, eArg1, bArg2, eArg3, eArg4);
}

void CTriggerEvent::MessageEvent(ETriggerStateType eType, uint32 uFromID, uint32 uToID, ESpecialEventType eArg1, bool bArg2)
{
	CEntityServer* pEntityFrom = CEntityServerManager::GetEntityByID(uFromID);
	CFighterDictator* pFrom = pEntityFrom ? pEntityFrom->GetFighter() : NULL;
	CEntityServer* pEntityTo = CEntityServerManager::GetEntityByID(uToID);
	CFighterDictator* pTo = pEntityTo ? pEntityTo->GetFighter() : NULL;
	MessageEvent(eType, pFrom, pTo, eArg1, bArg2);
}

void CTriggerEvent::MessageEvent(ETriggerStateType eType, uint32 uFromID, const CFPos& pTo, ESpecialEventType eArg1, bool bArg2)
{
	CEntityServer* pEntityFrom = CEntityServerManager::GetEntityByID(uFromID);
	CFighterDictator* pFrom = pEntityFrom ? pEntityFrom->GetFighter() : NULL;
	MessageEvent(eType, pFrom, pTo, eArg1, bArg2);
}





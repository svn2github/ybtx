#include "stdafx.h"
#include "MagicOps_Function.h"
#include "CMoveMagicServer.h"
#include "CMoveMagicCfgServer.h"
#include "CBulletMagicServer.h"
#include "CBulletMagicCfgServer.h"
#include "CTransferableMagicServer.h"
#include "CTransferableMagicCfgServer.h"
#include "CCoolDownMgrServer.h"
#include "CGenericTarget.h"
#include "CAureMagicServer.h"
#include "CAureMagicCfgServer.h"
#include "CCfgCalc.inl"
#include "FighterProperty.inl"
#include "CTempVarServer.h"
#include "NormalAttack.h"
#include "CSkillMgrServer.h"
#include "CFighterMediator.h"
#include "ICharacterMediatorCallbackHandler.h"
#include "CoreCommon.h"
#include "CCharacterDictator.h"
#include "CCoreSceneServer.h"
#include "CMagicEffServer.h"
#include "CSkillServer.h"
#include "CMagicStateMgr.h"
#include "CTriggerStateMgr.h"
#include "CDamageChangeStateMgr.h"
#include "CCumuliTriggerStateMgr.h"
#include "CSpecialStateMgr.h"
#include "CSkillInstServer.h"
#include "COtherStateMgr.inl"
#include "CMagicMgrServer.h"
#include "CBaseStateServer.h"
#include "CBattleStateManager.h"
#include "CCharacterMediator.h"
#include "CActorServer.h"
#include "CCastingProcess.h"
#include "CCastingProcessCfg.h"
#include "CRelationMgrServer.h"
#include "CPixelPath.h"
#include "CTriggerEvent.h"
#include "PtrlGas2GacCPPDef.h"
#include "CPositionMagicServer.h"
#include "CPositionMagicCfgServer.h"
#include "BaseHelper.h"
#include "CBurstSoulMgrServer.h"
#include "CCfgColChecker.inl"
#include "CShockWaveMagicServer.h"
#include "CShockWaveMagicCfgServer.h"
#include "CSkillReplaceServer.h"
#include "CBurstSoulCfgServer.h"
#include "CMagicOpCfgArg.h"
#include "CRivalStateMgr.h"
#include "CDelayMagicOpTick.h"
#include "CActorCfg.h"

inline static float GetRandFloat()
{
	return rand() / float(RAND_MAX);
}

uint32 CCreateNPCMOP::Do(CSkillInstServer* pSkillInst, const CCfgCalc& Arg, CFighterDictator* pFrom, CFighterDictator* pTo)
{
	if (pFrom==NULL)
	{
		return eHR_Fail;
	}
	const string& strNPCName = Arg.GetString();
	ICharacterDictatorCallbackHandler* pHandler = pFrom->GetCallBackHandler();
	if (pHandler)
	{
		CFPos pos = pFrom->GetPixelPos();
		pHandler->OnCreateNPCWithTarget(pFrom->GetEntityID(), pTo->GetEntityID(), pFrom->GetEntityID(),
				strNPCName.c_str(), pFrom->GetCharacter()->CppGetLevel(), pos.x, pos.y);
		return eHR_Success;
	}
	return eHR_Fail;
}

uint32 CDestoryNPCMOP::Do(CSkillInstServer* pSkillInst, const CCfgCalc& Arg, CFighterDictator* pFrom, CFighterDictator* pTo)
{
	if (!pFrom || !pTo)
		return eHR_Fail;

	if (pTo->GetCharacter() && !pTo->GetCharacter()->CastToCharacterMediator())
	{
		pTo->GetCharacter()->SetOnDisappear(true);
		return eHR_Success;
	}
	return eHR_Fail;
}

uint32 CCreatePassiveNPCMOP::Do(CSkillInstServer* pSkillInst, const CCfgCalc& Arg, CFighterDictator* pFrom, CFighterDictator* pTo)
{
	if (pFrom==NULL)
	{
		return eHR_Fail;
	}
	const string& strNPCName = Arg.GetString();
	ICharacterDictatorCallbackHandler* pHandler = pFrom->GetCallBackHandler();
	if (pHandler)
	{
		CFPos pos = pFrom->GetPixelPos();
		if(pFrom != pTo)	//如果施法者和目标一样则不创建npc, 阴魂
		{
			pHandler->OnCreateNPCWithTarget(pFrom->GetEntityID(), pTo->GetEntityID(), pFrom->GetEntityID(),
			strNPCName.c_str(), pFrom->GetCharacter()->CppGetLevel(), pos.x, pos.y);
		}
		return eHR_Success;
	}
	return eHR_Fail;
}

uint32 CCreateNPCOnTargetPosMOP::Do(CSkillInstServer* pSkillInst, const CCfgCalc& Arg, CFighterDictator* pFrom, CFighterDictator* pTo)
{
	if (pFrom==NULL || pTo==NULL)
	{
		return eHR_Fail;
	}
	const string& strNPCName = Arg.GetString();
	ICharacterDictatorCallbackHandler* pHandler = pFrom->GetCallBackHandler();
	if (pHandler)
	{
		CFPos pos = pTo->GetPixelPos();
		pHandler->OnCreateNPCWithTarget(pFrom->GetEntityID(), 0, 0, strNPCName.c_str(), 0, pos.x, pos.y);
		return eHR_Success;
	}
	return eHR_Fail;
}

uint32 CSetupOrEraseTriggerStateMOP::Do(CSkillInstServer* pSkillInst, const CCfgArg* pArg, CFighterDictator* pFrom, CFighterDictator* pTo)
{
	CAllStateMgrServer* pStateMgr = pTo->GetAllStateMgr();
	Ast(pStateMgr);
	CTriggerStateMgrServer* pTriggerStateMgr = pStateMgr->GetTriggerStateMgrServer();
	CTriggerStateCfgArg* pTriggerStateCfg = static_cast<CTriggerStateCfgArg*>(const_cast<CCfgArg*>(pArg));
	if (pTriggerStateMgr)
	{
		if (pTriggerStateMgr->ExistState(*pTriggerStateCfg->m_pCfg))
		{
			pTriggerStateMgr->ClearState(*pTriggerStateCfg->m_pCfg);
		}
		else
		{
			CSetupTriggerStateMOP setupTriggerStateMOP;
			setupTriggerStateMOP.Do(pSkillInst, pArg, pFrom, pTo);
		}
	}
	return eHR_Success;
}

CCfgArg* CSetupOrEraseTriggerStateMOP::InitArg(const string& szArg)
{
	CTriggerStateCfgArg* pCfgArg = new CTriggerStateCfgArg(&CTriggerStateCfgServer::Get(szArg));
	return pCfgArg;
}

void CSetupOrEraseTriggerStateMOP::Cancel(CSkillInstServer* pSkillInst, const CCfgArg* pArg, CFighterDictator* pFrom, CFighterDictator* pTo)
{
	CAllStateMgrServer* pStateMgr = pTo->GetAllStateMgr();
	CTriggerStateMgrServer* pTriggerStateMgr =pStateMgr? pStateMgr->GetTriggerStateMgrServer():NULL;
	CTriggerStateCfgArg* pTriggerStateCfg = static_cast<CTriggerStateCfgArg*>(const_cast<CCfgArg*>(pArg));
	if (pTriggerStateMgr)
		pTriggerStateMgr->ClearState(*pTriggerStateCfg->m_pCfg);
}

uint32 CSetupOrEraseMagicStateMOP::Do(CSkillInstServer* pSkillInst, const CCfgArg* pArg, CFighterDictator* pFrom, CFighterDictator* pTo)
{
	CAllStateMgrServer* pStateMgr = pTo->GetAllStateMgr();
	Ast(pStateMgr);
	CMagicStateMgrServer* pMagicStateMgr = pStateMgr->GetMagicStateMgrServer();
	if (pMagicStateMgr)
	{
		CMagicStateCfgArg* pMagicStateCfg = static_cast<CMagicStateCfgArg*>(const_cast<CCfgArg*>(pArg));
		if (pMagicStateMgr->ExistState(*pMagicStateCfg->m_pCfg))
		{
			pMagicStateMgr->ClearState(*pMagicStateCfg->m_pCfg);
		}
		else
		{
			CSetupMagicStateMOP setupMagicStateMOP;
			setupMagicStateMOP.Do(pSkillInst, pArg, pFrom, pTo);
		}
	}
	return eHR_Success;
}

CCfgArg* CSetupOrEraseMagicStateMOP::InitArg(const string& szArg)
{
	CMagicStateCfgArg* pCfgArg = new CMagicStateCfgArg(&CMagicStateCfgServer::Get(szArg));
	return pCfgArg;
}

void CSetupOrEraseMagicStateMOP::Cancel(CSkillInstServer* pSkillInst, const CCfgArg* pArg, CFighterDictator* pFrom, CFighterDictator* pTo)
{
	CAllStateMgrServer* pStateMgr = pTo->GetAllStateMgr();
	CMagicStateMgrServer* pMagicStateMgr = pStateMgr?pStateMgr->GetMagicStateMgrServer():NULL;
	if (pMagicStateMgr)
	{
		CMagicStateCfgArg* pMagicStateCfg = static_cast<CMagicStateCfgArg*>(const_cast<CCfgArg*>(pArg));
		pMagicStateMgr->ClearState(*pMagicStateCfg->m_pCfg);
	}
}

uint32 CSetupMultiMagicStateMOP::Do(CSkillInstServer *pSkillInst, const CCfgArg* pArg, CFighterDictator *pFrom, CFighterDictator *pTo)
{
	CAllStateMgrServer* pStateMgr = pTo->GetAllStateMgr();
	Ast(pStateMgr);
	CMagicStateMgrServer* pMagicStateMgr = pStateMgr->GetMagicStateMgrServer();
	if (pMagicStateMgr)
	{
		CMagicStateCfgMultiArg* pMagicStateCfg = static_cast<CMagicStateCfgMultiArg*>(const_cast<CCfgArg*>(pArg));
		uint32 uMulti = pMagicStateCfg->m_pArg->GetIntValue(pFrom, pTo, pSkillInst->GetLevelIndex());

		for (uint32 i = 0; i < uMulti; ++i)
		{
			CSetupMagicStateMOP setupMagicStateMOP;
			setupMagicStateMOP.Do(pSkillInst, pArg, pFrom, pTo);
		}
	}
	return eHR_Success;
}

void CSetupMultiMagicStateMOP::Cancel(CSkillInstServer *pSkillInst, const CCfgArg* pArg, CFighterDictator *pFrom, CFighterDictator *pTo)
{
	CAllStateMgrServer* pStateMgr = pTo->GetAllStateMgr();
	CMagicStateMgrServer* pMagicStateMgr = pStateMgr?pStateMgr->GetMagicStateMgrServer():NULL;
	if (pMagicStateMgr)
	{
		CMagicStateCfgMultiArg* pMagicStateCfg = static_cast<CMagicStateCfgMultiArg*>(const_cast<CCfgArg*>(pArg));
		pMagicStateMgr->ClearState(*pMagicStateCfg->m_pCfg);
	}
}

CCfgArg* CSetupMultiMagicStateMOP::InitArg(const string& szArg)
{
	CCfgCalc pCalcCalc(szArg);
	CMagicStateCfgMultiArg* pCfgArg = new CMagicStateCfgMultiArg(&CMagicStateCfgServer::Get(pCalcCalc.GetString(0)),pCalcCalc.GetString(1));
	return pCfgArg;
}

//安装或清除姿态
uint32 CSetupOrEraseAureMagicMOP::Do(CSkillInstServer* pSkillInst, const CCfgArg* pArg, CFighterDictator* pFrom, CFighterDictator* pTo)
{
	CAureMagicCfgArg* pArueMagicCfgArg = static_cast<CAureMagicCfgArg*>(const_cast<CCfgArg*>(pArg));
	CAureMagicCfgServerSharedPtr& cfg = *pArueMagicCfgArg->m_pCfg;
	if (!CAureMagicServer::CancelTheExistedStance(cfg, pFrom))
	{
		new CAureMagicServer(pSkillInst, cfg, pFrom, pTo);
	}
	return eHR_Success;
}

CCfgArg* CSetupOrEraseAureMagicMOP::InitArg(const string& szArg)
{
	CAureMagicCfgArg* pCfgArg = new CAureMagicCfgArg(&CAureMagicCfgServer::GetAureMagic(szArg));
	return pCfgArg;
}

//安装姿态
uint32  CSetupAureMagicMOP::Do(CSkillInstServer* pSkillInst, const CCfgArg* pArg, CFighterDictator* pFrom, CFighterDictator* pTo)
{
	CAureMagicCfgArg* pArueMagicCfgArg = static_cast<CAureMagicCfgArg*>(const_cast<CCfgArg*>(pArg));
	CAureMagicCfgServerSharedPtr& cfg = *pArueMagicCfgArg->m_pCfg;
	if (!CAureMagicServer::HasAureMagic(cfg, pFrom))
	{
		new CAureMagicServer(pSkillInst, cfg, pFrom, pTo);
	}
	return eHR_Success;
}

CCfgArg* CSetupAureMagicMOP::InitArg(const string& szArg)
{
	CAureMagicCfgArg* pCfgArg = new CAureMagicCfgArg(&CAureMagicCfgServer::GetAureMagic(szArg));
	return pCfgArg;
}

void CSetupAureMagicMOP::Cancel(CSkillInstServer* pSkillInst, const CCfgArg* pArg, CFighterDictator* pFrom, CFighterDictator* pTo)
{
	CAureMagicCfgArg* pArueMagicCfgArg = static_cast<CAureMagicCfgArg*>(const_cast<CCfgArg*>(pArg));
	CAureMagicCfgServerSharedPtr& cfg = *pArueMagicCfgArg->m_pCfg;
	CAureMagicServer::CancelTheExistedStance(cfg, pFrom);
}

//切换姿态
uint32 CChangeAureMagicMOP::Do(CSkillInstServer* pSkillInst, const CCfgArg* pArg, CFighterDictator* pFrom, CFighterDictator* pTo)
{
	CAureMagicCfgArg* pArueMagicCfgArg = static_cast<CAureMagicCfgArg*>(const_cast<CCfgArg*>(pArg));
	CAureMagicCfgServerSharedPtr& cfg = *pArueMagicCfgArg->m_pCfg;

	CAureMagicServer::CancelAllAureMagicByType(pTo, eSS_MutexStance);
	new CAureMagicServer(pSkillInst, cfg, pFrom, pTo);

	return eHR_Success;
}

CCfgArg* CChangeAureMagicMOP::InitArg(const string& szArg)
{
	CAureMagicCfgArg* pCfgArg = new CAureMagicCfgArg(&CAureMagicCfgServer::GetAureMagic(szArg));
	return pCfgArg;
}

//清除姿态
uint32 CEraseAureMagicMOP::Do(CSkillInstServer* pSkillInst, const CCfgArg* pArg, CFighterDictator* pFrom, CFighterDictator* pTo)
{
	CAureMagicCfgArg* pArueMagicCfgArg = static_cast<CAureMagicCfgArg*>(const_cast<CCfgArg*>(pArg));
	CAureMagicCfgServerSharedPtr& cfg = *pArueMagicCfgArg->m_pCfg;
	CAureMagicServer::CancelTheExistedStance(cfg, pTo);
	return eHR_Success;
}
CCfgArg* CEraseAureMagicMOP::InitArg(const string& szArg)
{
	CAureMagicCfgArg* pCfgArg = new CAureMagicCfgArg(&CAureMagicCfgServer::GetAureMagic(szArg));
	return pCfgArg;
}

uint32 CEraseAureMagicOnCancellingMOP::Do(CSkillInstServer* pSkillInst, const CCfgArg* pArg, CFighterDictator* pFrom, CFighterDictator* pTo)
{
	return eHR_Success;
}

void CEraseAureMagicOnCancellingMOP::Cancel(CSkillInstServer* pSkillInst, const CCfgArg* pArg, CFighterDictator* pFrom, CFighterDictator* pTo)
{
	CEraseAureMagicMOP EraseAureMagicMOP;
	EraseAureMagicMOP.Do(pSkillInst, pArg, pFrom, pTo);
}

CCfgArg* CEraseAureMagicOnCancellingMOP::InitArg(const string& szArg)
{
	CAureMagicCfgArg* pCfgArg = new CAureMagicCfgArg(&CAureMagicCfgServer::GetAureMagic(szArg));
	return pCfgArg;
}

void MessageEventSetupState(EDecreaseStateType eType, CFighterDictator* pFrom, CFighterDictator* pTo)
{
	switch (eType)
	{
	case eDST_Control:
		CTriggerEvent::MessageEvent(eTST_Trigger, pFrom, pTo, eSET_SetupControlState);
		break;
	case eDST_Pause:
		CTriggerEvent::MessageEvent(eTST_Trigger, pFrom, pTo, eSET_SetupPauseState);
		break;
	case eDST_Crippling:
		CTriggerEvent::MessageEvent(eTST_Trigger, pFrom, pTo, eSET_SetupCripplingState);
		break;
	case eDST_Special:
		CTriggerEvent::MessageEvent(eTST_Trigger, pFrom, pTo, eSET_SetupSpecialState);
		break;
	default:
		break;
	}
}

uint32 CIgnoreImmuneSetupSpecialStateMOP::Do(CSkillInstServer *pSkillInst, const CCfgArg *pArg, CFighterDictator *pFrom, CFighterDictator *pTo)
{
	CAllStateMgrServer* pStateMgr = pTo->GetAllStateMgr();
	CMagicStateMgrServer* pMagicStateMgr = pStateMgr->GetMagicStateMgrServer();

	CMagicStateCfgArg* pMagicStateCfg = static_cast<CMagicStateCfgArg*>(const_cast<CCfgArg*>(pArg));
	CMagicStateCfgServerSharedPtr& pCfg = *pMagicStateCfg->m_pCfg;

	if (pSkillInst->GetForbitSetupState() && pCfg->GetNeedSaveToDB())
		return eHR_Success;

	EHurtResult Ret = eHR_Fail;

	if (pMagicStateMgr&&pMagicStateMgr->IgnoreImmuneSetupState(pSkillInst, pCfg, pFrom))
	{
		Ret = eHR_Success;
	}

	if (pCfg->GetTouchBattleState() && pFrom != pTo && CRelationMgrServer::IsEnemy(pFrom, pTo))
	{
		CMagicStateTouchBattleStateMOP magicStateTouchBattleState;
		magicStateTouchBattleState.Do(pSkillInst, pArg, pFrom, pTo);
	}

	if ( eHR_Success ==Ret)
	{
		MessageEventSetupState(pCfg->GetDecreaseType(), pFrom, pTo);
	}

	if (!pCfg->GetDecrease() && pFrom && pFrom->CastToFighterMediator() &&
		pFrom != pTo && pTo->GetCtrlState(eFCS_InBattle))
	{
		pTo->Notify(eCE_EntryEnmity,reinterpret_cast<void*>(pFrom));
	}

	return Ret;
}

void CIgnoreImmuneSetupSpecialStateMOP::Cancel(CSkillInstServer* pSkillInst, const CCfgArg* pArg, CFighterDictator* pFrom, CFighterDictator* pTo)
{
	//	因为在CAllStateMgrServer::~CAllStateMgrServer()中先析构MagicStateMgrServer，
	//	如果在接下来析构的触发器的可撤销魔法效果中安装了某个魔法状态的话就会调到这里来，
	//	但此时MagicStateMgrServer已经被析构了
	CAllStateMgrServer* pStateMgr = pTo->GetAllStateMgr();
	CMagicStateMgrServer* pMagicStateMgr = pStateMgr?pStateMgr->GetMagicStateMgrServer():NULL;
	if (pMagicStateMgr)	//因为在Fighter
	{
		CMagicStateCfgArg* pMagicStateCfg = static_cast<CMagicStateCfgArg*>(const_cast<CCfgArg*>(pArg));
		pMagicStateMgr->DecStateCascade(*pMagicStateCfg->m_pCfg, 1, true);
	}
	//pTo->GetAllStateMgr()->GetMagicStateMgrServer()->ClearState(Arg.GetString()/*, pFrom*/);
}

CCfgArg* CIgnoreImmuneSetupSpecialStateMOP::InitArg(const string& szArg)
{
	CMagicStateCfgArg* pCfgArg = new CMagicStateCfgArg(&CMagicStateCfgServer::Get(szArg));
	return pCfgArg;
}

uint32 CPassiveSetupMagicStateMOP::Do(CSkillInstServer* pSkillInst, const CCfgArg* pArg, CFighterDictator* pFrom, CFighterDictator* pTo)
{
	CAllStateMgrServer* pStateMgr = pTo->GetAllStateMgr();
	CMagicStateMgrServer* pMagicStateMgr = pStateMgr->GetMagicStateMgrServer();

	CMagicStateCfgArg* pMagicStateCfg = static_cast<CMagicStateCfgArg*>(const_cast<CCfgArg*>(pArg));
	CMagicStateCfgServerSharedPtr& pCfg = *pMagicStateCfg->m_pCfg;

	if (pSkillInst->GetForbitSetupState() && pCfg->GetNeedSaveToDB())
		return eHR_Success;

	EHurtResult Ret = eHR_Fail;

	if (pMagicStateMgr&&pMagicStateMgr->SetupState(pSkillInst, pCfg, pFrom))
	{
		Ret = eHR_Success;
	}

	if (pCfg->GetTouchBattleState() && pFrom != pTo && CRelationMgrServer::IsEnemy(pFrom, pTo))
	{
		// TODO:控制类，目标处于玩法中，直接给干掉
		CMagicStateTouchBattleStateMOP magicStateTouchBattleState;
		magicStateTouchBattleState.Do(pSkillInst, pArg, pFrom, pTo);
	}

	if ( eHR_Success ==Ret)
	{
		MessageEventSetupState(pCfg->GetDecreaseType(), pFrom, pTo);
	}
	return Ret;
}

void CPassiveSetupMagicStateMOP::Cancel(CSkillInstServer* pSkillInst, const CCfgArg* pArg, CFighterDictator* pFrom, CFighterDictator* pTo)
{
	//	因为在CAllStateMgrServer::~CAllStateMgrServer()中先析构MagicStateMgrServer，
	//	如果在接下来析构的触发器的可撤销魔法效果中安装了某个魔法状态的话就会调到这里来，
	//	但此时MagicStateMgrServer已经被析构了
	CAllStateMgrServer* pStateMgr = pTo->GetAllStateMgr();
	CMagicStateMgrServer* pMagicStateMgr = pStateMgr?pStateMgr->GetMagicStateMgrServer():NULL;
	if (pMagicStateMgr)	//因为在Fighter
	{
		CMagicStateCfgArg* pMagicStateCfg = static_cast<CMagicStateCfgArg*>(const_cast<CCfgArg*>(pArg));
		pMagicStateMgr->DecStateCascade(*pMagicStateCfg->m_pCfg, 1, true);
	}
	//pTo->GetAllStateMgr()->GetMagicStateMgrServer()->ClearState(Arg.GetString()/*, pFrom*/);
}

CCfgArg* CPassiveSetupMagicStateMOP::InitArg(const string& szArg)
{
	CMagicStateCfgArg* pCfgArg = new CMagicStateCfgArg(&CMagicStateCfgServer::Get(szArg));
	return pCfgArg;
}

uint32 CSetupMagicStateMOP::Do(CSkillInstServer* pSkillInst, const CCfgArg* pArg, CFighterDictator* pFrom, CFighterDictator* pTo)
{
	CMagicStateCfgArg* pMagicStateCfg = static_cast<CMagicStateCfgArg*>(const_cast<CCfgArg*>(pArg));
	uint32 Ret = CPassiveSetupMagicStateMOP::Do(pSkillInst, pArg, pFrom, pTo);
	CMagicStateCfgServerSharedPtr& pCfg = *pMagicStateCfg->m_pCfg;
	if (!pCfg->GetDecrease() && pFrom && pFrom->CastToFighterMediator() &&
		pFrom != pTo && pTo->GetCtrlState(eFCS_InBattle))
	{
		pTo->Notify(eCE_EntryEnmity,reinterpret_cast<void*>(pFrom));
	}

	return Ret;
}

CCfgArg* CSetupMagicStateMOP::InitArg(const string& szArg)
{
	CMagicStateCfgArg* pCfgArg = new CMagicStateCfgArg(&CMagicStateCfgServer::Get(szArg));
	return pCfgArg;
}

uint32 CSetupTriggerStateMOP::Do(CSkillInstServer* pSkillInst, const CCfgArg* pArg, CFighterDictator* pFrom, CFighterDictator* pTo)
{
	CAllStateMgrServer* pStateMgr = pTo->GetAllStateMgr();
	Ast(pStateMgr);
	CTriggerStateMgrServer* pTriggerStateMgr = pStateMgr->GetTriggerStateMgrServer();
	CTriggerStateCfgArg* pTriggerStateCfg = static_cast<CTriggerStateCfgArg*>(const_cast<CCfgArg*>(pArg));
	CTriggerStateCfgServerSharedPtr& pCfg = *pTriggerStateCfg->m_pCfg;
	if (pSkillInst->GetForbitSetupState() && pCfg->GetNeedSaveToDB())
		return eHR_Success;

	EHurtResult Ret = eHR_Fail;
	if (pTriggerStateMgr&&pTriggerStateMgr->SetupState(pSkillInst, pCfg, pFrom))
	{
		Ret = eHR_Success;
	}

	if (pCfg->GetTouchBattleState() && pFrom != pTo && CRelationMgrServer::IsEnemy(pFrom, pTo))
	{
		CMagicStateTouchBattleStateMOP magicStateTouchBattleState;
		magicStateTouchBattleState.Do(pSkillInst, pArg, pFrom, pTo);
	}
	if (!pCfg->GetDecrease() && pFrom && pFrom->CastToFighterMediator() &&
		pFrom != pTo && pTo->GetCtrlState(eFCS_InBattle))
	{
		pTo->Notify(eCE_EntryEnmity,reinterpret_cast<void*>(pFrom));
	}

	if ( eHR_Success ==Ret)
	{
		MessageEventSetupState(pCfg->GetDecreaseType(), pFrom, pTo);
	}
	return Ret;
}

CCfgArg* CSetupTriggerStateMOP::InitArg(const string& szArg)
{
	CTriggerStateCfgArg* pCfgArg = new CTriggerStateCfgArg(&CTriggerStateCfgServer::Get(szArg));
	return pCfgArg;
}

void CSetupTriggerStateMOP::Cancel(CSkillInstServer* pSkillInst, const CCfgArg* pArg, CFighterDictator* pFrom, CFighterDictator* pTo)
{
	CAllStateMgrServer* pStateMgr = pTo->GetAllStateMgr();
	CTriggerStateMgrServer* pTriggerStateMgr =pStateMgr? pStateMgr->GetTriggerStateMgrServer():NULL;
	CTriggerStateCfgArg* pTriggerStateCfg = static_cast<CTriggerStateCfgArg*>(const_cast<CCfgArg*>(pArg));
	if (pTriggerStateMgr)
		pTriggerStateMgr->ClearState(*pTriggerStateCfg->m_pCfg);
}


uint32 CSetupDamageChangeStateMOP::Do(CSkillInstServer* pSkillInst, const CCfgArg* pArg, CFighterDictator* pFrom, CFighterDictator* pTo)
{
	CDamageChangeStateCfgArg* pDamageChangeStateCfg = static_cast<CDamageChangeStateCfgArg*>(const_cast<CCfgArg*>(pArg));
	CDamageChangeStateCfgServerSharedPtr& pCfg = *pDamageChangeStateCfg->m_pCfg;
	CAllStateMgrServer* pStateMgr = pTo->GetAllStateMgr();
	Ast(pStateMgr);
	CDamageChangeStateMgrServer* pDamageChangeStateMgr = pStateMgr->GetDamageChangeStateMgrServer();
	if (pSkillInst->GetForbitSetupState() && pCfg->GetNeedSaveToDB())
		return eHR_Success;

	EHurtResult Ret = eHR_Fail;
	if (pDamageChangeStateMgr&&pDamageChangeStateMgr->SetupState(pSkillInst, pCfg,pFrom))
	{
		Ret = eHR_Success;
	}

	if (pCfg->GetTouchBattleState() && pFrom != pTo && CRelationMgrServer::IsEnemy(pFrom, pTo))
	{
		CMagicStateTouchBattleStateMOP magicStateTouchBattleState;
		magicStateTouchBattleState.Do(pSkillInst, pArg, pFrom, pTo);
	}
	if (!pCfg->GetDecrease() && pFrom && pFrom->CastToFighterMediator() &&
		pFrom != pTo && pTo->GetCtrlState(eFCS_InBattle))
	{
		pTo->Notify(eCE_EntryEnmity,reinterpret_cast<void*>(pFrom));
	}

	if ( eHR_Success ==Ret)
	{
		MessageEventSetupState(pCfg->GetDecreaseType(), pFrom, pTo);
	}
	return Ret;
}

CCfgArg* CSetupDamageChangeStateMOP::InitArg(const string& szArg)
{
	CDamageChangeStateCfgArg* pCfgArg = new CDamageChangeStateCfgArg(&CDamageChangeStateCfgServer::Get(szArg));
	return pCfgArg;
}

void CSetupDamageChangeStateMOP::Cancel(CSkillInstServer* pSkillInst, const CCfgArg* pArg, CFighterDictator* pFrom, CFighterDictator* pTo)
{
	CAllStateMgrServer* pStateMgr = pTo->GetAllStateMgr();
	CDamageChangeStateMgrServer* pDamageChangeStateMgr = pStateMgr?pStateMgr->GetDamageChangeStateMgrServer():NULL;
	if (pDamageChangeStateMgr)
	{
		CDamageChangeStateCfgArg* pDamageChangeStateCfg = static_cast<CDamageChangeStateCfgArg*>(const_cast<CCfgArg*>(pArg));
		pDamageChangeStateMgr->ClearState(*pDamageChangeStateCfg->m_pCfg);
	}
}

uint32 CSetupCumuliTriggerStateMOP::Do(CSkillInstServer* pSkillInst, const CCfgArg* pArg, CFighterDictator* pFrom, CFighterDictator* pTo)
{
	CAllStateMgrServer* pStateMgr = pTo->GetAllStateMgr();
	Ast(pStateMgr);
	CCumuliTriggerStateMgrServer* pCumuliTriggerStateMgr = pStateMgr->GetCumuliTriggerStateMgrServer();
	CCumuliTriggerStateCfgArg* pCumuliTriggerStateCfg = static_cast<CCumuliTriggerStateCfgArg*>(const_cast<CCfgArg*>(pArg));
	CCumuliTriggerStateCfgServerSharedPtr& pCfg = *pCumuliTriggerStateCfg->m_pCfg;
	if (pSkillInst->GetForbitSetupState() && pCfg->GetNeedSaveToDB())
		return eHR_Success;

	EHurtResult Ret = eHR_Fail;
	if (pCumuliTriggerStateMgr&&pCumuliTriggerStateMgr->SetupState(pSkillInst, pCfg, pFrom))
	{
		Ret = eHR_Success;
	}

	if (pCfg->GetTouchBattleState() && pFrom != pTo && CRelationMgrServer::IsEnemy(pFrom, pTo))
	{
		CMagicStateTouchBattleStateMOP magicStateTouchBattleState;
		magicStateTouchBattleState.Do(pSkillInst, pArg, pFrom, pTo);
	}
	if (!pCfg->GetDecrease() && pFrom && pFrom->CastToFighterMediator() &&
		pFrom != pTo && pTo->GetCtrlState(eFCS_InBattle))
	{
		pTo->Notify(eCE_EntryEnmity,reinterpret_cast<void*>(pFrom));
	}

	if ( eHR_Success ==Ret)
	{
		MessageEventSetupState(pCfg->GetDecreaseType(), pFrom, pTo);
	}
	return Ret;
}

CCfgArg* CSetupCumuliTriggerStateMOP::InitArg(const string& szArg)
{
	CCumuliTriggerStateCfgArg* pCfgArg = new CCumuliTriggerStateCfgArg(&CCumuliTriggerStateCfgServer::Get(szArg));
	return pCfgArg;
}

void CSetupCumuliTriggerStateMOP::Cancel(CSkillInstServer* pSkillInst, const CCfgArg* pArg, CFighterDictator* pFrom, CFighterDictator* pTo)
{
	CAllStateMgrServer* pStateMgr = pTo->GetAllStateMgr();
	CCumuliTriggerStateMgrServer* pCumuliTriggerStateMgr =pStateMgr? pStateMgr->GetCumuliTriggerStateMgrServer():NULL;
	CCumuliTriggerStateCfgArg* pCumuliTriggerStateCfg = static_cast<CCumuliTriggerStateCfgArg*>(const_cast<CCfgArg*>(pArg));
	if (pCumuliTriggerStateMgr)
		pCumuliTriggerStateMgr->ClearState(*pCumuliTriggerStateCfg->m_pCfg);
}


uint32 CSetupSpecialStateMOP::Do(CSkillInstServer* pSkillInst, const CCfgArg* pArg, CFighterDictator* pFrom, CFighterDictator* pTo)
{
	CAllStateMgrServer* pStateMgr = pTo->GetAllStateMgr();
	Ast(pStateMgr);
	CSpecialStateMgrServer* pSpecialStateMgr = pStateMgr->GetSpecialStateMgrServer();
	CSpecialStateCfgArg* pSpecialStateCfg = static_cast<CSpecialStateCfgArg*>(const_cast<CCfgArg*>(pArg));
	CSpecialStateCfgServerSharedPtr& pCfg = *pSpecialStateCfg->m_pCfg;
	if (pSkillInst->GetForbitSetupState() && pCfg->GetNeedSaveToDB())
		return eHR_Success;

	EHurtResult Ret = eHR_Fail;
	if (pSpecialStateMgr&&pSpecialStateMgr->SetupState(pSkillInst, pCfg,pFrom))
	{
		Ret = eHR_Success;
	}

	if (pCfg->GetTouchBattleState() && pFrom != pTo && CRelationMgrServer::IsEnemy(pFrom, pTo))
	{
		CMagicStateTouchBattleStateMOP magicStateTouchBattleState;
		magicStateTouchBattleState.Do(pSkillInst, pArg, pFrom, pTo);
	}
	if (!pCfg->GetDecrease() && pFrom && pFrom->CastToFighterMediator() &&
		pFrom != pTo && pTo->GetCtrlState(eFCS_InBattle))
	{
		pTo->Notify(eCE_EntryEnmity,reinterpret_cast<void*>(pFrom));
	}

	if ( eHR_Success ==Ret)
	{
		MessageEventSetupState(pCfg->GetDecreaseType(), pFrom, pTo);
	}
	return Ret;
}


void CSetupSpecialStateMOP::Cancel(CSkillInstServer* pSkillInst, const CCfgArg* pArg, CFighterDictator* pFrom, CFighterDictator* pTo)
{
	CAllStateMgrServer* pStateMgr = pTo->GetAllStateMgr();
	CSpecialStateMgrServer* pSpecialStateMgr = pStateMgr?pStateMgr->GetSpecialStateMgrServer():NULL;
	if (pSpecialStateMgr)
	{
		CSpecialStateCfgArg* pSpecialStateCfg = static_cast<CSpecialStateCfgArg*>(const_cast<CCfgArg*>(pArg));
		pSpecialStateMgr->ClearState(*pSpecialStateCfg->m_pCfg);
	}
}

CCfgArg* CSetupSpecialStateMOP::InitArg(const string& szArg)
{
	CSpecialStateCfgArg* pCfgArg = new CSpecialStateCfgArg(&CSpecialStateCfgServer::Get(szArg));
	return pCfgArg;
}

uint32 CSetupMagicStateByMaxSkillLevelMOP::Do(CSkillInstServer* pSkillInst, const CCfgArg* pArg, CFighterDictator* pFrom, CFighterDictator* pTo)
{
	CAllStateMgrServer* pStateMgr = pTo->GetAllStateMgr();
	CMagicStateMgrServer* pMagicStateMgr = pStateMgr->GetMagicStateMgrServer();

	CMagicStateCfgArg* pMagicStateCfg = static_cast<CMagicStateCfgArg*>(const_cast<CCfgArg*>(pArg));
	CMagicStateCfgServerSharedPtr& pCfg = *pMagicStateCfg->m_pCfg;

	CRivalStateMgr* pRivalStateMgr = pMagicStateMgr->GetRivalStateMgr();

	CStateFuncArg aStateFuncArg(pSkillInst, pArg, pFrom, pTo);
	pRivalStateMgr->AddState(pCfg->GetName(), aStateFuncArg);

	//if (pSkillInst->GetForbitSetupState() && pCfg->GetNeedSaveToDB())
	//	return eHR_Success;

	EHurtResult Ret = eHR_Success;

	//if (pMagicStateMgr&&pMagicStateMgr->SetupState(pSkillInst, pCfg, pFrom))
	//{
	//	Ret = eHR_Success;
	//}

	//if (pCfg->GetTouchBattleState() && pFrom != pTo && CRelationMgrServer::IsEnemy(pFrom, pTo))
	//{
	//	// TODO:控制类，目标处于玩法中，直接给干掉
	//	CMagicStateTouchBattleStateMOP magicStateTouchBattleState;
	//	magicStateTouchBattleState.Do(pSkillInst, pArg, pFrom, pTo);
	//}

	//if (!pCfg->GetDecrease() && pFrom && pFrom->CastToFighterMediator() &&
	//	pFrom != pTo && pTo->GetCtrlState(eFCS_InBattle))
	//{
	//	pTo->Notify(eCE_EntryEnmity,reinterpret_cast<void*>(pFrom));
	//}

	//if ( eHR_Success ==Ret)
	//{
	//	MessageEventSetupState(pCfg->GetDecreaseType(), pFrom, pTo);
	//}
	return Ret;
}

void CSetupMagicStateByMaxSkillLevelMOP::Cancel(CSkillInstServer* pSkillInst, const CCfgArg* pArg, CFighterDictator* pFrom, CFighterDictator* pTo)
{
	//	因为在CAllStateMgrServer::~CAllStateMgrServer()中先析构MagicStateMgrServer，
	//	如果在接下来析构的触发器的可撤销魔法效果中安装了某个魔法状态的话就会调到这里来，
	//	但此时MagicStateMgrServer已经被析构了
	CAllStateMgrServer* pStateMgr = pTo->GetAllStateMgr();
	CMagicStateMgrServer* pMagicStateMgr = pStateMgr?pStateMgr->GetMagicStateMgrServer():NULL;
	if (pMagicStateMgr)	//因为在Fighter
	{
		CMagicStateCfgArg* pMagicStateCfg = static_cast<CMagicStateCfgArg*>(const_cast<CCfgArg*>(pArg));
		//pMagicStateMgr->DecStateCascade(*pMagicStateCfg->m_pCfg, 1, true);
		CRivalStateMgr* pRivalStateMgr = pMagicStateMgr->GetRivalStateMgr();
		CStateFuncArg aStateFuncArg(pSkillInst, pArg, pFrom, pTo);
		pRivalStateMgr->DelState((*pMagicStateCfg->m_pCfg)->GetName(), aStateFuncArg);

	}
	//pTo->GetAllStateMgr()->GetMagicStateMgrServer()->ClearState(Arg.GetString()/*, pFrom*/);
}

CCfgArg* CSetupMagicStateByMaxSkillLevelMOP::InitArg(const string& szArg)
{
	CMagicStateCfgArg* pCfgArg = new CMagicStateCfgArg(&CMagicStateCfgServer::Get(szArg));
	return pCfgArg;
}

uint32 CCancelMagicStateCascadeMOP::Do(CSkillInstServer* pSkillInst, const CCfgArg* pArg, CFighterDictator* pFrom, CFighterDictator* pTo)
{
	CAllStateMgrServer* pStateMgr = pTo->GetAllStateMgr();
	Ast(pStateMgr);
	CMagicStateMgrServer* pMagicStateMgr = pStateMgr->GetMagicStateMgrServer();
	CMagicStateCfgMultiArg* pMagicStateCfg = static_cast<CMagicStateCfgMultiArg*>(const_cast<CCfgArg*>(pArg));
	uint32 uCascade = (uint32)atoi(pMagicStateCfg->m_pArg->GetString().c_str());
	if (pMagicStateMgr&&pMagicStateMgr->DecStateCascade(*pMagicStateCfg->m_pCfg, uCascade))
	{
		return eHR_Success;
	}
	else
	{
		return eHR_Fail;
	}
}

CCfgArg* CCancelMagicStateCascadeMOP::InitArg(const string& szArg)
{
	CCfgCalc pCalcCalc(szArg);
	return new CMagicStateCfgMultiArg(&CMagicStateCfgServer::Get(pCalcCalc.GetString(0)),pCalcCalc.GetString(1));
}

uint32 CCancelMagicStateCascadeByEffectMOP::Do(CSkillInstServer* pSkillInst, const CCfgArg* pArg, CFighterDictator* pFrom, CFighterDictator* pTo)
{
	CMagicStateCfgMultiArg* pMagicStateCfg = static_cast<CMagicStateCfgMultiArg*>(const_cast<CCfgArg*>(pArg));
	uint32 uCascade = (uint32)atoi(pMagicStateCfg->m_pArg->GetString().c_str());
	CAllStateMgrServer* pStateMgr = pTo->GetAllStateMgr();
	Ast(pStateMgr);
	CMagicStateMgrServer* pMagicStateMgr = pStateMgr->GetMagicStateMgrServer();
	if (pMagicStateMgr)
	{
		const string& sStateName = pMagicStateCfg->m_pCfg->get()->GetName();
		uint32 uOldCascade = (uint32)pMagicStateMgr->MagicStateCount(sStateName);
		if(uOldCascade < 1)
		{
			return eHR_Success;
		}
		else if(uOldCascade < uCascade)
		{
			uCascade = uOldCascade;
		}

		if(uOldCascade == uCascade)
		{
			//CEraseStateMOP pMop;
			//pMop.Do(pSkillInst, Arg, pFrom, pTo);
			pMagicStateMgr->DecStateCascade(*pMagicStateCfg->m_pCfg, uCascade);
			return eHR_Success;
		}
		else
		{
			uint32 l = uOldCascade - uCascade - 1;
			if(l == 0)
			{
				//pMagicStateMgr->ResetTime(*pMagicStateCfg->m_pCfg, pFrom);
				pMagicStateMgr->DecStateCascade(*pMagicStateCfg->m_pCfg, uOldCascade);
				CSetupMagicStateMOP setupMagicStateMOP;
				setupMagicStateMOP.Do(pSkillInst, pArg, pFrom, pTo);
			}
			else
			{
				pMagicStateMgr->DecStateCascade(*pMagicStateCfg->m_pCfg, uOldCascade - 1);
				for (uint32 i = 0; i < l; ++i)
				{
					CSetupMagicStateMOP setupMagicStateMOP;
					setupMagicStateMOP.Do(pSkillInst, pArg, pFrom, pTo);
				}
			}
			return eHR_Success;
		}
	}
	return eHR_Fail;
}

CCfgArg* CCancelMagicStateCascadeByEffectMOP::InitArg(const string& szArg)
{
	CCfgCalc pCfgCalc(szArg);
	CMagicStateCfgMultiArg* pCfgArg = new CMagicStateCfgMultiArg(&CMagicStateCfgServer::Get(pCfgCalc.GetString(0)), pCfgCalc.GetString(1));
	return pCfgArg;
}

uint32 CTriggerStateMessageEventMOP::Do(CSkillInstServer* pSkillInst,const CCfgCalc& nValue,CFighterDictator* pFrom,CFighterDictator* pTo)
{
	CAllStateMgrServer* pStateMgr = pTo->GetAllStateMgr();
	Ast(pStateMgr);
	CTriggerStateMgrServer* pTriggerStateMgr = pStateMgr->GetTriggerStateMgrServer();
	if (pTriggerStateMgr==NULL)
	{
		return eHR_Fail;
	}
	CTriggerStateServer * pTS = pTriggerStateMgr->FindState(nValue.GetString());
	CGenericTarget target(pFrom);
	if (pTS && pTS->MessageEvent(0, &target).second)
		return eHR_Success;
	else
		return eHR_Fail;
}

uint32 CResetMagicStateTimeMOP::Do(CSkillInstServer* pSkillInst, const CCfgArg* pArg, CFighterDictator* pFrom, CFighterDictator* pTo)
{
	CAllStateMgrServer* pStateMgr = pTo->GetAllStateMgr();
	Ast(pStateMgr);
	CMagicStateMgrServer* pMagicStateMgr = pStateMgr->GetMagicStateMgrServer();
	CMagicStateCfgArg* pMagicStateCfg = static_cast<CMagicStateCfgArg*>(const_cast<CCfgArg*>(pArg));
	if (pMagicStateMgr&&pMagicStateMgr->ResetTime(*pMagicStateCfg->m_pCfg, pFrom))
	{
		return eHR_Success;
	}
	else
	{
		return eHR_Fail;
	}
}

CCfgArg* CResetMagicStateTimeMOP::InitArg(const string& szArg)
{
	return new CMagicStateCfgArg(&CMagicStateCfgServer::Get(szArg));
}

uint32 CAddMagicStateTimeMOP::Do(CSkillInstServer* pSkillInst, const CCfgArg* pArg, CFighterDictator* pFrom, CFighterDictator* pTo)
{
	CAllStateMgrServer* pStateMgr = pTo->GetAllStateMgr();
	Ast(pStateMgr);
	CMagicStateMgrServer* pMagicStateMgr = pStateMgr->GetMagicStateMgrServer();
	CMagicStateCfgMultiArg* pMagicStateCfg = static_cast<CMagicStateCfgMultiArg*>(const_cast<CCfgArg*>(pArg));
	if (pMagicStateMgr&&pMagicStateMgr->AddTime(*pMagicStateCfg->m_pCfg, atoi(pMagicStateCfg->m_pArg->GetString().c_str())))
	{
		return eHR_Success;
	}
	else
	{
		return eHR_Fail;
	}
}

CCfgArg* CAddMagicStateTimeMOP::InitArg(const string& szArg)
{
	CCfgCalc pCfgCalc(szArg);
	return new CMagicStateCfgMultiArg(&CMagicStateCfgServer::Get(pCfgCalc.GetString(0)), pCfgCalc.GetString(1));
}

uint32 CSetDamageChangeStateAccumulateMOP::Do(CSkillInstServer* pSkillInst, const CCfgCalc& Arg, CFighterDictator* pFrom, CFighterDictator* pTo)
{
	CAllStateMgrServer* pStateMgr = pTo->GetAllStateMgr();
	Ast(pStateMgr);
	CDamageChangeStateMgrServer* pDamageChangeStateMgr = pStateMgr->GetDamageChangeStateMgrServer();
	int32 iValue = atoi(Arg.GetString(1).c_str());
	if(iValue < 0)
	{
		stringstream str;
		str << iValue;
		CfgChk::GenExpInfo("违反魔法操作约束：设置伤害变更状态累积值的第2个参数不能小于0", str);
		return eHR_Fail;
	}
	if (pDamageChangeStateMgr && pDamageChangeStateMgr->SetAccumulate(Arg.GetString(0), iValue))
	{
		return eHR_Success;
	}
	else
	{
		return eHR_Fail;
	}
}

uint32 CSetCumuliTriggerStateAccumulateMOP::Do(CSkillInstServer* pSkillInst, const CCfgCalc& Arg, CFighterDictator* pFrom, CFighterDictator* pTo)
{
	CAllStateMgrServer* pStateMgr = pTo->GetAllStateMgr();
	Ast(pStateMgr);
	CCumuliTriggerStateMgrServer* pCumuliTriggerStateMgr = pStateMgr->GetCumuliTriggerStateMgrServer();
	int32 iValue = atoi(Arg.GetString(1).c_str());
	if(iValue < 0)
	{
		stringstream str;
		str << iValue;
		CfgChk::GenExpInfo("违反魔法操作约束：设置伤害变更状态累积值的第2个参数不能小于0", str);
		return eHR_Fail;
	}
	if (pCumuliTriggerStateMgr && pCumuliTriggerStateMgr->SetAccumulate(Arg.GetString(0), iValue))
	{
		return eHR_Success;
	}
	else
	{
		return eHR_Fail;
	}
}

uint32 CRelMoveMagicMOP::Do(CSkillInstServer* pSkillInst, const CCfgArg* pArg, CFighterDictator* pFrom, CFighterDictator* pTo)
{
	if (!pFrom)
	{
		return eHR_Fail;
	}

	//位移魔法不能对战斗坐骑用
	CCharacterDictator* pChar = pTo->GetCharacter();
	if(pChar && pChar->IsBattleHorse())
	{
		CCharacterDictator* pMaster = pChar->GetMaster();
		pTo = pMaster->GetFighter();
		//cout << "m_pFighter = 主人\n";
	}
	else
	{
		//cout << "m_pFighter = 自己\n";
	}

	CMoveMagicCfgArg* pMoveMagicCfg = static_cast<CMoveMagicCfgArg*>(const_cast<CCfgArg*>(pArg));
	if (pFrom != pTo && CRelationMgrServer::IsEnemy(pFrom, pTo))
	{
		CMagicStateTouchBattleStateMOP magicStateTouchBattleState;
		magicStateTouchBattleState.Do(pSkillInst, NULL, pFrom, pTo);
	}

	CMoveMagicCfgServerSharedPtr& cfg = *pMoveMagicCfg->m_pCfg;
	new CMoveMagicServer(pSkillInst, cfg, pFrom, pTo);
	return eHR_Success;
}

CCfgArg* CRelMoveMagicMOP::InitArg(const string& szArg)
{
	CMoveMagicCfgArg* pCfgArg = new CMoveMagicCfgArg(&CMoveMagicCfgServer::GetMoveMagic(szArg));
	return pCfgArg;
}

uint32 CCancelableRelMoveMagicMOP::Do(CSkillInstServer* pSkillInst, const CCfgArg* pArg, CFighterDictator* pFrom, CFighterDictator* pTo)
{
	CRelMoveMagicMOP aMOP;
	return aMOP.Do(pSkillInst, pArg, pFrom, pTo);
}

void CCancelableRelMoveMagicMOP::Cancel(CSkillInstServer* pSkillInst, const CCfgArg* pArg, CFighterDictator* pFrom, CFighterDictator* pTo)
{	
	CCharacterDictator* pChar = pTo->GetCharacter();
	Ast(pChar);
	CBaseMoveMagicServer* pBaseMoveMagic= pChar->GetMoveMagic();
	if(pBaseMoveMagic)					//这个pFrom = NULL需要考虑是否抛异常
	{
		CSkillInstServer* pMagicSkillInst = pBaseMoveMagic->GetSkillInst();
		Ast(pMagicSkillInst);
		CMoveMagicCfgArg* pMoveMagicCfg = static_cast<CMoveMagicCfgArg*>(const_cast<CCfgArg*>(pArg));
		CCharacterDictator* pChar = CCharacterDictator::GetCharacterByID(pMagicSkillInst->GetCreatorID());
		CFighterDictator* pMagicFrom = pChar ? pChar->GetFighter() : NULL;
		if(pMagicFrom == pFrom && pBaseMoveMagic->GetName() == pMoveMagicCfg->m_pCfg->get()->GetName() && 
			pMagicSkillInst->GetSkillName() == pSkillInst->GetSkillName())	//这里最好用ID判断（考虑SkillInst引入对象ID）
		{
			pTo->StopMoving();
		}
		//注意，如果位移魔法存在可叠加的情况，即一个消失后另一个还存在，则位移管理器要提供撤掉存在但没起作用的那个位移魔法，在这里撤掉
	}
}

CCfgArg* CCancelableRelMoveMagicMOP::InitArg(const string& szArg)
{
	CMoveMagicCfgArg* pCfgArg = new CMoveMagicCfgArg(&CMoveMagicCfgServer::GetMoveMagic(szArg));
	return pCfgArg;
}

uint32 CRelTransferMagicMOP::Do(CSkillInstServer* pSkillInst, const CCfgArg* pArg, CFighterDictator* pFrom, CFighterDictator* pTo)
{
	CTransferableMagicCfgArg* CTransferableMagicCfg = static_cast<CTransferableMagicCfgArg*>(const_cast<CCfgArg*>(pArg));
	CTransferableMagicCfgServerSharedPtr& cfg = *CTransferableMagicCfg->m_pCfg;
	new CTransferableMagicServer(pSkillInst, cfg, pFrom, pTo);
	return eHR_Success;
}

CCfgArg* CRelTransferMagicMOP::InitArg(const string& szArg)
{
	CTransferableMagicCfgArg* pCfgArg = new CTransferableMagicCfgArg(&CTransferableMagicCfgServer::GetTransferableMagic(szArg));
	return pCfgArg;
}

uint32 CRelBulletMagicMOP::Do(CSkillInstServer* pSkillInst, const CCfgArg* pArg, CFighterDictator* pFrom, CFighterDictator* pTo)
{
	CBulletMagicCfgArg* pBulletMagicCfg = static_cast<CBulletMagicCfgArg*>(const_cast<CCfgArg*>(pArg));
	CBulletMagicCfgServerSharedPtr& cfg = *pBulletMagicCfg->m_pCfg;

	CPixelPath* pPath =pFrom->GetScene()->CreatePath(pFrom->GetPixelPos(), pTo->GetPixelPos(), eFPT_HypoLine, eBT_MidBarrier);

	CPos CurPos,DesPos;
	pFrom->GetGridPos(CurPos);
	pTo->GetGridPos(DesPos);
	if (pPath == NULL && pFrom != pTo && CurPos != DesPos)	//中间有障碍，当自己对自己放子弹时例外（被法术反射了）
		return eHR_Fail;
	SafeRelease(pPath);

	new CBulletMagicServer(pSkillInst, cfg, pFrom, pTo);
	return eHR_Success;
}

CCfgArg* CRelBulletMagicMOP::InitArg(const string& szArg)
{
	CBulletMagicCfgArg* pCfgArg = new CBulletMagicCfgArg(&CBulletMagicCfgServer::GetBulletMagic(szArg));
	return pCfgArg;
}

//延迟施放子弹Tick
class CDelayRelBulletMagicTick
	:public CDelayMagicOpTick
	,public CConfigMallocObject
{
public:
	CDelayRelBulletMagicTick(CSkillInstServer* pSkillInst,const CBulletMagicCfgServerSharedPtr& cfg,CFighterDictator* pFrom,uint32 uTargetID);

	uint32 m_uTargetID;
	CBulletMagicCfgServerSharedPtr* m_pCfg;
private:
	~CDelayRelBulletMagicTick();
	void OnTick();
};

CDelayRelBulletMagicTick::CDelayRelBulletMagicTick(CSkillInstServer* pSkillInst,const CBulletMagicCfgServerSharedPtr& cfg,CFighterDictator* pFrom,uint32 uTargetID)
:CDelayMagicOpTick(pSkillInst, pFrom)
,m_uTargetID(uTargetID)
,m_pCfg(new CBulletMagicCfgServerSharedPtr(cfg))
{
	pSkillInst->AddRef();
}

CDelayRelBulletMagicTick::~CDelayRelBulletMagicTick()
{
	m_pSkillInst->DelRef();
	SafeDelete(m_pCfg);
}

void CDelayRelBulletMagicTick::OnTick()
{
	DetachEvent();
	CCharacterDictator* pCharacter = CCharacterDictator::GetCharacterByID(m_uTargetID);
	CFighterDictator* pTo = pCharacter ? pCharacter->GetFighter() : NULL;
	if (m_pFrom&&pTo&&pTo->CppIsAlive())
	{
		new CBulletMagicServer(m_pSkillInst, *m_pCfg, m_pFrom, pTo);
	}
	delete this;
}

uint32 CDelayRelBulletMagicMOP::Do(CSkillInstServer* pSkillInst, const CCfgArg* pArg, CFighterDictator* pFrom, CFighterDictator* pTo)
{
	if (!pFrom)
	{
		return eHR_Fail;
	}
	CBulletMagicCfgArg* pBulletMagicCfg = static_cast<CBulletMagicCfgArg*>(const_cast<CCfgArg*>(pArg));
	CBulletMagicCfgServerSharedPtr& cfg = *pBulletMagicCfg->m_pCfg;

	CPixelPath* pPath =pFrom->GetScene()->CreatePath(pFrom->GetPixelPos(), pTo->GetPixelPos(), eFPT_HypoLine, eBT_MidBarrier);

	CPos CurPos,DesPos;
	pFrom->GetGridPos(CurPos);
	pTo->GetGridPos(DesPos);
	if (pPath == NULL && pFrom != pTo && CurPos != DesPos)	//中间有障碍，当自己对自己放子弹时例外（被法术反射了）
		return eHR_Fail;
	SafeRelease(pPath);

	pFrom->RegistDistortedTick(new CDelayRelBulletMagicTick(pSkillInst, cfg, pFrom, pTo->GetEntityID()),uint32(330));
	return eHR_Success;
}

CCfgArg* CDelayRelBulletMagicMOP::InitArg(const string& szArg)
{
	CBulletMagicCfgArg* pCfgArg = new CBulletMagicCfgArg(&CBulletMagicCfgServer::GetBulletMagic(szArg));
	return pCfgArg;
}


uint32 CEraseMoveMagicMOP::Do(CSkillInstServer* pSkillInst, const CCfgCalc& Arg, CFighterDictator* pFrom, CFighterDictator* pTo)
{
	CMagicMgrServer* pMagicMgr= pTo->GetMagicMgr();
	if(pMagicMgr)
	{
		CMoveMagicServer* pMagic = pMagicMgr->GetMoveMagic();
		if(pMagic && pMagic->GetCfgSharedPtr()->GetName() == Arg.GetString())
		{
			pTo->StopMoving();
		}
	}
	return eHR_Success;
}

uint32 CErasePositionMagicMOP::Do(CSkillInstServer* pSkillInst, const CCfgCalc& Arg, CFighterDictator* pFrom, CFighterDictator* pTo)
{
	CMagicMgrServer* pMagicMgr= pTo->GetMagicMgr();
	if(pMagicMgr)
	{
		CPositionMagicServer* pPosMagic = pMagicMgr->GetPositionMagicByName(Arg.GetString());
		if(pPosMagic)	//这里最好用ID判断（考虑SkillInst引入对象ID）
		{
			pPosMagic->Cancel();
		}
		//注意，如果地点魔法存在可叠加的情况，即一个消失后另一个还存在，则魔法管理器要提供撤掉存在但没起作用的那个位移魔法，在这里撤掉
	}

	return eHR_Success;
}

uint32 CEraseAllPositionMagicMOP::Do(CSkillInstServer* pSkillInst, const CCfgCalc& Arg, CFighterDictator* pFrom, CFighterDictator* pTo)
{
	CMagicMgrServer* pMagicMgr= pTo->GetMagicMgr();
	if(pMagicMgr)
	{
		CPositionMagicServer* pPosMagic = pMagicMgr->GetPositionMagic();
		if(pPosMagic)
		{
			pPosMagic->Cancel();
		}
		//注意，如果地点魔法存在可叠加的情况，即一个消失后另一个还存在，则魔法管理器要提供撤掉存在但没起作用的那个位移魔法，在这里撤掉
	}

	return eHR_Success;
}

uint32 CReplacePositionMagicMOP::Do(CSkillInstServer* pSkillInst, const CCfgArg* pArg, CFighterDictator* pFrom, CFighterDictator* pTo)
{
	CPositionMagicCfgArg* pPositionMagicCfg = static_cast<CPositionMagicCfgArg*>(const_cast<CCfgArg*>(pArg));
	if(pPositionMagicCfg->m_pArg->GetStringCount() != 2)
	{
		GenErr("[替换地点魔法]魔法操作参数必须为2个：", pPositionMagicCfg->m_pArg->GetTestString().c_str());
		return eHR_Fail;
	}

	CMagicMgrServer* pMagicMgr= pTo->GetMagicMgr();
	if(pMagicMgr)
	{
		CPositionMagicServer* pPosMagic = pMagicMgr->GetPositionMagic();
		if(pPosMagic && pPosMagic->GetCfgSharedPtr()->GetName() == pPositionMagicCfg->m_pArg->GetString(0))	//这里最好用ID判断（考虑SkillInst引入对象ID）
		{
			CFPos newPos = pPosMagic->m_Pos;
			pPosMagic->Cancel();

			CPositionMagicCfgServerSharedPtr& pCfg = *pPositionMagicCfg->m_pCfg;
			new CPositionMagicServer(pSkillInst, pCfg, pFrom, newPos);
		}
		//注意，如果地点魔法存在可叠加的情况，即一个消失后另一个还存在，则魔法管理器要提供撤掉存在但没起作用的那个位移魔法，在这里撤掉
	}

	return eHR_Success;
}

CCfgArg* CReplacePositionMagicMOP::InitArg(const string& szArg)
{
	CCfgCalc pCfgCalc(szArg);
	CPositionMagicCfgArg* pCfgArg = new CPositionMagicCfgArg(&CPositionMagicCfgServer::GetPositionMagic(pCfgCalc.GetString(1)), szArg);
	return pCfgArg;
}

uint32 CEraseAllDecreaseStateMOP::Do(CSkillInstServer* pSkillInst, const CCfgCalc& Arg, CFighterDictator* pFrom, CFighterDictator* pTo)
{
	CAllStateMgrServer* pStateMgr = pTo->GetAllStateMgr();
	Ast(pStateMgr);
	if (Arg.GetTestString().empty())
		pStateMgr->EraseDispellableDecreaseState();
	else
		pStateMgr->EraseDispellableDecreaseState(Arg.GetIntValue(pFrom, pTo, pSkillInst->GetLevelIndex()));

	if (pFrom && pFrom->CastToFighterMediator() && pFrom != pTo && pTo->GetCtrlState(eFCS_InBattle))
	{
		pTo->Notify(eCE_EntryEnmity,reinterpret_cast<void*>(pFrom));
	}

	return eHR_Success;
}

uint32 CRandEraseAllDecreaseStateMOP::Do(CSkillInstServer* pSkillInst, const CCfgCalc& Arg, CFighterDictator* pFrom, CFighterDictator* pTo)
{
	CAllStateMgrServer* pStateMgr = pTo->GetAllStateMgr();
	Ast(pStateMgr);

	pStateMgr->EraseDispellableDecreaseStateRand(1);

	if (pFrom && pFrom->CastToFighterMediator() && pFrom != pTo && pTo->GetCtrlState(eFCS_InBattle))
	{
		pTo->Notify(eCE_EntryEnmity,reinterpret_cast<void*>(pFrom));
	}

	return eHR_Success;
}

uint32 CEraseMoveDecreaseStateMOP::Do(CSkillInstServer* pSkillInst, const CCfgCalc& Arg, CFighterDictator* pFrom, CFighterDictator* pTo)
{
	CAllStateMgrServer* pStateMgr = pTo->GetAllStateMgr();
	Ast(pStateMgr);
	pStateMgr->EraseDispellableState(eDST_Control);
	pStateMgr->EraseDispellableState(eDST_Pause);
	pStateMgr->EraseDispellableState(eDST_Crippling);

	return eHR_Success;
}

uint32 CEraseSpeedDecreaseStateMOP::Do(CSkillInstServer* pSkillInst, const CCfgCalc& Arg, CFighterDictator* pFrom, CFighterDictator* pTo)
{
	CAllStateMgrServer* pStateMgr = pTo->GetAllStateMgr();
	Ast(pStateMgr);
	pStateMgr->EraseState(eDST_Crippling);

	return eHR_Success;
}

uint32 CEraseIncreaseStateMOP::Do(CSkillInstServer* pSkillInst, const CCfgCalc& Arg, CFighterDictator* pFrom, CFighterDictator* pTo)
{
	CAllStateMgrServer* pStateMgr = pTo->GetAllStateMgr();
	Ast(pStateMgr);
	if (Arg.GetTestString().empty())
		pStateMgr->EraseDispellableIncreaseState();
	else
		pStateMgr->EraseDispellableIncreaseState(Arg.GetIntValue(pFrom, pTo, pSkillInst->GetLevelIndex()));
	return eHR_Success;
}

uint32 CRandEraseIncreaseStateMOP::Do(CSkillInstServer* pSkillInst, const CCfgCalc& Arg, CFighterDictator* pFrom, CFighterDictator* pTo)
{
	CAllStateMgrServer* pStateMgr = pTo->GetAllStateMgr();
	Ast(pStateMgr);
	pStateMgr->EraseDispellableIncreaseStateRand(1);
	return eHR_Success;
}



uint32 CEraseRidingStateMOP::Do(CSkillInstServer* pSkillInst, const CCfgCalc& Arg, CFighterDictator* pFrom, CFighterDictator* pTo)
{
	CAllStateMgrServer* pStateMgr = pTo->GetAllStateMgr();
	Ast(pStateMgr);
	pStateMgr->EraseState(eDST_Riding);
	return eHR_Success;
}

uint32 CEraseAllBufferMOP::Do(CSkillInstServer* pSkillInst, const CCfgCalc& Arg, CFighterDictator* pFrom, CFighterDictator* pTo)
{
	CAllStateMgrServer* pStateMgr = pTo->GetAllStateMgr();
	Ast(pStateMgr);
	pStateMgr->EraseAllState();
	return eHR_Success;
}

uint32 CEraseSelfStateMOP::Do(CSkillInstServer* pSkillInst, const CCfgArg* pArg, CFighterDictator* pFrom, CFighterDictator* pTo)
{
	CAllStateMgrServer* pStateMgr = pTo->GetAllStateMgr();
	Ast(pStateMgr);
	CBaseStateCfgArg* pBaseStateCfgArg = static_cast<CBaseStateCfgArg*>(const_cast<CCfgArg*>(pArg));
	pStateMgr->EraseState(pBaseStateCfgArg->m_pCfg, pFrom);
	return eHR_Success;
}

CCfgArg* CEraseSelfStateMOP::InitArg(const string& szArg)
{
	CBaseStateCfg* pCfg = CBaseStateCfg::GetByGlobalName(szArg);
	return new CBaseStateCfgArg(pCfg);
}

uint32 CEraseStateMOP::Do(CSkillInstServer* pSkillInst, const CCfgArg* pArg, CFighterDictator* pFrom, CFighterDictator* pTo)
{
	CAllStateMgrServer* pStateMgr = pTo->GetAllStateMgr();
	Ast(pStateMgr);
	CBaseStateCfgArg* pBaseStateCfgArg = static_cast<CBaseStateCfgArg*>(const_cast<CCfgArg*>(pArg));
	pStateMgr->EraseState(pBaseStateCfgArg->m_pCfg);
	return eHR_Success;
}

CCfgArg* CEraseStateMOP::InitArg(const string& szArg)
{
	CBaseStateCfg* pCfg = CBaseStateCfg::GetByGlobalName(szArg);
	return new CBaseStateCfgArg(pCfg);
}

uint32 CEraseStateOnCancellingMOP::Do(CSkillInstServer* pSkillInst, const CCfgCalc& Arg, CFighterDictator* pFrom, CFighterDictator* pTo)
{
	return eHR_Success;
}

void CEraseStateOnCancellingMOP::Cancel(CSkillInstServer* pSkillInst, const CCfgArg* pArg, CFighterDictator* pFrom, CFighterDictator* pTo)
{
	CAllStateMgrServer* pStateMgr = pTo->GetAllStateMgr();
	Ast(pStateMgr);
	CBaseStateCfgArg* pBaseStateCfgArg = static_cast<CBaseStateCfgArg*>(const_cast<CCfgArg*>(pArg));
	pStateMgr->EraseState(pBaseStateCfgArg->m_pCfg);
}

CCfgArg* CEraseStateOnCancellingMOP::InitArg(const string& szArg)
{
	CBaseStateCfg* pCfg = CBaseStateCfg::GetByGlobalName(szArg);
	return new CBaseStateCfgArg(pCfg);
}

uint32 CEraseStateByDecreaseTypeMOP::Do(CSkillInstServer* pSkillInst, const CCfgArg* pArg, CFighterDictator* pFrom, CFighterDictator* pTo)
{
	CAllStateMgrServer* pStateMgr = pTo->GetAllStateMgr();
	Ast(pStateMgr);
	CTwoCfgCalcArg* pTowCfgCalcArg = static_cast<CTwoCfgCalcArg*>(const_cast<CCfgArg*>(pArg));
	if (pTowCfgCalcArg->m_pArg2 == NULL)
	{
		pStateMgr->EraseDispellableState(CBaseStateCfgServer::GetDecreaseType(pTowCfgCalcArg->m_pArg1->GetString()));
	}
	else
	{
		pStateMgr->EraseDispellableState(CBaseStateCfgServer::GetDecreaseType(pTowCfgCalcArg->m_pArg1->GetString()),
		                                  pTowCfgCalcArg->m_pArg2->GetIntValue(pFrom, pTo, pSkillInst->GetLevelIndex()));
	}
	return eHR_Success;
}

CCfgArg* CEraseStateByDecreaseTypeMOP::InitArg(const string& szArg)
{
	CCfgCalc pCfgCalc(szArg);
	if (pCfgCalc.GetStringCount() == 1)
	{
		return new CTwoCfgCalcArg(pCfgCalc.GetString(0),"");
	}
	else
	{
		return new CTwoCfgCalcArg(pCfgCalc.GetString(0), pCfgCalc.GetString(1));
	}
}

uint32 CReflectStateByDecreaseTypeMOP::Do(CSkillInstServer* pSkillInst, const CCfgCalc& Arg, CFighterDictator* pFrom, CFighterDictator* pTo)
{
	CAllStateMgrServer* pStateMgr = pTo->GetAllStateMgr();
	Ast(pStateMgr);
	pStateMgr->ReflectState(CBaseStateCfgServer::GetDecreaseType(Arg.GetString()));
	return eHR_Success;
}

// -------------------------------------------------------------------------------------------------------------------
// 从这里开始移动
// -------------------------------------------------------------------------------------------------------------------

// 播放全身动作
uint32 CDoAllBodyActionMOP::Do(CSkillInstServer* pSkillInst, const CCfgCalc& Arg, CFighterDictator* pFrom, CFighterDictator* pTo)
{
	const string& strActionName = Arg.GetString();
	EActionState eActionState = CActorCfg::GetEnumByString(strActionName.c_str());
	if (eAS_Error == eActionState)
		return eHR_Fail;

	if (pTo->GetCtrlState(eFCS_InNormalHorse) && (eAS_HitDown == eActionState || eAS_Stun == eActionState))
	{
		CEraseRidingStateMOP eraseRidingStateMOP;
		eraseRidingStateMOP.Do(pSkillInst, Arg, pFrom, pTo);
	}
	
	CCharacterDictator* pCharacter = pTo->GetCharacter();
	if (pCharacter)
	{
		if (eAS_SitDown != eActionState)
		{
			pCharacter->SetAndSyncMoveState(eMS_LockMove, true);
			pCharacter->SetAndSyncActionState(eActionState);
		}
		else
		{
			if (pTo->GetCtrlState(eFCS_InNormalHorse))
			{
				pCharacter->SetAndSyncActionState(eAS_TakeFood);
			}
			else
			{
				if (pCharacter->GetActor()->GetActionState() == eAS_Sitting)
				{
					pCharacter->SetAndSyncActionState(eAS_TakeFood);
				}
				else
				{
					if (pCharacter->GetActor()->GetActionState() != eAS_SitDown && pCharacter->GetActor()->GetActionState() != eAS_TakeFood)
						pCharacter->SetAndSyncActionState(eActionState);
				}
			}
		}
	}
	pCharacter->GetActor()->AddCascadeCount();
	return eHR_Success;
}

void CDoAllBodyActionMOP::Cancel(CSkillInstServer* pSkillInst, const CCfgCalc& Arg, CFighterDictator* pFrom, CFighterDictator* pTo)
{
	CCharacterDictator* pCharacter = pTo->GetCharacter();
	pCharacter->GetActor()->SubCascadeCount();
	if(pCharacter->IsMoving())
	{
		pCharacter->SetAndSyncMoveState(eMS_Move, true);
	}
	else
	{
		if (pCharacter->CppIsLive())
		{
			if ((pCharacter->GetActor()->GetActionState() == eAS_SitDown || pCharacter->GetActor()->GetActionState() == eAS_TakeFood)
				&& !pTo->GetCtrlState(eFCS_InNormalHorse))
			{
				if (pCharacter->GetActor()->GetCascadeCount() == 0)
					pCharacter->SetAndSyncActionState(eAS_Sitting);
			}
			else
			{
				pCharacter->SetAndSyncMoveState(eMS_Stop, true);
				pCharacter->SetAndSyncActionState(eAS_Idle_BackWpn);
			}
		}
		else
		{
			pCharacter->SetAndSyncMoveState(eMS_LockStop, true);
			pCharacter->SetAndSyncActionState(eAS_Die);
		}
	}

	const string& strActionName = Arg.GetString();
	EActionState eActionState = CActorCfg::GetEnumByString(strActionName.c_str());
	if (eAS_HoldStill == eActionState)
		pCharacter->SetAndSyncActionState(eAS_ResumeAni);
}

// 魔法状态触发战斗状态转换
uint32 CMagicStateTouchBattleStateMOP::Do(CSkillInstServer* pSkillInst, const CCfgArg* pArg, CFighterDictator* pFrom, CFighterDictator* pTo)
{
	if (pFrom==NULL)
	{
		return eHR_Fail;
	}
	CFighterMediator* pFighter = pFrom->CastToFighterMediator();
	CFighterMediator* pFighterTo = pTo->CastToFighterMediator();
	if (pFighter)
	{
		if (pFighterTo)
		{
			CBattleStateManager* pBattleStateMgrFrom = pFrom->GetBattleStateMgr();
			Ast(pBattleStateMgrFrom)
			CBattleStateManager* pBattleStateMgrTo = pTo->GetBattleStateMgr();
			Ast(pBattleStateMgrTo)
			pTo->SetEnmityToTarget(pFrom);
			pBattleStateMgrFrom->EnterBattleStateByPlayer();
			pBattleStateMgrTo->EnterBattleStateByPlayer();
		}
		else
		{
			// 通知NPC被攻击，NPC加玩家进入仇恨列表，同时玩家进入战斗状态
			pTo->Notify(eCE_BeAttacked, reinterpret_cast<void*>(pFrom));
			pTo->Notify(eCE_BeAttackedFromNotDot,reinterpret_cast<void*>(pFrom));
		}
	}
	if (pFighterTo)
		pFighterTo->OnBeAttacked();
	CTriggerEvent::MessageEvent(eTST_Trigger, pFrom, pTo, eSET_Attack);
	return eHR_Success;
}

// 目标脱离战斗状态
uint32 CTargetLeftBattleStateMOP::Do(CSkillInstServer* pSkillInst, const CCfgCalc& Arg, CFighterDictator* pFrom, CFighterDictator* pTo)
{
	CBattleStateManager* pBattleStateMgrTo = pTo->GetBattleStateMgr();
	Ast(pBattleStateMgrTo)
	pBattleStateMgrTo->LeaveBattleStateByForce();
	return eHR_Success;
}


// 目标禁止移动
uint32 CTargetForbitMoveMOP::Do(CSkillInstServer* pSkillInst, const CCfgCalc& Arg, CFighterDictator* pFrom, CFighterDictator* pTo)
{
	Ast(pTo->GetMagicMgr());
	pTo->StopMoving();	
	CCharacterDictator* pCharacter = pTo->GetCharacter();
	if (pCharacter&&!pTo->GetCtrlState(eFCS_InBattle))
		pTo->Notify(eCE_BeAttacked, reinterpret_cast<void*>(pFrom));
	pTo->GetEntity()->DisableDirectorMoving(true);
	pTo->SetCtrlState(eFCS_ForbitMove,true, pSkillInst);
	if (pCharacter && pCharacter->IsBattleHorse() && pCharacter->GetMaster())
	{
		pCharacter->GetMaster()->DisableDirectorMoving(true);
		pCharacter->GetMaster()->SetCtrlState(eFCS_ForbitMove,true, pSkillInst);
	}

	return eHR_Success;
}

void CTargetForbitMoveMOP::Cancel(CSkillInstServer* pSkillInst, const CCfgCalc& Arg, CFighterDictator* pFrom, CFighterDictator* pTo)
{
	pTo->GetEntity()->DisableDirectorMoving(false);
	pTo->SetCtrlState(eFCS_ForbitMove, false, pSkillInst);
	CCharacterDictator* pCharacter = pTo->GetCharacter();
	if (pCharacter && pCharacter->IsBattleHorse() && pCharacter->GetMaster())
	{
		pCharacter->GetMaster()->DisableDirectorMoving(false);
		pCharacter->GetMaster()->SetCtrlState(eFCS_ForbitMove,false, pSkillInst);
	}
	CMagicMgrServer* pMagicMgr = pTo->GetMagicMgr();
	Ast(pMagicMgr);
	pMagicMgr->SetMoveMagic(NULL);
}

uint32 CForTestTargetCancelForbitMoveMOP::Do(CSkillInstServer* pSkillInst, const CCfgCalc& Arg, CFighterDictator* pFrom, CFighterDictator* pTo)
{
	pTo->GetEntity()->DisableDirectorMoving(false);
	pTo->SetCtrlState(eFCS_ForbitMove, false, pSkillInst);
	CCharacterDictator* pCharacter = pTo->GetCharacter();
	if (pCharacter && pCharacter->IsBattleHorse() && pCharacter->GetMaster())
	{
		pCharacter->GetMaster()->DisableDirectorMoving(false);
		pCharacter->GetMaster()->SetCtrlState(eFCS_ForbitMove,false, pSkillInst);
	}
	CMagicMgrServer* pMagicMgr = pTo->GetMagicMgr();
	Ast(pMagicMgr);
	pMagicMgr->SetMoveMagic(NULL);

	return eHR_Success;
}

// 目标禁止转向
uint32 CTargetForbitTurnAroundMOP::Do(CSkillInstServer* pSkillInst, const CCfgCalc& Arg, CFighterDictator* pFrom, CFighterDictator* pTo)
{
	if (!pTo->GetCtrlState(eFCS_InBattle))
		pTo->Notify(eCE_BeAttacked, reinterpret_cast<void*>(pFrom));
	pTo->SetCtrlState(eFCS_ForbitTurnAround, true, pSkillInst);

	return eHR_Success;
}

void CTargetForbitTurnAroundMOP::Cancel(CSkillInstServer* pSkillInst, const CCfgCalc& Arg, CFighterDictator* pFrom, CFighterDictator* pTo)
{
	pTo->SetCtrlState(eFCS_ForbitTurnAround, false, pSkillInst);
}

// 目标撤销普攻
uint32 CTargetCancelNAMOP::Do(CSkillInstServer* pSkillInst, const CCfgCalc& Arg, CFighterDictator* pFrom, CFighterDictator* pTo)
{
	if (pTo->IsAttacking())
		pTo->CancelAttack();

	return eHR_Success;
}

// 目标暂停普攻
uint32 CTargetSuspendNAMOP::Do(CSkillInstServer* pSkillInst, const CCfgCalc& Arg, CFighterDictator* pFrom, CFighterDictator* pTo)
{
	if (pTo->IsAttacking())
		pTo->PauseAttack(false,false);

	return eHR_Success;
}

void CTargetSuspendNAMOP::Cancel(CSkillInstServer* pSkillInst, const CCfgCalc& Arg, CFighterDictator* pFrom, CFighterDictator* pTo)
{
	pTo->RestartAttack();
}

// 目标禁止普通攻击
uint32 CTargetForbitNormalAttackMOP::Do(CSkillInstServer* pSkillInst, const CCfgCalc& Arg, CFighterDictator* pFrom, CFighterDictator* pTo)
{
	CTargetSuspendNAMOP targetPauseNA;
	targetPauseNA.Do(pSkillInst, Arg, pFrom, pTo);
	pTo->SetCtrlState(eFCS_ForbitNormalAttack,true, pSkillInst);
	return eHR_Success;
}

void CTargetForbitNormalAttackMOP::Cancel(CSkillInstServer* pSkillInst, const CCfgCalc& Arg, CFighterDictator* pFrom, CFighterDictator* pTo)
{
	pTo->SetCtrlState(eFCS_ForbitNormalAttack, false, pSkillInst);

	CNormalAttackMgr* pAttackMgr = pTo->GetNormalAttackMgr();
	if (pAttackMgr)
	{
		//if(pAttackMgr->IsPaused() && pAttackMgr->IsAttacking())
		if(pAttackMgr->IsAttacking())
		{
			pTo->RestartAttack();
		}
		//else
		//{
		//	CFighterDictator* pTargetObject = pTo->GetTarget();
		//	//CFighterDictator* pTargetObject = m_pTo->GetFighter();
		//	if(pTargetObject && CRelationMgrServer::IsEnemy(pTargetObject, pTo))
		//	{
		//		pTo->DoNormalAttack(pTargetObject);
		//	}
		//}
	}	
}

// 目标禁止施放技能
uint32 CTargetForbitUseSkillMOP::Do(CSkillInstServer* pSkillInst, const CCfgCalc& Arg, CFighterDictator* pFrom, CFighterDictator* pTo)
{
	pTo->SetDoSkillCtrlState(eDSCS_ForbitUseSkill,true);
	pTo->Notify(eCE_BeInterrupted, pFrom);
	return eHR_Success;
}

void CTargetForbitUseSkillMOP::Cancel(CSkillInstServer* pSkillInst, const CCfgCalc& Arg, CFighterDictator* pFrom, CFighterDictator* pTo)
{
	pTo->SetDoSkillCtrlState(eDSCS_ForbitUseSkill,false);
}

//目标禁止施放自然系法术
uint32 CTargetForbitUseNatureSkillMOP::Do(CSkillInstServer* pSkillInst, const CCfgCalc& Arg, CFighterDictator* pFrom, CFighterDictator* pTo)
{
	//pTo->Notify(eCE_BeInterrupted);
	pTo->SetDoSkillCtrlState(eDSCS_ForbitNatureMagic,true);
	return eHR_Success;
}

void CTargetForbitUseNatureSkillMOP::Cancel(CSkillInstServer* pSkillInst, const CCfgCalc& Arg, CFighterDictator* pFrom, CFighterDictator* pTo)
{
	pTo->SetDoSkillCtrlState(eDSCS_ForbitNatureMagic,false);
}

// 目标禁止施放破坏系法术
uint32 CTargetForbitUseDestructionSkillMOP::Do(CSkillInstServer* pSkillInst, const CCfgCalc& Arg, CFighterDictator* pFrom, CFighterDictator* pTo)
{
	//pTo->Notify(eCE_BeInterrupted);
	pTo->SetDoSkillCtrlState(eDSCS_ForbitDestructionMagic,true);
	return eHR_Success;
}

void CTargetForbitUseDestructionSkillMOP::Cancel(CSkillInstServer* pSkillInst, const CCfgCalc& Arg, CFighterDictator* pFrom, CFighterDictator* pTo)
{
	pTo->SetDoSkillCtrlState(eDSCS_ForbitDestructionMagic,false);
}

// 目标禁止施放暗黑系法术
uint32 CTargetForbitUseEvilSkillMOP::Do(CSkillInstServer* pSkillInst, const CCfgCalc& Arg, CFighterDictator* pFrom, CFighterDictator* pTo)
{
	//pTo->Notify(eCE_BeInterrupted);
	pTo->SetDoSkillCtrlState(eDSCS_ForbitEvilMagic,true);
	return eHR_Success;
}

void CTargetForbitUseEvilSkillMOP::Cancel(CSkillInstServer* pSkillInst, const CCfgCalc& Arg, CFighterDictator* pFrom, CFighterDictator* pTo)
{
	pTo->SetDoSkillCtrlState(eDSCS_ForbitEvilMagic,false);
}
// 目标禁止客户端操作
uint32 CTargetForbitClientOperateMOP::Do(CSkillInstServer* pSkillInst, const CCfgCalc& Arg, CFighterDictator* pFrom, CFighterDictator* pTo)
{
	CEntityServer* pEntity = pTo->GetEntity();
	Ast(pEntity);
	pEntity->DisableDirectorMoving(true);
	return eHR_Success;
}

void CTargetForbitClientOperateMOP::Cancel(CSkillInstServer* pSkillInst, const CCfgCalc& Arg, CFighterDictator* pFrom, CFighterDictator* pTo)
{
	CEntityServer* pEntity = pTo->GetEntity();
	Ast(pEntity);
	pEntity->DisableDirectorMoving(false);
}

// 目标禁止施放职业技能
uint32 CTargetForbitUseClassSkillMOP::Do(CSkillInstServer* pSkillInst, const CCfgCalc& Arg, CFighterDictator* pFrom, CFighterDictator* pTo)
{
	pTo->SetDoSkillCtrlState(eDSCS_ForbitUseClassSkill, true);
	return eHR_Success;
}

void CTargetForbitUseClassSkillMOP::Cancel(CSkillInstServer* pSkillInst, const CCfgCalc& Arg, CFighterDictator* pFrom, CFighterDictator* pTo)
{
	pTo->SetDoSkillCtrlState(eDSCS_ForbitUseClassSkill,false);
}

// 目标禁止施放物品技能
uint32 CTargetForbitUseDrugItemSkillMOP::Do(CSkillInstServer* pSkillInst, const CCfgCalc& Arg, CFighterDictator* pFrom, CFighterDictator* pTo)
{
	pTo->SetDoSkillCtrlState(eDSCS_ForbitUseDrugItemSkill,true);
	return eHR_Success;
}

void CTargetForbitUseDrugItemSkillMOP::Cancel(CSkillInstServer* pSkillInst, const CCfgCalc& Arg, CFighterDictator* pFrom, CFighterDictator* pTo)
{
	pTo->SetDoSkillCtrlState(eDSCS_ForbitUseDrugItemSkill,false);
}

// 目标禁止施放非战斗技能
uint32 CTargetForbitUseNotFightSkillMOP::Do(CSkillInstServer* pSkillInst, const CCfgCalc& Arg, CFighterDictator* pFrom, CFighterDictator* pTo)
{
	pTo->SetDoSkillCtrlState(eDSCS_ForbitUseNonFightSkill,true);
	return eHR_Success;
}

void CTargetForbitUseNotFightSkillMOP::Cancel(CSkillInstServer* pSkillInst, const CCfgCalc& Arg, CFighterDictator* pFrom, CFighterDictator* pTo)
{
	pTo->SetDoSkillCtrlState(eDSCS_ForbitUseNonFightSkill,false);
}

uint32 CTargetForbitUseMissionItemSkillMOP::Do(CSkillInstServer* pSkillInst, const CCfgCalc& Arg, CFighterDictator* pFrom, CFighterDictator* pTo)
{
	pTo->SetDoSkillCtrlState(eDSCS_ForbitUseMissionItemSkill,true);
	return eHR_Success;
}

void CTargetForbitUseMissionItemSkillMOP::Cancel(CSkillInstServer* pSkillInst, const CCfgCalc& Arg, CFighterDictator* pFrom, CFighterDictator* pTo)
{
	pTo->SetDoSkillCtrlState(eDSCS_ForbitUseMissionItemSkill,false);
}

// 假死
uint32 CFeignDeathMOP::Do(CSkillInstServer* pSkillInst, const CCfgCalc& Arg, CFighterDictator* pFrom, CFighterDictator* pTo)
{
	pTo->Notify(eCE_BeInterrupted);
	pTo->Notify(eCE_Die);
	CAllStateMgrServer* pStateMgr = pTo->GetAllStateMgr();
	Ast(pStateMgr);
	pTo->Attach(pStateMgr->GetMagicStateMgrServer(), eCE_MoveBegan);
	pTo->Attach(pStateMgr->GetMagicStateMgrServer(), eCE_Attack);
	
	CEntityServer* pToEntity = pFrom->GetEntity();
	pToEntity->DisableDirectorMoving(true);
	pToEntity->DisableDirectorMoving(false);
	pTo->OnAgileValueChanged(ePID_HealthPoint + 3, 0);
	pTo->CppSetCtrlState(eFCS_FeignDeath, true);
	return eHR_Success;
}

void CFeignDeathMOP::Cancel(CSkillInstServer* pSkillInst, const CCfgCalc& Arg, CFighterDictator* pFrom, CFighterDictator* pTo)
{
	CAllStateMgrServer* pStateMgr = pTo->GetAllStateMgr();
	Ast(pStateMgr);
	pTo->Detach(pStateMgr->GetMagicStateMgrServer(), eCE_MoveBegan);
	pTo->Detach(pStateMgr->GetMagicStateMgrServer(), eCE_Attack);

	pTo->CppSetCtrlState(eFCS_FeignDeath, false);
	pTo->OnAgileValueChanged(ePID_HealthPoint + 3, (float)(pTo->m_HealthPoint.LimitGet()));
}

// 嘲讽
uint32 CTauntMOP::Do(CSkillInstServer* pSkillInst, const CCfgCalc& Arg, CFighterDictator* pFrom, CFighterDictator* pTo)
{
	CFighterMediator* pFighter = pTo->CastToFighterMediator();
	if(pFighter)
	{
		pFighter->SetTarget(NULL);
		pFighter->SetEnmityToTarget(pFrom);
		pFighter->TellDirectorToDoNA(pFrom, true);
		ICharacterMediatorCallbackHandler* pHandler = pFighter->GetCallBackHandler();
		if(pHandler)
		{
			uint32 uEntityID = pFighter->GetEntityID();
			pHandler->OnSetLockObj(uEntityID, pFrom->GetEntityID());
		}
	}
	else
	{
		pTo->Notify(eCE_BeAttacked, reinterpret_cast<void*>(pFrom));
		pFrom->Notify(eCE_Attack, reinterpret_cast<void*>(pTo));
		pTo->SetLockingTarget(pFrom);
	}

	pTo->SetTarget(pFrom);
	CCharacterDictator* pToChar = pTo->GetCharacter();
	Ast(pToChar);
	uint32 uChaosTime = Arg.GetIntValue(pFrom);
	pToChar->ForceLock(pFrom->GetCharacter(), uChaosTime*1000);
	return eHR_Success;
}

// 重生复活
uint32 CRenascenceMOP::Do(CSkillInstServer* pSkillInst, const CCfgCalc& Arg, CFighterDictator* pFrom, CFighterDictator* pTo)
{
	if (!pTo->CppIsAlive())
		pTo->CppReborn();

	return eHR_Success;
}

// 原地重生
uint32 CRebornWhereYouBeMOP::Do(CSkillInstServer* pSkillInst, const CCfgCalc& Arg, CFighterDictator* pFrom, CFighterDictator* pTo)
{
	pFrom->CppReborn();
	return eHR_Success;
}

uint32 CPermitRebornMOP::Do(CSkillInstServer* pSkillInst, const CCfgCalc& Arg, CFighterDictator* pFrom, CFighterDictator* pTo)
{
	if (pTo->CastToFighterMediator())
	{
		ICharacterMediatorCallbackHandler* pHandler = pTo->CastToFighterMediator()->GetCallBackHandler();
		pHandler->OnPermitReborn(pFrom->GetEntityID(), pTo->GetEntityID());
	}
	return eHR_Success;
}

// 持续回蓝
uint32 CContinueAddMPMOP::Do(CSkillInstServer* pSkillInst, const CCfgCalc& Arg, CFighterDictator* pFrom, CFighterDictator* pTo)
{
	float fValue = float(Arg.GetDblValue(pFrom, pTo, pSkillInst->GetLevelIndex()));
	CChangeMPUpdateRateAdderMOP Adder;
	Adder.ChangeAdder(pSkillInst,(int32)fValue * pTo->m_ManaPoint.Get(pTo),pTo);
	return eHR_Success;
}

void CContinueAddMPMOP::Cancel(CSkillInstServer* pSkillInst, const CCfgCalc& Arg, CFighterDictator* pFrom, CFighterDictator* pTo)
{
	float fValue = float(Arg.GetDblValue(pFrom, pTo, pSkillInst->GetLevelIndex()));
	pTo->m_ManaPoint.ResetRenewMP(pTo);
	CChangeMPUpdateRateAdderMOP Adder;
	Adder.ChangeAdder(pSkillInst,(int32)(-1*fValue * pTo->m_ManaPoint.Get(pTo)),pTo);
}

// 停止运动
uint32 CStopMoveMOP::Do(CSkillInstServer* pSkillInst, const CCfgCalc& Arg, CFighterDictator* pFrom, CFighterDictator* pTo)
{
	if (pTo->IsAttacking())
	{
		pTo->PauseAttack();
		if(pTo->CastToFighterMediator())
		{
			pTo->CastToFighterMediator()->CancelNormalAttackAutoTracking();
		}
	}

	return eHR_Success;
}

void CStopMoveMOP::Cancel(CSkillInstServer* pSkillInst, const CCfgCalc& Arg, CFighterDictator* pFrom, CFighterDictator* pTo)
{
	pTo->RestartAttack();
}

// 暴气
uint32 CBreakOutMOP::Do(CSkillInstServer* pSkillInst, const CCfgCalc& Arg, CFighterDictator* pFrom, CFighterDictator* pTo)
{
	CTempVarMgrServer* pTempVarMgr = pTo->GetTempVarMgr();
	Ast(pTempVarMgr);
	pTempVarMgr->m_bLockIncreaseAgile=true;

	return eHR_Success;
}

void CBreakOutMOP::Cancel(CSkillInstServer* pSkillInst, const CCfgCalc& Arg, CFighterDictator* pFrom, CFighterDictator* pTo)
{
	CTempVarMgrServer* pTempVarMgr = pTo->GetTempVarMgr();
	Ast(pTempVarMgr);
	pTempVarMgr->m_bLockIncreaseAgile=false;
}

// 灵魂链接
uint32 CSoulLinkMOP::Do(CSkillInstServer* pSkillInst, const CCfgCalc& Arg, CFighterDictator* pFrom, CFighterDictator* pTo)
{
	CTempVarMgrServer* pTempVarMgrServer = pFrom->GetTempVarMgr();
	pTempVarMgrServer->m_fDamageShareProportion = float(Arg.GetDblValue(pFrom, pTo, pSkillInst->GetLevelIndex()));
	pTempVarMgrServer->m_pLinkFighter = pTo;
	return eHR_Success;
}

void CSoulLinkMOP::Cancel(CSkillInstServer* pSkillInst, const CCfgCalc& Arg, CFighterDictator* pFrom, CFighterDictator* pTo)
{
	if (pFrom)
	{
		CTempVarMgrServer* pTempVarMgrServer = pFrom->GetTempVarMgr();
		if (pTempVarMgrServer)
		{
			pTempVarMgrServer->m_fDamageShareProportion = 0;
			pTempVarMgrServer->m_pLinkFighter = NULL;
		}
	}
}

// 施放被动技能
uint32 CRelPassiveSkillMOP::Do(CSkillInstServer* pSkillInst, const CCfgCalc& Arg, CFighterDictator* pFrom, CFighterDictator* pTo)
{
	const string& strSkillName = Arg.GetString();
	CFighterMediator* pFighter = pTo->CastToFighterMediator();
	if (pFighter == NULL)
		return eHR_Fail;
	pFighter->SetFighterPos(pTo->GetPixelPos());
	pFighter->DoPassiveSkill(strSkillName, pSkillInst->GetSkillLevel(), 0);

	return eHR_Success;
}
uint32 CRelNPCSkillMOP::Do(CSkillInstServer* pSkillInst, const CCfgCalc& Arg, CFighterDictator* pFrom, CFighterDictator* pTo)
{
	const string& strSkillName = Arg.GetString();
	if(pTo->CastToFighterMediator())
		return eHR_Fail;
	CGenericTarget genericTarget;
	genericTarget.SetFighter(pTo);
	pTo->DoNPCSkill(strSkillName, pSkillInst->GetSkillLevel(), &genericTarget, false);

	//cout << pTo->GetEntityID() << " 施放技能 " << strSkillName << endl;
	return eHR_Success;
}
uint32 CRelNPCDirSkillMOP::Do(CSkillInstServer* pSkillInst, const CCfgCalc& Arg, CFighterDictator* pFrom, CFighterDictator* pTo)
{
	const string& strSkillName = Arg.GetString(0);
	float fDist = float(atof(Arg.GetString(1).c_str()));
	if(pTo->CastToFighterMediator())
		return eHR_Fail;
	CFPos pos;
	CFPos posSelf;
	pTo->GetPixelPos(posSelf);
	CCharacterDictator* pCharacter=pTo->GetCharacter();
	CDir dir = pCharacter?pCharacter->GetActionDir():0;
	CVector2f vector2Dir;
	dir.Get(vector2Dir);
	pos.x = vector2Dir.x * fDist*eGridSpanForObj + posSelf.x;
	pos.y = vector2Dir.y * fDist*eGridSpanForObj + posSelf.y;

	CGenericTarget genericTarget;
	genericTarget.SetPos(pos);
	pTo->DoNPCSkill(strSkillName, pSkillInst->GetSkillLevel(), &genericTarget, false);

	//cout << pTo->GetEntityID() << " 施放方向技能 " << strSkillName << endl;
	return eHR_Success;
}

uint32 CBurstSoulPrizeToExpOwnerMOP::Do(CSkillInstServer* pSkillInst, const CCfgCalc& Arg, CFighterDictator* pFrom, CFighterDictator* pTo)
{
	if (pTo == NULL)
		return eHR_Fail;
	CCharacterDictator* pChar = pTo->GetCharacter();
	if (pChar == NULL)
		return eHR_Fail;
	if (pChar->CastToCharacterMediator())
	{
		GenErr("奖励魂操作, 目标不能是Mediator!");
	}
	uint32 uTargetID = pChar->GetExpOwnerID();
	CCharacterDictator* pCharacter = CCharacterDictator::GetCharacterByID(uTargetID);
	CFighterDictator* pTarget = pCharacter?pCharacter->GetFighter():NULL;
	if (pTarget == NULL)
		return eHR_Fail;
	CBurstSoulPrizeMOP BurstSoulPrizeMOP;
	return BurstSoulPrizeMOP.Do(pSkillInst,Arg,pTo,pTarget);
}

uint32 CCBurstSoulExtraPrizeToExpOwnerMOP::Do(CSkillInstServer* pSkillInst, const CCfgCalc& Arg, CFighterDictator* pFrom, CFighterDictator* pTo)
{
	if (pTo == NULL)
		return eHR_Fail;
	CCharacterDictator* pChar = pTo->GetCharacter();
	if (pChar == NULL)
		return eHR_Fail;
	uint32 uTargetID = pChar->GetExpOwnerID();
	CCharacterDictator* pCharacter = CCharacterDictator::GetCharacterByID(uTargetID);
	CFighterDictator* pTarget = pCharacter?pCharacter->GetFighter():NULL;
	if (pTarget == NULL || !pTarget->CastToFighterMediator())
		return eHR_Fail;

	return pTarget->CastToFighterMediator()->GetBurstSoulMgrServer()->DoBurstSoulExtraEff(pSkillInst,pTo,CBurstSoulCfgServer::ms_mapBurstSoulType[Arg.GetString()]);
}

uint32 CTakeOverTRuckMop::Do(CSkillInstServer* pSkillInst, const CCfgCalc& Arg, CFighterDictator* pFrom, CFighterDictator* pTo)
{
	if (!pFrom || !pTo)
		return eHR_Fail;
	CCharacterDictator* pCharFrom = pFrom->GetCharacter();
	CCharacterDictator* pCharTo = pTo->GetCharacter();
	ENpcType eNpcType = pCharTo->GetNpcType();
	if (eNpcType != ENpcType_Truck || !pCharTo->TruckCanBeTakeOver(pCharFrom))
		return eHR_Fail;
	pCharFrom->TakeOverTruck(pCharTo);
	return eHR_Success;
}

uint32 CMoveSelfToTargetPosMop::Do(CSkillInstServer* pSkillInst, const CCfgCalc& Arg, CFighterDictator* pFrom, CFighterDictator* pTo)
{
	CFPos PosFrom ,PosTo;
	PosFrom=pFrom->GetPixelPos();
	PosTo=pTo->GetPixelPos();
	CEntityServer* pFromEntity = pFrom->GetEntity();
	pFromEntity->DisableDirectorMoving(true);
	pFrom->StopMoving();
	pFrom->SetPixelPos(PosTo);
	pFromEntity->DisableDirectorMoving(false);
	return eHR_Success;
}

uint32 CTargetSetupMagicStateToSelfMOP::Do(CSkillInstServer *pSkillInst, const CCfgArg* pArg, CFighterDictator *pFrom, CFighterDictator *pTo)
{
	if (pTo == NULL)
		return eHR_Fail;
	CSetupMagicStateMOP setupMagicStateMOP;
	return setupMagicStateMOP.Do(pSkillInst, pArg, pTo, pTo);
}

void CTargetSetupMagicStateToSelfMOP::Cancel(CSkillInstServer* pSkillInst, const CCfgArg* pArg, CFighterDictator* pFrom, CFighterDictator* pTo)
{
	CAllStateMgrServer* pStateMgr = pTo->GetAllStateMgr();
	CMagicStateMgrServer* pMagicStateMgr = pStateMgr?pStateMgr->GetMagicStateMgrServer():NULL;
	if (pMagicStateMgr)
	{
		CMagicStateCfgArg* pMagicStateCfg = static_cast<CMagicStateCfgArg*>(const_cast<CCfgArg*>(pArg));
		pMagicStateMgr->ClearState(*pMagicStateCfg->m_pCfg);
	}
}

uint32 CTargetSetHiddenMOP::Do(CSkillInstServer *pSkillInst, const CCfgCalc& Arg, CFighterDictator *pFrom, CFighterDictator *pTo)
{
	CEntityServer* pToEntity = pTo->GetEntity();
	if (pToEntity && pToEntity->GetGameEntityType() == eGET_CharacterDictator)
	{
		pTo->GetCharacter()->SetNoDissolve();
		pToEntity->SetSize(0);
		return eHR_Success;
	}
	else if (pToEntity && pToEntity->GetGameEntityType() == eGET_CharacterMediator)
	{
		pToEntity->SetSize(0);
		return eHR_Success;
	}
	return eHR_Fail;
}

void CTargetSetHiddenMOP::Cancel(CSkillInstServer* pSkillInst, const CCfgCalc& Arg, CFighterDictator* pFrom, CFighterDictator* pTo)
{
	CEntityServer* pToEntity = pTo->GetEntity();
	if (pToEntity && pToEntity->GetGameEntityType() == eGET_CharacterDictator)
	{
		pToEntity->SetSize(pTo->GetCharacter()->GetNpcSize());
	}
	else if (pToEntity && pToEntity->GetGameEntityType() == eGET_CharacterMediator)
	{
		pToEntity->SetSize(0.5);
	}
}

// 缴械
uint32 CForbitUseWeaponMOP::Do(CSkillInstServer* pSkillInst, const CCfgCalc& Arg, CFighterDictator* pFrom, CFighterDictator* pTo)
{
	CFighterMediator* pFighterMediator = pTo->CastToFighterMediator();
	if (pFighterMediator)
	{
		bool bFirstForbitUseWeapon = pTo->GetCtrlState(eFCS_ForbitUseWeapon);
		pTo->CppSetCtrlState(eFCS_ForbitUseWeapon, true);
		if (!bFirstForbitUseWeapon)
		{
			CTriggerEvent::MessageEvent(eTST_Trigger, pTo, NULL, eSET_RemoveWeapon);
			CTriggerEvent::MessageEvent(eTST_Trigger, pTo, NULL, eSET_WeaponModChange);
			ICharacterMediatorCallbackHandler* pHandler = pFighterMediator->GetCallBackHandler();
			if (pHandler)
				pHandler->OnForbitUseWeapon(pTo->GetEntityID(), true);
		}
		
		return eHR_Success;
	}
	return eHR_Fail;
}

void CForbitUseWeaponMOP::Cancel(CSkillInstServer* pSkillInst, const CCfgCalc& Arg, CFighterDictator* pFrom, CFighterDictator* pTo)
{
	CFighterMediator* pFighterMediator = pTo->CastToFighterMediator();
	if (pFighterMediator)
	{
		pTo->CppSetCtrlState(eFCS_ForbitUseWeapon, false);
		if (!pTo->GetCtrlState(eFCS_ForbitUseWeapon))
		{
			CTriggerEvent::MessageEvent(eTST_Trigger, pTo, NULL, eSET_WeaponModChange);
			ICharacterMediatorCallbackHandler* pHandler = pFighterMediator->GetCallBackHandler();
			if (pHandler)
				pHandler->OnForbitUseWeapon(pTo->GetEntityID(), false);
			//EWeaponType eWeaponType = pTo->GetNormalAttackMgr()->GetMHNormalAttack()->GetWeaponType();
			//if(IsSingleHandsWeapon(eWeaponType))
			//CTriggerEvent::MessageEvent(eTST_Trigger, pTo->GetEntityID(), NULL, eSET_SetupWeapon);
		}	
	}
}

// 大剑士怒气	=((伤害*15/(2*(123.604 + 等级*9.176)))*0.6 + 5*主手武器攻击速度*0.4) * "天赋修正值"
// 兽人战士怒气	=((伤害*15/(2*(123.604 + 等级*9.176)))*2.0 + 5*主手武器攻击速度*0.4) * "天赋修正值"
uint32 CNormalAttackProduceRagePoint::Do(CSkillInstServer* pSkillInst, const CCfgCalc& Arg, CFighterDictator* pFrom, CFighterDictator* pTo)
{
	uint32 uDamage = Arg.GetIntValue(pFrom, pTo, pSkillInst->GetLevelIndex());
	float fDamageModulus = 0.0f;
	float fAttackSpeedModulus = 0.0f;
	CTempVarMgrServer* pTempVarMgr = pTo->GetTempVarMgr();
	Ast(pTempVarMgr);
	if (pTo->CppGetClass() == eCL_Warrior)
	{
		fDamageModulus = 0.6f;
		fAttackSpeedModulus = 0.4f;
	}
	else if (pTo->CppGetClass() == eCL_OrcWarrior)
	{
		fDamageModulus = 2.0f;
		fAttackSpeedModulus = 0.4f;
	}
	else
	{
		// 有NPC在用，暂时保留，当NPC普攻补全后要有错误抛出
		// GenErr("此职业普攻不会产生怒气,请检查技能表是否填错!");
		return eHR_Success;
	}

	uint32 uTempRagePoint = uint32((uDamage * 15 / (2 * (123.604 + pTo->CppGetLevel() * 9.176)) * fDamageModulus
		+ 5 * pTo->m_MHWeaponIntervalExtraDamageRate.Get(pTo) * fAttackSpeedModulus) * pTo->m_RPProduceRate.Get(pTo) + 0.5);

	if (uTempRagePoint > 50)
		uTempRagePoint = 50;

	uint32 uCurRP = pTo->m_RagePoint.LimitGet() + uTempRagePoint;
	if (pTempVarMgr->m_bLockIncreaseAgile==false)
	{
		pTo->m_RagePoint.LimitSet(uCurRP, pTo);
	}

	return eHR_Success;
}

// 怒气	= (被伤害*15/(2*(123.604 + 等级*9.176))+5*2.5)*0.08
uint32 CBeDamagePreduceRagePoint::Do(CSkillInstServer* pSkillInst, const CCfgCalc& Arg, CFighterDictator* pFrom, CFighterDictator* pTo)
{
	uint32 uBeDamage = Arg.GetIntValue(pFrom, pTo, pSkillInst->GetLevelIndex());
	CTempVarMgrServer* pTempVarMgr = pFrom->GetTempVarMgr();
	Ast(pTempVarMgr);
	uint32 uTempRagePoint = uint32((uBeDamage * 15 / (2 * (123.604 + pTo->CppGetLevel() * 9.176)) + 5 * 2.5) * 0.08 + 0.5);

	if (uTempRagePoint > 50)
		uTempRagePoint = 50;

	uint32 uCurRP = pFrom->m_RagePoint.LimitGet() + uTempRagePoint;
	if (!pTempVarMgr->m_bLockIncreaseAgile)
		pFrom->m_RagePoint.LimitSet(uCurRP, pFrom);

	return eHR_Success;
}
// -------------------------------------------------------------------------------------------------------------------
// 移动完毕
// -------------------------------------------------------------------------------------------------------------------

uint32 CResistControlMOP::Do(CSkillInstServer* pSkillInst, const CCfgCalc& Arg, CFighterDictator* pFrom, CFighterDictator* pTo)
{
	CChangeControlDecreaseResistRateMultiplierMOP Multiplier;
	Multiplier.ChangeMultiplier(pSkillInst,1,pTo);
	return eHR_Success;
}

void CResistControlMOP::Cancel(CSkillInstServer* pSkillInst, const CCfgCalc& Arg, CFighterDictator* pFrom, CFighterDictator* pTo)
{
	CChangeControlDecreaseResistRateMultiplierMOP Multiplier;
	Multiplier.ChangeMultiplier(pSkillInst,-1,pTo);
}

uint32 CResistPauseMOP::Do(CSkillInstServer* pSkillInst, const CCfgCalc& Arg, CFighterDictator* pFrom, CFighterDictator* pTo)
{
	CChangePauseDecreaseResistRateMultiplierMOP Multiplier;
	Multiplier.ChangeMultiplier(pSkillInst,1,pTo);
	return eHR_Success;
}

void CResistPauseMOP::Cancel(CSkillInstServer* pSkillInst, const CCfgCalc& Arg, CFighterDictator* pFrom, CFighterDictator* pTo)
{
	CChangePauseDecreaseResistRateMultiplierMOP Multiplier;
	Multiplier.ChangeMultiplier(pSkillInst,-1,pTo);
}

uint32 CResistCripplingMOP::Do(CSkillInstServer* pSkillInst, const CCfgCalc& Arg, CFighterDictator* pFrom, CFighterDictator* pTo)
{
	CChangeCripplingDecreaseResistRateMultiplierMOP Multiplier;
	Multiplier.ChangeMultiplier(pSkillInst,1,pTo);
	return eHR_Success;
}

void CResistCripplingMOP::Cancel(CSkillInstServer* pSkillInst, const CCfgCalc& Arg, CFighterDictator* pFrom, CFighterDictator* pTo)
{
	CChangeCripplingDecreaseResistRateMultiplierMOP Multiplier;
	Multiplier.ChangeMultiplier(pSkillInst,-1,pTo);
}

uint32 CResistDOTMOP::Do(CSkillInstServer* pSkillInst, const CCfgCalc& Arg, CFighterDictator* pFrom, CFighterDictator* pTo)
{
	CChangeDOTDecreaseResistRateMultiplierMOP Multiplier;
	Multiplier.ChangeMultiplier(pSkillInst,1,pTo);
	return eHR_Success;
}

void CResistDOTMOP::Cancel(CSkillInstServer* pSkillInst, const CCfgCalc& Arg, CFighterDictator* pFrom, CFighterDictator* pTo)
{
	CChangeDOTDecreaseResistRateMultiplierMOP Multiplier;
	Multiplier.ChangeMultiplier(pSkillInst,-1,pTo);
}

uint32 CResetSingleCoolDownTimeMOP::Do(CSkillInstServer* pSkillInst, const CCfgCalc& Arg, CFighterDictator* pFrom, CFighterDictator* pTo)
{
	const string& skillName = Arg.GetString();
	const CNormalSkillServerSharedPtr& cfg = CNormalSkillServer::GetSkillByName(skillName.c_str());
	CCoolDownMgrServer* pCDMgr = pTo->GetCoolDownMgr();
	Ast(pCDMgr);
	CSkillMgrServer* pSkillMgr = pTo->GetSkillMgr();
	Ast(pSkillMgr);
	pCDMgr->IntNewCoolDown(skillName.c_str(), cfg->GetCoolDownType(), 0);
	pSkillMgr->OnCoolDownEnd(skillName.c_str());
	return eHR_Success;
}
uint32 CAllSkillReadyMOP::Do(CSkillInstServer* pSkillInst, const CCfgCalc& Arg, CFighterDictator* pFrom, CFighterDictator* pTo)
{
	CCoolDownMgrServer* pCDMgr = pTo->GetCoolDownMgr();
	Ast(pCDMgr);
	CSkillMgrServer* pSkillMgr = pTo->GetSkillMgr();
	Ast(pSkillMgr);
	pCDMgr->ResetAllCoolDown();
	pSkillMgr->OnAllCoolDownEnd();
	return eHR_Success;
}

uint32 CAllFightSkillReadyMOP::Do(CSkillInstServer* pSkillInst, const CCfgCalc& Arg, CFighterDictator* pFrom, CFighterDictator* pTo)
{
	CCoolDownMgrServer* pCDMgr = pTo->GetCoolDownMgr();
	Ast(pCDMgr);
	CSkillMgrServer* pSkillMgr = pTo->GetSkillMgr();
	Ast(pSkillMgr);
	pCDMgr->ResetCoolDownByCoolDownType(eSCDT_FightSkill);
	pCDMgr->ResetCoolDownByCoolDownType(eSCDT_UnrestrainedFightSkill);
	pCDMgr->ResetCoolDownByCoolDownType(eSCDT_NoComCDFightSkill);
	pSkillMgr->OnAllCoolDownEnd();
	return eHR_Success;
}

uint32 CSetNoSingTimeAboutNextSkill::Do(CSkillInstServer *pSkillInst, const CCfgCalc& Arg, CFighterDictator *pFrom, CFighterDictator *pTo)
{
	CTempVarMgrServer* pTempVarMgr = pTo->GetTempVarMgr();
	Ast(pTempVarMgr);
	const string& strSkillName = Arg.GetString();
	uint32 uSkillID;
	if ( strSkillName == "全部" )
	{
		pTempVarMgr->SetNoSingTime();
		uSkillID = 0;
	}
	else
	{
		pTempVarMgr->SetNoSingTimeSkill(strSkillName);
		const CNormalSkillServerSharedPtr& pSkill = CNormalSkillServer::GetSkillByName(strSkillName);
		if(!pSkill)
		{
			if(strSkillName == "战斗技能")
			{
				uSkillID = 0;
			}
			else
			{
				stringstream str;
				str << strSkillName << endl;
				GenErr("执行 下一次吟唱时间为空 魔法操作 没有这个技能：", str.str());
				return eHR_Fail;
			}
		}
		else
		{
			uSkillID = pSkill->GetId();
		}
	}

	CGas2GacCPP_OnSetNoSingTime Cmd;
	Cmd.uObjID = pTo->GetEntityID();
	Cmd.bNoSingTime = true;
	Cmd.uSkillID = uSkillID;
	pTo->SendCmdToGac(&Cmd,eOnlyToDirector);

	return eHR_Success;
}

void CSetNoSingTimeAboutNextSkill::Cancel(CSkillInstServer* pSkillInst, const CCfgCalc& Arg, CFighterDictator* pFrom, CFighterDictator* pTo)
{
	const string& strSkillName = Arg.GetString();
	CTempVarMgrServer* pTempVarMgr = pTo->GetTempVarMgr();
	Ast(pTempVarMgr);
	uint32 uSkillID;
	if ( strSkillName == "全部" )
	{
		pTempVarMgr->CancelNoSingTime();
		uSkillID = 0;
	}
	else
	{
		pTempVarMgr->CancelNoSingTimeSkill(strSkillName);
		const CNormalSkillServerSharedPtr& pSkill = CNormalSkillServer::GetSkillByName(strSkillName);
		if(!pSkill)
		{
			if(strSkillName == "战斗技能")
			{
				uSkillID = 0;
			}
			else
			{
				stringstream str;
				str << strSkillName << endl;
				GenErr("撤销 下一次吟唱时间为空 魔法操作 没有这个技能：", str.str());
				return;
			}
		}
		else
		{
			uSkillID = pSkill->GetId();
		}
}

	CGas2GacCPP_OnSetNoSingTime Cmd;
	Cmd.uObjID = pTo->GetEntityID();
	Cmd.bNoSingTime = false;
	Cmd.uSkillID = uSkillID;
	pTo->SendCmdToGac(&Cmd, eOnlyToDirector);

}

uint32 CSetNoSingTimeForever::Do(CSkillInstServer *pSkillInst, const CCfgCalc& Arg, CFighterDictator *pFrom, CFighterDictator *pTo)
{
	CTempVarMgrServer* pTempVarMgr = pTo->GetTempVarMgr();
	Ast(pTempVarMgr);
	pTempVarMgr->SetEternalNoSingTime();

	CGas2GacCPP_OnSetNoSingTime Cmd;
	Cmd.uObjID = pTo->GetEntityID();
	Cmd.bNoSingTime = true;
	Cmd.uSkillID = 0;
	pTo->SendCmdToGac(&Cmd,eOnlyToDirector);

	return eHR_Success;
}

void CSetNoSingTimeForever::Cancel(CSkillInstServer* pSkillInst, const CCfgCalc& Arg, CFighterDictator* pFrom, CFighterDictator* pTo)
{
	CTempVarMgrServer* pTempVarMgr = pTo->GetTempVarMgr();
	Ast(pTempVarMgr);
	pTempVarMgr->CancelNoSingTime();

	CGas2GacCPP_OnSetNoSingTime Cmd;
	Cmd.uObjID = pTo->GetEntityID();
	Cmd.bNoSingTime = false;
	Cmd.uSkillID = 0;
	pTo->SendCmdToGac(&Cmd, eOnlyToDirector);

}

uint32 CLockAgileValueMOP::Do(CSkillInstServer* pSkillInst, const CCfgCalc& Arg, CFighterDictator* pFrom, CFighterDictator* pTo)
{
	CTempVarMgrServer* pTempVarMgr = pTo->GetTempVarMgr();
	Ast(pTempVarMgr);
	if (Arg.GetStringCount()==1)
	{
		if (Arg.GetString(0)=="生命值")
		{
			pTempVarMgr->m_bLockDecreaseHP=true;
			pTempVarMgr->m_bLockIncreaseHP=true;
		}
		else
		{
			pTempVarMgr->m_bLockDecreaseAgile=true;
			pTempVarMgr->m_bLockIncreaseAgile=true;
		}
	}
	else
	{
		if (Arg.GetString(0)=="生命值")
		{
			if (Arg.GetString(1)=="增加")
			{
				pTempVarMgr->m_bLockIncreaseHP=true;
			}
			else
			{
				pTempVarMgr->m_bLockDecreaseHP=true;
			}
		}
		else
		{
			if (Arg.GetString(1)=="增加")
			{
				pTempVarMgr->m_bLockIncreaseAgile=true;
			}
			else
			{
				pTempVarMgr->m_bLockDecreaseAgile=true;

			}
		}
	}
	return eHR_Success;
}

void CLockAgileValueMOP::Cancel(CSkillInstServer* pSkillInst, const CCfgCalc& Arg, CFighterDictator* pFrom, CFighterDictator* pTo)
{
	CTempVarMgrServer* pTempVarMgr = pTo->GetTempVarMgr();
	Ast(pTempVarMgr);
	if (Arg.GetStringCount()==1)
	{
		if (Arg.GetString(0)=="生命值")
		{
			pTempVarMgr->m_bLockDecreaseHP=false;
			pTempVarMgr->m_bLockIncreaseHP=false;
		}
		else
		{
			pTempVarMgr->m_bLockDecreaseAgile=false;
			pTempVarMgr->m_bLockIncreaseAgile=false;
		}
	}
	else
	{
		if (Arg.GetString(0)=="生命值")
		{
			if (Arg.GetString(0)=="增加")
			{
				pTempVarMgr->m_bLockIncreaseHP=false;
			}
			else
			{
				pTempVarMgr->m_bLockDecreaseHP=false;
			}
		}
		else
		{
			if (Arg.GetString(1)=="增加")
			{
				pTempVarMgr->m_bLockIncreaseAgile=false;
			}
			else
			{
				pTempVarMgr->m_bLockDecreaseAgile=false;

			}
		}
	}
}

uint32 CChangeTargetToBarrierMOP::Do(CSkillInstServer* pSkillInst, const CCfgCalc& Arg, CFighterDictator* pFrom, CFighterDictator* pTo)
{
	pTo->GetEntity()->SetDynamicBarrier(eBT_MidBarrier);
	return eHR_Success;
}

void CChangeTargetToBarrierMOP::Cancel(CSkillInstServer* pSkillInst, const CCfgCalc& Arg, CFighterDictator* pFrom, CFighterDictator* pTo)
{
	pTo->GetEntity()->SetDynamicBarrier(eBT_NoBarrier);
}

uint32  CNonCombatBehaviorMOP::Do(CSkillInstServer* pSkillInst, const CCfgCalc& Arg, CFighterDictator* pFrom, CFighterDictator* pTo)
{
	if (pTo && pTo->CastToFighterMediator())
	{
		ICharacterMediatorCallbackHandler* pHandler = pTo->CastToFighterMediator()->GetCallBackHandler();
		if (pHandler)
		{
			if(Arg.GetIntValue() == nCB_ItemSkill)
			{
				pHandler->OnItemSkillEnd(pTo->GetEntityID(), pSkillInst->GetSkillName().c_str());
				return eHR_Success;
			}
		}
	}
	return eHR_Fail;
}

uint32  CChangeSceneMOP::Do(CSkillInstServer* pSkillInst, const CCfgCalc& Arg, CFighterDictator* pFrom, CFighterDictator* pTo)
{
	if (pTo && pTo->CastToFighterMediator())
	{
		ICharacterMediatorCallbackHandler* pHandler = pTo->CastToFighterMediator()->GetCallBackHandler();
		if (pHandler)
		{
			const string& szSceneName = Arg.GetString(0);
			int32 uPosX,uPosY;
			CfgChk::SetValue(uPosX, Arg.GetString(1));
			CfgChk::SetValue(uPosY, Arg.GetString(2));
			if (uPosX < 0 || uPosY < 0)
				return eHR_Fail;
			pHandler->OnChangeScene(pTo->GetEntityID(), szSceneName.c_str(), (uint32)uPosX, (uint32)uPosY);
		}
	}
	return eHR_Success;
}

uint32  CSetServantLeftTimeMOP::Do(CSkillInstServer* pSkillInst, const CCfgCalc& Arg, CFighterDictator* pFrom, CFighterDictator* pTo)
{
	CCharacterDictator* pCharTo = pTo->GetCharacter();
	if (pCharTo==NULL||!pFrom)
	{
		return eHR_Fail;
	}
	uint32 uLeftTimeInSecond = Arg.GetIntValue(pFrom, pTo, pSkillInst->GetLevelIndex());
	pCharTo->ReSetNpcExistTick(uLeftTimeInSecond);

	return eHR_Success;
}

uint32 CCreateServantMOP::Do(CSkillInstServer *pSkillInst, const CCfgCalc& Arg, CFighterDictator *pFrom, CFighterDictator* pTo)
{	
	CFPos posT;
	CFPos posSelf;
	pFrom->GetPixelPos(posSelf);
	CCharacterDictator* pCharacter=pFrom->GetCharacter();
	CDir dir = pCharacter?pCharacter->GetActionDir():0;
	CVector2f vector2Dir;
	dir.Get(vector2Dir);
	posT.x = vector2Dir.x * 3 * eGridSpanForObj + posSelf.x;
	posT.y = vector2Dir.y * 3 * eGridSpanForObj + posSelf.y;
	//障碍检测

	CPixelPath* pPath =pFrom->GetScene()->CreatePath(pFrom->GetPixelPos(), posT, eFPT_HypoLine, eBT_LowBarrier);
	if (pPath == NULL && posSelf != posT)
		posT = posSelf;
	SafeRelease(pPath);

	const string& strServantName = Arg.GetString();
	if(pFrom->GetCallBackHandler()) 
		pFrom->GetCallBackHandler()->OnCreateServant(pFrom->GetEntityID(),strServantName.c_str(),pFrom->CppGetLevel(),posT.x,posT.y,true);
	return eHR_Success;
}

uint32 CCreateServantWithTargetLevelFollowMasterMOP::Do(CSkillInstServer *pSkillInst, const CCfgCalc& Arg, CFighterDictator *pFrom, CFighterDictator* pTo)
{	
	CFPos posT;
	CFPos posSelf;
	pFrom->GetPixelPos(posSelf);
	CCharacterDictator* pCharacter=pFrom->GetCharacter();
	CDir dir = pCharacter?pCharacter->GetActionDir():0;
	CVector2f vector2Dir;
	dir.Get(vector2Dir);
	posT.x = vector2Dir.x * 3 * eGridSpanForObj + posSelf.x;
	posT.y = vector2Dir.y * 3 * eGridSpanForObj + posSelf.y;
	//障碍检测

	CPixelPath* pPath =pFrom->GetScene()->CreatePath(pFrom->GetPixelPos(), posT, eFPT_HypoLine, eBT_LowBarrier);
	if (pPath == NULL && posSelf != posT)
		posT = posSelf;
	SafeRelease(pPath);

	const string& strServantName = Arg.GetString();
	CFighterDictator* pTarget = pFrom->GetTarget();
	if(pFrom->GetCallBackHandler() && pTarget) 
		pFrom->GetCallBackHandler()->OnCreateServantWithTarget(pFrom->GetEntityID(),strServantName.c_str(),pFrom->CppGetLevel(),pTarget->GetEntityID(),posT.x,posT.y,true);
	return eHR_Success;
}

uint32 CSetMonsterCardSkillLevelByMasterSkillLevelMOP::Do(CSkillInstServer *pSkillInst, const CCfgCalc& Arg, CFighterDictator *pFrom, CFighterDictator* pTo)
{	
	CCharacterDictator* pCharTo = pTo->GetCharacter();
	if (!pCharTo)
	{
		return eHR_Fail;
	}
	CCharacterDictator* pPet= pCharTo->GetServantByType(ENpcType_MonsterCard);
	if(pPet)
	{
		pPet->SetNpcSkillLevel(pSkillInst->GetSkillLevel());
		return eHR_Success;
	}
	else
	{
		return eHR_Fail;
	}
}

uint32  CControlServantMOP::Do(CSkillInstServer* pSkillInst, const CCfgCalc& Arg, CFighterDictator* pFrom, CFighterDictator* pTo)
{
	CCharacterDictator* pCharTo = pTo->GetCharacter();

	if (pCharTo==NULL||!pFrom)
	{
		return eHR_Fail;
	}
	CCharacterDictator* pCharFrom = pFrom->GetCharacter();
	Ast(pCharTo->GetNpcAIHandler());
	Ast(pCharTo->GetNpcAIHandler()->CastToNpcAI());
	if(pCharTo->GetNpcType() == ENpcType_Summon || pCharTo->GetNpcType() == ENpcType_NotCtrlSummon)
	{
		CCharacterDictator* pMasterOfCharTo = pCharTo->GetMaster();
		if (pMasterOfCharTo == NULL || pMasterOfCharTo == pCharFrom)
		{
			return eHR_Fail;
		}
		CPos posFrom;
		pCharFrom->GetGridPos(posFrom);
		pCharTo->ChangeAIType(pCharFrom, ENpcAIType_Servant, ENpcType_NotCtrlSummon, pCharFrom->CppGetCamp());
		pCharTo->SetCanDelInBattle(true);
		pCharTo->ChaseBack();
	}
	else if (pCharTo->GetNpcType() == ENpcType_Normal)
	{
		CPos posFrom;
		pCharFrom->GetGridPos(posFrom);
		pCharTo->ChangeAIType(pCharFrom, ENpcAIType_Servant, ENpcType_NotCtrlSummon, pCharFrom->CppGetCamp());
		pCharTo->SetCanDelInBattle(true);
		pCharTo->ChaseBack();
	}
	
	return eHR_Success;
}

uint32 CRemoveServantEnemyMOP::Do(CSkillInstServer *pSkillInst, const CCfgCalc& Arg, CFighterDictator *pFrom, CFighterDictator* pTo)
{	
	Ast (pFrom);
	CCharacterDictator* pMaster = pFrom->GetCharacter();
	Ast (pMaster);
	CCharacterDictator* pServant = pMaster->GetServantByType(ENpcType_Summon);
	if (pServant)
		pServant->ChaseBack();
	pServant = pMaster->GetServantByType(ENpcType_NotCtrlSummon);
	if (pServant)
		pServant->ChaseBack();
	return eHR_Success;
}

uint32  CDestroyServantMOP::Do(CSkillInstServer* pSkillInst, const CCfgCalc& Arg, CFighterDictator* pFrom, CFighterDictator* pTo)
{
	if (NULL==pTo->GetPet())
	{
		return eHR_Fail;
	}
	CFighterDictator* pPet = pTo->GetPet();
	pPet->GetCharacter()->SetOnDead(true);
	return eHR_Success;
}

uint32 CDestroyServantOnCancellingMOP::Do(CSkillInstServer* pSkillInst, const CCfgCalc& Arg, CFighterDictator* pFrom, CFighterDictator* pTo)
{
	return eHR_Success;
}

void CDestroyServantOnCancellingMOP::Cancel(CSkillInstServer* pSkillInst, const CCfgCalc& Arg, CFighterDictator* pFrom, CFighterDictator* pTo)
{
	CFighterDictator* pPet = pTo->GetPet();
	CCharacterDictator* pServant = pPet?pPet->GetCharacter():NULL;
	if (pServant && pServant->GetNpcName() == Arg.GetString())
		pServant->SetOnDead();
}

class CMOPTick
	:public CDelayMagicOpTick
	,public CConfigMallocObject
{
public:
	CMOPTick(CSkillInstServer* pSkillInst,CFighterDictator* pFrom,uint32 uTargetID,const CMagicEffServerSharedPtr& pMagicEff);

	uint32 m_uTargetID;
	CMagicEffServerSharedPtr* m_pMagicEff;
private:
	~CMOPTick();
	void OnTick();
};

CMOPTick::CMOPTick(CSkillInstServer* pSkillInst,CFighterDictator* pFrom,uint32 uTargetID,const CMagicEffServerSharedPtr& pMagicEff)
:CDelayMagicOpTick(pSkillInst, pFrom)
,m_uTargetID(uTargetID)
,m_pMagicEff(new CMagicEffServerSharedPtr(pMagicEff))
{
	m_pSkillInst = CSkillInstServer::CreateLocal(pSkillInst, NULL);
}

CMOPTick::~CMOPTick()
{
	m_pSkillInst->DestroyLocal();
	SafeDelete(m_pMagicEff);
}

void CMOPTick::OnTick()
{
	DetachEvent();
	CCharacterDictator* pCharacter = CCharacterDictator::GetCharacterByID(m_uTargetID);
	CFighterDictator* pTo = pCharacter?pCharacter->GetFighter():NULL;
	if (m_pFrom&&pTo&&pTo->CppIsAlive())
	{
		m_pMagicEff->get()->Do(m_pSkillInst,m_pFrom,pTo);
	}
	delete this;
}

uint32 CRelTickMOP::Do(CSkillInstServer* pSkillInst, const CCfgArg* pArg, CFighterDictator* pFrom, CFighterDictator* pTo)
{
	if (!pFrom)
	{
		return eHR_Fail;
	}
	CMagicEffCfgArg* pMagicEffCfg = static_cast<CMagicEffCfgArg*>(const_cast<CCfgArg*>(pArg));
	double dTime=pMagicEffCfg->m_pArg->GetDblValue(pFrom, pTo, pSkillInst->GetLevelIndex());
	pFrom->RegistDistortedTick(new CMOPTick(pSkillInst,pFrom,pTo->GetEntityID(),*pMagicEffCfg->m_pCfg),uint32(dTime*1000));
	return eHR_Success;
}

CCfgArg* CRelTickMOP::InitArg(const string& szArg)
{
	CCfgCalc pTempCalc(szArg);
	CMagicEffCfgArg* pCfgArg = new CMagicEffCfgArg(&CMagicEffServer::GetMagicEff(pTempCalc.GetString(1)), pTempCalc.GetString(0));
	return pCfgArg;
}


uint32 CSetServantTargetMOP::Do(CSkillInstServer* pSkillInst, const CCfgCalc& Arg, CFighterDictator* pFrom, CFighterDictator* pTo)
{
	if (pFrom==NULL)
	{
		return eHR_Fail;
	}
	CCharacterDictator* pCharacterMaster = pFrom->GetCharacter();
	if (pCharacterMaster==NULL)
	{
		return eHR_Fail;
	}
	CCharacterDictator* pCharacterServant = pCharacterMaster->GetServantByType(ENpcType_Summon);
	if (pCharacterServant == NULL)
	{
		pCharacterServant = pCharacterMaster->GetServantByType(ENpcType_BattleHorse);
		if (pCharacterServant == NULL)
			return eHR_Fail;
	}

	return eHR_Success;
}

uint32 CLetServantAttackTargetMOP::Do(CSkillInstServer* pSkillInst, const CCfgCalc& Arg, CFighterDictator* pFrom, CFighterDictator* pTo)
{
	if (pFrom==NULL)
	{
		return eHR_Fail;
	}
	CCharacterDictator* pCharacterMaster = pFrom->GetCharacter();
	if (pCharacterMaster==NULL)
	{
		return eHR_Fail;
	}
	CCharacterDictator* pCharacterServant = pCharacterMaster->GetServantByType(ENpcType_Summon);
	if (pCharacterServant == NULL)
	{
		pCharacterServant = pCharacterMaster->GetServantByType(ENpcType_BattleHorse);
		if (pCharacterServant == NULL)
			return eHR_Fail;
	}

	CCharacterDictator* pCharacterTarget = pTo->GetCharacter();
	if(!pCharacterTarget)
	{
		return eHR_Fail;
	}
	CFighterDictator* pFighterServant = pCharacterTarget->GetFighter();
	if (pFighterServant)
		pFighterServant->Notify(eCE_BeAttacked, reinterpret_cast<void*>(pCharacterTarget->GetFighter()));
	return eHR_Success;
}

uint32 CSetServantAttackMOP::Do(CSkillInstServer *pSkillInst, const CCfgCalc& Arg, CFighterDictator *pFrom, CFighterDictator *pTo)
{
	pTo->Notify(eCE_BeAttacked,reinterpret_cast<void*>(pFrom));
	return eHR_Success;
}

uint32 CRemoveEnemyFromServantEnmityListMOP::Do(CSkillInstServer *pSkillInst, const CCfgCalc& Arg, CFighterDictator *pFrom, CFighterDictator *pTo)
{
	if (!pFrom)
	{
		return eHR_Fail;
	}
	CCharacterDictator* pCharacterMaster = pFrom->GetCharacter();
	if (!pCharacterMaster)
	{
		return eHR_Fail;
	}
	CCharacterDictator* pCharacterTarget = pTo->GetCharacter();
	if (!pCharacterTarget)
	{
		return eHR_Fail;
	}
	pCharacterMaster->RemoveEnemyFromServantEnmityList(pCharacterTarget);
	return eHR_Success;
}

// -------------- 坐骑相关 BEGIN --------------
//使用普通坐骑
uint32 CUseNormalHorseMOP::Do(CSkillInstServer* pSkillInst, const CCfgCalc& Arg, CFighterDictator* pFrom, CFighterDictator* pTo)
{
	if (pTo->GetCtrlState(eFCS_InBattle))
		return eDSR_IsNotInBattleSkill;

	// 不是同一坐骑在吟唱上马
	CCharacterDictator* pChar = pTo->GetCharacter();
	if (pChar->GetHorseName() != Arg.GetString(0))
	{
		if (!pTo->CppSetCtrlState(eFCS_InNormalHorse, true))
			return eHR_Fail;
		pChar->SetHorseName(Arg.GetString(0));
		pChar->SetAndSyncActionState(eAS_Idle_BackWpn);
		ChangeRidingHorse(pTo, Arg);
	}

	return eHR_Success;
}

// 添加模型，改动作包，Variant中添加坐骑信息
void CUseNormalHorseMOP::ChangeRidingHorse(CFighterDictator* pFighter, const CCfgCalc& Arg)
{
	CFighterMediator* pFighterMediator = pFighter->CastToFighterMediator();
	if (pFighterMediator)
	{
		ICharacterMediatorCallbackHandler* pHandler = pFighterMediator->GetCallBackHandler();
		if (pHandler)
			pHandler->OnRidingHorseChanged(pFighter->GetEntityID(), Arg.GetString(0).c_str(), Arg.GetString(1).c_str());
	}
}

void CUseNormalHorseMOP::Cancel(CSkillInstServer* pSkillInst, const CCfgCalc& Arg, CFighterDictator* pFrom, CFighterDictator* pTo)
{
	if (pTo->GetCharacter()->GetHorseName() != "")
	{
		CTakeBackRidingHorseMOP takeBackRidingHorseMOP;
		takeBackRidingHorseMOP.Do(pSkillInst, Arg, pFrom, pTo);
	}
}

//使用战斗坐骑
uint32 CUseBattleHorseMOP::Do(CSkillInstServer* pSkillInst, const CCfgCalc& Arg, CFighterDictator* pFrom, CFighterDictator* pTo)
{
	CUseNormalHorseMOP useNormalHorseMOP;
	
	if (pFrom->GetCharacter()->GetHorseName() != Arg.GetString(0))
	{
		if (!pTo->CppSetCtrlState(eFCS_InBattleHorse, true))
			return eHR_Fail;
		pTo->GetCharacter()->SetHorseName(Arg.GetString(0));
		useNormalHorseMOP.ChangeRidingHorse(pTo, Arg);
	}
	
	return eHR_Success;
}

void CUseBattleHorseMOP::Cancel(CSkillInstServer* pSkillInst, const CCfgCalc& Arg, CFighterDictator* pFrom, CFighterDictator* pTo)
{
	if (pTo->GetCharacter()->GetHorseName() != "")
	{
		CTakeBackRidingHorseMOP takeBackRidingHorseMOP;
		takeBackRidingHorseMOP.Do(pSkillInst, Arg, pFrom, pTo);
	}
}

//召唤战斗坐骑
uint32 CCallBattleHorseMOP::Do(CSkillInstServer* pSkillInst, const CCfgCalc& Arg, CFighterDictator* pFrom, CFighterDictator* pTo)
{
	const string& strHorseName = Arg.GetString(0);
	if (pFrom->GetCharacter()->GetHorseName() != strHorseName)
	{
		/*CAureMagicCfgServerSharedPtr& cfg = CAureMagicCfgServer::GetAureMagic("骑士披甲姿态");
		CAureMagicServer::CancelTheExistedStance(cfg, pTo);*/
			
		CFPos pos;
		pTo->GetPixelPos(pos);
		ICharacterDictatorCallbackHandler* pHandler = pFrom->GetCallBackHandler();
		if (pHandler)
			pHandler->OnCreateBattleHorse(pTo->GetEntityID(), strHorseName.c_str(), Arg.GetString(1).c_str(),
															pSkillInst->GetSkillLevel(), pos.x, pos.y);
		CUseBattleHorseMOP useBattleHorseMOP;
		useBattleHorseMOP.Do(pSkillInst, Arg, pFrom, pTo);
	}
	else
	{
		pTo->GetCharacter()->SetHorseName("");
	}

	return eHR_Success;
}

void CCallBattleHorseMOP::Cancel(CSkillInstServer* pSkillInst, const CCfgCalc& Arg, CFighterDictator* pFrom, CFighterDictator* pTo)
{
	if (pTo->GetCharacter()->GetHorseName() != "")
	{
		CTakeBackRidingHorseMOP takeBackRidingHorseMOP;
		takeBackRidingHorseMOP.Do(pSkillInst, Arg, pFrom, pTo);
	}
}

//收回骑乘坐骑
uint32 CTakeBackRidingHorseMOP::Do(CSkillInstServer* pSkillInst, const CCfgCalc& Arg, CFighterDictator* pFrom, CFighterDictator* pTo)
{
	if (pTo->GetCtrlState(eFCS_InNormalHorse))
	{
		pTo->CppSetCtrlState(eFCS_InNormalHorse, false);
	}
	
	CCharacterDictator* pCharacter = pTo->GetCharacter();
	if (pTo->GetCtrlState(eFCS_InBattleHorse))
	{
		pTo->CppSetCtrlState(eFCS_InBattleHorse, false);

		if (!pCharacter)
			return eHR_Fail;
		
		CCharacterDictator* pServantHorse = pCharacter->GetServantByType(ENpcType_BattleHorse);
		if (pServantHorse)
			pCharacter->RemoveServant(pServantHorse);
	}

	pCharacter->SetHorseName("");
	CFighterMediator* pFighterMediator = pTo->CastToFighterMediator();
	if (pFighterMediator)
	{
		ICharacterMediatorCallbackHandler* pHandler = pFighterMediator->GetCallBackHandler();
		if (pHandler)
			pHandler->OnRidingHorseChanged(pTo->GetEntityID(), "", "");
	}

	return eHR_Success;
}
// -------------- 坐骑相关 END --------------

uint32 CHandActionMOP::Do(CSkillInstServer* pSkillInst, const CCfgCalc& Arg, CFighterDictator* pFrom, CFighterDictator* pTo)
{
	CCharacterDictator* pCharacter = pTo->GetCharacter();
	if (pCharacter)
		pCharacter->SetAndSyncActionState(eAS_Cast);

	return eHR_Success;
}

uint32 CFootActionMOP::Do(CSkillInstServer* pSkillInst, const CCfgCalc& Arg, CFighterDictator* pFrom, CFighterDictator* pTo)
{
	CCharacterDictator* pCharacter = pTo->GetCharacter();
	if (pCharacter)
		pCharacter->SetAndSyncActionState(eAS_Cast);

	return eHR_Success;
}

uint32 CBurstSoulPrizeMOP::Do(CSkillInstServer* pSkillInst, const CCfgCalc& Arg, CFighterDictator* pFrom, CFighterDictator* pTo)
{
	float uSoulMul = float(Arg.GetDblValue());

	CFighterMediator* pFighterMed = pTo->CastToFighterMediator();
	if (pTo == NULL || pFrom == NULL || !pFighterMed)
		return eHR_Fail;
	uint32 pFromID = pFrom->GetEntityID();
	uint32 pToID = pTo->GetEntityID();
	CCharacterDictator* pChar = pTo->GetCharacter();
	if (pChar && pChar->GetMaster())
		pToID = pChar->GetMaster()->GetEntityID();
	
	ICharacterMediatorCallbackHandler* pHandler = pFighterMed->GetCallBackHandler();
	if (pHandler)
		pHandler->OnBurstSoulPrize(pToID, pFromID, uSoulMul);

	return eHR_Success;
}

uint32 CTargetRelBulletMagicMOP::Do(CSkillInstServer* pSkillInst, const CCfgCalc& Arg, CFighterDictator* pFrom, CFighterDictator* pTo)
{
	if (!pFrom || !pTo)
		return eHR_Fail;
	const string& magicName = Arg.GetString();
	CBulletMagicCfgServerSharedPtr& cfg = CBulletMagicCfgServer::GetBulletMagic(magicName);

	CPixelPath* pPath =pFrom->GetScene()->CreatePath(pFrom->GetPixelPos(), pTo->GetPixelPos(), eFPT_HypoLine, eBT_MidBarrier);

	if (pPath == NULL && pFrom != pTo && pFrom->GetPixelPos() != pTo->GetPixelPos())	//中间有障碍，当自己对自己放子弹时例外（被法术反射了）
		return eHR_Fail;
	SafeRelease(pPath);

	new CBulletMagicServer(pSkillInst, cfg, pTo, pFrom);
	return eHR_Success;
}
uint32 CBurstSoulFXMOP::Do(CSkillInstServer* pSkillInst, const CCfgArg* pArg, CFighterDictator* pFrom, CFighterDictator* pTo)
{
	if (pFrom->CastToFighterMediator())
	{
		uint32 uCount = pFrom->CastToFighterMediator()->GetBurstSoulMgrServer()->GetContinuousBurstTimes();
		if (uCount <= 1)
		{
			uCount = 2;
		}
		else if (uCount >= 4)
		{
			uCount = 5;
		}
		else
		{
			uCount = uCount+1;
		}
		CBulletMagicCfgArg* pBulletMagicCfg = static_cast<CBulletMagicCfgArg*>(const_cast<CCfgArg*>(pArg));
		CBulletMagicCfgServerSharedPtr& pCfg = *pBulletMagicCfg->m_pCfg;

		CGas2GacCPP_OnBurstSoul Cmd;
		Cmd.uCategoryID = pCfg->GetId();
		Cmd.uFromEntityID = pTo->GetEntityID();
		Cmd.uTargetEntityID = pFrom->GetEntityID();
		Cmd.uBurstSoulCount = uCount;
		Cmd.uBurstSoulType = eBST_Normal;
		pFrom->SendCmdToGac(&Cmd);
		return eHR_Success;
	}
	return eHR_Fail;
}

CCfgArg* CBurstSoulFXMOP::InitArg(const string& szArg)
{
	CBulletMagicCfgArg* pCfgArg = new CBulletMagicCfgArg(&CBulletMagicCfgServer::GetBulletMagic(szArg));
	return pCfgArg;
}

uint32 CBurstSoulBallFXMOP::Do(CSkillInstServer* pSkillInst, const CCfgArg* pArg, CFighterDictator* pFrom, CFighterDictator* pTo)
{
	CBulletMagicCfgArg* pBulletMagicCfg = static_cast<CBulletMagicCfgArg*>(const_cast<CCfgArg*>(pArg));
	CBulletMagicCfgServerSharedPtr& pCfg = *pBulletMagicCfg->m_pCfg;

	CGas2GacCPP_OnBurstSoul Cmd;
	Cmd.uCategoryID = pCfg->GetId();
	Cmd.uFromEntityID = pFrom->GetEntityID();
	Cmd.uTargetEntityID = pTo->GetEntityID();
	Cmd.uBurstSoulCount = 6;
	Cmd.uBurstSoulType = eBST_Normal;
	pTo->SendCmdToGac(&Cmd);

	return eHR_Success;
}

CCfgArg* CBurstSoulBallFXMOP::InitArg(const string& szArg)
{
	CBulletMagicCfgArg* pCfgArg = new CBulletMagicCfgArg(&CBulletMagicCfgServer::GetBulletMagic(szArg));
	return pCfgArg;
}

uint32 CTargetBurstSoulFXMOP::Do(CSkillInstServer* pSkillInst, const CCfgArg* pArg, CFighterDictator* pFrom, CFighterDictator* pTo)
{
	CBurstSoulFXMOP burstSoulFXMOP;
	burstSoulFXMOP.Do(pSkillInst,pArg,pTo,pFrom);
	return eHR_Success;
}

CCfgArg* CTargetBurstSoulFXMOP::InitArg(const string& szArg)
{
	CBulletMagicCfgArg* pCfgArg = new CBulletMagicCfgArg(&CBulletMagicCfgServer::GetBulletMagic(szArg));
	return pCfgArg;
}

uint32 CAddBurstSoulTimesMOP::Do(CSkillInstServer* pSkillInst, const CCfgCalc& Arg, CFighterDictator* pFrom, CFighterDictator* pTo)
{
	if (!pTo->CastToFighterMediator())
		return eHR_Fail;
	int32 uAddTimes = Arg.GetIntValue();
	if (!pTo || uAddTimes < 0)
	{
		return eHR_Fail;
	}
	pTo->CastToFighterMediator()->GetBurstSoulMgrServer()->AddBurstSoulTimes(uAddTimes);
	pTo->OnPrintFightInfo(EFII_AddBurstSoulTimes, pFrom?pFrom->GetEntityID():0, pTo->GetEntityID(), pSkillInst, uAddTimes, eATT_None, 0, 0);
	return eHR_Success;
}

uint32 CConsumeBurstSoulTimesMOP::Do(CSkillInstServer* pSkillInst, const CCfgCalc& Arg, CFighterDictator* pFrom, CFighterDictator* pTo)
{
	if (!pTo->CastToFighterMediator())
		return eHR_Fail;
	CBurstSoulMgrServer* pBurstSoulMgrServer = pTo->CastToFighterMediator()->GetBurstSoulMgrServer();
	uint32 uConsumeTimes = pBurstSoulMgrServer->GetNeedBurstTimesByLevel(Arg.GetIntValue());
	if (!pTo || uConsumeTimes > pBurstSoulMgrServer->GetBurstSoulTimes())
	{
		return eHR_Fail;
	}
	pBurstSoulMgrServer->ConsumeBurstSoulTimes(uConsumeTimes);
	return eHR_Success;
}

uint32 CAddContinuousBurstTimesMOP::Do(CSkillInstServer* pSkillInst, const CCfgCalc& Arg, CFighterDictator* pFrom, CFighterDictator* pTo)
{
	if (!pTo || !pTo->CastToFighterMediator())
		return eHR_Fail;
	CBurstSoulMgrServer* pBurstSoulMgrServer = pTo->CastToFighterMediator()->GetBurstSoulMgrServer();
	pBurstSoulMgrServer->AddContinuousBurstTimes(int32(Arg.GetIntValue()));
	return eHR_Success;
}

void CAddContinuousBurstTimesMOP::Cancel(CSkillInstServer* pSkillInst, const CCfgCalc& Arg, CFighterDictator* pFrom, CFighterDictator* pTo)
{
	if (!pTo || !pTo->CastToFighterMediator())
		return;
	CBurstSoulMgrServer* pBurstSoulMgrServer = pTo->CastToFighterMediator()->GetBurstSoulMgrServer();
	pBurstSoulMgrServer->AddContinuousBurstTimes( 0-int32(Arg.GetIntValue()));
}

// 归阵
uint32 CReturnBattleArrayMOP::Do(CSkillInstServer* pSkillInst, const CCfgCalc& Arg, CFighterDictator* pFrom, CFighterDictator* pTo)
{
	if (pTo&&pTo->CastToFighterMediator())
	{
		pTo->CastToFighterMediator()->ReturnArray();
	}
	return eHR_Success;
}

// 改变阵营
uint32 CChangeCampMOP::Do(CSkillInstServer* pSkillInst, const CCfgCalc& Arg, CFighterDictator* pFrom, CFighterDictator* pTo)
{
	const string& strCampName = Arg.GetString();
	if (strCampName.compare("艾米帝国联盟") == 0)
	{
		pTo->CppSetCamp(eCamp_AmyEmpire);
	}
	else if (strCampName.compare("西帝君联盟") == 0)
	{
		pTo->CppSetCamp(eCamp_WestEmpire);
	}
	else if (strCampName.compare("恶魔岛势力") == 0)
	{
		pTo->CppSetCamp(eCamp_DevilIsland);
	}
	else if (strCampName.compare("路人") == 0)
	{
		pTo->CppSetCamp(eCamp_Passerby);
	}
	else if (strCampName.compare("怪物") == 0)
	{
		pTo->CppSetCamp(eCamp_Monster);
	}
	else if (strCampName.compare("混乱") == 0)
	{
		pTo->CppSetCamp(eCamp_Chaos);
	}
	else
	{
		//printf("改变阵营失败，无此阵营！");
		return eHR_Fail;
	}

	return eHR_Success;
}

void CChangeCampMOP::Cancel(CSkillInstServer* pSkillInst, const CCfgCalc& Arg, CFighterDictator* pFrom, CFighterDictator* pTo)
{
	pTo->CppSetCamp(pTo->CppGetBirthCamp());
}

//uint32 CUseMagaicEff::Do(CSkillInstServer* pSkillInst, const CCfgCalc& Arg, CFighterDictator* pFrom, CFighterDictator* pTo)
//{
//	string strMagicEffName = Arg.GetString();
//	CMagicEffServer* pMagicEff = CMagicEffServer::GetMagicEff(strMagicEffName).get();
//	if (pMagicEff)
//	{
//		pMagicEff->Do(pSkillInst,pFrom,pTo);
//	}
//	return eHR_Success;
//}
//
//void CUseMagaicEff::Cancel(CSkillInstServer* pSkillInst, const CCfgCalc& Arg, CFighterDictator* pFrom, CFighterDictator* pTo)
//{
//	string strMagicEffName = Arg.GetString();
//	CMagicEffServer* pMagicEff = CMagicEffServer::GetMagicEff(strMagicEffName).get();
//	if (pMagicEff)
//	{
//		pMagicEff->Cancel(pSkillInst,pTo);
//	}
//}

uint32 COnlyPlayFX::Do(CSkillInstServer* pSkillInst, const CCfgArg* pArg, CFighterDictator* pFrom, CFighterDictator* pTo)
{
	return eHR_Success;
}

void COnlyPlayFX::Cancel(CSkillInstServer* pSkillInst, const CCfgArg* pArg, CFighterDictator* pFrom, CFighterDictator* pTo)
{
	if (!pArg)
		 return;
	CMagicEffCfgArg* pMagicEffCfgArg = static_cast<CMagicEffCfgArg*>(const_cast<CCfgArg*>(pArg));
	if (pMagicEffCfgArg && pMagicEffCfgArg->m_pCfg)
	{
		pMagicEffCfgArg->m_pCfg->get()->Do(pSkillInst,pFrom,pTo);
	}
}
CCfgArg* COnlyPlayFX::InitArg(const string& szArg)
{
	CMagicEffCfgArg* pCfgArg = NULL;
	if (szArg != "")
	{
		pCfgArg = new CMagicEffCfgArg(&CMagicEffServer::GetMagicEff(szArg), szArg);
	}
	return pCfgArg;
}

//uint32 CCancelMagicEff::Do(CSkillInstServer* pSkillInst, const CCfgCalc& Arg, CFighterDictator* pFrom, CFighterDictator* pTo)
//{
//	return eHR_Success;
//}
//
//void CCancelMagicEff::Cancel(CSkillInstServer* pSkillInst, const CCfgCalc& Arg, CFighterDictator* pFrom, CFighterDictator* pTo)
//{
//	string strMagicEffName = Arg.GetString();
//	CMagicEffServer* pMagicEff = CMagicEffServer::GetMagicEff(strMagicEffName).get();
//	if (pMagicEff)
//	{
//		pMagicEff->Cancel(pSkillInst,pTo);
//	}
//}

// 改变普攻动作速率
uint32 CChangeNAActionRhythmMOP::Do(CSkillInstServer* pSkillInst, const CCfgCalc& Arg, CFighterDictator* pFrom, CFighterDictator* pTo)
{
	float fMulti = pTo->m_MainHandWeaponInterval.GetMultiplier();
	float fNAActionRhythm;
	if (fMulti != 0)
		fNAActionRhythm	= 1/fMulti;
	else
		fNAActionRhythm = 1.0f;

	CCharacterDictator* pCharacter = pTo->GetCharacter();
	Ast(pCharacter);
	CActorServer* pActor = pCharacter->GetActor();
	Ast(pActor);
	pActor->GetVariantNAActionRhythm()->SetNumber(fNAActionRhythm);

	return eHR_Success;
}

void CChangeNAActionRhythmMOP::Cancel(CSkillInstServer* pSkillInst, const CCfgCalc& Arg, CFighterDictator* pFrom, CFighterDictator* pTo)
{
	float fMulti = pTo->m_MainHandWeaponInterval.GetMultiplier();
	float fNAActionRhythm;
	if (fMulti != 0)
		fNAActionRhythm	= 1/fMulti;
	else
		fNAActionRhythm = 1.0f;

	CCharacterDictator* pCharacter = pTo->GetCharacter();
	Ast(pCharacter);
	CActorServer* pActor = pCharacter->GetActor();
	Ast(pActor);
	pActor->GetVariantNAActionRhythm()->SetNumber(fNAActionRhythm);
}

uint32 CSelfMurderBlastMOP::Do(CSkillInstServer* pSkillInst, const CCfgCalc& Arg, CFighterDictator* pFrom, CFighterDictator* pTo)
{
	CTriggerEvent::MessageEvent(eTST_Trigger, pFrom, pTo, eSET_DeadlyHurt);

	int32 CurHP = pTo->m_HealthPoint.LimitGet();
	pTo->m_HealthPoint.LimitSet(0 , pTo);
	if (CurHP != 0)
	{
		if (pTo->CastToFighterMediator())
			pTo->CastToFighterMediator()->CppDie(pFrom);
		else
			pTo->CppDie(pFrom,pSkillInst->GetSkillName().c_str());
	}
	uint32 uFromID = pFrom ? pFrom->GetEntityID() : 0;
	uint32 uToID = pTo ? pTo->GetEntityID() : 0;
	CTriggerEvent::MessageEvent(eTST_Trigger, uFromID, uToID, eSET_Killed);
	if (pTo->CastToFighterMediator())
	{
		CTriggerEvent::MessageEvent(eTST_Trigger, uFromID, uToID, eSET_KillPlayer);
	}
	else
	{
		CTriggerEvent::MessageEvent(eTST_Trigger, uFromID, uToID, eSET_KillNPC);
	}
	return eHR_Success;
}

uint32 CDisTriggerEventMOP::Do(CSkillInstServer* pSkillInst, const CCfgCalc& Arg, CFighterDictator* pFrom, CFighterDictator* pTo)
{
	CTempVarMgrServer* pTempVarMgr = pTo->GetTempVarMgr();
	Ast(pTempVarMgr);
	++pTempVarMgr->m_iDisTriggerEvent;

	return eHR_Success;
}

uint32 CReplaceSkillMOP::Do(CSkillInstServer* pSkillInst, const CCfgCalc& Arg, CFighterDictator* pFrom, CFighterDictator* pTo)
{
	CFighterMediator* pFighterMed = pTo->CastToFighterMediator();
	if(pSkillInst->GetType() == eIT_MagicStateInst && pFighterMed)
	{
		return pFighterMed->GetSkillReplaceServer()->ReplaceSkill(pSkillInst->GetMagicStateInst()->GetCfg()->GetName());
	}
	return eHR_Fail;
}

void CReplaceSkillMOP::Cancel(CSkillInstServer* pSkillInst, const CCfgCalc& Arg, CFighterDictator* pFrom, CFighterDictator* pTo)
{
	CFighterMediator* pFighterMed = pTo->CastToFighterMediator();
	if(pSkillInst->GetType() == eIT_MagicStateInst && pFighterMed)
	{
		return pFighterMed->GetSkillReplaceServer()->CancelReplaceSkill(pSkillInst->GetMagicStateInst()->GetCfg()->GetName());
	}
}

void CDisTriggerEventMOP::Cancel(CSkillInstServer* pSkillInst, const CCfgCalc& Arg, CFighterDictator* pFrom, CFighterDictator* pTo)
{
	CTempVarMgrServer* pTempVarMgr = pTo->GetTempVarMgr();
	Ast(pTempVarMgr);
	--pTempVarMgr->m_iDisTriggerEvent;
}

uint32 CSetSameTargetMOP::Do(CSkillInstServer* pSkillInst, const CCfgCalc& Arg, CFighterDictator* pFrom, CFighterDictator* pTo)
{
	if(pFrom && pFrom->GetTarget())
	{
		pTo->SetTarget(NULL);
		pTo->SetEnmityToTarget(pFrom->GetTarget());
	}
	return eHR_Success;
}

uint32 CSetTargetSelfMOP::Do(CSkillInstServer* pSkillInst, const CCfgCalc& Arg, CFighterDictator* pFrom, CFighterDictator* pTo)
{
	if(pFrom)
	{
		if (pTo->CastToFighterMediator())
		{
			pTo->SetTarget(NULL);
			pTo->SetEnmityToTarget(pFrom);
		}
		else
		{
			pTo->Notify(eCE_BeAttacked, reinterpret_cast<void*>(pFrom));
			pFrom->Notify(eCE_Attack, reinterpret_cast<void*>(pTo));
			pTo->SetLockingTarget(pFrom);
		}
		pTo->SetTarget(pFrom);
	}
	return eHR_Success;
}


uint32 CSetTargetNULLMOP::Do(CSkillInstServer* pSkillInst, const CCfgCalc& Arg, CFighterDictator* pFrom, CFighterDictator* pTo)
{
	pTo->SetTarget(NULL);
	return eHR_Success;
}

template<typename PropertyType>
uint32 TImmuneMOP<PropertyType>::
Do(CSkillInstServer* pSkillInst, const CCfgCalc& Arg, CFighterDictator* pFrom, CFighterDictator* pTo)
{
	PropertyType Multiplier;
	Multiplier.ChangeMultiplier(pSkillInst,1,pTo);
	return eHR_Success;
}


template<typename PropertyType>
void TImmuneMOP<PropertyType>::
Cancel(CSkillInstServer* pSkillInst,const CCfgCalc& Arg,CFighterDictator* pFrom,CFighterDictator* pTo)
{
	PropertyType Multiplier;
	Multiplier.ChangeMultiplier(pSkillInst,-1,pTo);
}


#define InstantiateImmuneMOP(Name) \
	template class TImmuneMOP<CChange##Name##ImmuneRateMultiplierMOP>

InstantiateImmuneMOP(PunctureDamage);
InstantiateImmuneMOP(ChopDamage);
InstantiateImmuneMOP(BluntDamage);
InstantiateImmuneMOP(NatureDamage);
InstantiateImmuneMOP(DestructionDamage);
InstantiateImmuneMOP(EvilDamage);
InstantiateImmuneMOP(ControlDecrease);
InstantiateImmuneMOP(PauseDecrease);
InstantiateImmuneMOP(CripplingDecrease);
InstantiateImmuneMOP(DebuffDecrease);
InstantiateImmuneMOP(DOTDecrease);
InstantiateImmuneMOP(SpecialDecrease);
InstantiateImmuneMOP(Cure);
InstantiateImmuneMOP(MoveMagic);
InstantiateImmuneMOP(NonTypePhysicsDamage);
InstantiateImmuneMOP(NonTypeDamage);
InstantiateImmuneMOP(NonTypeCure);
InstantiateImmuneMOP(InterruptCastingProcess);

#undef InstantiateImmuneMOP



uint32 CImmuneTaskTypeDamageMOP::Do(CSkillInstServer* pSkillInst, const CCfgCalc& Arg, CFighterDictator* pFrom, CFighterDictator* pTo)
{
	pTo->ChangeTaskAttackImmuneValue(Arg.GetString(), 1);
	return eHR_Success;
}

void CImmuneTaskTypeDamageMOP::Cancel(CSkillInstServer* pSkillInst, const CCfgCalc& Arg, CFighterDictator* pFrom, CFighterDictator* pTo)
{
	pTo->ChangeTaskAttackImmuneValue(Arg.GetString(), -1);
}

uint32 CClosePrintInfoMOP::Do(CSkillInstServer* pSkillInst, const CCfgCalc& Arg, CFighterDictator* pFrom, CFighterDictator* pTo)
{
	++pTo->GetTempVarMgr()->m_iPrintInfoOff;
	return eHR_Success;
}

void CClosePrintInfoMOP::Cancel(CSkillInstServer* pSkillInst, const CCfgCalc& Arg, CFighterDictator* pFrom, CFighterDictator* pTo)
{
	if(--pTo->GetTempVarMgr()->m_iPrintInfoOff < 0)
	{
		stringstream sErr;
		sErr << "CClosePrintInfoMOP:Cancel() 关闭免疫显示变量 m_iPrintInfoOff = " << pTo->GetTempVarMgr()->m_iPrintInfoOff << "\n";
		GenErr("CClosePrintInfoMOP::Cancel", sErr.str());
	};
}

uint32 CSwapPositionMOP::Do(CSkillInstServer* pSkillInst, const CCfgCalc& Arg, CFighterDictator* pFrom, CFighterDictator* pTo)
{
	CFPos PosFrom ,PosTo;
	PosFrom=pFrom->GetPixelPos();
	PosTo=pTo->GetPixelPos();
	CCoreSceneServer* pScene = pFrom->GetScene();
	CPixelPath* pPath = pScene->CreatePath(PosFrom, PosTo, eFPT_Line, eBT_LowBarrier);
	if(pPath)
	{
		CEntityServer* pFromEntity = pFrom->GetEntity();
		CEntityServer* pToEntity = pTo->GetEntity();
		pFromEntity->DisableDirectorMoving(true);
		pToEntity->DisableDirectorMoving(true);
		pFrom->StopMoving();
		pTo->StopMoving();
		pFrom->SetCtrlState(eFCS_ForbitMove, true, pSkillInst);
		pTo->SetCtrlState(eFCS_ForbitMove, true, pSkillInst);
		pFrom->SetPixelPos(PosTo);
		pTo->SetPixelPos(PosFrom);
		pFromEntity->DisableDirectorMoving(false);
		pToEntity->DisableDirectorMoving(false);
		pFrom->SetCtrlState(eFCS_ForbitMove, false, pSkillInst);
		pTo->SetCtrlState(eFCS_ForbitMove, false, pSkillInst);
		SafeRelease(pPath);
		return eHR_Success;
	}
	else
	{
		SafeRelease(pPath);
		return eHR_Fail;
	}
}

uint32 CInterruptCastingProcessMagicMOP::Do(CSkillInstServer* pSkillInst, const CCfgCalc& Arg, CFighterDictator* pFrom, CFighterDictator* pTo)
{
	if(pTo->m_InterruptCastingProcessImmuneRate.Get() >= 1.0f)
	{
		CCharacterDictator* pCreatorChar = CCharacterDictator::GetCharacterByID(pSkillInst->GetCreatorID());
		CFighterDictator* pCreatorFighter = pCreatorChar?pCreatorChar->GetFighter():NULL;
		pTo->GetSkillMgr()->DisplayFightInfo(pCreatorFighter,pFrom,0,eAT_HP,eHR_Immune);
		CTriggerEvent::MessageEvent(eTST_Trigger, pFrom, pTo, eHR_Immune, true);
		return eHR_Immune;
	}
	CTriggerEvent::MessageEvent(eTST_Trigger, pFrom, pTo, eSET_InterruptMopHit, false);
	pTo->Notify(eCE_BeInterrupted, pFrom);
	if (pFrom != pTo && CRelationMgrServer::IsEnemy(pFrom, pTo))
	{
		CMagicStateTouchBattleStateMOP magicStateTouchBattleState;
		magicStateTouchBattleState.Do(pSkillInst, NULL, pFrom, pTo);
	}
	return eHR_Success;
}

uint32 CFillHPMPMOP::Do(CSkillInstServer* pSkillInst, const CCfgCalc& Arg, CFighterDictator* pFrom, CFighterDictator* pTo)
{
	pTo->m_HealthPoint.FullFill(pTo);
	pTo->m_ManaPoint.FullFill(pTo);
	return eHR_Success;
}

uint32 CTalentChangeManaPointMOP::Do(CSkillInstServer* pSkillInst, const CCfgArg* pArg, CFighterDictator* pFrom, CFighterDictator* pTo)
{
	CTwoCfgCalcArg* pTowCfgCalcArg = static_cast<CTwoCfgCalcArg*>(const_cast<CCfgArg*>(pArg));
	int32 iValue = pTowCfgCalcArg->m_pArg1->GetIntValue(pFrom, pTo, pSkillInst->GetLevelIndex());
	int32 iOriginalValue = iValue;
	CTempVarMgrServer* pTemeVarMgrFrom= pFrom?pFrom->GetTempVarMgr():NULL;
	CTempVarMgrServer* pTemeVarMgrTo= pTo->GetTempVarMgr();
	Ast(pTemeVarMgrTo);

	int32 temp = pTo->m_ManaPoint.LimitGet() + iValue;
	if (iValue > 0)
	{
		iValue = temp > (int32)pTo->m_ManaPoint.Get(pTo) ? (int32)pTo->m_ManaPoint.Get(pTo) - (int32)pTo->m_ManaPoint.LimitGet() : iValue;
	}
	else
	{
		iValue = temp < 0 ? -1 * pTo->m_ManaPoint.LimitGet() : iValue;
		if(pTemeVarMgrFrom) pTemeVarMgrFrom->SetSubMP(-iValue);
		pTemeVarMgrTo->SetBeSubMP(-iValue);
		CTriggerEvent::MessageEvent(eTST_All, pFrom, pTo, eSET_SubMP);
		iValue = -(int32)(pTemeVarMgrTo->GetBeSubMP());
	}
	CCharacterDictator* pCreatorChar = CCharacterDictator::GetCharacterByID(pSkillInst->GetCreatorID());
	CFighterDictator* pCreatorFighter = pCreatorChar?pCreatorChar->GetFighter():NULL;
	pTo->GetSkillMgr()->DisplayFightInfo(pCreatorFighter,pFrom,iOriginalValue,eAT_MP,eHR_Hit);

	if (iOriginalValue > 0)
	{
		const string& sArg = pTowCfgCalcArg->m_pArg2->GetString();
		pTo->OnPrintFightInfo(EFII_RenewMP, pFrom?pFrom->GetEntityID():0, pTo->GetEntityID(), NULL, iOriginalValue, eATT_None, 0, 0, false, 0, sArg.c_str(), (uint16)sArg.size());
	}
	if (iOriginalValue < 0)
	{
		const string& sArg = pTowCfgCalcArg->m_pArg2->GetString();
		pTo->OnPrintFightInfo(EFII_LostMP, pFrom?pFrom->GetEntityID():0, pTo->GetEntityID(), NULL, iOriginalValue, eATT_None, 0, 0, false, 0, sArg.c_str(), (uint16)sArg.size());
	}

	pTo->m_ManaPoint.LimitSet( pTo->m_ManaPoint.LimitGet()+iValue,pTo );
	return eHR_Success;
}

CCfgArg* CTalentChangeManaPointMOP::InitArg(const string& szArg)
{
	CCfgCalc pCfgCalc(szArg);
	return new CTwoCfgCalcArg(pCfgCalc.GetString(0), pCfgCalc.GetString(1));	
}

uint32 CRelMoveMagicAtShockwaveMagicPosMOP::Do(CSkillInstServer* pSkillInst, const CCfgArg* pArg,CFighterDictator* pFrom, CFighterDictator* pTo)
{
	if (!pFrom)
	{
		return eHR_Fail;
	}

	if (!pTo)
	{
		return eHR_Fail;
	}

	if(pTo->m_ImmuneRate.Get() >= 1.0f || pTo->m_MoveMagicImmuneRate.Get() >= 1.0)
	{
		CCharacterDictator* pCreatorChar = CCharacterDictator::GetCharacterByID(pSkillInst->GetCreatorID());
		CFighterDictator* pCreatorFighter = pCreatorChar?pCreatorChar->GetFighter():NULL;
		pTo->GetSkillMgr()->DisplayFightInfo(pCreatorFighter,pFrom,0,eAT_HP,eHR_Immune);
		CTriggerEvent::MessageEvent(eTST_Trigger, pFrom, pTo, eHR_Immune, true);
		return eHR_Fail;
	}

	CMoveMagicCfgMultiArg* pMoveMagicCfg = static_cast<CMoveMagicCfgMultiArg*>(const_cast<CCfgArg*>(pArg));
	CMagicMgrServer* pMagicMgr = pFrom->GetMagicMgr();
	CShockWaveMagicHandlerServer* pMagic = pMagicMgr->GetShockwaveMagic(pMoveMagicCfg->m_pArg->GetString(0));
	
	if(!pMagic)
	{
		return eHR_Fail;
	}

	CFPos pos = pMagic->GetDestPos();

	CMoveMagicCfgServerSharedPtr& pCfg = *pMoveMagicCfg->m_pCfg;

	CCharacterDictator* pCharacter = pTo->GetCharacter();
	pCharacter->SetAndSyncActionDirByPos(pos);
	new CMoveMagicServer(pSkillInst,pCfg,pTo,pos);
	return eHR_Success;
}

CCfgArg* CRelMoveMagicAtShockwaveMagicPosMOP::InitArg(const string& szArg)
{
	CCfgCalc pCfgCalc(szArg);
	CMoveMagicCfgServerSharedPtr& pCfg = CMoveMagicCfgServer::GetMoveMagic(pCfgCalc.GetString(1));
	return new CMoveMagicCfgMultiArg(&pCfg, szArg);
}

uint32 CRelPositionMagicAtTargetPosMOP::Do(CSkillInstServer* pSkillInst, const CCfgArg* pArg, CFighterDictator* pFrom, CFighterDictator* pTo)
{	
	CPositionMagicCfgArg* pPositionMagicCfg = static_cast<CPositionMagicCfgArg*>(const_cast<CCfgArg*>(pArg));
	CPositionMagicCfgServerSharedPtr& pCfg = *pPositionMagicCfg->m_pCfg;
	new CPositionMagicServer(pSkillInst, pCfg, pFrom, pTo->GetPixelPos());
	return eHR_Success;
}

CCfgArg* CRelPositionMagicAtTargetPosMOP::InitArg(const string& szArg)
{
	CPositionMagicCfgArg* pCfgArg = new CPositionMagicCfgArg(&CPositionMagicCfgServer::GetPositionMagic(szArg), szArg);
	return pCfgArg;
}

uint32 CRelShockWaveMagicAtTargetPosMOP::Do(CSkillInstServer* pSkillInst, const CCfgArg* pArg, CFighterDictator* pFrom, CFighterDictator* pTo)
{	
	CShockWaveMagicCfgArg* pShockWaveMagicCfg = static_cast<CShockWaveMagicCfgArg*>(const_cast<CCfgArg*>(pArg));
	CShockWaveMagicCfgServerSharedPtr& pCfg = *pShockWaveMagicCfg->m_pCfg;
	new CShockWaveMagicServer(pSkillInst,pCfg,pFrom,pTo->GetPixelPos());
	return eHR_Success;
}

CCfgArg* CRelShockWaveMagicAtTargetPosMOP::InitArg(const string& szArg)
{
	CShockWaveMagicCfgArg* pCfgArg = new CShockWaveMagicCfgArg(&CShockWaveMagicCfgServer::GetShockWaveMagic(szArg));
	return pCfgArg;
}

uint32 CNonCombatStateMagicOp::Do(CSkillInstServer* pSkillInst,const CCfgArg* pArg,CFighterDictator* pFrom,CFighterDictator* pTo)
{
	CTwoCfgCalcArg* pTowCfgCalcArg = static_cast<CTwoCfgCalcArg*>(const_cast<CCfgArg*>(pArg));
	double dValue = 0.0;
	if(pTowCfgCalcArg->m_pArg2)
	{
		dValue = pTowCfgCalcArg->m_pArg2->GetDblValue(pFrom, pTo, pSkillInst->GetLevelIndex());
	} 
	Ast(pTo);
	CFighterMediator* pFighter=pTo->CastToFighterMediator();
	if(pFighter)
	{
		ICharacterMediatorCallbackHandler* pHandler = pFighter->GetCallBackHandler();
		if(pHandler)
			pHandler->OnDoNonCombatStateMagicOp(
			pTowCfgCalcArg->m_pArg1->GetString().c_str(), dValue, 
			(pFrom && pFrom->GetHandler()) ? pFrom->GetEntityID() : 0,
			pTo->GetEntityID());
	}
	return eHR_Success;
}

void CNonCombatStateMagicOp::Cancel(CSkillInstServer* pSkillInst,const CCfgArg* pArg,CFighterDictator* pFrom,CFighterDictator* pTo)
{
	CTwoCfgCalcArg* pTowCfgCalcArg = static_cast<CTwoCfgCalcArg*>(const_cast<CCfgArg*>(pArg));
	double dValue = 0.0;
	if(pTowCfgCalcArg->m_pArg2)
	{
		dValue = pTowCfgCalcArg->m_pArg2->GetDblValue(pFrom, pTo, pSkillInst->GetLevelIndex());
	}
	Ast(pTo);
	CFighterMediator* pFighter=pTo->CastToFighterMediator();
	if(pFighter)
	{
		ICharacterMediatorCallbackHandler* pHandler = pFighter->GetCallBackHandler();
		if(pHandler)
			pHandler->OnCancelNonCombatStateMagicOp(
			pTowCfgCalcArg->m_pArg1->GetString().c_str(), dValue, 
			(pFrom && pFrom->GetHandler()) ? pFrom->GetEntityID() : 0,
			pTo->GetEntityID());
	}
}

CCfgArg* CNonCombatStateMagicOp::InitArg(const string& szArg)
{
	CCfgCalc pCfgCalc(szArg);
	if(pCfgCalc.GetStringCount() == 2)
	{
		return new CTwoCfgCalcArg(pCfgCalc.GetString(0), pCfgCalc.GetString(1));	
	}
	else
	{
		return new CTwoCfgCalcArg(pCfgCalc.GetString(0), "");	
	}
}

uint32 CBecomeTargetShadowMOP::Do(CSkillInstServer *pSkillInst, const CCfgCalc& Arg, CFighterDictator *pFrom, CFighterDictator *pTo)
{
	if (pFrom && pTo && pTo->CastToFighterMediator())
	{
		ICharacterDictatorCallbackHandler* pHandler = pFrom->GetCallBackHandler();
		if (pHandler)
		{
			pHandler->BecomeTargetShadow(pFrom->GetCharacter(),pTo->GetCharacter());
		}
		return eHR_Success;
	}
	return eHR_Fail;
}

uint32 CForceSetDOTDamageMOP::Do(CSkillInstServer *pSkillInst, const CCfgCalc& Arg, CFighterDictator *pFrom, CFighterDictator *pTo)
{
	pSkillInst->SetInterval(true);
	return eHR_Success;
}

uint32 CBindTargetObjToSelfMOP::Do(CSkillInstServer *pSkillInst, const CCfgCalc& Arg, CFighterDictator *pFrom, CFighterDictator *pTo)
{
	if(!pFrom || !pTo)
		return eHR_Fail;

	CCharacterDictator* pCharFrom = pFrom->GetCharacter();
	CCharacterDictator* pCharTo = pTo->GetCharacter();
	pCharTo->BindTo(pCharFrom);
	return eHR_Success;
}


uint32 CTestTargetMoveMagicMOP::Do(CSkillInstServer* pSkillInst, const CCfgCalc& Arg, CFighterDictator* pFrom, CFighterDictator* pTo)
{
	if (!pTo)
		return eDSR_Fail;
	if (!pTo->IsExistMoveMgr()&&(pTo==pFrom&&!pTo->GetCtrlState(eFCS_ForbitMove)||pTo!=pFrom))
	{
		if(pTo->m_ImmuneRate.Get() >= 1.0f || pTo->m_MoveMagicImmuneRate.Get() >= 1.0)
		{
			CCharacterDictator* pCreatorChar = CCharacterDictator::GetCharacterByID(pSkillInst->GetCreatorID());
			CFighterDictator* pCreatorFighter = pCreatorChar?pCreatorChar->GetFighter():NULL;
			pTo->GetSkillMgr()->DisplayFightInfo(pCreatorFighter,pFrom,0,eAT_HP,eHR_Immune);
			CTriggerEvent::MessageEvent(eTST_Trigger, pFrom, pTo, eHR_Immune, true);
			return eDSR_Fail;
		}
		return eDSR_Success;
	}
	return eDSR_Fail;
}

uint32 CDisplayMsgMOP::Do(CSkillInstServer* pSkillInst, const CCfgArg* pArg, CFighterDictator* pFrom, CFighterDictator* pTo)
{
	CDisplayMsgCfgArg* pTowStringArg = static_cast<CDisplayMsgCfgArg*>(const_cast<CCfgArg*>(pArg));
	CGas2GacCPP_OnDisplayMsg Cmd;
	Cmd.uObjID	= pTo->GetEntityID();
	Cmd.uMsgID = pTowStringArg->m_uArg1;
	Cmd.uMsgFXLen = pTowStringArg->m_strArg2.size();
	pTo->SendCmdToGac(&Cmd);
	pTo->SendToGac(pTowStringArg->m_strArg2.c_str(), pTowStringArg->m_strArg2.size());	
	return eDSR_Success;
}

CCfgArg* CDisplayMsgMOP::InitArg(const string& szArg)
{
	CCfgCalc pCfgCalc(szArg);
	return new CDisplayMsgCfgArg(atoi(pCfgCalc.GetString(0).c_str()), pCfgCalc.GetString(1));
}

uint32 CDebaseEnmityMOP::Do(CSkillInstServer *pSkillInst, const CCfgCalc& Arg, CFighterDictator *pFrom, CFighterDictator* pTo)
{	
	//直接通知所有存在pTo的仇恨列表降仇恨
	pTo->Notify(eCE_DebaseEnmity);
	return eHR_Success;
}


uint32 CDoWorldSkillMOP::Do(CSkillInstServer *pSkillInst, const CCfgArg* pArg, CFighterDictator *pFrom, CFighterDictator* pTo)
{	
	CCharacterMediator* pPlayer = pTo->GetCharacter()->CastToCharacterMediator();
	CPlayerSkillCfgArg* pCfgArg = static_cast<CPlayerSkillCfgArg*>(const_cast<CCfgArg*>(pArg));
	CNormalSkillServerSharedPtr& pCfg = *pCfgArg->m_pCfg;
	if (pPlayer)
	{
		ICharacterDictatorCallbackHandler* pHandler = pPlayer->GetCallbackHandler();
		if (pHandler)
		{
			pHandler->CastToMediatorHandler()->OnDoWorldSkill(pPlayer,pCfg->GetName().c_str(), pCfgArg->m_uSkillLevel);
		}
	}
	return eHR_Success;
}

CCfgArg* CDoWorldSkillMOP::InitArg(const string& szArg)
{
	CCfgCalc pCfgCalc(szArg);
	return new CPlayerSkillCfgArg(&CNormalSkillServer::GetSkillByName(pCfgCalc.GetString(0)),atoi(pCfgCalc.GetString(1).c_str()));
}

uint32 CDoCampSkillMOP::Do(CSkillInstServer *pSkillInst, const CCfgArg* pArg, CFighterDictator *pFrom, CFighterDictator* pTo)
{	
	CCharacterMediator* pPlayer = pTo->GetCharacter()->CastToCharacterMediator();
	CPlayerSkillCfgArg* pCfgArg = static_cast<CPlayerSkillCfgArg*>(const_cast<CCfgArg*>(pArg));
	CNormalSkillServerSharedPtr& pCfg = *pCfgArg->m_pCfg;
	if (pPlayer)
	{
		ICharacterDictatorCallbackHandler* pHandler = pPlayer->GetCallbackHandler();
		if (pHandler)
		{
			pHandler->CastToMediatorHandler()->OnDoCampSkill(pPlayer,pCfg->GetName().c_str(), pCfgArg->m_uSkillLevel);
		}
	}
	return eHR_Success;
}

CCfgArg* CDoCampSkillMOP::InitArg(const string& szArg)
{
	CCfgCalc pCfgCalc(szArg);
	return new CPlayerSkillCfgArg(&CNormalSkillServer::GetSkillByName(pCfgCalc.GetString(0)),atoi(pCfgCalc.GetString(1).c_str()));
}

uint32 CDoArmyCorpsSkillMOP::Do(CSkillInstServer *pSkillInst, const CCfgArg* pArg, CFighterDictator *pFrom, CFighterDictator* pTo)
{	
	CCharacterMediator* pPlayer = pTo->GetCharacter()->CastToCharacterMediator();
	CPlayerSkillCfgArg* pCfgArg = static_cast<CPlayerSkillCfgArg*>(const_cast<CCfgArg*>(pArg));
	CNormalSkillServerSharedPtr& pCfg = *pCfgArg->m_pCfg;
	if (pPlayer)
	{
		ICharacterDictatorCallbackHandler* pHandler = pPlayer->GetCallbackHandler();
		if (pHandler)
		{
			pHandler->CastToMediatorHandler()->OnDoArmyCorpsSkill(pPlayer,pCfg->GetName().c_str(), pCfgArg->m_uSkillLevel);
		}
	}
	return eHR_Success;
}

CCfgArg* CDoArmyCorpsSkillMOP::InitArg(const string& szArg)
{
	CCfgCalc pCfgCalc(szArg);
	return new CPlayerSkillCfgArg(&CNormalSkillServer::GetSkillByName(pCfgCalc.GetString(0)),atoi(pCfgCalc.GetString(1).c_str()));
}

uint32 CDoTongSkillMOP::Do(CSkillInstServer *pSkillInst, const CCfgArg* pArg, CFighterDictator *pFrom, CFighterDictator* pTo)
{	
	CCharacterMediator* pPlayer = pTo->GetCharacter()->CastToCharacterMediator();
	CPlayerSkillCfgArg* pCfgArg = static_cast<CPlayerSkillCfgArg*>(const_cast<CCfgArg*>(pArg));
	CNormalSkillServerSharedPtr& pCfg = *pCfgArg->m_pCfg;
	if (pPlayer)
	{
		ICharacterDictatorCallbackHandler* pHandler = pPlayer->GetCallbackHandler();
		if (pHandler)
		{
			pHandler->CastToMediatorHandler()->OnDoTongSkill(pPlayer,pCfg->GetName().c_str(), pCfgArg->m_uSkillLevel);
		}
	}
	return eHR_Success;
}

CCfgArg* CDoTongSkillMOP::InitArg(const string& szArg)
{
	CCfgCalc pCfgCalc(szArg);
	return new CPlayerSkillCfgArg(&CNormalSkillServer::GetSkillByName(pCfgCalc.GetString(0)),atoi(pCfgCalc.GetString(1).c_str()));
}


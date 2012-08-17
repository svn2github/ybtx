#include "stdafx.h"
#include "CSkillMgrServer.h"
#include "CCastingProcess.h"
#include "CSkillServer.h"
#include "CFighterMediator.h"
#include "CGenericTarget.h"
#include "CSkillServer.h"
#include "CSkillInstServer.h"
#include "PtrlGas2GacCPPDef.h"
#include "NormalAttack.h"
#include "ErrLogHelper.h"
#include "CCharacterDictator.h"
#include "CSyncVariantServer.h"
#include "DebugHelper.h"
#include "TSqrAllocator.inl"
#include "BaseHelper.h"
#include "IFighterServerHandler.h"
#include "CIntObjServer.h"
#include "CAniKeyFrameMgrServer.h"


uint32 CSkillMgrServer::ms_uPlayerDoSkillSuccNum = 0;
uint32 CSkillMgrServer::ms_uPlayerDoSkillFailNum = 0;
uint32 CSkillMgrServer::ms_uNpcDoSkillNum = 0;

CSkillMgrServer::CSkillMgrServer(CFighterDictator* pFighter)
:m_pFighter(pFighter),m_pCastingProcess(NULL)
{
	m_pAniKeyMgr=new CAniKeyFrameMgrServer(pFighter);
}

CSkillMgrServer::~CSkillMgrServer(void)
{
	delete m_pAniKeyMgr;
}



void CSkillMgrServer::DoNPCSkill(const string& szName, uint8 uSkillLevel,CGenericTarget* pTarget, bool bDoSkillRule)
{
	const CNormalSkillServerSharedPtr& pSkill = CNormalSkillServer::GetNPCSkill(szName);
	DoNPCSkill(pSkill, uSkillLevel, pTarget, bDoSkillRule);
}

void CSkillMgrServer::DoNPCSkill(const CNormalSkillServerSharedPtr& pSkill, uint8 uSkillLevel,CGenericTarget* pTarget,bool bDoSkillRule)
{
	if (!pSkill)
		return;
	//Npc释放技能并说话
	CCharacterDictator* pChar = GetFighter()->GetCharacter();
	if(pChar)
	{
		uint32 uTargetID = 0;
		const CFighterDictator* pTargetFighter = pTarget->GetFighter();
		if (pTargetFighter)
			uTargetID = pTargetFighter->GetEntityID();
		pChar->NpcShowContentBySkill(pSkill.get()->GetName().c_str(), uTargetID);
	}
	NoConditionDoSkill(pSkill, uSkillLevel, pTarget, bDoSkillRule);
}

void CSkillMgrServer::DoIntObjSkill(const string& szName, uint8 uSkillLevel,CGenericTarget* pTarget)
{
	const CNormalSkillServerSharedPtr& pSkill = CNormalSkillServer::GetIntObjSkill(szName);
	if (!pSkill)
		return;
	NoConditionDoSkill(pSkill, uSkillLevel, pTarget, false);
}

void CSkillMgrServer::NoConditionDoSkill(const CNormalSkillServerSharedPtr& pSkill, uint8 uSkillLevel,CGenericTarget* pTarget, bool bDoSkillRule)
{
	CSkillInstServer* pInst = CSkillInstServer::CreateOrigin(GetFighter()->GetEntityID(),pSkill->GetName(),pSkill,uSkillLevel,pSkill->GetAttackType());
	CNormalAttackMgr* pAttackMgr=GetFighter()->GetNormalAttackMgr();
	if (pInst->GetAttackType() == eATT_FollowWeapon)
	{
		if(pAttackMgr)
		{
			pInst->SetAttackType(pAttackMgr->GetAttackType());
		}
		else
		{
			pInst->SetAttackType(eATT_None);
		}
	}
	CFPos posFighterPos;
	CFighterMediator* pMedFighter = GetFighter()->CastToFighterMediator();
	if(pMedFighter)
	{
		posFighterPos = pMedFighter->GetFighterPos();
	}
	else
	{
		posFighterPos = GetFighter()->GetPixelPos();
	}

	//cout << "NoConditionDoSkill " << posFighterPos.x << "," << posFighterPos.y << endl;
	pInst->SetSkillPos(posFighterPos);
	if (GetFighter()->GetEntity() && GetFighter()->GetEntity()->GetGameEntityType() == eGET_CharacterDictator)
	{
		pInst->SetDir(GetFighter()->GetCharacter()->GetActionDir());
	}
	pInst->SetTargetAliveCheckIsIgnored(pSkill->GetTargetIsCorpse());

	pSkill->DoEffect(pInst,GetFighter(), pTarget, uSkillLevel,bDoSkillRule);

	pInst->DestroyOrigin();

#if defined _FightSkillCounter
	++ms_uNpcDoSkillNum;
#endif
}


void CSkillMgrServer::SetCurCasting(CCastingProcess* pCastingProcess)
{
	if (m_pCastingProcess && pCastingProcess)
	{
		string sLogErrType = " Log SetCurCasting error! ";
		ostringstream strm;		
		if (GetFighter()->CastToFighterMediator())
		{
			strm <<"Player "<< m_pCastingProcess->GetCastingProcessCfg()->GetName() << "  " <<
				pCastingProcess->GetCastingProcessCfg()->GetName();
		}
		else
		{
			strm <<"Npc: "<<GetFighter()->GetCharacter()->m_sNpcName<<" "<< m_pCastingProcess->GetCastingProcessCfg()->GetName() << "  " <<
				pCastingProcess->GetCastingProcessCfg()->GetName();
		}
		CConnServer* pConn = GetFighter()->GetConnection();
		if (pConn)
			LogErr(sLogErrType, (strm.str()).c_str(), pConn);
		else
			LogErr(sLogErrType, (strm.str()).c_str());
		m_pCastingProcess->CancelCasting();
	}
	m_pCastingProcess = pCastingProcess;
}
void CSkillMgrServer::CancelCurCasting(CCastingProcess* pCastingProcess)
{
	if (m_pCastingProcess && pCastingProcess && m_pCastingProcess != pCastingProcess)
	{
		string sLogErrType = " Log CancelCurCasting error! ";
		ostringstream strm;		
		strm << m_pCastingProcess->GetCastingProcessCfg()->GetName() << "  " <<
			pCastingProcess->GetCastingProcessCfg()->GetName();
		CConnServer* pConn = GetFighter()->GetConnection();
		if (pConn)
			LogErr(sLogErrType, (strm.str()).c_str(), pConn);
		else
			LogErr(sLogErrType, (strm.str()).c_str());
		return;
	}
	m_pCastingProcess = NULL;
}

CCastingProcess* CSkillMgrServer::GetCurCasting()const
{
	return m_pCastingProcess;
}

bool CSkillMgrServer::CurCastingProcessCanBeInterrupted()
{
	if (GetCurCasting() && GetCurCasting()->CanBeInterrupted())
	{
		return true;
	}
	return false;
}

void CSkillMgrServer::CancelCastingProcess()
{
	if (GetCurCasting())
	{
		if (GetCurCasting()->GetCastingProcessCfg()->GetCastingType() == eCPT_GradeSing)
			GetCurCasting()->DoEffect();
		else
			GetCurCasting()->OnBeCancel();
	}
}

void CSkillMgrServer::InterruptCastingProcessByType(ECastingInterruptType eCastingInterruptType)
{
	if (GetCurCasting())
	{
		if (GetCurCasting()->GetCastingProcessCfg()->GetTurnAroundInterrupt())
			GetCurCasting()->OnBeCancel();
	}
}

void CSkillMgrServer::SyncCastingProcess(CFighterDictator* pFighter)
{
	if(!m_pCastingProcess)
	{
		return;
	}
	m_pCastingProcess->SyncCasting(pFighter);
}

// RPC
void CSkillMgrServer::OnSkillBegin(uint32 uSkillID, uint8 uSkillLevel, ETargetType eTargetType, uint32 uTargetID)
{
	CGas2GacCPP_OnSkillBegin Cmd;
	Cmd.uObjID = GetFighter()->GetEntityID();
	Cmd.uTargetID = uTargetID;
	Cmd.uSkillID = uSkillID;
	Cmd.uSkillLevel = uSkillLevel;
	Cmd.uTargetType = uint8(eTargetType);
	CCharacterDictator* pCharacter = GetFighter()->GetCharacter();
	if(pCharacter)
		Cmd.uActionDir = pCharacter->GetActionDir();
	else
		Cmd.uActionDir = 0;

	GetFighter()->SetDoSkillCtrlState(eDSCS_InDoingSkill, true);
	GetFighter()->OnSkillBegin();
	GetFighter()->SendCmdToGac(&Cmd);
}

void CSkillMgrServer::OnSkillEnd(uint32 uId, bool bSucceed)
{
	GetFighter()->SetDoSkillCtrlState(eDSCS_InDoingSkill, false);
	GetFighter()->OnSkillEnd(bSucceed);
}

void CSkillMgrServer::OnMagicCondFail(uint32 MessageId, const TCHAR* sSkillName)const
{
	CGas2GacCPP_OnMagicCondFail cmd;
	cmd.uObjID = GetFighter()->GetEntityID();
	cmd.uMessageID = MessageId;
	string strSkillName(sSkillName);
	cmd.udbSkillNameLen = strSkillName.size();
	GetFighter()->SendCmdToGac(&cmd, eOnlyToDirector);
	GetFighter()->SendToGac(strSkillName.c_str(), cmd.udbSkillNameLen, eOnlyToDirector);
}

void CSkillMgrServer::OnCoolDownBegin(const TCHAR* szSkillName, uint8 uSkillLevel, uint32 uLeftTime, bool bIsSwitchEquipSkill)
{
	string strSkillName(szSkillName);
	CGas2GacCPP_OnCoolDownBegin Cmd;
	Cmd.uObjID = GetFighter()->GetEntityID();
	Cmd.uSkillLevel = uSkillLevel;
	Cmd.uLeftTime	= uLeftTime;
	Cmd.bIsSwitchEquipSkill = bIsSwitchEquipSkill;
	Cmd.udbSkillNameLen = strSkillName.size();
	GetFighter()->SendCmdToGac(&Cmd, eOnlyToDirector);
	GetFighter()->SendToGac(strSkillName.c_str(), Cmd.udbSkillNameLen, eOnlyToDirector);
}

void CSkillMgrServer::OnCoolDownEnd(const TCHAR* szSkillName)
{
	string strSkillName(szSkillName);
	CGas2GacCPP_OnCoolDownEnd Cmd;
	Cmd.uObjID = GetFighter()->GetEntityID();
	Cmd.udbSkillNameLen = strSkillName.size();
	GetFighter()->SendCmdToGac(&Cmd, eOnlyToDirector);
	GetFighter()->SendToGac(strSkillName.c_str(), Cmd.udbSkillNameLen, eOnlyToDirector);
}

void CSkillMgrServer::OnClearCommonCD()
{
	CGas2GacCPP_OnClearCommonCD Cmd;
	Cmd.uObjID = GetFighter()->GetEntityID();
	GetFighter()->SendCmdToGac(&Cmd, eOnlyToDirector);
}

void CSkillMgrServer::OnAllCoolDownEnd()
{
	CGas2GacCPP_OnAllCoolDownEnd Cmd;
	Cmd.uObjID = GetFighter()->GetEntityID();
	GetFighter()->SendCmdToGac(&Cmd, eOnlyToDirector);
}

void CSkillMgrServer::OnDoMagicOp(uint32 uMagicOpID, bool bNotPosition, uint32 uCreatorID, uint32 uTargetID, const CFPos& Pos, uint32 uHurtResult, bool bPlayFx)
{
	CGas2GacCPP_OnDoMagicOp Cmd;
	Cmd.uObjID = GetFighter()->GetEntityID();
	Cmd.uMagicOpID = uMagicOpID;
	Cmd.uCreatorID =uCreatorID ; 
	Cmd.uTargetID = uTargetID;
	Cmd.bNotPosition = bNotPosition; 
	Cmd.bPlayFx = bPlayFx;
	Cmd.fPos = Pos;
	Cmd.uHurtResult = uint8(uHurtResult);
	GetFighter()->SendCmdToGac(&Cmd, eSyncSight);
}

void CSkillMgrServer::OnCastBegin(uint32 uId, float fCastTime, uint64 uStartTime, uint32 uTargetID)
{
	//cout << "castbegin " << fCastTime << " " << fExpiredTime << endl;
	CFighterDictator* pFighterFrom = GetFighter();
	if(GetCurCasting() && !GetCurCasting()->GetNoStop())
		pFighterFrom->CppSetCtrlState(eFCS_ForbitAutoTrack, true);	//流程过程中禁止客户端进行自动追踪
	//uint64 uProcessTime = pFighterFrom->GetDistortedFrameTime();
	//int64 uGuessStartTime = uProcessTime - (uint64)(fCastTime*1000) + (uint64)(fExpiredTime*1000);
	int64 uGuessStartTime = uStartTime;

	pFighterFrom->m_pCastIDVariant->SetNumber((uint16)uId);
	pFighterFrom->m_pCastTimeVariant->SetNumber(fCastTime);
	pFighterFrom->m_pCastGuessStartTimeVariant->SetNumber(uGuessStartTime);
	pFighterFrom->m_pCastTargetIDVariant->SetNumber(uTargetID);
}

void CSkillMgrServer::OnCastEnd(CCastingProcess* pCastingProcess, uint32 uSkillID, uint32 uId,bool bIsFinish)
{
	GetFighter()->GetSkillMgr()->CancelCurCasting(pCastingProcess);

	CFighterDictator* pFighter = GetFighter();
	/*if (!pFighter->m_castIDVariant && !pFighter->m_castTimeVariant)
		return;
	pFighter->m_castIDVariant->SetNumber(uId);
	pFighter->m_castTimeVariant->SetNumber(0.0f);*/

	pFighter->m_pCastIsFinishVariant->SetNumber(bIsFinish);
	pFighter->m_pCastSkillIDVariant->SetNumber((uint16)uSkillID);
	pFighter->m_pCastIDVariant->SetNumber(uint16(0));
	pFighter->m_pCastSkillIDVariant->SetNumber(uint16(0));
	if(pCastingProcess && !pCastingProcess->GetNoStop())
		GetFighter()->CppSetCtrlState(eFCS_ForbitAutoTrack, false); 
}

CFighterDictator* GetMaster(CFighterDictator* pFighter)
{
	CCharacterDictator* pMaster = NULL;
	if (pFighter)
    {
	    if (pFighter->GetEntity()->GetGameEntityType()==eGET_CharacterDictator)
        {
		    CCharacterDictator* pCharacter = pFighter->GetCharacter();
		    pMaster = pCharacter ? pCharacter->GetMaster() : NULL;
	    }
	    else
    	{   
	    	CIntObjServer* pIntObj = pFighter->GetHandler()->GetIntObj();
	    	pMaster =pIntObj?pIntObj->GetOwner():NULL;
    	}
	}
	return pMaster?pMaster->GetFighter():NULL;
}

template<typename CmdType>
void SendDisplayInfo(const CmdType& Cmd,CFighterDictator* pFighter,CFighterDictator* pCreator, CFighterDictator* pFrom)
{
	pFighter->SendCmdToGac(&Cmd, eOnlyToDirector); 
	CFighterDictator* pFromMaster = GetMaster(pFrom);
	CFighterDictator* pCreatorMaster = GetMaster(pCreator);
	if (pFrom&&pFrom!=pFighter)
	{
		if (pFromMaster)
		{
			if (pFromMaster!=pFighter)
			{
				pFromMaster->SendCmdToGac(&Cmd, eOnlyToDirector);
			}
		}
		else
		{
			pFrom->SendCmdToGac(&Cmd, eOnlyToDirector); 
		}
	}
	if (pCreator&&pCreator!=pFighter&&pCreator!=pFrom&&pCreator!=pFromMaster)
	{

		if (pCreatorMaster)
		{
			if (pCreatorMaster!=pFighter&&pCreatorMaster!=pFrom&&pCreatorMaster!=pFromMaster)
			{
				pCreatorMaster->SendCmdToGac(&Cmd, eOnlyToDirector);
			}
		}
		else
		{
			pCreator->SendCmdToGac(&Cmd, eOnlyToDirector); 
		}
	}
}

void CSkillMgrServer::DisplayFightInfo(CFighterDictator* pCreator, CFighterDictator* pFrom, int32 iValue, EAgileType eAglieType, EHurtResult eHurtResult)
{
	GetFighter()->NotifyNPCBeHurt(iValue);
	CGas2GacCPP_OnDisplayFightInfo	Cmd;
	Cmd.uObjID=GetFighter()->GetEntityID();
	Cmd.iValue=iValue;
	Cmd.bOverTime	=false ;
	Cmd.uHurtType=(uint8)eHurtResult;
	Cmd.eAglieType = (uint8)eAglieType;
	Cmd.bIsSkill = true;
	Cmd.uSrcEntityID = pFrom?pFrom->GetEntityID():(pCreator?pCreator->GetEntityID():0);
	SendDisplayInfo(Cmd,GetFighter(),pCreator,pFrom);
}

void CSkillMgrServer::DisplayFightInfo(CFighterDictator* pCreator, CFighterDictator* pFrom, int32 iValue, EHurtResult eHurtResult, bool bIsSkill, uint32 uCoreObjID)
{
	GetFighter()->NotifyNPCBeHurt(iValue);
	CGas2GacCPP_OnDisplayMagicFightInfo	Cmd;
	Cmd.uObjID=GetFighter()->GetEntityID();
	Cmd.iValue=iValue;
	Cmd.uHurtType=(uint8)eHurtResult;
	Cmd.bIsSkill = bIsSkill;
	Cmd.uCoreObjID = uCoreObjID;
	SendDisplayInfo(Cmd,GetFighter(),pCreator,pFrom);
}

void CSkillMgrServer::DisplayFightInfo(CFighterDictator* pCreator, CFighterDictator* pFrom, int32 iValue, EHurtResult eHurtResult,bool bIsSkill)
{

	GetFighter()->NotifyNPCBeHurt(iValue);
	CGas2GacCPP_OnDisplayFightInfo	Cmd;
	Cmd.uObjID=GetFighter()->GetEntityID();
	Cmd.iValue=iValue;
	Cmd.bOverTime	=false;
	Cmd.uHurtType=(uint8)eHurtResult;
	Cmd.eAglieType = (uint8)eAT_HP;
	Cmd.bIsSkill = bIsSkill;
	Cmd.uSrcEntityID = GetFighter()->GetEntityID();
	SendDisplayInfo(Cmd,GetFighter(),pCreator,pFrom);
}

void CSkillMgrServer::DisplayFightInfo(CFighterDictator* pCreator, CFighterDictator* pFrom, int32 iValue, EHurtResult eHurtResult, bool bOverTime,bool bIsSkill,bool bIsArea)
{

	GetFighter()->NotifyNPCBeHurt(iValue);
	CGas2GacCPP_OnDisplayFightInfo	Cmd;
	Cmd.uObjID=GetFighter()->GetEntityID();
	Cmd.iValue=iValue;
	Cmd.bOverTime	=bOverTime;
	Cmd.uHurtType=(uint8)eHurtResult;
	Cmd.eAglieType = (uint8)eAT_HP;
	Cmd.bIsSkill = bIsSkill;
	if (!bOverTime)
	{
		Cmd.uSrcEntityID = pFrom?pFrom->GetEntityID():(pCreator?pCreator->GetEntityID():0);
		SendDisplayInfo(Cmd,GetFighter(),pCreator,pFrom);
	}
	else
	{
		Cmd.uSrcEntityID = GetFighter()->GetEntityID();
		SendDisplayInfo(Cmd,GetFighter(),pCreator,pFrom);
	}
}

uint32 CSkillMgrServer::GetPlayerDoSkillSuccNum()
{
	return ms_uPlayerDoSkillSuccNum;
}

uint32 CSkillMgrServer::GetPlayerDoSkillFailNum()
{
	return ms_uPlayerDoSkillFailNum;
}

uint32 CSkillMgrServer::GetNpcDoSkillNum()
{
	return ms_uNpcDoSkillNum;
}

void CSkillMgrServer::SetDoSkillCtrlState(EDoSkillCtrlState eSkillCtrlState, bool bSet)
{
	if (!GetFighter()->CppIsAlive())
	{
		//LogErr("死亡状态下不能再设置技能控制值,死亡时已经清空");
		return;
	}
	CDoSkillCtrlMgr::SetDoSkillCtrlState(eSkillCtrlState, bSet);

	CGas2GacCPP_OnUseSkillStateChanged cmd;
	cmd.uObjID			= GetFighter()->GetEntityID();
	cmd.uSkillCtrlState = eSkillCtrlState;
	cmd.bSet			= bSet;
	GetFighter()->SendCmdToGac(&cmd, eOnlyToDirector);
}

void CSkillMgrServer::RevertSkillCtrlState()
{
	CDoSkillCtrlMgr::RevertSkillCtrlState();

	CGas2GacCPP_OnRevertSkillCtrlState cmd;
	cmd.uObjID			= GetFighter()->GetEntityID();
	GetFighter()->SendCmdToGac(&cmd, eOnlyToDirector);
}

#include "stdafx.h"
#include "CFighterFollower.h"
#include "CMagicStateClient.h"
#include "COtherStateClient.inl"
#include "CTalent.h"
#include "CRenderObject.h"
#include "TPropertyHolder.inl"
#include "CCharacterFollower.h"
#include "PtrlGas2GacCPPDef.h"
#include "CSkillClient.h"
#include "FxPlayer.h"
#include "CMagicOpClient.h"
#include "CCastingProccessClient.h"
#include "CFacingMgr.h"
#include "CEntityClientManager.h"
#include "CMoveMagicClient.h"
#include "CActorCfgClient.h"
#include "IFighterFollowerHandler.h"
#include "CSoundPlayer.h"
#include "CFighterVariantCallback.h"
#include "CSyncVariantClient.h"
#include "CRenderLinkMgr.h"
#include "CoreCommon.h"
#include "CCoreObjectFollower.h"
#include "CBulletMagicClient.h"

template TPtRefer<CFighterFollower,CFighterFollower>;

CFighterFollower::CFighterFollower()
:CPtCORSubject(6)
,m_pSetStateVariantCallback(NULL)
{
	m_Target.SetHolder(this);
	m_RefsByFighter.SetHolder(this);
	m_pTalentHolder = new CTalentHolder();
	m_pMagicStateMgrClient = new CMagicStateMgrClient(this);
	m_pTriggerStateMgrClient = new COtherStateMgrClient<CTriggerStateClient>(this);
	m_pDamageChangeStateMgrClient = new COtherStateMgrClient<CDamageChangeStateClient>(this);
	m_pCumuliTriggerStateMgrClient = new COtherStateMgrClient<CCumuliTriggerStateClient>(this);
	m_pSpecialStateMgrClient = new COtherStateMgrClient<CSpecialStateClient>(this);
	m_pAllStateMgrClient = new CAllStateMgrClient(this);
}

CFighterFollower::~CFighterFollower()
{
	SafeDelete(m_pTalentHolder);
	SafeDelete(m_pMagicStateMgrClient);
	SafeDelete(m_pTriggerStateMgrClient);
	SafeDelete(m_pDamageChangeStateMgrClient);
	SafeDelete(m_pCumuliTriggerStateMgrClient);
	SafeDelete(m_pSpecialStateMgrClient);
	SafeDelete(m_pAllStateMgrClient);
	m_pCastingFX->CancelFx();
	m_pCastingFX = NULL;
	m_pMagicFX->CancelFx();
	m_pMagicFX = NULL;
	m_pLeadingFX->CancelFx();
	m_pLeadingFX = NULL;
	SetTarget(NULL);
}

void CFighterFollower::SetHandler(IFighterFollowerHandler* pHandler)
{
	m_pHandler = pHandler;
	m_pCastingFX = new CPlayerFX(GetEntity());
	m_pLeadingFX = new CLeadingFX(GetEntity());
	m_pMagicFX = new CPlayerFX(GetEntity());
}

ICharacterFollowerCallbackHandler*	CFighterFollower::GetCallBackHandler()const
{
	if(GetCharacter())
	{
		return GetCharacter()->GetCallbackHandler();
	}
	return NULL;
}

void CFighterFollower::SetTarget( CFighterFollower* pTarget )
{
	if(pTarget != GetTarget())
	{
		if(GetTarget() != NULL)
		{
			GetTarget()->Detach(this, eCE_Offline);
			GetTarget()->Detach(this, eCE_ChangeMapEnd);
		}
		if(pTarget != NULL)
		{
			pTarget->Attach(this, eCE_Offline);
			pTarget->Attach(this, eCE_ChangeMapEnd);
			m_Target.Attach(pTarget->m_RefsByFighter);
		}
		else
		{
			m_Target.Detach();
		}
	}
}

void CFighterFollower::OnCOREvent(CPtCORSubject* pSubject, uint32 uEvent,void* pArg)
{
	switch(uEvent)
	{
	case eCE_Offline:
	case eCE_ChangeMapEnd:
		SetTarget(NULL);
	}
}


uint32 CFighterFollower::GetTalentPoint(const string& szName)
{
	return m_pTalentHolder->GetPoint(szName);
}

void CFighterFollower::SetTargetBuff()
{
	//m_pMagicStateMgrClient->SetTargetBuff();
	//m_pTriggerStateMgrClient->SetTargetBuff();
	//m_pDamageChangeStateMgrClient->SetTargetBuff();
	//m_pCumuliTriggerStateMgrClient->SetTargetBuff();
	//m_pSpecialStateMgrClient->SetTargetBuff();
	m_pAllStateMgrClient->SetTargetBuff();
}

float CFighterFollower::GetRemainTime(const TCHAR* sName, uint32 uId)
{
	CBaseStateCfg* pCfg = CBaseStateCfg::GetByGlobalName(sName);
	switch(pCfg->GetGlobalType())
	{
	case eSGT_MagicState:
		return m_pMagicStateMgrClient->GetRemainTime(sName, uId);
	case eSGT_TriggerState:
		return m_pTriggerStateMgrClient->GetRemainTime(sName, uId);
	case eSGT_DamageChangeState:
		return m_pDamageChangeStateMgrClient->GetRemainTime(sName, uId);
	case eSGT_CumuliTriggerState:
		return m_pCumuliTriggerStateMgrClient->GetRemainTime(sName, uId);
	case eSGT_SpecialState:
		return m_pSpecialStateMgrClient->GetRemainTime(sName, uId);
	}
	return 0.0f;
}

bool CFighterFollower::ExistState(const TCHAR* sName)
{
	CBaseStateCfg* pCfg = CBaseStateCfg::GetByGlobalName(sName);
	if (pCfg)
	{
		switch(pCfg->GetGlobalType())
		{
			case eSGT_MagicState:
				return m_pMagicStateMgrClient->ExistState(sName);
			case eSGT_TriggerState:
				return m_pTriggerStateMgrClient->ExistState(sName);
			case eSGT_DamageChangeState:
				return m_pDamageChangeStateMgrClient->ExistState(sName);
			case eSGT_CumuliTriggerState:
				return m_pCumuliTriggerStateMgrClient->ExistState(sName);
			case eSGT_SpecialState:
				return m_pSpecialStateMgrClient->ExistState(sName);
			default:
				{
					stringstream ExpStr;
					ExpStr << "状态[" << sName << "]种类类型不正确";
					GenErr("状态种类类型不正确", ExpStr.str());
				}
		}
	}
	return false;
}

bool CFighterFollower::IsCanCancelState(const TCHAR*  uName)
{
	return CBaseStateCfg::GetByGlobalName(uName)->GetCancelable();
}

CCharacterFollower* CFighterFollower::GetCharacter()const
{
	IFighterFollowerHandler* pHandler = GetHandler();
	Ast(pHandler!=NULL);
	Ast(pHandler->GetCharacter());
	return pHandler->GetCharacter();
}

uint32 CFighterFollower::GetEntityID()const
{
	IFighterFollowerHandler* pHandler = GetHandler();
	Ast(pHandler!=NULL); 
	return pHandler->GetEntityID();
}

CEntityClient* CFighterFollower::GetEntity()const
{
	IFighterFollowerHandler* pHandler = GetHandler();
	Ast(pHandler!=NULL);
	Ast(pHandler->GetEntity());
	return pHandler->GetEntity();
}

void CFighterFollower::OnFighterCommand(const CGas2GacCPP_OnSyncCastingProcess* pCmd)
{
	uint64 uFrameTime = GetDistortedServerFrameTime();
	int64 uExpiredTime = uFrameTime - pCmd->uStartTime;
	float fExpiredTime = (float)uExpiredTime/1000;
	if (fExpiredTime < 0.0f)
		fExpiredTime = 0.0f;
	OnCastBegin(pCmd->uFromID,pCmd->uCastID,0,pCmd->fCastTime,fExpiredTime,true);
}

void CFighterFollower::OnCastBegin(uint32 uFromID, uint32 uID, uint32 uTargetID, float fCastTime, float fExpiredTime, bool bIsSync)
{
	const CCastingProcessCfgClientSharedPtr& pCastingProcessCfgClient = CCastingProcessCfgClient::GetById(uID);
	string strProcessAction = pCastingProcessCfgClient->GetProcessAction();
	string strProcessEffect = pCastingProcessCfgClient->GetProcessEffect();
	CActorFollower* pActor= GetCharacter()->GetActor();

	// LUA 回调播放吟唱条
	ECastingProcessType eCastType = pCastingProcessCfgClient->GetCastingType();
	if (eCastType != eCPT_ImmovableSing)
	{
		GetCallBackHandler()->OnCastingBarBegin(uFromID, pCastingProcessCfgClient->GetName(), eCastType, fCastTime, fExpiredTime, bIsSync);
	}

	if (bIsSync)
		return;
	if(!strProcessAction.empty())
	{
		if ((eCastType == eCPT_Sing || eCastType == eCPT_GradeSing || eCastType == eCPT_ImmovableSing) && fCastTime > 0.0f)
		{
			CActorCfgClient::SetSingAniName(strProcessAction);
			if (pActor->GetActionState() == eAS_Cast)
				pActor->SetComboSing(true);
			else
			{
				if (pActor->GetActionState() != eAS_Sing)
				{
					pActor->DoActionState(eAS_Sing, pActor->GetActionDir());
				}
			}
		}
		else if (eCastType == eCPT_Channel && pActor->GetActionState() != eAS_Channel)
		{
			CActorCfgClient::SetChannelAniName(strProcessAction);
			pActor->DoActionState(eAS_Channel, pActor->GetActionDir());
		}
	}

	if(!strProcessEffect.empty())
	{
		string strFXFile,strFXName;
		pActor->SplitSkillFxFile(strProcessEffect,strFXFile,strFXName,true);
		// 如果已经正在施放流程了，就不用播放特效了
		if(fExpiredTime < 0.2)
		{
			if (!strFXFile.empty())
			{
				if (pCastingProcessCfgClient->IsLinkEffect())
				{
					CCharacterFollower* pTarget = CEntityClientManager::GetInst()->GetCharacter(uTargetID);
					if (pTarget)
						GetLeadingFX()->PlayLoopFX(strFXFile, strFXName,pTarget);
				} 
				else
				{
					GetCastingFX()->PlayLoopFX(strFXFile, strFXName);
				}
			}
		}
	}
}

void CFighterFollower::OnCastEnd(uint32 uSkillID, uint32 uId, bool bIsFinish)
{
	const CCastingProcessCfgClientSharedPtr& pCastingProcessCfgClient = CCastingProcessCfgClient::GetById(uId);
	string strProcessAction = pCastingProcessCfgClient->GetProcessAction();
	string strProcessEffect = pCastingProcessCfgClient->GetProcessEffect();

	// LUA 回调播放吟唱条
	uint32 eCastType = pCastingProcessCfgClient->GetCastingType();
	if (eCastType != eCPT_ImmovableSing)
	{
		GetCallBackHandler()->OnCastingBarEnd(GetEntityID());
	}

	// 撤销特效
	if (pCastingProcessCfgClient->IsLinkEffect())
	{
		GetLeadingFX()->StopFX();
	} 
	else
	{
		GetCastingFX()->StopFX();
	}

	CActorFollower* pActor= GetCharacter()->GetActor();
	const CSkillClientSharedPtr& pSkillClient = CSkillClient::GetSkillByID(uSkillID);
	string strCastEffect = pSkillClient->GetCastEffect();
	string strCastAction = pSkillClient->GetCastAction();
	bool bAllBodyCastAni = pSkillClient->IsAllBodyCastAni();

	// 撤销动作
	if (!GetCharacter()->CppIsAlive())
		return;

	if (!strProcessAction.empty())
	{
		pActor->SetCurOnceActionLevel(eAPP_NULL);
		if(GetCharacter()->IsMoving())
			pActor->DoMoveState(eMS_Move, true);
		else
			pActor->DoMoveState(eMS_Stop, true);
	}

	if( !strCastAction.empty() && bIsFinish )
	{
		CActorCfgClient::SetCastAniName(strCastAction);
		pActor->SetAllBodyCastAni(bAllBodyCastAni);
		pActor->DoActionState(eAS_Cast);
	}
	else
	{	
		if (GetCharacter()->CppGetCtrlState(eFCS_InBattle))
			GetCharacter()->SetDirLockedTarget(true);

		if (pActor->GetActionState() != eAS_Cast)
			pActor->DoMoveAnimation();
		else
			pActor->SetComboSing(false);
	}

	if(!strCastEffect.empty() && bIsFinish)
	{
		string strFXFile,strFXName;
		pActor->SplitSkillFxFile(strCastEffect,strFXFile,strFXName,true);

		if (!strFXFile.empty())
		{
			CPlayerFX::PlayOnceFX(GetCharacter(), strFXFile, strFXName);
		}
	} 
}

// RPC
//void CFighterFollower::OnFighterCommand(const CGas2GacCPP_OnCtrlValueChanged* pCmd)
//{
//}
void CFighterFollower::OnCtrlVariantChanged(uint32 uValue, uint32 uValueTypeID, bool bSet, bool bIsAlive)
{
	CCharacterFollower* pCharFol = GetCharacter();
	CActorFollower* pActor = pCharFol->GetActor();

	CppSetAlive(bIsAlive);

	if (uValueTypeID == 0)			// 全
	{
		uint32 uOldCtrlValue = GetAllCtrlState();

		if (uOldCtrlValue == uValue)
			return;
	
		EFighterCtrlState eFighterCtrlState = eFCS_InNormalHorse;
		while (eFighterCtrlState < eFCS_Max)
		{
			if (((uOldCtrlValue ^ uValue) & eFighterCtrlState) == eFighterCtrlState)
			{
				CppSetCtrlState(eFighterCtrlState, (uValue & eFighterCtrlState) == eFighterCtrlState);
				GetCallBackHandler()->OnCtrlValueChanged(GetEntityID(), eFighterCtrlState, GetCtrlState(eFighterCtrlState));
			}

			eFighterCtrlState = (EFighterCtrlState)(eFighterCtrlState << 1);
		}
	}
	else									// 某一
	{
		CppSetCtrlState(EFighterCtrlState(uValueTypeID), bSet);
		GetCallBackHandler()->OnCtrlValueChanged(GetEntityID(), uValueTypeID, bSet);

		if (uValueTypeID == eFCS_InBattle && !pCharFol->CppGetCtrlState(eFCS_FeignDeath))
		{
			GetCharacter()->GetRenderLinkMgr()->DoWeaponChangeAni(bSet);

			if (eAPP_NULL == pActor->GetCurOnceActionLevel())
			{
				if (pCharFol->IsMoving())
					pCharFol->DoMoveState(eMS_Move);
				else
					pCharFol->DoMoveState(eMS_Stop);
			}
			pCharFol->SetDirLockedTarget(bSet);

			if (!pCharFol->GetIsPlayer() && bSet && pCharFol->GetNpcType() != ENpcType_Shadow)
			{
				CSoundPlayer::PlayYelpByType(pCharFol, eSTN_EnterBattle);
			}
		}
		if (uValueTypeID == eFCS_InDrawWeaponMode && bSet == true)
		{
			GetCharacter()->GetRenderLinkMgr()->DoWeaponChangeAni(false);
			GetCharacter()->GetRenderLinkMgr()->DoWeaponChangeAni(true);				
		}
		if (uValueTypeID == eFCS_InNormalAttacking || uValueTypeID == eFCS_InWalkState)
		{
			if (pCharFol->IsMoving())
				pCharFol->DoMoveState(eMS_Move);
			else
			{
				if (pActor->GetCurOnceActionLevel() != eAPP_SkillAction && pActor->GetCurOnceActionLevel() != eAPP_AttackAction)
					pCharFol->DoMoveState(eMS_Stop);
			}
		}
		if (uValueTypeID == eFCS_ForbitUseWeapon)
		{
			if (bSet == true)
				GetCharacter()->GetRenderLinkMgr()->SetWeaponVisible(true,false);
			else
				GetCharacter()->GetRenderLinkMgr()->SetWeaponVisible(true,true);
		}
		if (uValueTypeID == eFCS_InNormalHorse || uValueTypeID == eFCS_InBattleHorse)
		{
			pActor->SetCurOnceActionLevel(eAPP_NULL);
		}
	}
}

void CFighterFollower::OnFighterCommand(const CGas2GacCPP_OnLevelChanged* pCmd)
{
	CppSetLevel(pCmd->uLevel);
	ICharacterFollowerCallbackHandler* pHandler = GetCallBackHandler();
	if(pHandler == NULL)
	{
		GenErr("CallBackHandler == NULL");
	}
	pHandler->OnRelationChanged(pCmd->uObjID);
	if (GetCharacter() && GetCharacter()->GetAgileValueBeCare())
	{
		pHandler->OnAgileValueChanged(pCmd->uObjID);
	}
}

void CFighterFollower::OnFighterCommand(const CGas2GacCPP_OnClassChanged* pCmd)
{
	CppSetClass(EClass(pCmd->uClass));
}

void CFighterFollower::OnFighterCommand(const CGas2GacCPP_OnCampChanged* pCmd)
{
	CppSetCamp(ECamp(pCmd->uCamp));
	GetCallBackHandler()->OnRelationChanged(pCmd->uObjID);
}

void CFighterFollower::OnFighterCommand(const CGas2GacCPP_OnPKStateChanged* pCmd)
{
	SetPKState(pCmd->bPKState);
	GetCallBackHandler()->OnRelationChanged(pCmd->uObjID);
}

//void CFighterFollower::OnFighterCommand(const CGas2GacCPP_OnPKStateChanged* pCmd)
//{
//	SetPKState(EPKState(pCmd->uPKState));
//	GetCallBackHandler()->OnRelationChanged(pCmd->uObjID);
//}

//void CFighterFollower::OnFighterCommand(const CGas2GacCPP_OnZoneTypeChanged* pCmd)
//{
//	SetZoneType(EZoneType(pCmd->uZoneType));
//}

void CFighterFollower::OnFighterCommand(const CGas2GacCPP_OnDisplayFightInfo* pCmd)
{
	CFPos Pos(0,0);
	CCharacterFollower* pObj = CCharacterFollower::GetCharacterByID(pCmd->uSrcEntityID);
	if (pObj)
	{
		Pos = pObj->GetPixelPos();
	}
	GetCharacter()->DisplayHurt(pCmd->iValue, EAgileType(pCmd->eAglieType), EHurtResult(pCmd->uHurtType), pCmd->bOverTime, pCmd->bIsSkill,Pos);
}

void CFighterFollower::OnFighterCommand(const CGas2GacCPP_OnDisplayMagicFightInfo* pCmd)
{
	CFPos Pos(0,0);
	CDir dir(32);
	CFPos DesPos = GetCharacter()->GetPixelPos();
	CCoreObjectFollower* pObj = CCoreObjectFollower::GetObjectFollower(pCmd->uCoreObjID);
	if (pObj)
	{
		const CSyncVariantClientHolder* pViewVariant = pObj->GetViewVariantHolder(eCTI_Static);
		if (pViewVariant)
		{
			uint8 uDir = pViewVariant->GetNumber<uint8>("Attribute/Direction");
			dir.SetDir(uDir);
		}
	}
	CVector2f vecDirf;
	dir.Get(vecDirf);
	Pos.x = DesPos.x + eGridSpan * vecDirf.x;
	Pos.y = DesPos.y + eGridSpan * vecDirf.y;
	GetCharacter()->DisplayHurt(pCmd->iValue, eAT_HP, EHurtResult(pCmd->uHurtType), false, pCmd->bIsSkill,Pos);
}

void CFighterFollower::OnFighterCommand(const CGas2GacCPP_OnDisplayMsg* pCmd)
{
	string strMsgFX((TCHAR*)(pCmd+1), pCmd->GetPayloadSizeDerived());
	GetCallBackHandler()->OnDisplayMsg(GetCharacter(),pCmd->uMsgID,strMsgFX.c_str());
}

void CFighterFollower::OnFighterCommand(const CGas2GacCPP_OnSkillBegin* pCmd)
{
	// 如目标下线, 返回
	if (ETargetType(pCmd->uTargetType) == eTT_Fighter)
	{
		CCharacterFollower* pTargetFollower = CEntityClientManager::GetInst()->GetCharacter(pCmd->uTargetID);
		if(pTargetFollower == NULL)
			return;
	}

	const CSkillClientSharedPtr& pSkillClient = CSkillClient::GetSkillByID(pCmd->uSkillID);
	if (!pSkillClient->HasCastingProcess())
	{	
		string strCastAction = pSkillClient->GetCastAction();
		string strCastEffect = pSkillClient->GetCastEffect();
		bool bAllBodyCastAni = pSkillClient->IsAllBodyCastAni();

		CCharacterFollower* pCharacterFol = class_cast<CCharacterFollower*>(CEntityClientManager::GetInst()->GetCharacter(pCmd->uObjID));

		if(!strCastAction.empty())
		{
			CActorCfgClient::SetCastAniName(strCastAction);
			pCharacterFol->GetActor()->SetAllBodyCastAni(bAllBodyCastAni);
			pCharacterFol->GetActor()->DoActionState(eAS_Cast, pCmd->uActionDir);
		}

		if(!strCastEffect.empty())
		{
			string strFXFile,strFXName;
			GetCharacter()->GetActor()->SplitSkillFxFile(strCastEffect,strFXFile,strFXName,true);
			if (!strFXFile.empty())
			{
				CPlayerFX::PlayOnceFX(pCharacterFol,strFXFile, strFXName);
			}
		}
	}

	if (pSkillClient->GetSelectTargetType() == eFSTT_Position && GetTarget())
		GetCharacter()->SetDirLockedTarget(false);
}

void CFighterFollower::OnFighterCommand(const CGas2GacCPP_OnDoMagicOp* pCmd)
{
	CMagicOpCfgClient* pMagicOpCfgClient = CMagicOpCfgClient::GetById(pCmd->uMagicOpID);
	string strFX = pMagicOpCfgClient->GetFX();
	string strFXFile, strFXName, strSoundFile, strSoundName;
	CCharacterFollower* pCharacter= CCharacterFollower::GetCharacterByID(pCmd->uCreatorID);
	CCharacterFollower* pTargetCharFol = CCharacterFollower::GetCharacterByID(pCmd->uTargetID);
	EHurtResult eHurtResult = EHurtResult(pCmd->uHurtResult);

	if (pMagicOpCfgClient->GetFxType()==eFT_Suffer)
	{
		if (pCharacter)
		{
			if (strFX.empty())
			{
				pCharacter->GetActor()->SplitAttackFxFile(strFXFile,strFXName);
			}
			else
			{
				pCharacter->GetActor()->SplitSkillFxFile(strFX,strFXFile,strFXName,true);
			}
		}
	}
	else if (pMagicOpCfgClient->GetFxType()!=eFT_Local||GetCharacter()->CastToCharacterDirector())
	{
		vector<string> strFXTable = CBaseFX::Split(strFX, ",");
		strFXFile	= strFXTable[0]+strFXTable[1]+".efx";
		strFXName	= strFXTable[1];
	}

	if (strFXName.empty())
		return;

	if (pCmd->bNotPosition)
	{
		if (!pTargetCharFol)
			return;

		CFighterFollower* pFighter = pTargetCharFol->GetFighter();

		if (pMagicOpCfgClient->GetFxType() == eFT_Leading)
		{
			if (pCharacter)
			{
				CVector3f vec3f = pTargetCharFol->GetRenderObject()->GetCenterPosition();
				CLeadingFX::PlayOnceFX(pCharacter,strFXFile, strFXName, vec3f);
			}
		}
		else
		{
			if (eHurtResult == eHR_Miss)
				return;

			if (pCmd->bPlayFx && eHurtResult != eHR_PhyDodge && eHurtResult != eHR_MagDodge)
			{
				if (pMagicOpCfgClient->GetFxType()==eFT_Suffer||pMagicOpCfgClient->GetFxType()==eFT_LineDirection)
				{
					CLineDirectionFX::PlayOnceFX(pTargetCharFol, strFXFile, strFXName,pCmd->uCreatorID);
				}
				else
				{
					CPlayerFX::PlayOnceFX(pTargetCharFol, strFXFile, strFXName);
				}
			}

			if (eHurtResult != eHR_Hit && eHurtResult && eHR_Strike && eHurtResult != eHR_Resist)
				CSoundPlayer::Play3DSoundByName(pTargetCharFol, CSoundPlayer::GetOtherHitNmae(EHurtResult(pCmd->uHurtResult)));
		}
	}
	else
	{
		CFPos pos;
		if (pCharacter && pCharacter->CastToCharacterDirector())
		{
			pCharacter->GetFighter()->GetTargetPos(pos);		
		}
		else
		{
			pos = pCmd->fPos;
		}
		CPositionFX::PlayOnceFX(strFXFile, strFXName, pos);
	}
}

void CFighterFollower::OnFighterCommand(const CGas2GacCPP_OnClearState* pCmd)
{
	//cout << "[" << uint32(this) << "]::";
	//if(this->CastToFighterDirector())
	//	cout << "OnDirector";
	//else
	//	cout << "OnFollower";
	//cout << "Command[" << pCmd->uCmdOrder << "](CGas2GacCPP_OnClearState) by " << pCmd->uSendBy << "\n";

	OnClearAllState();
}

void CFighterFollower::OnClearAllState()
{
	GetMagicStateMgrClient()->ClearAll();
	GetTriggerStateMgrClient()->ClearAll();
	GetDamageChangeStateMgrClient()->ClearAll();
	GetSpecialStateMgrClient()->ClearAll();
}

void CFighterFollower::OnFighterCommand(const CGas2GacCPP_OnDeleteState* pCmd)
{
	//cout << "[" << uint32(this) << "]::";
	//if(this->CastToFighterDirector())
	//	cout << "OnDirector";
	//else
	//	cout << "OnFollower";
	//cout << "Command[" << pCmd->uCmdOrder << "](CGas2GacCPP_OnDeleteState[" << pCmd->uCategoryId << "," << pCmd->uDynamicId << "]) by " << pCmd->uSendBy << "\n";

	OnDeleteState(pCmd->uCategoryId, pCmd->uDynamicId);

}

void CFighterFollower::OnDeleteState(uint32 uCategoryId, uint32 uDynamicId)
{
	CBaseStateCfg* pStateCfg = CBaseStateCfg::GetByGlobalId(uCategoryId);
	if(!pStateCfg)
	{
		GenErr("CGas2GacCPP_OnDeleteState() pStateCfg为空\n");
		return ;
	}

	switch(pStateCfg->GetGlobalType())
	{
	case eSGT_MagicState:
		GetMagicStateMgrClient()->DeleteState(uCategoryId, uDynamicId);
		break;
	case eSGT_TriggerState:
		GetTriggerStateMgrClient()->DeleteState(uCategoryId, uDynamicId);
		break;
	case eSGT_DamageChangeState:
		GetDamageChangeStateMgrClient()->DeleteState(uCategoryId, uDynamicId);
		break;
	case eSGT_CumuliTriggerState:
		GetCumuliTriggerStateMgrClient()->DeleteState(uCategoryId, uDynamicId);
		break;
	case eSGT_SpecialState:
		GetSpecialStateMgrClient()->DeleteState(uCategoryId, uDynamicId);
		break;
	default:
		GenErr("StateType is invalid");
	}

}

void CFighterFollower::OnFighterCommand(const CGas2GacCPP_OnSetState* pCmd)
{
	//cout << "[" << uint32(this) << "]::";
	//if(this->CastToFighterDirector())
	//	cout << "OnDirector";
	//else
	//	cout << "OnFollower";
	//cout << "Command[" << pCmd->uCmdOrder << "](CGas2GacCPP_OnSetState[" << pCmd->uCategoryId << "," << pCmd->uDynamicId << "]) by " << pCmd->uSendBy << "\n";
	
	OnSetState(pCmd->uCategoryId, pCmd->uDynamicId, pCmd->uCount, pCmd->iTime, pCmd->iGuessStartTime, pCmd->uSkillLevel,pCmd->uInstallerID,pCmd->iCalcValue);					
}

void CFighterFollower::OnSetState(uint32 uCategoryId, uint32 uDynamicId, uint32 uCount, int64 iTime,
								  int64 iGuessStartTime, uint32 uSkillLevel, uint32 uInstallerID, int32 iCalcValue)
{
	CBaseStateCfg* pStateCfg = CBaseStateCfg::GetByGlobalId(uCategoryId);
	if(!pStateCfg)
	{
		GenErr("CGas2GacCPP_OnSetState() pStateCfg为空\n");
		return ;
	}

	int64 iRemainTime;

#ifdef TICK_BY_REMAINTIME
	iRemainTime = iGuessStartTime;
#else
	if(iGuessStartTime == STATE_TIME_INT64_MIN_VALUE)			//-9223372036854775808
	{
		iRemainTime = -2;
	}
	else if(iGuessStartTime == STATE_TIME_INT64_MIN_VALUE + 1)
	{
		iRemainTime = -1;
	}
	else
	{
		if(iTime < 0)
		{
			//stringstream str;
			//str << "[" << pStateCfg->GetName() << "]OnSetState(" << uCategoryId << ", " << uDynamicId << ", " << uCount << ", " << iTime << ", " <<
			//	iGuessStartTime << ") iTime(" << iTime << ") <= 0\n";
			//GenErr(str.str());
		}
		iRemainTime = iTime - (int64(GetDistortedServerFrameTime()) - iGuessStartTime + 500)/1000;
		if(iRemainTime < 0)		//误差可能会导致iRemainTime小于0，这时仍要显示图标
			iRemainTime = 0;
		else if(iRemainTime > iTime)
			iRemainTime = iTime;
		//cout << "onsetstate " << GetDistortedServerFrameTime() << " " << iGuessStartTime << endl;
	}
#endif

	int32 iTime32 = int32(iTime);
	int32 iRemainTime32 = int32(iRemainTime);

	//cout << "[" << GetDistortedServerFrameTime() << "]OnSetState(" << uCategoryId << ", " << uDynamicId << ", " 
	//	<< uCount << ", " << iTime32 << ", " << iRemainTime32 << ") Guess=" << iGuessStartTime << "\n";

	switch(pStateCfg->GetGlobalType())
	{
	case eSGT_MagicState:
		GetMagicStateMgrClient()->SetState(uCategoryId, uDynamicId, uCount, iTime32, iRemainTime32, uSkillLevel,uInstallerID,iCalcValue);
		break;
	case eSGT_TriggerState:
		GetTriggerStateMgrClient()->SetupState(uCategoryId,uDynamicId,  iTime32, iRemainTime32, uSkillLevel, uInstallerID);
		break;
	case eSGT_DamageChangeState:
		GetDamageChangeStateMgrClient()->SetupState(uCategoryId, uDynamicId, iTime32, iRemainTime32, uSkillLevel, uInstallerID);
		break;
	case eSGT_CumuliTriggerState:
		GetCumuliTriggerStateMgrClient()->SetupState(uCategoryId,uDynamicId,  iTime32, iRemainTime32, uSkillLevel, uInstallerID);
		break;
	case eSGT_SpecialState:
		GetSpecialStateMgrClient()->SetupState(uCategoryId, uDynamicId, iTime32, iRemainTime32, uSkillLevel, uInstallerID);
		break;
	default:
		GenErr("StateType is invalid");
	}
}

void CFighterFollower::OnFighterCommand(const CGas2GacCPP_OnCastMagicSkill* pCmd)
{
	vector<string> strFXTable;
	switch (EMagicGlobalType(pCmd->uType))
	{
	case eMGT_Aure:
		break;
	case eMGT_Move:
		{
			const CMoveMagicCfgClientSharedPtr& pMoveCfg = CMoveMagicCfgClient::GetByID(pCmd->uMagicID);
			if (pMoveCfg->GetActionType() == eMAT_Jump)
			{
				new CMoveMagicClient(this, pMoveCfg);
			}
			else
			{
				string strFXFile;	
				string strFXName;	
				uint32 uDelayTime = 0;
				GetCharacter()->GetActor()->SplitSkillFxFile(pMoveCfg->GetProcessFx(),strFXFile,strFXName,uDelayTime,false);
				if (!strFXFile.empty())
				{
					if (pCmd->bLoop)
					{
						GetMagicFX()->PlayLoopFX(strFXFile, strFXName,uDelayTime);
					}
					else
					{
						CPlayerFX::PlayOnceFX(GetCharacter(),strFXFile, strFXName,uDelayTime);
					}
				}
			}
			break;
		}
	}
}

void CFighterFollower::OnFighterCommand(const CGas2GacCPP_OnCancelMagicSkill* pCmd)
{
	GetMagicFX()->StopFX();
}

void CFighterFollower::OnFighterCommand(const CGas2GacCPP_OnSetNormalSkillTarget* pCmd)
{
	CCharacterFollower* pTargetFol = CCharacterFollower::GetCharacterByID(pCmd->uTargetID);
	GetCallBackHandler()->SetLockObj(pCmd->uObjID,pCmd->uTargetID);
	if(pTargetFol == NULL)
	{
		SetTarget(NULL);
		GetCharacter()->GetFacingMgr()->RemoveFacingPair();
		return ;
	}

	CCharacterFollower* pFol = this->GetCharacter();
	if(pFol && pTargetFol)
	{
		SetTarget(pTargetFol->GetFighter());
		GetCharacter()->GetFacingMgr()->CreateFacingPair(pFol, pTargetFol);
	}
}

void CFighterFollower::OnFighterCommand(const CGas2GacCPP_OnGameCampChanged* pCmd)
{
	GetCharacter()->CppSetGameCamp(pCmd->iGameCamp);
	GetCallBackHandler()->OnRelationChanged(pCmd->uObjID);
}

void CFighterFollower::OnFighterCommand(const CGas2GacCPP_OnCanBeTakeOver* pCmd)
{
	CCharacterFollower* pTargetFol = CCharacterFollower::GetCharacterByID(pCmd->uEntityID);
	if (NULL != pTargetFol)
	{
		pTargetFol->SetCanBeTakeOver(true);
	}
}

void CFighterFollower::OnFighterCommand(const CGas2GacCPP_OnCanNotTakeOver* pCmd)
{
	CCharacterFollower* pTargetFol = CCharacterFollower::GetCharacterByID(pCmd->uEntityID);
	if (NULL != pTargetFol)
	{
		pTargetFol->SetCanBeTakeOver(false);
	}
}

void CFighterFollower::OnFighterCommand(const CGas2GacCPP_OnLostTarget* pCmd)
{
	CCharacterFollower* pTargetFol = CCharacterFollower::GetCharacterByID(pCmd->uObjID);
	CCharacterFollower* pFol = this->GetCharacter();
	if(pFol)
	{
		GetCharacter()->GetFacingMgr()->RemoveFacingPair();
	}
}

void CFighterFollower::OnFighterCommand(const CGas2GacCPP_OnRealNameChange* pCmd)
{
	CCharacterFollower* pCharFol = CCharacterFollower::GetCharacterByID(pCmd->uObjID);
	if (pCharFol)
	{
		string strRealName((TCHAR*)(pCmd+1), pCmd->udbRealNameLen);
		pCharFol->SetRealName(strRealName);	
	}
}

void CFighterFollower::ClearAllState()
{
	if(m_pMagicStateMgrClient) m_pMagicStateMgrClient->ClearAll();
	if(m_pTriggerStateMgrClient) m_pTriggerStateMgrClient->ClearAll();
	if(m_pDamageChangeStateMgrClient) m_pDamageChangeStateMgrClient->ClearAll();
	if(m_pCumuliTriggerStateMgrClient) m_pCumuliTriggerStateMgrClient->ClearAll();
	if(m_pSpecialStateMgrClient) m_pSpecialStateMgrClient->ClearAll();
}

void CFighterFollower::CppSetClass(EClass eClass)
{
	SetClass(eClass);
}

void CFighterFollower::CppSetCamp(ECamp eCamp)
{
	SetCamp(eCamp);
}

void CFighterFollower::CppSetLevel(uint32 uLevel)
{
	SetLevel(uLevel);
}

void CFighterFollower::CppSetBirthCamp(ECamp eCamp)
{
	SetBirthCamp(eCamp);
}

void CFighterFollower::CppSetPKState(bool bPKState)
{
	SetPKState(bPKState);
}

//void CFighterFollower::CppSetPKState(EPKState ePKState)
//{
//	SetPKState(ePKState);
//}
//
//void CFighterFollower::CppSetZoneType(EZoneType eZoneType)
//{
//	SetZoneType(eZoneType);
//}

void CFighterFollower::InitAgileValue()
{
	const CSyncVariantClient* pVariant = GetCharacter()->GetViewVariant(eCTI_Agile);
	const CSyncVariantClient* pAgileValueVariant = pVariant->GetMember("AgileValue");
	CppSetPropertyValue(ePID_HealthPoint*ePFT_Count+ePFT_Adder, (float)pAgileValueVariant->GetMember("HealthPointAdder")->GetNumber<uint32>());
	CppSetPropertyValue(ePID_HealthPoint*ePFT_Count+ePFT_Multiplier, pAgileValueVariant->GetMember("HealthPointMutiplier")->GetNumber<float>());
	CppSetPropertyValue(ePID_HealthPoint*ePFT_Count+ePFT_Agile, (float)pAgileValueVariant->GetMember("HealthPointAgile")->GetNumber<uint32>());
	CppSetPropertyValue(ePID_ManaPoint*ePFT_Count+ePFT_Adder, (float)pAgileValueVariant->GetMember("ConsumePointAdder")->GetNumber<uint32>());
	CppSetPropertyValue(ePID_ManaPoint*ePFT_Count+ePFT_Multiplier, pAgileValueVariant->GetMember("ConsumePointMutiplier")->GetNumber<float>());
	CppSetPropertyValue(ePID_ManaPoint*ePFT_Count+ePFT_Agile, (float)pAgileValueVariant->GetMember("ConsumePointAgile")->GetNumber<uint32>());
	CppSetPropertyValue(ePID_ComboPoint*ePFT_Count+ePFT_Adder, (float)pAgileValueVariant->GetMember("ConsumePointAdder")->GetNumber<uint32>());
	CppSetPropertyValue(ePID_ComboPoint*ePFT_Count+ePFT_Multiplier, pAgileValueVariant->GetMember("ConsumePointMutiplier")->GetNumber<float>());
	CppSetPropertyValue(ePID_ComboPoint*ePFT_Count+ePFT_Agile, (float)pAgileValueVariant->GetMember("ConsumePointAgile")->GetNumber<uint32>());
	CppSetPropertyValue(ePID_RagePoint*ePFT_Count+ePFT_Adder, (float)pAgileValueVariant->GetMember("ConsumePointAdder")->GetNumber<uint32>());
	CppSetPropertyValue(ePID_RagePoint*ePFT_Count+ePFT_Multiplier, pAgileValueVariant->GetMember("ConsumePointMutiplier")->GetNumber<float>());
	CppSetPropertyValue(ePID_RagePoint*ePFT_Count+ePFT_Agile, (float)pAgileValueVariant->GetMember("ConsumePointAgile")->GetNumber<uint32>());
	CppSetPropertyValue(ePID_EnergyPoint*ePFT_Count+ePFT_Adder, (float)pAgileValueVariant->GetMember("ConsumePointAdder")->GetNumber<uint32>());
	CppSetPropertyValue(ePID_EnergyPoint*ePFT_Count+ePFT_Multiplier, pAgileValueVariant->GetMember("ConsumePointMutiplier")->GetNumber<float>());
	CppSetPropertyValue(ePID_EnergyPoint*ePFT_Count+ePFT_Agile, (float)pAgileValueVariant->GetMember("ConsumePointAgile")->GetNumber<uint32>());
	CCharacterFollower* pCharacter = GetCharacter();
	pCharacter->UpdateHeadBloodRendler();

	CSyncVariantClient* pValueVariant = pVariant->GetMember("CtrlValue");
	OnCtrlVariantChanged(pValueVariant->GetMember("Value")->GetNumber<uint32>(), 0, 
		false, pValueVariant->GetMember("IsAlive")->GetNumber<bool>());

	if (GetCallBackHandler() && pCharacter && pCharacter->GetAgileValueBeCare())
	{
		GetCallBackHandler()->OnAgileValueChanged(GetEntityID());
	}
}

void CFighterFollower::InitVariantCallback()
{
	m_pHealthPointVariantCallback	= new CFighterHealthPointVariantCallback(this);
	m_pConsumePointVariantCallback		= new CFighterConsumePointVariantCallback(this);
	GetCharacter()->RegViewVarChangedCallback("HealthPointAdder", m_pHealthPointVariantCallback, eCTI_Agile);
	GetCharacter()->RegViewVarChangedCallback("HealthPointMutiplier", m_pHealthPointVariantCallback, eCTI_Agile);
	GetCharacter()->RegViewVarChangedCallback("HealthPointAgile", m_pHealthPointVariantCallback, eCTI_Agile);
	GetCharacter()->RegViewVarChangedCallback("ConsumePointAdder", m_pConsumePointVariantCallback, eCTI_Agile);
	GetCharacter()->RegViewVarChangedCallback("ConsumePointMutiplier", m_pConsumePointVariantCallback, eCTI_Agile);
	GetCharacter()->RegViewVarChangedCallback("ConsumePointAgile", m_pConsumePointVariantCallback, eCTI_Agile);
	m_pCastBeginVariantCallback = new CFighterCastingBeginVariantCallback(this);
	GetCharacter()->RegSyncVarChangedCallback("TargetIDOfCast", m_pCastBeginVariantCallback, eCTI_Agile);
	m_pCastEndVariantCallback = new CFighterCastingEndVariantCallback(this);
	GetCharacter()->RegSyncVarChangedCallback("uSkillID", m_pCastEndVariantCallback, eCTI_Agile);
	//m_pSetStateVariantCallback = new CFighterSetStateVariantCallback(this);
	//GetCharacter()->RegSyncVarChangedCallback("zzzzzStateIsSync", m_pSetStateVariantCallback);
	m_pDelStateVariantCallback = new CFighterDelStateVariantCallback(this);
	GetCharacter()->RegSyncVarChangedCallback("zzzzzStateIsFinish", m_pDelStateVariantCallback, eCTI_Agile);
	m_pClearAllStateVariantCallback = new CFighterClearAllStateVariantCallback(this);
	GetCharacter()->RegSyncTableClearCallback("root", m_pClearAllStateVariantCallback, eCTI_Agile);
	m_CtrlValueVariantCallback = new CFighterCtrlValueVariantCallback(this);
	GetCharacter()->RegViewVarChangedCallback("ValueTypeID", m_CtrlValueVariantCallback, eCTI_Agile);
}

void CFighterFollower::CppSetGameCamp(int32 iGameCamp)
{
	SetGameCamp(iGameCamp);
}

bool CFighterFollower::OnSetStateForInit(const void* pContext, const TCHAR* sName, CCypherVariant& pVariant)
{
	//cerr << "CFighterFollower::OnSetStateForInit( " << sName <<")" << endl;
	ICypherVariantHolderHandler* pHandler = static_cast<ICypherVariantHolderHandler*>(const_cast<void*>(pContext));
	pHandler->OnValueChanged(static_cast<CCypherVariant*>( &pVariant ));

	return true;
}

void CFighterFollower::RegistDistortedTick(CDistortedTick* pTick, uint32 uCyc)
{
	GetEntity()->RegistDistortedTick(pTick,uCyc);
}

void CFighterFollower::UnRegistDistortedTick(CDistortedTick* pTick)
{
	GetEntity()->UnRegistDistortedTick(pTick);
}

uint64 CFighterFollower::GetDistortedProcessTime()
{
	return GetEntity()->GetDistortedProcessTime();
}

uint64 CFighterFollower::GetDistortedFrameTime()
{
	return GetEntity()->GetDistortedFrameTime();
}

uint64 CFighterFollower::GetDistortedServerFrameTime()
{
	return GetEntity()->GetDistortedServerFrameTime();
}

#include "stdafx.h"
#include "CFighterDictator.h"
#include "BaseHelper.h"
#include "TPropertyHolder.inl"
#include "IFighterServerHandler.h"
#include "CTempVarServer.h"
#include "NormalAttack.h"
#include "CNormalAttackCfg.h"
#include "CSkillMgrServer.h"
#include "CSkillServer.h"
#include "CAllStateMgr.h"
#include "CTalentEffServer.h"
#include "CMagicMgrServer.h"
#include "CMagicStateMgr.h"
#include "PtrlGas2GacCPPDef.h"
#include "CCharacterDictator.h"
#include "ICharacterMediatorCallbackHandler.h"
#include "CCoolDownMgrServer.h"
#include "CSpeedChangeMgr.h"
#include "CEntityServerManager.h"
#include "CBattleStateManager.h"
#include "CCharacterMediator.h"
#include "CIntObjServer.h"
#include "CTeamServerManager.h"
#include "CAureMagicServer.h"
#include "CAureMagicCfgServer.h"
#include "CTriggerEvent.h"
#include "CStateDBData.h"
#include <ctime>
#include "PatternRef.inl"
#include "CConnServer.h"
#include "CSkillInstServer.h"
#include "CServantServerMgr.h"
#include "CSyncVariantServer.h"
#include "ISend.h"
#include "FighterProperty.inl"
#include "NpcInfoMgr.h"
#include "CBulletMagicCfgServer.h"
#include "TSqrAllocator.inl"
#include "MagicOps_ChangeValue.h"
#include "CSkillCfg.h"
#include "CRelationMgrServer.h"
#include "CFightStatisticMgr.h"

template class TPtRefer<CFighterDictator,IFighterServerHandler>;
template class TPtRefer<CFighterDictator,CFighterDictator>;
CFighterDictator::MapSyncCalProperty CFighterDictator::ms_mapSyncCalProperty;

CFighterDictator::CFighterDictator()
:CPtCORSubject(6)
,m_pTempVarMgr(NULL)
,m_pNormalAttackMgr(NULL)
,m_pSkilllMgr(NULL)
,m_pMagicMgr(NULL)
,m_pAllStateMgr(NULL)
,m_pCoolDownMgr(NULL)
,m_pTalentHolder(NULL)
,m_pMoveSpeedChangeMgr(NULL)
,m_pWalkSpeedChangeMgr(NULL)
,m_pNASpeedChangeMgr(NULL)
,m_pBattleStateMgr(NULL)
,m_pSkillCoolDownRet(NULL)
,m_pAureMagicRet(NULL)
,m_bLevelPress(true)
,m_pCastIDVariant(NULL)
,m_pCastTimeVariant(NULL)
,m_pCastGuessStartTimeVariant(NULL)
,m_pCastTargetIDVariant(NULL)
,m_pCastSkillIDVariant(NULL)
,m_pCastIsFinishVariant(NULL)
,m_pCtrlValueVariant(NULL)
,m_pCtrlValueTypeIDVariant(NULL)
,m_pCtrlSetVariant(NULL)
,m_pCtrlIsAliveVariant(NULL)
,m_pHealthPointAdderVariant(NULL)
,m_pHealthPointMutiplierVariant(NULL)
,m_pHealthPointAgileVariant(NULL)
,m_pConsumePointAdderVariant(NULL)
,m_pConsumePointMutiplierVariant(NULL)
,m_pConsumePointAgileVariant(NULL)
{
	m_Handler.SetHolder(this);
	m_RefsByFighter.SetHolder(this);
	m_RefsByMagic.SetHolder(this);
	m_RefsByCast.SetHolder(this);
	m_Target.SetHolder(this);
	m_LockingTarget.SetHolder(this);
	
	m_pAllStateMgr			= new CAllStateMgrServer(this);
	m_pCoolDownMgr			= new CCoolDownMgrServer(this);
	m_pTalentHolder			= new CTalentHolderServer();
	m_pTempVarMgr			= new CTempVarMgrServer(this);
	m_pMagicMgr				= new CMagicMgrServer(this);
	m_pMoveSpeedChangeMgr	= new CSpeedChangeMgr();
	m_pWalkSpeedChangeMgr	= new CSpeedChangeMgr();
	m_pNASpeedChangeMgr		= new CSpeedChangeMgr();
	m_pNormalAttackMgr		= new CNormalAttackMgr(this);
	m_pBattleStateMgr		= new CBattleStateManager(this);
}

CFighterDictator::~CFighterDictator()
{
	m_HealthPoint.Unit();
	m_EnergyPoint.Unit();
	m_RagePoint.Unit();
	m_ManaPoint.Unit();
	SafeDelete(m_pTempVarMgr);
	SafeDelete(m_pTalentHolder);	
	SafeDelete(m_pCoolDownMgr);	
	SafeDelete(m_pNormalAttackMgr);
	SafeDelete(m_pSkilllMgr);	
	SafeDelete(m_pMagicMgr);
	SafeDelete(m_pMoveSpeedChangeMgr);
	SafeDelete(m_pWalkSpeedChangeMgr);
	SafeDelete(m_pNASpeedChangeMgr);
	SafeDelete(m_pBattleStateMgr);
	SafeDelete(m_pAllStateMgr);
	m_mapActiveSkill.clear();
	SetHandler(NULL);
	m_Target.Detach();
	SetLockingTarget(NULL);
}

void CFighterDictator::Init()
{
	m_pSkilllMgr = new CSkillMgrServer(this);
}

IFighterServerHandler* CFighterDictator::GetHandler()const	
{
	Ast(m_Handler.Get()); 
	return m_Handler.Get();
}

void CFighterDictator::SetHandler(IFighterServerHandler* pHandler)
{
	pHandler ? m_Handler.Attach(pHandler->GetRefByFighter()) : m_Handler.Detach();
}

ICharacterDictatorCallbackHandler*	CFighterDictator::GetCallBackHandler()const	
{
	CCharacterDictator* pCharacter = GetCharacter();
	if (pCharacter)
	{
		return pCharacter->GetCallbackHandler();
	}
	return NULL;
}

void CFighterDictator::CppInit(EClass eClass, uint32 uLevel, ECamp eCamp)
{
	CppSetClass(eClass);
	CppSetLevel(uLevel);
	CppSetCamp(eCamp);
	InitAgileValue();
	m_HealthPoint.FullFill(this);
	m_ManaPoint.FullFill(this);
}

void CFighterDictator::CppSetLevel(uint32 uLevel)
{
	SetLevel(uLevel);
	if(m_pTalentHolder->GetHandler())
	{
		CTalentEffServer* pTalentEffServer = class_cast<CTalentEffServer*>(m_pTalentHolder->GetHandler());
		pTalentEffServer->Update();
	}
	OnLevelChanged(uLevel);
}

void CFighterDictator::CppSetClass(EClass eClass)
{
	SetClass(eClass);
	OnClassChanged(eClass);
}

void CFighterDictator::CppSetBirthCamp(ECamp eCamp)
{
	OnPlayerBirthCampChange(eCamp);
	SetBirthCamp(eCamp);
}

void CFighterDictator::CppSetCamp(ECamp eCamp)
{
	SetCamp(eCamp);
	OnCampChanged(eCamp);
	Notify(eCE_BattleRelationChange);
	CCharacterDictator* pCharacter = GetCharacter();
	if(pCharacter)
	{
		CServantServerMgr* pServantMgr = pCharacter->GetServantMgr();
		if(pServantMgr)	
		{
			const CServantServerMgr::ServantIDMapType& ServantMap = pServantMgr->GetServantIDMapType();
			CServantServerMgr::ServantIDMapType::const_iterator it = ServantMap.begin();
			for(;it!=ServantMap.end();++it)
			{
				CCharacterDictator* pServant = (*it).second;
				if (pServant && NpcInfoMgr::CanChangeCamp(pServant->GetNpcType()))
					pServant->CppSetCamp(eCamp);
			}
		}
	}
}

void CFighterDictator::CppSetGameCamp(int32 iGameCamp)
{
	SetGameCamp(iGameCamp);
	OnGameCampChange(iGameCamp);
	Notify(eCE_BattleRelationChange);
	CCharacterDictator* pCharacter = GetCharacter();
	if(pCharacter)
	{
		CServantServerMgr* pServantMgr = pCharacter->GetServantMgr();
		if(pServantMgr)	
		{
			const CServantServerMgr::ServantIDMapType& ServantMap = pServantMgr->GetServantIDMapType();
			CServantServerMgr::ServantIDMapType::const_iterator it = ServantMap.begin();
			for(;it!=ServantMap.end();++it)
			{
				CCharacterDictator* pServant = (*it).second;
				if (pServant && NpcInfoMgr::CanChangeCamp(pServant->GetNpcType()))
					pServant->CppSetGameCamp(iGameCamp);
			}
		}
	}
}

void CFighterDictator::CppSetPKState(bool bPKState)
{
	SetPKState(bPKState);
	OnPKStateChanged(bPKState);
	Notify(eCE_BattleRelationChange);
	CCharacterDictator* pCharacter = GetCharacter();
	if(pCharacter)
	{
		CServantServerMgr* pServantMgr = pCharacter->GetServantMgr();
		if(pServantMgr)	
		{
			const CServantServerMgr::ServantIDMapType& ServantMap = pServantMgr->GetServantIDMapType();
			CServantServerMgr::ServantIDMapType::const_iterator it = ServantMap.begin();
			for(;it!=ServantMap.end();++it)
			{
				CCharacterDictator* pServant = (*it).second;
				if (pServant && NpcInfoMgr::CanChangeCamp(pServant->GetNpcType()))
					pServant->CppSetPKState(bPKState);
			}
		}
	}
}

//void CFighterDictator::CppSetPKState(EPKState ePKState)
//{
//	SetPKState(ePKState);
//	OnPKStateChanged(ePKState);
//	Notify(eCE_BattleRelationChange);
//	CCharacterDictator* pCharacter = GetCharacter();
//	if(pCharacter)
//	{
//		CServantServerMgr* pServantMgr = pCharacter->GetServantMgr();
//		if(pServantMgr)	
//		{
//			const ServantIDMapType& ServantMap = pServantMgr->GetServantIDMapType();
//			ServantIDMapType::const_iterator it = ServantMap.begin();
//			for(;it!=ServantMap.end();++it)
//			{
//				CCharacterDictator* pServant = (*it).second;
//				if (pServant && NpcInfoMgr::CanChangeCamp(pServant->GetNpcType()))
//					pServant->CppSetPKState(ePKState);
//			}
//		}
//	}
//}
//
//void CFighterDictator::CppSetZoneType(EZoneType eZoneType)
//{
//	OnZoneTypeChanged(eZoneType);
//	SetZoneType(eZoneType);
//	CCharacterDictator* pCharacter = GetCharacter();
//	if(pCharacter)
//	{
//		CServantServerMgr* pServantMgr = pCharacter->GetServantMgr();
//		if(pServantMgr)	
//		{
//			const ServantIDMapType& ServantMap = pServantMgr->GetServantIDMapType();
//			ServantIDMapType::const_iterator it = ServantMap.begin();
//			for(;it!=ServantMap.end();++it)
//			{
//				CCharacterDictator* pServant = (*it).second;
//				if (pServant && NpcInfoMgr::CanChangeCamp(pServant->GetNpcType()))
//					pServant->CppSetZoneType(eZoneType);
//			}
//		}
//	}
//}

bool CFighterDictator::DoNormalAttack(CFighterDictator* pTarget)
{
	if(GetNormalAttackMgr())
	{
		return GetNormalAttackMgr()->DoAttack(pTarget);

	}
	return false;
}


void CFighterDictator::CppInitMHNA(const TCHAR* szMHName, const TCHAR* MHAttackType, const TCHAR* MHWeaponType, float fAttakSpeed)
{
	if(!m_pNormalAttackMgr)
		return;

	SetMainHandSpeed(this,fAttakSpeed);
//	if(m_pNormalAttackMgr->GetMHNormalAttack()->IsShortNormalAttack() &&
//		MHWeaponType > eWT_LongStick && MHWeaponType < eWT_End)
//	{
//		delete m_pNormalAttackMgr;
//		//m_pNormalAttackMgr = new CLongNormalAttackMgr(this);
//		m_pNormalAttackMgr = new CNormalAttackMgr(this);
//	}
//	else if(!m_pNormalAttackMgr->GetMHNormalAttack()->IsShortNormalAttack() &&
//		(MHWeaponType > eWT_None && MHWeaponType < eWT_Bow))
//	{
//		delete m_pNormalAttackMgr;
//		m_pNormalAttackMgr = new CNormalAttackMgr(this);
//	}

	//m_pNormalAttackMgr->Pause();
	m_pNormalAttackMgr->InitMHNA(szMHName, CSkillCfg::ms_mapAttackType[MHAttackType], CNormalAttackCfg::ms_mapWeaponType[MHWeaponType]);
	//m_pNormalAttackMgr->GetMHNormalAttack()->SetWeaponInterval(this->m_MainHandWeaponInterval.Get(this));
	//m_pNormalAttackMgr->Restart(false);
}

void CFighterDictator::CppInitAHNA(const TCHAR* szAHName, const TCHAR* AHAttackType, const TCHAR* AHWeaponType, float fAttakSpeed)
{
	if(!m_pNormalAttackMgr)
		return;

	SetOffHandSpeed(this,fAttakSpeed);
	//换副手武器时不能更换CNormalAttackMgr的类型，例如从CLongNormalAttackMgr变到CNormalAttackMgr


	m_pNormalAttackMgr->InitAHNA(szAHName,CSkillCfg::ms_mapAttackType[AHAttackType], CNormalAttackCfg::ms_mapWeaponType[AHWeaponType]);
	//m_pNormalAttackMgr->GetAHNormalAttack()->SetWeaponInterval(this->m_AssistantWeaponInterval.Get(this));
}

void CFighterDictator::CppInitNPCNormalAttack(const TCHAR* szMHName)
{
	if(!m_pNormalAttackMgr)
		return;
	
	CSingleHandNATick* pMainHandAttack=GetNormalAttackMgr()->GetMHNormalAttack();

	if(!pMainHandAttack)
		return;

	m_pNormalAttackMgr->InitNPCNormalAttack(szMHName);
}

void CFighterDictator::CppInitNormalAttack(const TCHAR* szMHName, EAttackType MHAttackType, EWeaponType MHWeaponType, 
										   const TCHAR* szAHName, EAttackType AHAttackType, EWeaponType AHWeaponType)
{
	if(!m_pNormalAttackMgr)
		return;
	
	CSingleHandNATick* pMainHandAttack=GetNormalAttackMgr()->GetMHNormalAttack();

	if(!pMainHandAttack)
		return;

	m_pNormalAttackMgr->InitNormalAttack(szMHName,MHAttackType,MHWeaponType,szAHName,AHAttackType,AHWeaponType);
}

void CFighterDictator::SetLockingTarget(CFighterDictator* pTarget)
{
	//cout<<"void CFighterDictator::SetLockingTarget:"<<pTarget<<endl;
	pTarget ? m_LockingTarget.Attach(pTarget->m_RefsByFighter) : m_LockingTarget.Detach();
}

void CFighterDictator::SetEnmityToTarget(CFighterDictator* pTarget)
{
	if (!m_Target.Get() && pTarget)//&& !this->GetCtrlState(eFCS_InBattle)
	{
		this->SetTarget(pTarget);
		CGas2GacCPP_OnSetNormalSkillTarget cmd;
		cmd.uObjID = GetEntityID();
		cmd.uTargetID = pTarget->GetEntityID();
		SendCmdToGac(&cmd, eOnlyToDirector);
	}
}

void CFighterDictator::SetTarget(CFighterDictator* pTarget, bool bCalNAImmediately)
{
	CGas2GacCPP_OnSetNormalSkillTarget cmd;
	cmd.uObjID = GetEntityID();
	if(pTarget)
	{
		if (pTarget == GetTarget())
			return;
		m_Target.Attach(pTarget->m_RefsByFighter);
		cmd.uTargetID = pTarget->GetEntityID();
		GetCharacter()->GetTargetIDVariant()->SetNumber((uint32)pTarget->GetEntityID());
	}
	else
	{
		m_Target.Detach();
		cmd.uTargetID = 0;
		//cout << "SetTarget(0) cancel NA" << endl;
		CancelAttack(bCalNAImmediately);
		GetCharacter()->GetTargetIDVariant()->SetNumber((uint32)0);
	}
	GetIS(eSyncSight)->Send(&cmd, sizeof(CGas2GacCPP_OnSetNormalSkillTarget));//只发给follower
}


void CFighterDictator::PauseAttack(bool bLockRestart, bool bCalDamageImmediately)
{
}

bool CFighterDictator::RestartAttack(bool bUnlockRestart)
{
	return false;
}

void CFighterDictator::CancelAttack(bool bCalDamageImmediately)
{
	if(!GetNormalAttackMgr())
	{
		return;
	}
	//if (!IsAttacking())
	//{
	//	return;
	//}

	GetNormalAttackMgr()->Cancel(bCalDamageImmediately, false);
	SetLockingTarget(NULL);
}

bool CFighterDictator::IsAttacking()
{
	if(!GetNormalAttackMgr())
	{
		return false;
	}
	return	m_pNormalAttackMgr->CNormalAttackMgr::IsAttacking();
}

void CFighterDictator::DoNPCSkill(const string& strSkillName, uint8 uSkillLevel, CGenericTarget* pTarget,bool bDoSkillRule)
{
	m_pSkilllMgr->DoNPCSkill(strSkillName,uSkillLevel,pTarget,bDoSkillRule);
}

void CFighterDictator::DoNPCSkill(const CNormalSkillServerSharedPtr* pCfg, uint8 uSkillLevel, CGenericTarget* pTarget,bool bDoSkillRule)
{
	m_pSkilllMgr->DoNPCSkill(*pCfg,uSkillLevel,pTarget,bDoSkillRule);
}

void CFighterDictator::DoIntObjSkill(const string& strSkillName, uint8 uSkillLevel, CGenericTarget* pTarget)
{
	m_pSkilllMgr->DoIntObjSkill(strSkillName,uSkillLevel,pTarget);
}

void CFighterDictator::IntDie()
{
	if(!CppIsAlive())
		return;

	GetAllStateMgr()->ClearAllNonpersistentState();
	GetBattleStateMgr()->LeaveBattleStateByDead();	//这句话必须调用，不然lua得不到脱离战斗的回调
	CppSetAlive(false);	//这句会将m_pAliveState置为空，必须放在所有清空操作之后
	GetSkillMgr()->RevertSkillCtrlState();
}

void CFighterDictator::CppDie(CFighterDictator* pAttacker, const TCHAR* szSkillName)
{	
	if(!CppIsAlive())
		return;
	Notify(eCE_BeforeDie);

	uint32 uFromID = pAttacker ? pAttacker->GetEntityID() : 0;
	uint32 uToID = GetEntityID();
	CTriggerEvent::MessageEvent(eTST_Trigger, uFromID, uToID, eSET_Kill);

	if(IsAttacking())
		CancelAttack();
	//cancel攻击者的AutoTrack
	if(pAttacker && pAttacker->GetLockingTarget() == this)
		pAttacker->CancelNormalAttackAutoTracking();

	CppSetCtrlState(eFCS_OnMission, false);
	CppSetCtrlState(eFCS_InNormalHorse, false);
	CppSetCtrlState(eFCS_InBattleHorse, false);
	CppSetCtrlState(eFCS_ForbitMove, true);
	OnDeadToQuest(pAttacker, szSkillName);

	IntDie();

	ResetCtrlValueVariant();
	OnDead(pAttacker,szSkillName);
	m_HealthPoint.StopRenewHP();
	m_ManaPoint.StopRenewMP();
	m_RagePoint.StopReduceRP();
	m_EnergyPoint.StopRenewEP();
}

void CFighterDictator::CppReborn()
{
	if (CppIsAlive())
		return;
	GetAllStateMgr()->ClearAllNonpersistentState();
	m_HealthPoint.ResetRenewHP(this);
	m_HealthPoint.FullFill(this);	
	if (CppGetClass() == eCL_Warrior || CppGetClass() == eCL_OrcWarrior)
	{
		m_RagePoint.ResetReduceRP(this);
		m_RagePoint.LimitSet(0, this);
	}
	else if (CppGetClass() == eCL_DwarfPaladin)
	{
		m_EnergyPoint.ResetRenewEP(this);
		m_EnergyPoint.LimitSet(0, this);
	}
	else
	{
		m_ManaPoint.ResetRenewMP(this);
		m_ManaPoint.FullFill(this);
	}

	CppSetAlive(true);
	OnReborn();
	Notify(eCE_Reborn);
}

bool CFighterDictator::ExistState(const string& name)
{
	return GetAllStateMgr()->ExistState(name);
}

uint32 CFighterDictator::GetStateCascade(const string& name)
{
	return GetAllStateMgr()->GetStateCascade(name);
}

void CFighterDictator::ClearState(const string& name)
{
	GetAllStateMgr()->EraseState(name);
}

bool CFighterDictator::EraseErasableState(const string& name)
{
	return GetAllStateMgr()->EraseErasableState(name);
}


void CFighterDictator::InsertTalent(const string& szName, uint32 uPoint)
{
	Ast(CppIsAlive());
	m_pTalentHolder->ChangeTalent(szName, uPoint);
}

void CFighterDictator::InsertEquipTalent(const string& szName, uint32 uPoint)
{
	Ast(CppIsAlive());
	m_pTalentHolder->InsertEquipTalent(szName, uPoint);
}

bool CFighterDictator::RemoveEquipTalent(const string& szName, uint32 uPoint)
{
	Ast(CppIsAlive());
	return m_pTalentHolder->RemoveEquipTalent(szName, uPoint);
}
		
void CFighterDictator::ClearAllTalent()
{
	Ast(CppIsAlive());

	// 清除抓来的召唤兽
	CCharacterDictator* pServant = GetCharacter()->GetServantByType(ENpcType_NotCtrlSummon);
	if (pServant)
		pServant->SetOnDead();

	m_pTalentHolder->ClearAllSkillTalent();
	Notify(eCE_BeInterrupted);
}
void CFighterDictator::AddSkill(const string& strSkillName,uint8 uSkillLevel)
{
	m_mapActiveSkill[strSkillName]=uSkillLevel;
}

void CFighterDictator::RemoveSkill(const string& strSkillName)
{
	MapActiveSkill::iterator it=m_mapActiveSkill.find(strSkillName);
	if ( it!=m_mapActiveSkill.end())
	{
		m_mapActiveSkill.erase(it);
	}
}

bool CFighterDictator::IsSkillActive(const string& strSkillName,uint8 uSkillLevel)const
{
	MapActiveSkill::const_iterator it=m_mapActiveSkill.find(strSkillName);
	if (it!=m_mapActiveSkill.end())
	{
		return	true;
	}
	return	false;
}

uint32 CFighterDictator::GetActiveSkillLevel(const string& strSkillName)const
{
	MapActiveSkill::const_iterator it=m_mapActiveSkill.find(strSkillName);
	if (it!=m_mapActiveSkill.end())
	{
		return it->second;
	}
	return 0;
}

void CFighterDictator::ClearAllSkill()
{
	m_mapActiveSkill.clear();
}

CFighterDictator* CFighterDictator::GetTotem()const
{
	CCharacterDictator* pCharacter = GetCharacter();
	if (!pCharacter || !pCharacter->GetServantByType(ENpcType_Totem))
	{
		return NULL;
	}
	return pCharacter->GetServantByType(ENpcType_Totem)->GetFighter();
}

CFighterDictator* CFighterDictator::GetPet()const	
{
	CCharacterDictator* pCharacter = GetCharacter();
	if (!pCharacter)
	{
		return NULL;
	}
	CCharacterDictator* pPetChar = pCharacter->GetServantByType(ENpcType_Summon);
	if (!pPetChar)
	{
		pPetChar = pCharacter->GetServantByType(ENpcType_BattleHorse);
		if(!pPetChar)
			return NULL;
	}
	return pPetChar->GetFighter();
}

bool CFighterDictator::SendToGac(const void* pData,uint32 uSize, uint32 uRange) const
{
	if(uRange == eOnlyToDirector)
		return false;

	GetIS(uRange)->Send(pData,uSize);

	CCharacterDictator* pCharacter = GetCharacter();
	CCharacterDictator* pCharMaster ;
	pCharMaster= pCharacter ? pCharacter->GetMaster() : NULL;
	if(pCharMaster)
	{
		CCharacterMediator* pCharMasterMediator = pCharMaster->CastToCharacterMediator();
		//这里有疑问，为何要加pCharacter->IsBattleHorse()的限制，
		//如果this是普通召唤兽要不要考虑pCharMaster->GetConnection()->Send(pData,uSize);
		if(pCharMasterMediator && !pCharMasterMediator->DirectorMovingIsDisabled() && pCharacter->IsBattleHorse())
		{
			CConnServer* pConn=pCharMaster->GetConnection();
			if( pConn )
				pConn->Send(pData,uSize);
		}
	}
	return true;
}

/**********************************RPC**************************************/
void CFighterDictator::OnDeadToQuest(CFighterDictator* pAttacker, const TCHAR* szDeadBySkillName)const
{
	CCharacterDictator* pCharacter = GetCharacter();
	if(!pCharacter)
		return;

	if(pCharacter->GetGameEntityType()==eGET_CharacterDictator)
	{
		// 战斗坐骑死亡需要清除玩家的坐骑状态
		CCharacterDictator* pMaster = pCharacter->GetMaster();
		if (pMaster)
		{
			CAllStateMgrServer* pStateMgr= pMaster->GetFighter()->GetAllStateMgr();
			if (pMaster->CppGetCtrlState(eFCS_InBattleHorse) && (int32)pCharacter->GetNpcType() == (int32)ENpcAIType_BattleHorse)
			{
				pStateMgr->EraseState(eDST_Riding);
			}
			pMaster->RemoveServantFromServantMgr(pCharacter->GetEntityID());	//把自己从老大的召唤兽管理器列表中删除
			//这个要在ClearAllNonpersistentState()之前,否则触发器事件不会被处理,状态已经被删除了
		}
	}
	if(!const_cast<CFighterDictator*>(this)->CastToFighterMediator())
	{	
		//杀死召唤兽不用回调
		if (pCharacter->GetNpcType() == ENpcType_Summon || pCharacter->GetNpcType() == ENpcType_NotCtrlSummon)
			return;
		// 当玩家的召唤坐骑杀死NPC时，算到玩家身上
		uint32 KillerId = 0;
		if(pAttacker)
		{
			CCharacterDictator* pAttackCharacter= pAttacker->GetCharacter();
			if (pAttackCharacter)
			{
				if (pAttackCharacter->GetMaster() && pAttacker != this)
				{
					KillerId = pAttackCharacter->GetMaster()->GetEntityID();
				} 
				else
				{
					KillerId = pAttackCharacter->GetEntityID();
				}
			}
		}

		uint32 uExpOwnerID = 0;
		//cout<<"GetCharacter()->GetExpOwnerID():"<<GetCharacter()->GetExpOwnerID()<<endl;
		CCharacterDictator* pCharacter = GetCharacter();
		CCharacterDictator* pExpOwner = CEntityServerManager::GetInst()->GetCharacter(pCharacter->GetExpOwnerID());
		if (pExpOwner)
		{
			if (pExpOwner->GetMaster() && pExpOwner != pCharacter)
			{
				uExpOwnerID = pExpOwner->GetMaster()->GetEntityID();
			} 
			else
			{
				uExpOwnerID = pExpOwner->GetEntityID();
			}
		}

		//这里的pAttacker若为NULL则会爆
		ICharacterDictatorCallbackHandler* pCallBackHandler = GetCallBackHandler();
		if( pCallBackHandler )
			pCallBackHandler->OnDeadToQuest(pCharacter, szDeadBySkillName, KillerId, uExpOwnerID);
	}
}

void CFighterDictator::OnDead(CFighterDictator* pAttacker, const TCHAR* szDeadBySkillName)const
{
	StopMoving();
	CCharacterDictator* pCharacter = GetCharacter();
	if(!pCharacter)
		return;
	pCharacter->ClearAllServantByMasterDie(); //清除所有小弟	
	if(pCharacter->GetGameEntityType()==eGET_CharacterDictator)
	{	
		//这个dying的出现 是为了在死之前通知 npc的AI 一边做些特殊的事情（比如死后自爆之类的）
		uint32 uExpOwnerID = pCharacter->GetExpOwnerID();
		uint32 KillerId = 0;
		if(pAttacker)
		{
			CCharacterDictator* pAttackCharacter= pAttacker->GetCharacter();
			if (pAttackCharacter)
			{
				if (pAttackCharacter->GetMaster() && pAttacker != this)
					KillerId = pAttackCharacter->GetMaster()->GetEntityID();
				else
					KillerId = pAttackCharacter->GetEntityID();
			}
		}
		pCharacter->OnBeginDestory(true, uExpOwnerID, KillerId);
	}
	else
	{
		const_cast<CFighterDictator*>(this)->Notify(eCE_Die, pAttacker);
		pCharacter->SetAndSyncMoveState(eMS_LockStop, true);
		pCharacter->SetAndSyncActionState(eAS_Die);
	}

	bool bToTeamMates = false;
	if(pCharacter->GetGameEntityType()==eGET_CharacterMediator)
		bToTeamMates = true;

	uint32 uAttackerEntityID = pAttacker?pAttacker->GetEntityID():0;
	const_cast<CFighterDictator*>(this)->OnPrintFightInfo(EFII_Die, uAttackerEntityID, GetEntityID(), NULL, 0, eATT_None, 0, 0, bToTeamMates);

	CGas2GacCPP_OnDead Cmd;
	Cmd.uEntityID=GetEntityID();
	SendCmdToGac(&Cmd);
}

void CFighterDictator::OnReborn()const
{
	CCharacterDictator* pCharacter = GetCharacter();
	if( !pCharacter )
		return;

	pCharacter->SetAndSyncMoveState(eMS_Stop, true);
	pCharacter->SetAndSyncActionState(eAS_Idle_BackWpn);

	CGas2GacCPP_OnReborn Cmd;
	Cmd.uEntityID=GetEntityID();
	SendCmdToGac(&Cmd);
}

//属性
void CFighterDictator::OnAgileValueChanged(uint32 uValueTypeID, float fValue) const
{
	if( !m_Handler.Get() )
		return;

	if (uValueTypeID/ePFT_Count == ePID_HealthPoint)
		const_cast<CHealthPointServer*>(&this->m_HealthPoint)->OnRenewAgileValue(this);
	else if (uValueTypeID/ePFT_Count == ePID_ManaPoint)
		const_cast<CManaPointServer*>(&this->m_ManaPoint)->OnRenewAgileValue(this);
	CCharacterDictator* pCharacter = GetCharacter();
	pCharacter->OnAgileValueChanged(uValueTypeID, fValue);

	if (GetEntity()->GetGameEntityType() == eGET_IntObject)
		return;
	switch(uValueTypeID)
	{
	case ePID_HealthPoint*ePFT_Count+ePFT_Adder:
		{
			m_pHealthPointAdderVariant->SetNumber((uint32)fValue);
			break;
		}
	case ePID_HealthPoint*ePFT_Count+ePFT_Multiplier:
		{
			m_pHealthPointMutiplierVariant->SetNumber(fValue);
			break;
		}
	case ePID_HealthPoint*ePFT_Count+ePFT_Agile:
		{
			m_pHealthPointAgileVariant->SetNumber((uint32)fValue);
			break;
		}
	default:
		{
			if ( (uValueTypeID/ePFT_Count != ePID_RagePoint && (CppGetClass() == eCL_Warrior || CppGetClass() == eCL_OrcWarrior)) ||
				(uValueTypeID/ePFT_Count != ePID_EnergyPoint && CppGetClass() == eCL_DwarfPaladin) )
			{
				return;
			}
			uint32 uChangeType = uValueTypeID%ePFT_Count;
			if (uChangeType == ePFT_Adder)
			{
				m_pConsumePointAdderVariant->SetNumber((uint32)fValue);
			}
			else if (uChangeType == ePFT_Multiplier)
			{
				m_pConsumePointMutiplierVariant->SetNumber(fValue);
			}
			else if (uChangeType == ePFT_Agile)
			{
				m_pConsumePointAgileVariant->SetNumber((uint32)fValue);
			}
		}
	}
}

void CFighterDictator::OnChangeAttackType()
{
	if( !m_Handler.Get() )
		return;

	// 攻击类型 会删除,这里临时用DPS
	SetSyncEntity::iterator it = m_setSyncEntity.begin();
	for(;it!=m_setSyncEntity.end();++it)
	{
		CGas2GacCPP_OnAttackTypeChanged Cmd;
		Cmd.uTargetID = GetHandler()->GetEntityID();
		Cmd.uObjID	= *it;
		Cmd.uAttackType = m_pNormalAttackMgr->GetMHNormalAttack()->GetAttackType();
		CCharacterDictator* pCharacterObserver = CEntityServerManager::GetInst()->GetCharacter(Cmd.uObjID);
		if(!pCharacterObserver)
			continue;
		
		CFighterDictator* pObserver = pCharacterObserver->GetFighter();
		pObserver->SendCmdToGac(&Cmd, eOnlyToDirector);
	}
}

void CFighterDictator::OnCalValueChanged(uint32 uValueTypeID, float fValue)const
{
	if( !m_Handler.Get() )
		return;

	if (!const_cast<CFighterDictator*>(this)->CastToFighterMediator())
		return;

	uint32 uEntityID = GetHandler()->GetEntityID();
	uint32 uPropertyID = (uValueTypeID-ePFT_Adder)/ePFT_Count;
	switch(uPropertyID)
	{
	case ePID_MPConsumeRate:
	case ePID_RPConsumeRate:
	case ePID_EPConsumeRate:
	case ePID_NatureMPConsumeRate:
	case ePID_DestructionMPConsumeRate:
	case ePID_EvilMPConsumeRate:
		{
			CGas2GacCPP_OnSyncToSelfCalValueChanged cmd;
			cmd.uObjID = uEntityID;
			cmd.uValueTypeID = uValueTypeID;
			cmd.fValue = fValue;
			this->SendCmdToGac(&cmd, eOnlyToDirector);
			break;
		}
	default:
		break;
	}

	MapSyncCalProperty::const_iterator it_Property =ms_mapSyncCalProperty.find(EPropertyID(uPropertyID));
	if (it_Property==ms_mapSyncCalProperty.end())
	{
		return;
	}
	SetSyncEntity::const_iterator it = m_setSyncEntity.begin();
	for(;it!=m_setSyncEntity.end();++it)
	{
		uint32 uObjID = *it;
		CCharacterDictator* pCharacterObserver = CEntityServerManager::GetInst()->GetCharacter(uObjID);
		if(!pCharacterObserver)
			continue;
		CFighterDictator* pObserver = pCharacterObserver->GetFighter();
		
		EAttackType eAttackType = m_pNormalAttackMgr->GetMHNormalAttack()->GetAttackType();
		CGas2GacCPP_OnCalValueChanged Cmd;
		Cmd.uTargetID = uEntityID;
		Cmd.uObjID	= uObjID;
		Cmd.fValue	= fValue;
		Cmd.uValueTypeID = uValueTypeID;
		Cmd.uAttackType = eAttackType;
		pObserver->SendCmdToGac(&Cmd, eOnlyToDirector);
	}

	CFighterDictator* pTargetFighter = GetTarget();
	if(!pTargetFighter)
		return;

	//如果是抗性或者护甲,则更新碾压比例
	switch(uPropertyID)
	{
	case ePID_NatureResistanceValue:
	case ePID_DestructionResistanceValue:
	case ePID_EvilResistanceValue:
	case ePID_Defence:
	{
		if(pTargetFighter->m_setSyncEntity.count(uEntityID)!=0)//是玩家并且锁定了目标
		{
			CGas2GacCPP_UpdateSmashRate cmd;
			cmd.uObjID = uEntityID;
			cmd.uTargetID = pTargetFighter->GetEntityID();
			EAttackType attackType = pTargetFighter->GetNormalAttackMgr()->GetMHNormalAttack()->GetAttackType();
			cmd.fValue = pTargetFighter->CalcSmashRate(pTargetFighter, this, this, attackType);
			this->SendCmdToGac(&cmd, eOnlyToDirector);
		}
	}
	default:
		break;
	}
}

void CFighterDictator::OnCtrlValueChanged(uint32 uValue, uint32 uValueTypeID, bool bSet)const
{
	if(!m_Handler.Get())
		return;

	GetHandler()->OnCtrlValueChanged(uValue, uValueTypeID, bSet);

	if (eFCS_InWalkState == uValueTypeID)
	{
		if (bSet)
			OnMoveSpeedChange(m_WalkSpeed.Get(this), "跑变走", NULL);
		else
			OnMoveSpeedChange(m_RunSpeed.Get(this), "走变跑", NULL);
	}

	if (GetEntity()->GetGameEntityType() == eGET_IntObject)
		return;
	m_pCtrlValueVariant->SetNumber(uValue);
	m_pCtrlSetVariant->SetNumber(bSet);
	m_pCtrlValueTypeIDVariant->SetNumber(uValueTypeID);
}

void CFighterDictator::SyncToClientTalentChange(const TCHAR* szTalentName, int32 iPoint)const
{
	if( !GetHandler() )
		return;
	string strTalentName(szTalentName);
	CGas2GacCPP_OnTalentChanged	Cmd;
	Cmd.uObjID=GetEntityID();
	Cmd.iPoint=iPoint;
	Cmd.udbTalentNameLen = strTalentName.size();
	SendCmdToGac(&Cmd, eOnlyToDirector);
	SendToGac(strTalentName.c_str(), Cmd.udbTalentNameLen, eOnlyToDirector);
}

void CFighterDictator::SyncToClientTalentRemove(const TCHAR* szTalentName)const
{
	if( !GetHandler() )
		return;
	string strTalentName(szTalentName);
	CGas2GacCPP_OnRemoveTalent	Cmd;
	Cmd.uObjID=GetEntityID();
	Cmd.udbTalentNameLen = strTalentName.size();
	SendCmdToGac(&Cmd, eOnlyToDirector);
	SendToGac(strTalentName.c_str(), Cmd.udbTalentNameLen, eOnlyToDirector);
}

void CFighterDictator::OnLevelChanged(uint32 uLevel) const
{
	if( !GetHandler() )
		return;
	if (GetCharacter()->GetLevelVariant())
	{
		uint8 uSyncLevel = uint8(uLevel);
		GetCharacter()->GetLevelVariant()->SetNumber(uSyncLevel);
		if(uint32(uSyncLevel) != uLevel)
		{
			ostringstream strm;
			strm << uLevel;
			LogErr("Level is great than 255", strm.str());
		}
	}
	const_cast<CFighterDictator*>(this)->Notify(eCE_LevelChanged, NULL);
}

void CFighterDictator::OnClassChanged(EClass eClass) const
{
	if( !GetHandler() )
		return;
	if (GetCharacter()->GetClassVariant())
		GetCharacter()->GetClassVariant()->SetNumber((uint8)eClass);
}

void CFighterDictator::OnCampChanged(ECamp eCamp) const
{
	if( !GetHandler() )
		return;
	if (GetCharacter()->GetCampVariant())
		GetCharacter()->GetCampVariant()->SetNumber((uint8)eCamp);
	//Npc的BirthCamp和Camp是相同的，player的BirthCamp需要单独设置
	if (!const_cast<CFighterDictator*>(this)->CastToFighterMediator())
		GetCharacter()->GetBirthCampVariant()->SetNumber((uint8)eCamp);
}

void CFighterDictator::OnGameCampChange(int32 iGameCamp) const
{
	if( !GetHandler() )
		return;
	if (GetCharacter()->GetGameCampVariant())
		GetCharacter()->GetGameCampVariant()->SetNumber(iGameCamp);
}

void CFighterDictator::OnPlayerBirthCampChange(ECamp eCamp) const
{
	if (!GetHandler())
		return;
	if (const_cast<CFighterDictator*>(this)->CastToFighterMediator() && GetCharacter()->GetBirthCampVariant())
		GetCharacter()->GetBirthCampVariant()->SetNumber((uint8)eCamp);
}

void CFighterDictator::OnPKStateChanged(bool bPKState) const
{
	if( !GetHandler() )
		return;
	if (GetCharacter()->GetPKSwitchTypeVariant())
		GetCharacter()->GetPKSwitchTypeVariant()->SetNumber(bPKState);
}

//void CFighterDictator::OnPKStateChanged(EPKState ePKState) const
//{
//	if( !GetHandler() )
//		return;
//	if (GetCharacter()->GetPKSwitchTypeVariant())
//		GetCharacter()->GetPKSwitchTypeVariant()->SetNumber((uint8)ePKState);
//}
//
//void CFighterDictator::OnZoneTypeChanged(EZoneType eZoneType) const
//{
//	if( !GetHandler() )
//		return;
//	if (GetCharacter()->GetZoneTypeVariant())
//		GetCharacter()->GetZoneTypeVariant()->SetNumber((uint8)eZoneType);
//}

void CFighterDictator::RemoveFacingPair()const
{
	if( !GetHandler() )
		return;
	CGas2GacCPP_OnLostTarget Cmd;
	Cmd.uObjID=GetEntityID();
	SendCmdToGac(&Cmd);
}

void CFighterDictator::ServerRelSoulBullet(uint32 uTargetEntityID, const TCHAR* szBulletName, EBurstSoulType eBurstSoulType, uint32 uCount)
{
	CBulletMagicCfgServerSharedPtr& pCfg = CBulletMagicCfgServer::GetBulletMagic(szBulletName);

	CCharacterDictator* pTargetChar = CCharacterDictator::GetCharacterByID(uTargetEntityID);
	if ( !pTargetChar )
		return;
	CGas2GacCPP_OnBurstSoul Cmd;
	Cmd.uCategoryID = pCfg->GetId();
	Cmd.uFromEntityID = GetEntityID();
	Cmd.uTargetEntityID = uTargetEntityID;
	Cmd.uBurstSoulCount = uCount;
	Cmd.uBurstSoulType = eBurstSoulType;
	SendCmdToGac(&Cmd);
}

void CFighterDictator::OnRealNameChange(ENpcType eType, const TCHAR* szServantShowName)const
{
	if (GetCharacter()->GetRealNameVariant())
	{
		GetCharacter()->GetRealNameVariant()->SetString(szServantShowName);
	}
}

void CFighterDictator::NotifyNPCBeHurt( int32 iHurt)
{
	if( !GetHandler() )
		return;
	if(iHurt<0)
	{
		CCharacterDictator* pCharacter = GetCharacter();
		if(pCharacter)
			pCharacter->OnBeHurt(iHurt);
	}
}

void CFighterDictator::OnMoveSpeedChange(float fSpeed, const char* szSpeedChangeReason, const char* szSkillName)const
{
	//const_cast<CFighterDictator*>(this)->Notify(eCE_MoveSpeedChange);
	CTriggerEvent::MessageEvent(eTST_Special, (CFighterDictator*)this, NULL, eSET_DirectionOrMoveSpeedChange);

	if( !GetHandler() )
		return;
	SetSpeed(fSpeed);
	CGas2GacCPP_OnMoveSpeedChange	Cmd;
	Cmd.uEntityID=GetEntityID();
	Cmd.fSpeed=fSpeed;
	SendCmdToGac(&Cmd);
}

void CFighterDictator::OnAddServant(ENpcType eType, uint32 uServantObjID)
{
	CCharacterDictator* pServant = CCharacterDictator::GetCharacterByID(uServantObjID);
	if (pServant)
	{
		if (pServant->GetMasterIDVariant())
			pServant->GetMasterIDVariant()->SetNumber((uint32)GetEntityID());
	}
}
void CFighterDictator::OnDestroyServant(ENpcType eType, uint32 uServantID)
{
	CGas2GacCPP_OnDestroyServant	Cmd;
	Cmd.uObjID = GetEntityID();
	Cmd.uServantID = uServantID;
	Cmd.uType = uint8(eType);
	SendCmdToGac(&Cmd, eOnlyToDirector);
}

void CFighterDictator::SynToFollowCanBeTakeOver()
{
	CGas2GacCPP_OnCanBeTakeOver Cmd;
	Cmd.uEntityID = GetEntityID();
	Ast(GetIS(eSyncSight));
	GetIS(eSyncSight)->Send(&Cmd, sizeof(CGas2GacCPP_OnCanBeTakeOver));
}

void CFighterDictator::SynToFollowCanNotTakeOver()
{
	CGas2GacCPP_OnCanNotTakeOver Cmd;
	Cmd.uEntityID = GetEntityID();
	Ast(GetIS(eSyncSight));
	GetIS(eSyncSight)->Send(&Cmd, sizeof(CGas2GacCPP_OnCanNotTakeOver));
}

void CFighterDictator::OpenObjPanelByID(uint32 uEntityID, bool bIsCare, bool bIsTargetPanel)
{	//uEntityID对象关注this的计算值变化
	if (bIsCare)
	{
		if (bIsTargetPanel)
		{
			CCharacterDictator* pCharacterObserver = CEntityServerManager::GetInst()->GetCharacter(uEntityID);
			if(!pCharacterObserver || const_cast<CFighterDictator*>(this)->CastToFighterMediator()) //this不能是Mediator,Mediator没有碾压值 
				return;
			CFighterDictator* pObserver = pCharacterObserver->GetFighter();
			CGas2GacCPP_UpdateSmashRate cmdUpdateSmashRate;
			EAttackType eAttackType = m_pNormalAttackMgr->GetMHNormalAttack()->GetAttackType();
			cmdUpdateSmashRate.uObjID = uEntityID;
			cmdUpdateSmashRate.uTargetID = GetEntityID();
			cmdUpdateSmashRate.fValue = CalcSmashRate(this, pObserver, pObserver, eAttackType);
			pObserver->SendCmdToGac(&cmdUpdateSmashRate, eOnlyToDirector);
		}
		else
		{
			CCharacterDictator* pCharacterObserver = CEntityServerManager::GetInst()->GetCharacter(uEntityID);
			if(!pCharacterObserver || !const_cast<CFighterDictator*>(this)->CastToFighterMediator()) //this必须是Mediator,Mediator才有属性面板
				return;
			CFighterDictator* pObserver = pCharacterObserver->GetFighter();
			MapSyncCalProperty::const_iterator it = ms_mapSyncCalProperty.begin();
			for (;it != ms_mapSyncCalProperty.end();++it)
			{
				(*it).second->SyncToClient(pObserver,this,(*it).first);
			}
		}
		m_setSyncEntity.insert(uEntityID);
	}
	else
	{
		m_setSyncEntity.erase(uEntityID);
	}
}

CSkillCoolDownDBDataMgr* CFighterDictator::ReturnSkillCoolDownTime()
{
	m_pSkillCoolDownRet = new CSkillCoolDownDBDataMgr;
	CoolDownInfoMap_t& m_mapSkillCoolDownInfo = m_pCoolDownMgr->GetCoolDownInfoMap();
	const char * szDrugItemSkillName = "";
	uint32 uDrugItemSkillLeftTime = 0;
	for (CoolDownInfoMap_t::iterator it=m_mapSkillCoolDownInfo.begin(); it != m_mapSkillCoolDownInfo.end(); ++it)
	{
		const CNormalSkillServerSharedPtr& pSkill = CNormalSkillServer::GetSkillByName(it->first);	
		if (!pSkill)
		{
			continue;
		}
		ESkillCoolDownType eCoolDownType = pSkill->GetCoolDownType();
		uint32 uLeftTime = m_pCoolDownMgr->GetCoolDownTime(it->first.c_str(), eCoolDownType);
		bool bSwitchEquipSkill = m_pCoolDownMgr->IsSwitchEquipSkill(it->first.c_str());
		if (eCoolDownType == eSCDT_DrugItemSkill)
		{
			if (uLeftTime > uDrugItemSkillLeftTime)
			{
				szDrugItemSkillName = it->first.c_str();
				uDrugItemSkillLeftTime = uLeftTime;
			}
			continue;
		}
		if (uLeftTime >= 10000)
		{
			CSkillCoolDownDBData* pCoolDownData = new CSkillCoolDownDBData(it->first.c_str(), uLeftTime, bSwitchEquipSkill);
			m_pSkillCoolDownRet->GetSkillCoolDownDBDateVec()->PushBack(pCoolDownData);
		}
	}
	if (uDrugItemSkillLeftTime >= 10000)
	{
		CSkillCoolDownDBData* pCoolDownData = new CSkillCoolDownDBData(szDrugItemSkillName, uDrugItemSkillLeftTime, false);
		m_pSkillCoolDownRet->GetSkillCoolDownDBDateVec()->PushBack(pCoolDownData);
	}
	return m_pSkillCoolDownRet;
}

uint32 CFighterDictator::GetSkillCoolDownTimeByName(const TCHAR* szSkillName)
{
	uint32 uLeftTime = 0;
	const CNormalSkillServerSharedPtr& pSkill = CNormalSkillServer::GetSkillByName(szSkillName);	
	if (!pSkill)
	{
		return uLeftTime;
	}
	ESkillCoolDownType eCoolDownType = pSkill->GetCoolDownType();
	uLeftTime = m_pCoolDownMgr->GetCoolDownTime(szSkillName, eCoolDownType);
	return uLeftTime;
}

void CFighterDictator::SaveSkillCoolDownTimeToDBEnd()
{
	if(m_pSkillCoolDownRet)
	{
		delete m_pSkillCoolDownRet;
		m_pSkillCoolDownRet = NULL;
	}
}


void CFighterDictator::CPPInitSkillCoolDownTime(CSkillCoolDownDBDataMgr* pSkillCoolDownDBDataMgr)
{
	SQR_TRY
	{
	CSkillCoolDownDBDataVec* pSkillCoolDownDBDataVec = pSkillCoolDownDBDataMgr->GetSkillCoolDownDBDateVec();
	if ( !pSkillCoolDownDBDataVec->Empty() )
	{
		uint32 uCount = pSkillCoolDownDBDataVec->GetCount();
		for (uint32 i=0; i<uCount; i++)
		{
			const TCHAR* szSkillName = pSkillCoolDownDBDataVec->Get(i)->GetSkillName();
			uint32 uCoolDownLeftTime = pSkillCoolDownDBDataVec->Get(i)->GetLeftTime();
			bool bIsSwitchEquipSkill = pSkillCoolDownDBDataVec->Get(i)->GetIsSwitchEquipSkill();
			MapActiveSkill::iterator it=m_mapActiveSkill.find(szSkillName);
			uint32 uSkillLevel = 1;
			if (it!=m_mapActiveSkill.end())
			{
				uSkillLevel = it->second;
			}
			const CNormalSkillServerSharedPtr& pSkill = CNormalSkillServer::GetSkillByName(szSkillName);
			ESkillCoolDownType eCoolDownType = pSkill->GetCoolDownType();
			if (uCoolDownLeftTime>pSkill->GetCoolDownTime()->GetDblValue(this) * 1000 && bIsSwitchEquipSkill ==false
				&& eCoolDownType != eSCDT_DrugItemSkill) //药品技能,公共CD填表的,而且可以不同
			{
				LogErr("冷却时间数据库的值大于配置表了! 可能配置表做了修改", szSkillName);
				return;
			}
			if(eCoolDownType == eSCDT_FightSkill)
			{
				GetCoolDownMgr()->IntNewCoolDown(szSkillName, eCoolDownType, max(uint32(uCoolDownLeftTime), CCoolDownMgrServer::ms_uFightSkillCommonCDTime), bIsSwitchEquipSkill);
			}
			else
			{
				GetCoolDownMgr()->IntNewCoolDown(szSkillName, eCoolDownType, uint32(uCoolDownLeftTime), bIsSwitchEquipSkill);
			}
			m_pSkilllMgr->OnCoolDownBegin(szSkillName, uSkillLevel, uCoolDownLeftTime, bIsSwitchEquipSkill);
		}
	}
}
	SQR_CATCH(exp)
	{
		CConnServer* pConnServer = GetConnection();
		if (pConnServer)
			LogExp(exp, pConnServer);
		else
			LogExp(exp);
	}
	SQR_TRY_END;
}

CAureMagicDBDataMgr* CFighterDictator::SerializeAureMagicToDB()
{
	m_pAureMagicRet = new CAureMagicDBDataMgr();
	CMagicMgrServer::MapAureMagic_t::iterator it =GetMagicMgr()->GetAureMagicMap().begin();
	for(;it!=GetMagicMgr()->GetAureMagicMap().end(); ++it)
	{
		if (it->second->GetCfgSharedPtr()->GetNeedSaveToDB())
		{
			CAureMagicDBData* pAureMagicData = new CAureMagicDBData(
				it->second->GetCfgSharedPtr()->GetName().c_str()
				,it->second->GetSkillInst()->GetSkillLevel()
				,it->second->GetSkillInst()->GetSkillName().c_str()
				,it->second->GetSkillInst()->GetAttackType());
			m_pAureMagicRet->GetAureMagicDBDateVec()->PushBack(pAureMagicData);
		}
	}

	CMagicMgrServer::SetAureMagic_t& setInvisibleAureMagic = GetMagicMgr()->GetAureMagicSet();
	CMagicMgrServer::SetAureMagic_t::iterator itr = setInvisibleAureMagic.begin();
	for (; itr != setInvisibleAureMagic.end(); ++itr)
	{
		if ((*itr)->GetCfgSharedPtr()->GetNeedSaveToDB())
		{
			CAureMagicDBData* pAureMagicData = new CAureMagicDBData(
				(*itr)->GetCfgSharedPtr()->GetName().c_str()
				,(*itr)->GetSkillInst()->GetSkillLevel()
				,(*itr)->GetSkillInst()->GetSkillName().c_str()
				,(*itr)->GetSkillInst()->GetAttackType());
			m_pAureMagicRet->GetAureMagicDBDateVec()->PushBack(pAureMagicData);
		}
	}

	return m_pAureMagicRet;
}

void CFighterDictator::SaveAureMagicToDBEnd()
{
	if(m_pAureMagicRet)
	{
		delete m_pAureMagicRet;
		m_pAureMagicRet = NULL;
	}
}

void CFighterDictator::LoadAureMagicFromDB(const TCHAR* szAureMagicName, uint8 uSkillLevel, const TCHAR* sSkillName, EAttackType eAttackType)
{
	CAureMagicCfgServerSharedPtr& cfg = CAureMagicCfgServer::GetAureMagic(szAureMagicName);
	if ( cfg &&!CAureMagicServer::HasAureMagic(cfg, this) )
	{
		CSkillInstServer* pInst = CSkillInstServer::CreateOrigin(GetEntityID(),sSkillName,uSkillLevel,eAttackType);
		new CAureMagicServer(pInst, cfg, this, this);
		pInst->DestroyOrigin();
	}
}

void CFighterDictator::ResetAllCoolDown()
{
	GetCoolDownMgr()->ResetAllCoolDown();
	GetSkillMgr()->OnAllCoolDownEnd();
}

void CFighterDictator::SyncTarget(uint32 uTargetID)
{
	CGas2GacCPP_OnSetNormalSkillTarget cmd;
	cmd.uObjID = GetEntityID();
	cmd.uTargetID = uTargetID;
	GetIS(eSyncSight)->Send(&cmd, sizeof(CGas2GacCPP_OnSetNormalSkillTarget));//只发给follower
}

// PrintFightInfo non-member help function
void TellClientPrintFightInfo(const CCharacterDictator* pChar, bool bToTeamMates, EFightInfoIndex eFightInfoIndex, uint32 uObj1ID,
						uint32 uObj2ID, bool bSkill, uint32 uCfgId, const TCHAR* szArgName, uint16 uArgLen, int32 iHPChanged,
						EAttackType eAttackType, uint32 uHPExtra, uint32 uRealChangeValue, uint32 uStateId)
{
	time_t T;
	struct tm * timenow;
	time ( &T );
	timenow = localtime ( &T );	
	uint16 uMillisecond = 0;
#ifndef _WIN32
	struct timespec ts;
	clock_gettime(CLOCK_REALTIME,&ts);     
	uMillisecond = ts.tv_nsec/1000000;
#else
	SYSTEMTIME ti;
	GetLocalTime(&ti);
	uMillisecond = ti.wMilliseconds;	
#endif

	CGas2GacCPP_OnPrintFightInfo cmd;
	cmd.uInfoIndex = (uint32)eFightInfoIndex;
	cmd.uObjID = pChar->GetEntityID();
	cmd.uObj1ID = uObj1ID;
	cmd.uObj2ID = uObj2ID;
	cmd.iHPChanged = iHPChanged;
	cmd.uAttackType = uint8(eAttackType); 
	cmd.uHPExtra = uHPExtra;
	cmd.uHPRealChange = uRealChangeValue;
	cmd.uHour = uint8(timenow->tm_hour);
	cmd.uMinute = uint8(timenow->tm_min);
	cmd.uSecond = uint8(timenow->tm_sec);
	cmd.uMillisecond = uMillisecond;
	cmd.uStateId = uStateId;
	cmd.uCfgId	 = uCfgId;
	cmd.bSkill	 = bSkill;
	cmd.uArgNameLen = uArgLen;
	pChar->GetFighter()->SendCmdToGac(&cmd, eOnlyToDirector);
	pChar->GetFighter()->SendToGac(szArgName, uArgLen, eOnlyToDirector);	

	if (bToTeamMates)
	{
		uint32 uTeamID = pChar->GetTeamID();
		if (uTeamID == 0)
			return;
		
		const CTeamServerManager::SetEntityID& setTeamMembers = CTeamServerManager::GetInst()->GetTeamMember(uTeamID, eTT_Team);
		if (setTeamMembers.size() == 0)
			return;
		CTeamServerManager::SetEntityID::const_iterator it = setTeamMembers.begin();
		for(; it != setTeamMembers.end(); ++it)
		{
			CCharacterDictator* pCharTeamMember = CCharacterDictator::GetCharacterByID(*it);
			if(pCharTeamMember && pCharTeamMember != pChar)			
			{
				cmd.uObjID = *it;
				pCharTeamMember->GetFighter()->SendCmdToGac(&cmd, eOnlyToDirector);
				pCharTeamMember->GetFighter()->SendToGac(szArgName, uArgLen, eOnlyToDirector);	
			}
		}
		return;
	}
}

void CFighterDictator::OnPrintFightInfo(EFightInfoIndex eFightInfoIndex, uint32 uObj1ID, uint32 uObj2ID, CSkillInstServer* pSkillInst, int32 iHPChanged, 
										EAttackType eAttackType, uint32 uHPExtra, uint32 uRealChangeValue, bool bToTeamMates, uint32 uStateId, const TCHAR* szArgName,uint16 uArgLen)
{
	if(PrintInfoIsOff())
		return;
	bool bSkill = false;
	uint32 uCfgId = 0;
	if (pSkillInst)
	{
		if (pSkillInst->GetType() == eIT_NormalAttackInst)
		{
			uCfgId = pSkillInst->GetNAInstServer()->GetNACfgId();
		}
		else if (!pSkillInst->GetIsTalent())
		{
			const CNormalSkillServerSharedPtr& pSkill = pSkillInst->GetSkillCfg();
			if (pSkill)
			{
				bSkill = true;
				uCfgId = pSkill.get()->GetId();
			}
		}

		if (pSkillInst->GetTrigger() && pSkillInst->GetCreatorID() != uObj1ID)
		{
			uObj1ID = 0;
		}
		else
		{
			uObj1ID = pSkillInst->GetCreatorID();
		}
	}
	CCharacterDictator* pCharacter1 = NULL;
	CCharacterDictator* pCharacter2 = NULL;
	CEntityServer* pEntity = CEntityServerManager::GetEntityByID(uObj1ID);
	if (pEntity)
	{
		switch(pEntity->GetGameEntityType())
		{
		case eGET_CharacterMediator:
		{
			pCharacter1=class_cast<CCharacterMediator*>(pEntity);
			break;
		}
		case eGET_CharacterDictator:
		{
			CCharacterDictator* pCharacter=class_cast<CCharacterDictator*>(pEntity);
			const CSyncVariantServer* pShowInClientVariant = pCharacter->GetViewShowInClientVariant();
			bool bShowInClient = (bool)pShowInClientVariant->GetNumber<bool>();
			if (!bShowInClient)
				return;
			pCharacter1 = pCharacter?pCharacter->GetMaster():NULL;
			break;
		}
		case eGET_IntObject:
		{
			CIntObjServer* pIntObj=class_cast<CIntObjServer*>(pEntity);
			pCharacter1 = pIntObj?pIntObj->GetOwner():NULL;
			break;
		}
		default:
			break;
		}
	}

	CEntityServer* pEntity2 = CEntityServerManager::GetEntityByID(uObj2ID);
	if (pEntity2)
	{
		switch(pEntity2->GetGameEntityType())
		{
		case eGET_CharacterMediator:
		{
			pCharacter2=class_cast<CCharacterMediator*>(pEntity2);
			break;
		}
		case eGET_CharacterDictator:
		{
			CCharacterDictator* pCharacter=class_cast<CCharacterDictator*>(pEntity2);
			const CSyncVariantServer* pShowInClientVariant = pCharacter->GetViewShowInClientVariant();
			bool bShowInClient = (bool)pShowInClientVariant->GetNumber<bool>();
			if (!bShowInClient)
				return;
			pCharacter2 = pCharacter?pCharacter->GetMaster():NULL;
			break;
		}
		case eGET_IntObject:
		{
			CIntObjServer* pIntObj=class_cast<CIntObjServer*>(pEntity2);
			pCharacter2 = pIntObj?pIntObj->GetOwner():NULL;
			break;
		}
		default:
			break;
		}
	}

	bool bTellTeamMates = CFightStatisticMgr::IsStatisticFightInfoIndex(eFightInfoIndex);
	if (pCharacter1 && pCharacter1->CastToCharacterMediator())
	{
		if ( !pSkillInst || !pSkillInst->GetIsArea() || pSkillInst->BeLastEffect() )
		{
			if (pSkillInst && pSkillInst->GetIsArea())
				uRealChangeValue = pSkillInst->GetEffectHPChangeValue();
			TellClientPrintFightInfo(pCharacter1, bToTeamMates||bTellTeamMates, eFightInfoIndex, uObj1ID, uObj2ID, bSkill, uCfgId,
				szArgName, uArgLen, iHPChanged, eAttackType, uHPExtra, uRealChangeValue, uStateId);
		}
		
	}
	if (pCharacter2 && uObj1ID != uObj2ID && pCharacter1 != pCharacter2 && pCharacter2->CastToCharacterMediator())
	{
		CRelationMgrServer& relationMgr = CRelationMgrServer::Inst();
		bool bTellChar2 = !(pCharacter1 && pCharacter1->CastToCharacterMediator() &&
			relationMgr.InTheSameTeam(pCharacter1->GetFighter(), pCharacter2->GetFighter()) && bTellTeamMates);
		if (bTellChar2)
		{
			TellClientPrintFightInfo(pCharacter2, bToTeamMates, eFightInfoIndex, uObj1ID, uObj2ID, bSkill, uCfgId,
				szArgName, uArgLen, iHPChanged, eAttackType, uHPExtra, uRealChangeValue, uStateId);
		}
	}
}

uint32 GetFighterEntityID(CFighterDictator* pFighter)
{
	if(pFighter)
	{
		return pFighter->GetEntityID();
	}
	else
	{
		return 0;
	}
}

CCoreSceneServer* CFighterDictator::GetScene() const
{
	CEntityServer* pEntity = GetEntity();
	Ast(pEntity);
	return pEntity->GetScene();
}

void CFighterDictator::GetGridPos(CPos& GridPos)const
{
	CEntityServer* pEntity = GetEntity();
	Ast(pEntity); 
	pEntity->GetGridPos(GridPos);
}

const CFPos& CFighterDictator::GetPixelPos()const
{
	CEntityServer* pEntity = GetEntity();
	Ast(pEntity); 
	return pEntity->GetPixelPos();
}

void CFighterDictator::GetPixelPos(CFPos& pos)const
{
	CEntityServer* pEntity = GetEntity();
	Ast(pEntity);  
	pEntity->GetPixelPos(pos);
}

ISend* CFighterDictator::GetIS(uint32 uRange)const
{
	CEntityServer* pEntity = GetEntity();
	Ast(pEntity); 
	return pEntity->GetIS(uRange);
}

bool CFighterDictator::IsMoving()const
{
	CEntityServer* pEntity = GetEntity();
	Ast(pEntity); 
	return pEntity->IsMoving();
}

ESetPosResult CFighterDictator::SetGridPos(const CPos& GridPos)
{
	CEntityServer* pEntity = GetEntity();
	Ast(pEntity); 
	return  pEntity->SetGridPos(GridPos);
}

ESetPosResult CFighterDictator::SetPixelPos(const CFPos& PixelPos)
{
	CEntityServer* pEntity = GetEntity();
	Ast(pEntity);  
	return  pEntity->SetPixelPos(PixelPos);
}

void CFighterDictator::SetSpeed( float fSpeed )const
{
	CEntityServer* pEntity = GetEntity();
	Ast(pEntity); 
	pEntity->SetCoreObjSpeed(fSpeed);
	if (GetCharacter()->GetSpeedVarint())
	{
		GetCharacter()->GetSpeedVarint()->SetNumber((float)fSpeed);
	}
}

void CFighterDictator::GM_SetMoveSpeedPercent(float fPercent)
{
	SetRunSpeed(this, max(0.0f, m_RunSpeed.GetAdder() + fPercent));
	OnMoveSpeedChange(m_RunSpeed.Get(this), "GM命令", NULL);
}

EStopMovingResult CFighterDictator::StopMoving()const
{
	CEntityServer* pEntity = GetEntity();
	Ast(pEntity); 
	return  pEntity->StopMoving();
}

bool CFighterDictator::IsExistMoveMgr()const
{
	CEntityServer* pEntity = GetEntity();
	Ast(pEntity);  
	return  pEntity->IsExistMoveMgr();
}

CConnServer* CFighterDictator::GetConnection() const
{
	CEntityServer* pEntity = GetEntity();
	Ast(pEntity); 
	return pEntity->GetConnection();
}

bool CFighterDictator::DirectorMovingIsDisabled()const
{
	CEntityServer* pEntity = GetEntity();
	Ast(pEntity); 
	return pEntity->DirectorMovingIsDisabled();
}

CCharacterDictator* CFighterDictator::GetCharacter() const
{
	IFighterServerHandler* pHandler = GetHandler();
	Ast(pHandler); 
	return pHandler->GetCharacter();
}

CEntityServer* CFighterDictator::GetEntity() const
{
	IFighterServerHandler* pHandler = GetHandler();
	Ast(pHandler); 
	return pHandler->GetEntity();
}

uint32 CFighterDictator::GetEntityID() const
{
	IFighterServerHandler* pHandler = GetHandler();
	Ast(pHandler); 
	return pHandler->GetEntityID();
}

CTalentHolder* CFighterDictator::GetTalentHolder()const			
{
	return m_pTalentHolder;

}
void CFighterDictator::ChangeTaskAttackImmuneValue(const string& szName, int32 iValue)
{
	CTempVarMgrServer::MapImmuneTaskAttack::iterator itr = GetTempVarMgr()->m_iImmuneTaskAttack.find(szName);
	if(itr != GetTempVarMgr()->m_iImmuneTaskAttack.end())
	{
		itr->second += iValue;
	}
	else
	{
		stringstream sExp;
		sExp << szName << endl;
		GenErr("任务类型伤害变量名不存在于map中", sExp.str());
	}
}

bool CFighterDictator::IsInTaskAttackImmuneType(const string& szName) const
{
	CTempVarMgrServer::MapImmuneTaskAttack::iterator itr = GetTempVarMgr()->m_iImmuneTaskAttack.find(szName);
	if(itr != GetTempVarMgr()->m_iImmuneTaskAttack.end())
	{
		return itr->second > 0;
	}
	else
	{
		stringstream sExp;
		sExp << szName << endl;
		GenErr("任务类型伤害变量名不存在于map中", sExp.str());
		return false;
	}
}

bool CFighterDictator::PrintInfoIsOff() const
{
	return GetTempVarMgr()->m_iPrintInfoOff >= 1;
}

void CFighterDictator::OnSkillBegin()const
{
	IFighterServerHandler* pHandler = GetHandler();
	Ast(pHandler); 
	pHandler->OnSkillBegin();
}

void CFighterDictator::OnSkillEnd(bool bSucceed)const
{
	IFighterServerHandler* pHandler = GetHandler();
	Ast(pHandler); 
	pHandler->OnSkillEnd(bSucceed);
}

bool CFighterDictator::CurCastingProcessCanBeInterrupted()const
{
	return GetSkillMgr()->CurCastingProcessCanBeInterrupted();
}

void CFighterDictator::OnTurnAround()
{
	GetSkillMgr()->InterruptCastingProcessByType(eCIT_TurnAround);
}

float CFighterDictator::GetFighterDist(const CFighterDictator* pFighter)const
{
	Ast(pFighter);
	CEntityServer* pEntitySelf = GetEntity();
	CEntityServer* pEntityTarget = pFighter->GetEntity();
	return pEntitySelf->GetEntityDist(pEntityTarget);
}

float CFighterDictator::GetFighterDistInGrid(const CFighterDictator* pFighter)const
{
	Ast(pFighter);
	CEntityServer* pEntitySelf = GetEntity();
	CEntityServer* pEntityTarget = pFighter->GetEntity();
	return pEntitySelf->GetEntityDistInGrid(pEntityTarget);
}

void CFighterDictator::InitAgileValue()
{
	CSyncVariantServer* pRootViewVariant= GetCharacter()->GetViewVariant(eCTI_Agile);
	CSyncVariantServer* pAgileVariant = pRootViewVariant->SetMember("AgileValue");
	
	(m_pHealthPointAdderVariant = pAgileVariant->SetMember("HealthPointAdder"))
		->SetNumber((uint32)m_HealthPoint.GetAdder());
	
	(m_pHealthPointMutiplierVariant = pAgileVariant->SetMember("HealthPointMutiplier"))
		->SetNumber(m_HealthPoint.GetMultiplier());

	(m_pHealthPointAgileVariant = pAgileVariant->SetMember("HealthPointAgile"))
		->SetNumber(m_HealthPoint.LimitGet());
	
	uint32 uConsumeAdder = 0;
	float uConsumeMulti = 0.0f;
	uint32 uConsumeAgile = 0;
	if (CppGetClass() == eCL_Warrior || CppGetClass() == eCL_OrcWarrior)
	{
		uConsumeAdder = m_RagePoint.GetAdder();
		uConsumeMulti = m_RagePoint.GetMultiplier();
		uConsumeAgile = m_RagePoint.LimitGet();
	}
	else if (CppGetClass() == eCL_DwarfPaladin)
	{
		uConsumeAdder = m_EnergyPoint.GetAdder();
		uConsumeMulti = m_EnergyPoint.GetMultiplier();
		uConsumeAgile = m_EnergyPoint.LimitGet();
	}
	else
	{
		uConsumeAdder = m_ManaPoint.GetAdder();
		uConsumeMulti = m_ManaPoint.GetMultiplier();
		uConsumeAgile = m_ManaPoint.LimitGet();
	}
	(m_pConsumePointAdderVariant = pAgileVariant->SetMember("ConsumePointAdder"))
		->SetNumber(uConsumeAdder);

	(m_pConsumePointMutiplierVariant = pAgileVariant->SetMember("ConsumePointMutiplier"))
		->SetNumber(uConsumeMulti);

	(m_pConsumePointAgileVariant = pAgileVariant->SetMember("ConsumePointAgile"))
		->SetNumber(uConsumeAgile);

	//CtrlValueVariant初始化
	CSyncVariantServer* pCtrlValueVariant = pRootViewVariant->SetMember("CtrlValue");
	(m_pCtrlIsAliveVariant = pCtrlValueVariant->SetMember("IsAlive"))->SetNumber(CppIsAlive());
	(m_pCtrlValueVariant = pCtrlValueVariant->SetMember("Value"))->SetNumber(GetAllCtrlState());
	(m_pCtrlSetVariant = pCtrlValueVariant->SetMember("Set"))->SetNumber(false);
	(m_pCtrlValueTypeIDVariant = pCtrlValueVariant->SetMember("ValueTypeID"))->SetNumber((uint32)0);

	CSyncVariantServer* pRootSyncVariant = GetCharacter()->GetSyncVariant(eCTI_Agile);
	CSyncVariantServer* pCastingProcess = pRootSyncVariant->SetMember("CastingProcess");
	(m_pCastIDVariant = pCastingProcess->SetMember("CastID"))->SetNumber(uint16(0));
	(m_pCastTimeVariant = pCastingProcess->SetMember("CastTime"))->SetNumber(float(0));
	(m_pCastGuessStartTimeVariant = pCastingProcess->SetMember("GuessStartTime"))->SetNumber(int64(0));
	(m_pCastTargetIDVariant = pCastingProcess->SetMember("TargetIDOfCast"))->SetNumber(uint32(0));
	(m_pCastSkillIDVariant = pCastingProcess->SetMember("uSkillID"))->SetNumber(uint16(0));
	(m_pCastIsFinishVariant = pCastingProcess->SetMember("bIsFinish"))->SetNumber(bool(true));
}

void CFighterDictator::ResetCtrlValueVariant()
{
	m_pCtrlIsAliveVariant->SetNumber(CppIsAlive());
	m_pCtrlValueVariant->SetNumber(GetAllCtrlState());
	m_pCtrlSetVariant->SetNumber(false);
	m_pCtrlValueTypeIDVariant->SetNumber((uint32)0);
}

bool CFighterDictator::SetCtrlState(EFighterCtrlState eState, bool bSet, CSkillInstServer* pSkillInst)
{
	bool bRet = false;
	SQR_TRY
	{
		bRet = TPropertyHolder<CFighterDictator>::CppSetCtrlState(eState, bSet);
	}
	SQR_CATCH(exp)
	{
		if(pSkillInst)
		{
			GetAllStateMgr()->CountCtrlState(eState, bSet, pSkillInst->GetStateId(), &exp);

			stringstream sExp;
			sExp << " 在技能[" << pSkillInst->GetSkillName() << "]中";
			exp.AppendMsg(sExp.str().c_str());
		}
		else
		{
			GetAllStateMgr()->CountCtrlState(eState, bSet, 0);
		}
		SQR_THROW(exp);
	}
	SQR_TRY_END;
	if(bRet)
	{
		if(pSkillInst)
		{
			GetAllStateMgr()->CountCtrlState(eState, bSet, pSkillInst->GetStateId());
		}
		else
		{
			GetAllStateMgr()->CountCtrlState(eState, bSet, 0);
		}
	}
	return bRet;
}

bool CFighterDictator::CppSetCtrlState(EFighterCtrlState eState, bool bSet)
{
	bool bRet = false;
	if (bSet && !FilterCtrlState(eState))	// 在bSet为true时，即进入某状态时才判断是否符合规则
		return bRet;
	bRet = TPropertyHolder<CFighterDictator>::CppSetCtrlState(eState, bSet);

	if(bRet)
		GetAllStateMgr()->CountCtrlState(eState, bSet, 0);
	return bRet;
}

bool CFighterDictator::FilterCtrlState(EFighterCtrlState eState)
{
	switch(eState)
	{
	case eFCS_InNormalHorse:
		{
			if (GetCtrlState(eFCS_InBattle) || GetCtrlState(eFCS_OnMission))
				return false;
			if (GetCtrlState(eFCS_InDrawWeaponMode))
				CppSetCtrlState(eFCS_InDrawWeaponMode, false);
		}
		break;
	case eFCS_InBattleHorse:
		{
			if (GetCtrlState(eFCS_OnMission))
				return false;
			if (!GetCtrlState(eFCS_InDrawWeaponMode))
				CppSetCtrlState(eFCS_InDrawWeaponMode, true);
		}
		break;
	case eFCS_FeignDeath:
		break;
	case eFCS_InBattle:
		{
			if (GetCtrlState(eFCS_InWalkState))
				CppSetCtrlState(eFCS_InWalkState, false);
			if (!GetCtrlState(eFCS_OnMission) && !GetCtrlState(eFCS_InDrawWeaponMode))
				CppSetCtrlState(eFCS_InDrawWeaponMode, true);
		}
		break;
	case eFCS_OnMission:
		{
			if (GetCtrlState(eFCS_InNormalHorse) || GetCtrlState(eFCS_InBattleHorse))
				return false;
			if (GetCtrlState(eFCS_InDrawWeaponMode))
				CppSetCtrlState(eFCS_InDrawWeaponMode, false);
		}
		break;
	case eFCS_InDuel:
		if (GetCtrlState(eFCS_InBattle) || GetCtrlState(eFCS_InDuel))
			return false;
		break;
	case eFCS_InWalkState:
		if (GetCtrlState(eFCS_InBattle))
			return false;
		break;
	case eFCS_InDrawWeaponMode:
		if (GetCtrlState(eFCS_InNormalHorse) || GetCtrlState(eFCS_InBattle))
			return false;
		break;
	default:
		break;
	}

	return true;
}

void CFighterDictator::CppSetAlive(bool bAlive)
{
	TFighterCtrlInfo<CountType>::CppSetAlive(bAlive);
	if(!bAlive)
		GetAllStateMgr()->ClearCtrlStateCount();
	m_pCtrlIsAliveVariant->SetNumber(bAlive);
}

void CFighterDictator::SetLevelPress(bool bLevelPress)
{
	m_bLevelPress = bLevelPress;
}

bool CFighterDictator::GetLevelPress()
{
	return m_bLevelPress;
}

void CFighterDictator::OnTransferEnd(CCoreSceneServer* pOldScene,const CFPos& OldPos)
{
	Ast (GetBattleStateMgr());
	GetBattleStateMgr()->LeaveBattleStateByForce();

	this->Notify(eCE_ChangeMapEnd, reinterpret_cast<void*>(pOldScene));
}

void CFighterDictator::ClearAllStateMgr()
{
	if (m_pAllStateMgr)
		m_pAllStateMgr->ClearAll();
}

template<typename Property, Property CFighterCalInfo::* property>
class CSyncPropertyAdderHolder:public CSyncPropertyHolder
{
	void SyncToClient(CFighterDictator* pTargetFighter, CFighterDictator* pOwnerObj,EPropertyID ePropertyID)
	{
		CGas2GacCPP_OnCalValueChanged Cmd;
		Cmd.uTargetID = pOwnerObj->GetEntityID();
		Cmd.uObjID	= pTargetFighter->GetEntityID();
		Cmd.uAttackType = pOwnerObj->GetNormalAttackMgr()->GetMHNormalAttack()->GetAttackType();
		Cmd.fValue	= (float)(pOwnerObj->*property).GetAdder();
		Cmd.uValueTypeID = ePropertyID*ePFT_Count + ePFT_Adder;
		pTargetFighter->SendCmdToGac(&Cmd, eOnlyToDirector);
	}
};

template<typename Property, Property CFighterCalInfo::* property>
class CSyncPropertyAdderMultiplierHolder:public CSyncPropertyHolder
{
	void SyncToClient(CFighterDictator* pTargetFighter, CFighterDictator* pOwnerObj,EPropertyID ePropertyID)
	{
		CGas2GacCPP_OnCalValueChanged AdderCmd;
		AdderCmd.uTargetID = pOwnerObj->GetEntityID();
		AdderCmd.uObjID	= pTargetFighter->GetEntityID();
		AdderCmd.uAttackType = pOwnerObj->GetNormalAttackMgr()->GetMHNormalAttack()->GetAttackType();
		AdderCmd.fValue	=(float) (pOwnerObj->*property).GetAdder();
		AdderCmd.uValueTypeID = ePropertyID*ePFT_Count  + ePFT_Adder;
		pTargetFighter->SendCmdToGac(&AdderCmd, eOnlyToDirector);
		CGas2GacCPP_OnCalValueChanged MultiplierCmd = AdderCmd;
		MultiplierCmd.fValue	= (pOwnerObj->*property).GetMultiplier();
		MultiplierCmd.uValueTypeID = ePropertyID*ePFT_Count  + ePFT_Multiplier;
		pTargetFighter->SendCmdToGac(&MultiplierCmd, eOnlyToDirector);
	}
};

template<typename Property, Property CFighterCalInfo::* property>
class CSyncPropertyMultiplierHolder:public CSyncPropertyHolder
{
	void SyncToClient(CFighterDictator* pTargetFighter, CFighterDictator* pOwnerObj,EPropertyID ePropertyID)
	{
		CGas2GacCPP_OnCalValueChanged Cmd;
		Cmd.uTargetID = pOwnerObj->GetEntityID();
		Cmd.uObjID	= pTargetFighter->GetEntityID();
		Cmd.uAttackType = pOwnerObj->GetNormalAttackMgr()->GetMHNormalAttack()->GetAttackType();
		Cmd.fValue	= (pOwnerObj->*property).GetMultiplier();
		Cmd.uValueTypeID = ePropertyID*ePFT_Count  + ePFT_Multiplier;
		pTargetFighter->SendCmdToGac(&Cmd, eOnlyToDirector);
	}
};




void CFighterDictator::BuildCalPropertyTable()
{
	ms_mapSyncCalProperty.insert(make_pair(ePID_StrikeMultiValue,new CSyncPropertyAdderMultiplierHolder<CStrikeMultiValue,&CFighterDictator::m_StrikeMultiValue>()));
	ms_mapSyncCalProperty.insert(make_pair(ePID_StrikeValue,new CSyncPropertyAdderMultiplierHolder<CStrikeValue,&CFighterDictator::m_StrikeValue>()));
	ms_mapSyncCalProperty.insert(make_pair(ePID_Defence,new CSyncPropertyAdderMultiplierHolder<CDefence,&CFighterDictator::m_Defence>()));
	ms_mapSyncCalProperty.insert(make_pair(ePID_BlockRate,new CSyncPropertyMultiplierHolder<CBlockRate,&CFighterDictator::m_BlockRate>()));
	ms_mapSyncCalProperty.insert(make_pair(ePID_BlockDamage,new CSyncPropertyAdderMultiplierHolder<CBlockDamage,&CFighterDictator::m_BlockDamage>()));
	ms_mapSyncCalProperty.insert(make_pair(ePID_PhysicalDodgeValue,new CSyncPropertyAdderMultiplierHolder<CPhysicalDodgeValue,&CFighterDictator::m_PhysicalDodgeValue>()));
	ms_mapSyncCalProperty.insert(make_pair(ePID_MissRate,new CSyncPropertyMultiplierHolder<CMissRate,&CFighterDictator::m_MissRate>()));
	ms_mapSyncCalProperty.insert(make_pair(ePID_ParryValue,new CSyncPropertyAdderMultiplierHolder<CParryValue,&CFighterDictator::m_ParryValue>()));
	ms_mapSyncCalProperty.insert(make_pair(ePID_ResilienceValue,new CSyncPropertyAdderMultiplierHolder<CResilienceValue,&CFighterDictator::m_ResilienceValue>()));
	ms_mapSyncCalProperty.insert(make_pair(ePID_StrikeResistanceValue,new CSyncPropertyAdderMultiplierHolder<CStrikeResistanceValue,&CFighterDictator::m_StrikeResistanceValue>()));
	ms_mapSyncCalProperty.insert(make_pair(ePID_AccuratenessValue,new CSyncPropertyAdderMultiplierHolder<CAccuratenessValue,&CFighterDictator::m_AccuratenessValue>()));
	ms_mapSyncCalProperty.insert(make_pair(ePID_PhysicalDPS,new CSyncPropertyAdderMultiplierHolder<CPhysicalDPS,&CFighterDictator::m_PhysicalDPS>()));
	ms_mapSyncCalProperty.insert(make_pair(ePID_PunctureDamage,new CSyncPropertyAdderMultiplierHolder<CPunctureDamage,&CFighterDictator::m_PunctureDamage>()));
	ms_mapSyncCalProperty.insert(make_pair(ePID_ChopDamage,new CSyncPropertyAdderMultiplierHolder<CChopDamage,&CFighterDictator::m_ChopDamage>()));
	ms_mapSyncCalProperty.insert(make_pair(ePID_BluntDamage,new CSyncPropertyAdderMultiplierHolder<CBluntDamage,&CFighterDictator::m_BluntDamage>()));
	ms_mapSyncCalProperty.insert(make_pair(ePID_MagicDamageValue,new CSyncPropertyAdderMultiplierHolder<CMagicDamageValue,&CFighterDictator::m_MagicDamageValue>()));
	ms_mapSyncCalProperty.insert(make_pair(ePID_NatureDamageValue,new CSyncPropertyAdderMultiplierHolder<CNatureDamageValue,&CFighterDictator::m_NatureDamageValue>()));
	ms_mapSyncCalProperty.insert(make_pair(ePID_DestructionDamageValue,new CSyncPropertyAdderMultiplierHolder<CDestructionDamageValue,&CFighterDictator::m_DestructionDamageValue>()));
	ms_mapSyncCalProperty.insert(make_pair(ePID_EvilDamageValue,new CSyncPropertyAdderMultiplierHolder<CEvilDamageValue,&CFighterDictator::m_EvilDamageValue>()));
	ms_mapSyncCalProperty.insert(make_pair(ePID_MagicDodgeValue,new CSyncPropertyAdderMultiplierHolder<CMagicDodgeValue,&CFighterDictator::m_MagicDodgeValue>()));
	ms_mapSyncCalProperty.insert(make_pair(ePID_MagicHitValue,new CSyncPropertyAdderMultiplierHolder<CMagicHitValue,&CFighterDictator::m_MagicHitValue>()));
	ms_mapSyncCalProperty.insert(make_pair(ePID_NatureResistanceValue,new CSyncPropertyAdderMultiplierHolder<CNatureResistanceValue,&CFighterDictator::m_NatureResistanceValue>()));
	ms_mapSyncCalProperty.insert(make_pair(ePID_DestructionResistanceValue,new CSyncPropertyAdderMultiplierHolder<CDestructionResistanceValue,&CFighterDictator::m_DestructionResistanceValue>()));
	ms_mapSyncCalProperty.insert(make_pair(ePID_EvilResistanceValue,new CSyncPropertyAdderMultiplierHolder<CEvilResistanceValue,&CFighterDictator::m_EvilResistanceValue>()));
	ms_mapSyncCalProperty.insert(make_pair(ePID_NatureDecreaseResistanceValue,new CSyncPropertyAdderMultiplierHolder<CNatureDecreaseResistanceValue,&CFighterDictator::m_NatureDecreaseResistanceValue>()));
	ms_mapSyncCalProperty.insert(make_pair(ePID_DestructionDecreaseResistanceValue,new CSyncPropertyAdderMultiplierHolder<CDestructionDecreaseResistanceValue,&CFighterDictator::m_DestructionDecreaseResistanceValue>()));
	ms_mapSyncCalProperty.insert(make_pair(ePID_EvilDecreaseResistanceValue,new CSyncPropertyAdderMultiplierHolder<CEvilDecreaseResistanceValue,&CFighterDictator::m_EvilDecreaseResistanceValue>()));
	ms_mapSyncCalProperty.insert(make_pair(ePID_DefencePenetrationValue,new CSyncPropertyAdderMultiplierHolder<CDefencePenetrationValue,&CFighterDictator::m_DefencePenetrationValue>()));
	ms_mapSyncCalProperty.insert(make_pair(ePID_MainHandMinWeaponDamage,new CSyncPropertyAdderMultiplierHolder<CMainHandMinWeaponDamage,&CFighterDictator::m_MainHandMinWeaponDamage>()));
	ms_mapSyncCalProperty.insert(make_pair(ePID_MainHandMaxWeaponDamage,new CSyncPropertyAdderMultiplierHolder<CMainHandMaxWeaponDamage,&CFighterDictator::m_MainHandMaxWeaponDamage>()));
	ms_mapSyncCalProperty.insert(make_pair(ePID_MHWeaponIntervalExtraDamageRate,new CSyncPropertyAdderMultiplierHolder<CMHWeaponIntervalExtraDamageRate,&CFighterDictator::m_MHWeaponIntervalExtraDamageRate>()));
	ms_mapSyncCalProperty.insert(make_pair(ePID_AHWeaponIntervalExtraDamageRate,new CSyncPropertyAdderMultiplierHolder<CAHWeaponIntervalExtraDamageRate,&CFighterDictator::m_AHWeaponIntervalExtraDamageRate>()));
	ms_mapSyncCalProperty.insert(make_pair(ePID_PenetrationValue,new CSyncPropertyAdderMultiplierHolder<CPenetrationValue,&CFighterDictator::m_PenetrationValue>()));
	ms_mapSyncCalProperty.insert(make_pair(ePID_ProtectionValue,new CSyncPropertyAdderMultiplierHolder<CProtectionValue,&CFighterDictator::m_ProtectionValue>()));
	ms_mapSyncCalProperty.insert(make_pair(ePID_ExtraStrikeRate,new CSyncPropertyMultiplierHolder<CExtraStrikeRate,&CFighterDictator::m_ExtraStrikeRate>()));
	ms_mapSyncCalProperty.insert(make_pair(ePID_ExtraPhysicDodgeRate,new CSyncPropertyMultiplierHolder<CExtraPhysicDodgeRate,&CFighterDictator::m_ExtraPhysicDodgeRate>()));
	ms_mapSyncCalProperty.insert(make_pair(ePID_ExtraParryRate,new CSyncPropertyMultiplierHolder<CExtraParryRate,&CFighterDictator::m_ExtraParryRate>()));
	ms_mapSyncCalProperty.insert(make_pair(ePID_ExtraCompleteResistanceRate,new CSyncPropertyMultiplierHolder<CExtraCompleteResistanceRate,&CFighterDictator::m_ExtraCompleteResistanceRate>()));
	ms_mapSyncCalProperty.insert(make_pair(ePID_ExtraPhysicDefenceRate,new CSyncPropertyMultiplierHolder<CExtraPhysicDefenceRate,&CFighterDictator::m_ExtraPhysicDefenceRate>()));
	ms_mapSyncCalProperty.insert(make_pair(ePID_ExtraNatureResistanceRate,new CSyncPropertyMultiplierHolder<CExtraNatureResistanceRate,&CFighterDictator::m_ExtraNatureResistanceRate>()));
	ms_mapSyncCalProperty.insert(make_pair(ePID_ExtraDestructionResistanceRate,new CSyncPropertyMultiplierHolder<CExtraDestructionResistanceRate,&CFighterDictator::m_ExtraDestructionResistanceRate>()));
	ms_mapSyncCalProperty.insert(make_pair(ePID_ExtraEvilResistanceRate,new CSyncPropertyMultiplierHolder<CExtraEvilResistanceRate,&CFighterDictator::m_ExtraEvilResistanceRate>()));
	ms_mapSyncCalProperty.insert(make_pair(ePID_ExtraMagicDodgeRate,new CSyncPropertyMultiplierHolder<CExtraMagicDodgeRate,&CFighterDictator::m_ExtraMagicDodgeRate>()));
	ms_mapSyncCalProperty.insert(make_pair(ePID_ExtraMagicResistanceRate,new CSyncPropertyMultiplierHolder<CExtraMagicResistanceRate,&CFighterDictator::m_ExtraMagicResistanceRate>()));
}

void CFighterDictator::ReleaseCalPropertyTable()
{
	MapSyncCalProperty::const_iterator it = ms_mapSyncCalProperty.begin();
	for (;it != ms_mapSyncCalProperty.end();++it)
	{
		delete (*it).second;
	}
	ms_mapSyncCalProperty.clear();
}

void CFighterDictator::RegistDistortedTick(CDistortedTick* pTick, uint32 uCyc)
{
	CEntityServer* pEntity = GetEntity();
	Ast(pEntity);
	return pEntity->RegistDistortedTick(pTick, uCyc);
}

void CFighterDictator::UnRegistDistortedTick(CDistortedTick* pTick)
{
	CEntityServer* pEntity = GetEntity();
	Ast(pEntity);
	return pEntity->UnRegistDistortedTick(pTick);
}

uint64 CFighterDictator::GetDistortedProcessTime()
{
	CEntityServer* pEntity = GetEntity();
	Ast(pEntity);
	return pEntity->GetDistortedProcessTime();
}

uint64 CFighterDictator::GetDistortedFrameTime()
{
	CEntityServer* pEntity = GetEntity();
	Ast(pEntity);
	return pEntity->GetDistortedFrameTime();
}

void CFighterDictator::SetDoSkillCtrlState(EDoSkillCtrlState eSkillCtrlState, bool bSet)
{
	GetSkillMgr()->SetDoSkillCtrlState(eSkillCtrlState, bSet);
	GetHandler()->OnDoSkillCtrlStateChanged(eSkillCtrlState, bSet);
}

bool CFighterDictator::GetDoSkillCtrlState(EDoSkillCtrlState eSkillCtrlState)
{
	return GetSkillMgr()->GetDoSkillCtrlState(eSkillCtrlState);
}

uint32 CFighterDictator::GetDefence()
{
	return m_Defence.Get(this);
}

uint32 CFighterDictator::GetNatureResistance()
{
	return m_NatureResistanceValue.Get(this);
}

uint32 CFighterDictator::GetDestructionResistance()
{
	return m_DestructionResistanceValue.Get(this);
}

uint32 CFighterDictator::GetEvilResistance()
{
	return m_EvilResistanceValue.Get(this);
}

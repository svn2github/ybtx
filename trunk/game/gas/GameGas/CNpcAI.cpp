#include "stdafx.h"
#include "CNpcAI.h"
#include "CNpcStateTransit.h"
#include "CNpcStateMetaData.h"
#include "CNpcStateMachine.h"
#include "CCharacterDictator.h"
#include "CNpcEnmityMgr.h"
#include "CDummyEnmityMgr.h"
#include "CMemberEnmityMgr.h"
#include "ICharacterDictatorCallbackHandler.h"
#include "CFighterDictator.h"
#include "CoreCommon.h"
#include "NormalAttack.h"
#include "CNpcAINatureMgr.h"
#include "CSyncVariantServer.h"
#include "CCoreSceneServer.h"
#include "CRelationMgrServer.h"
#include "Random.h"
#include "NpcInfoMgr.h"
#include "CNpcResServerData.h"
#include "CSkillRuleMgr.h"
#include "CNpcAlertMgr.h"
#include "BaseHelper.h"
#include "CGameConfigServer.h"
#include "TSqrAllocator.inl"
#include "MagicOps_ChangeValue.h"
#include "CDir.h"
#include "CNpcSpecialSkillCfg.h"
#include "ICoreObjectServerHandler.h"
#include "CNpcEnmityTargetFilterData.h"
#include "CPixelPath.h"
#include "CNpcEnmityMember.h"

template class TPtRefer<CNpcAI, CCharacterDictator>;
template class TPtRefer<CNpcAI, CEnmityMgr>;
template class TPtRefer<CNpcAI, CNpcNatureMgr>;
template class TPtRefer<CNpcAI, CSkillRuleMgr>;
template class TPtRefer<CNpcAI, CNpcAlertMgr>;

CNpcAI::CNpcAI( CCharacterDictator* pCharacter, const CNpcServerBaseData *pBaseData, const CNpcStateTransit* pStateTransit, ENpcType eRealNpcType, ENpcAIType eRealAIType, EObjectAoiType eObjAoiType)
	:m_pBaseData(pBaseData)
	,m_pExistTick(NULL)
	,m_pPatrolTick(NULL)
	,m_pSpecialActionTick(NULL)
	,m_pRandomMoveOrActionTick(NULL)
	,m_pSetNpcAoiTick(NULL)
	,m_pNpcAlertDelayAttackTick(NULL)
	,m_uExpOwnerID(0)
	,m_bDefaultWalkState(true)
	,m_uCurrentPoint(0)
	,m_bMoveDir(true)
	,m_isDoingAttack(false)
	,m_isDoingSkill(false)
	,m_eRealAIType(eRealAIType)
	,m_eRealNpcType(eRealNpcType)
	,m_uMoveRange(0)
	,m_eMoveType(ENpcMove_None)
	,m_eObjAoiType(eObjAoiType)
	,m_bLifeTimeEnd(false)
	,m_bDelInBattle(pBaseData->m_pAIBaseDataOne->m_bDelInBattle)
	,m_uGroupSelfID(0)
	,m_fSize(0)
	,m_uPlayNumInSleepDim(0)
	,m_bSleep(pBaseData->m_bSleep)
	,m_bInSleepState(false)
	,m_bAlertState(false)
	,m_bInChaseBackState(false)
	,m_uLasteTime(GetHDProcessTime())
	,m_uCycNum(0)
	,m_bMayDeadCycState(false)
	,m_sErrLog("")
	,m_uBirthDir(0)
	,m_uBirthTime(0)
	,m_uSpecialActionProbability(50)
	,m_bNormalAttackFirstTime(true)
	,m_uOriginObjID(0)
	,m_bTheaterMove(false)
{
 	m_RefsByMsg.SetHolder(this);
	m_CharacterHandler.SetHolder(this);
	SetCharacterHolder(pCharacter);
	pCharacter->SetNpcAIHandler(this);
	m_pEnmityMgrHandler.SetHolder(this);
	m_pSkillRuleMgrHandler.SetHolder(this);
	m_pNpcAlertMgrtHandler.SetHolder(this);
	
	//设置生命周期
	if (GetAIBaseData()->m_fExistTime != 0)
	{
		m_pExistTick = new ExistTick(this);
		GetCharacter()->RegistDistortedTick(m_pExistTick,uint32(GetAIBaseData()->m_fExistTime*1000));
		m_uBirthTime = GetCharacter()->GetDistortedProcessTime();
	}

	InitVariant();
	SetNpcSize();
	const CNpcStateTransit *pNpcStateTransit  = pStateTransit;
	m_lstStateMachineStack.push_back(new CNpcStateMachine(this, pNpcStateTransit));
	const CNpcAIBaseData* pAIBaseData = GetAIBaseData();
	Ast(pAIBaseData);
	SetMoveRange(pAIBaseData->m_uMoveRange);//设置随机移动距离
	CFighterDictator* pFighter = pCharacter->GetFighter();
	Ast(pFighter);
	pFighter->Attach(this, eCE_BeAttacked);
	pFighter->Attach(this, eCE_BeHurtFirstTime);
	pFighter->Attach(this, eCE_BeforeDie);
	SetRunSpeed(pFighter,pAIBaseData->m_uRunSpeed);
	SetWalkSpeed(pFighter,pAIBaseData->m_uWalkSpeed);
	pFighter->SetLevelPress((bool)m_pBaseData->m_pFightBaseData->m_bLevelPress);
	//设置NPC主手武器速度
	const CNpcFightBaseData* pFightBaseData = GetFightBaseData();
	Ast(pFightBaseData);
	m_fAttackScope = m_pBaseData->m_pFightBaseData->m_fAttackScope;
	pFighter->GetNormalAttackMgr()->GetMHNormalAttack()->SetAttackScope(pFightBaseData->m_fAttackScope);

	ENpcEnmityType eEnmityType = NpcInfoMgr::GetEnmityMgrType(m_eRealNpcType);
	switch(eEnmityType)
	{
	case ENpcEnmityType_Nomal:
			new CNpcEnmityMgr(this);
		break;
	case ENpcEnmityType_Dummy:	
			new CDummyEnmityMgr(this);
		break;
	case ENpcEnmityType_Member:
			new CMemberEnmityMgr(this);
		break;
	default:
		break;
	}
	
	SetNpcAoiAndNature(); 

	CCharacterDictator* pCharacterDictator = GetCharacter();
	Ast(pCharacterDictator);
	ICharacterDictatorCallbackHandler* pCharacterDictatorCallbackHandle = pCharacterDictator->GetCallbackHandler();
	Ast(pCharacterDictatorCallbackHandle);
	if (pCharacterDictatorCallbackHandle->HaveNature(pCharacterDictator->GetEntityID()))
	{
		m_pNpcNatureMgrHandler.SetHolder(this);
		CNpcNatureMgr* m_pNatureMgr = new CNpcNatureMgr(this);
		pCharacterDictatorCallbackHandle->InitNatureData(pCharacterDictator->GetEntityID());
		SetPersonalityHandler(m_pNatureMgr);
	}

	if (CGameConfigServer::Inst()->BePreventStateMachineCyc())
	{
		m_ReferByStateMachineCheckValid.SetHolder(this);
		StateMachineCheckValid* pStateMachineChecker = new StateMachineCheckValid(this);
		SetStateMachineCheckValidHolder(pStateMachineChecker);
	}

	if (ExitSkillRuleMgr())
		new CSkillRuleMgr(this);
}

void CNpcAI::InitVariant()
{
	CCharacterDictator* pCharacter = GetCharacter();
	if (pCharacter)
	{
		pCharacter->SetBeShowInClient(m_pBaseData->m_bSynToClient);
		pCharacter->SetNpcType(m_eRealNpcType);
		pCharacter->SetNpcName(m_pBaseData->GetName().c_str());
		pCharacter->SetNpcAIType(GetRealAIType());
		pCharacter->SetAttackType(GetNpcAttackType());
		bool bActive = NpcInfoMgr::BeActiveNpc(GetRealAIType());
		pCharacter->SetActiveState(bActive);
		if (pCharacter->GetMaster())
			pCharacter->CppSetGameCamp(pCharacter->GetMaster()->CppGetGameCamp());
		else
			pCharacter->ReSetGameCamp();
		pCharacter->CppSetPKState(true);
		//pCharacter->CppSetPKState(ePKS_Killer);
		//pCharacter->CppSetZoneType(eZT_WarZone);
		pCharacter->GetAlertActionVariant()->SetNumber((bool)false);
		pCharacter->GetAlertTargetIDVariant()->SetNumber((uint32)0);
	}
}


CNpcAI::~CNpcAI()
{
	ClearAllEnmity();
	SetTarget(NULL);
	CCharacterDictator* pCharacter = GetCharacter();
	Ast(pCharacter);
	CCoreObjectServer* pOriginObj = CCoreObjectServer::GetObjectServer(m_uOriginObjID);
	if(pOriginObj)
		pCharacter->GetScene()->DestroyObject(pOriginObj);
	//关闭AOI
	pCharacter->SetEyeSight(0);
	pCharacter->SetSize(0);
	pCharacter->SetStealth(0);
	pCharacter->SetKeenness(0);

	//删除状态机必须在最前做，因为在各种状态退出的时候可能会调用其他的对象做相应处理
	DeleteNpcAIStateMachine();	

	//解除Fighter对象身上消息监视
	CFighterDictator* pFighter = pCharacter->GetFighter();
	if(pFighter)
	{
		pFighter->Detach(this,eCE_BeAttacked);
		pFighter->Detach(this,eCE_BeHurtFirstTime);
		pFighter->Detach(this,eCE_BeforeDie);
	}

	for (MapMovePath::iterator iter = m_mapMovePath.begin(); iter != m_mapMovePath.end(); iter++)
		SafeDelete((*iter).second);
	
	//注销相应的tick
	if (m_pExistTick)
	{
		GetCharacter()->UnRegistDistortedTick(m_pExistTick);
		SafeDelete(m_pExistTick);
	}
	if (m_pPatrolTick)
	{
		GetCharacter()->UnRegistDistortedTick(m_pPatrolTick);
		SafeDelete(m_pPatrolTick);
	}
	if (m_pSpecialActionTick)
	{
		GetCharacter()->UnRegistDistortedTick(m_pSpecialActionTick);
		SafeDelete(m_pSpecialActionTick);
	}
	if (m_pRandomMoveOrActionTick)
	{
		GetCharacter()->UnRegistDistortedTick(m_pRandomMoveOrActionTick);
		SafeDelete(m_pRandomMoveOrActionTick);
	}
	if (m_pSetNpcAoiTick)
	{
		GetCharacter()->UnRegistDistortedTick(m_pSetNpcAoiTick);
		SafeDelete(m_pSetNpcAoiTick);
	}
	if (m_pNpcAlertDelayAttackTick)
	{
		GetCharacter()->UnRegistDistortedTick(m_pNpcAlertDelayAttackTick);
		SafeDelete(m_pNpcAlertDelayAttackTick);
	}

	//解引用各种管理器
	if (m_pEnmityMgrHandler.Get())
	{
		delete m_pEnmityMgrHandler.Get();
	}
	if (m_pSkillRuleMgrHandler.Get())
	{
		delete m_pSkillRuleMgrHandler.Get();
	}
	if (m_pNpcNatureMgrHandler.Get())
	{
		delete m_pNpcNatureMgrHandler.Get();
	}
	if (m_ReferByStateMachineCheckValid.Get())
	{
		delete m_ReferByStateMachineCheckValid.Get();
	}
	if (m_pNpcAlertMgrtHandler.Get())
	{
		delete m_pNpcAlertMgrtHandler.Get();
	}

	//CCharacterDictator对象解引用AI对象
	GetCharacter()->SetNpcAIHandler(NULL);

	//AI对象解引用CCharacterDictater对象
	SetCharacterHolder(NULL);

	SetNpcEnmityMgrHolder(NULL);
	SetSkillRuleMgrHolder(NULL);
	SetNpcNatureMgrHolder(NULL);
	SetStateMachineCheckValidHolder(NULL);
	SetNpcAlertMgrHolder(NULL);
}

void CNpcAI::RevertNpcBirthDir()
{
	GetCharacter()->SetAndSyncActionDir(m_uBirthDir);
}

void CNpcAI::NpcDoSkill(const CNormalSkillServerSharedPtr* pCfg,CCharacterDictator* pTarget)
{
	GetCharacter()->DoNPCSkill(pCfg, pTarget);
}

void CNpcAI::NpcDoPosSkill(const TCHAR* sName, const CFPos& Pos)
{
	GetCharacter()->DoPosNPCSkill(sName, Pos);
}

void CNpcAI::NpcDoPosSkill(const CNormalSkillServerSharedPtr* pCfg, const CFPos& Pos)
{
	GetCharacter()->DoPosNPCSkill(pCfg, Pos);
}

void CNpcAI::ForceLock(CCharacterDictator* pTarget, uint32 uChaosTime)
{
	Ast(GetEnmityMgr());
	GetEnmityMgr()->ForceLock(pTarget, uChaosTime);
	CNpcEventMsg::Create(this,eNpcEvent_OnBeAttacked);
}

uint32 CNpcAI::GetExpOwnerID()
{
	return m_uExpOwnerID;
}

void CNpcAI::SetExpOwnerID(uint32 uExpOwnerID)
{
	GetCharacter()->GetExpOwnerIDVariant()->SetNumber((uint32)uExpOwnerID);
	m_uExpOwnerID = uExpOwnerID;
}

void CNpcAI::OnAddEnemyToEnmityMgr(CCharacterDictator* pEnemy, bool bHurtEffect)
{
	if (m_pNpcAlertMgrtHandler.Get())
		m_pNpcAlertMgrtHandler.Get()->OnCanceled(false);
	CEnmityMgr* pEnemyMgr = GetEnmityMgr();
	if (pEnemyMgr )
		pEnemyMgr->AddEnemy(pEnemy, bHurtEffect);
}

CCharacterDictator* CNpcAI::GetTargetDictator() const
{
	if (GetTarget())
		return GetTarget()->GetCharacter();
	return NULL;
}

CCharacterDictator* CNpcAI::GetFirstEnmityEnemy()
{
	CEnmityMgr* pOwnEnmity = GetEnmityMgr();
	return pOwnEnmity->GetFirstEnemy();
}

CFighterDictator* CNpcAI::GetFirstEnenmyFighter()
{
	CCharacterDictator* pCharacter = GetFirstEnmityEnemy();
	if(pCharacter)
		return pCharacter->GetFighter();
	else
		return NULL;
}

CFighterDictator* CNpcAI::GetTarget() const
{
	Ast(GetCharacter());
	CFighterDictator* pTarget = GetCharacter()->GetFighter();
	if (pTarget)
		return pTarget->GetTarget();
	return NULL;
}

//必须保证locktarget和target相同
void CNpcAI::SetTarget(CFighterDictator* pTarget, bool bCalNAImmediately)
{
	CCharacterDictator* pCharacter = GetCharacter();
	Ast(pCharacter);
	CFighterDictator* pFighter = pCharacter->GetFighter();
	Ast (pFighter);
	if (!pTarget)
	{
		CCharacterDictator* pCurrent = GetTargetDictator();
		if (pCurrent)
			pCurrent->DelChaser(pCharacter->GetEntityID());
		pFighter->SetTarget(NULL, bCalNAImmediately);
		pFighter->SetLockingTarget(NULL);
		return;
	}
	CFighterDictator* pCurTarget = pFighter->GetTarget();
	CFighterDictator* pCurLockingTarget = pFighter->GetLockingTarget();
	if (pCurTarget != pTarget || pCurLockingTarget != pTarget)
	{
		SetTarget(NULL, bCalNAImmediately);
		pFighter->SetTarget(pTarget);
		pFighter->SetLockingTarget(pTarget);
		if (pTarget->GetCharacter())
			pTarget->GetCharacter()->AddChaser(GetCharacter()->GetEntityID());
	}
}

CPos CNpcAI::GetPosToTarget(const CPos& TargetPos, int32 iDist) const
{
	CPos MyPos;
	GetCharacter()->GetGridPos(MyPos);
	CPos Sub = MyPos - TargetPos;

	int32 iSubMag = Sub.Mag();
	if (iSubMag == 0)
		return TargetPos;

	double rate = iDist / (double) iSubMag;
	Sub.x = (int32)(Sub.x * rate);
	Sub.y = (int32)(Sub.y * rate);

	CPos Result = TargetPos + Sub;
	if (Result.x < 0) Result.x = 0;
	if (Result.y < 0) Result.y = 0;
	return Result;
}

CNpcStateMachine* CNpcAI::GetCurrentStateMachine() const
{
	return m_lstStateMachineStack.back();
}

void CNpcAI::OnCOREvent(CPtCORSubject* pSubject, uint32 uEvent,void* pArg)
{
	CCharacterDictator* pOwnCharacter = GetCharacter();
	Ast(pOwnCharacter);
	if(!pOwnCharacter->CppIsLive())
		return;
	switch(uEvent)
	{
	case eCE_BeAttacked:
		{
			if (!pArg) 
				return;
			CFighterDictator* pFighter = reinterpret_cast<CFighterDictator*>(pArg);		
			if (!pFighter || !pFighter->GetCharacter()) //有可能是intobj对Npc造成伤害
				return;
			OnBeAttacked(pFighter->GetCharacter(), true);	
		}
		break;
	case eCE_BeHurtFirstTime:
		{
			GetCharacter()->GetFighter()->Detach(this, eCE_BeHurtFirstTime);
			if (!GetEnmityMgr()->CanChangeEnemy())
				return;
			CFighterDictator* pFighter = (CFighterDictator*)pArg;
			GetEnmityMgr()->ForceLock(pFighter->GetCharacter());
		}
		break;
	case eCE_BeforeDie:
		{
			if (GetSkillRuleMgr())
			{
				GetSkillRuleMgr()->ExecuteDeadSkill();
			}
		}
		break;
	}
}

bool CNpcAI::IsInLockEnemyRange(CFighterDictator* pTarget)
{
	if (pTarget == NULL)
		return false;
	float fDist = GetCharacter()->GetEntityDist(pTarget->GetEntity());
	if (GetCharacter()->GetScene() != pTarget->GetScene())
		return false;
	if(fDist > float(GetAIBaseData()->m_uLockEnemyDis*eGridSpanForObj))
		return false;
	return true;
}

bool CNpcAI::IsInVisionRange(CFighterDictator* pTarget)
{
	float fDist = GetCharacter()->GetEntityDistInGrid(pTarget->GetEntity());
	if(fDist > GetAIBaseData()->m_uEyeSize)
		return false;
	return true;
}

void CNpcAI::BeginAttack(uint32 uEntityID)
{
	CCharacterDictator* pCharacter = CCharacterDictator::GetCharacterByID(uEntityID);
	if (CanFight(pCharacter))
	{
		if (GetEnmityMgr()->AddEnemy(pCharacter, false))
		{
			//DropAdscriptionJudgement(pCharacter);
			CNpcEventMsg::Create(this,eNpcEvent_OnBeginAttack);
		}
	}
}
bool CNpcAI::IsInAttackRange(CFighterDictator* pTarget)
{
	if (pTarget)
	{
		CFPos PosSelf, PosTarget;
		CCharacterDictator* pCharacterDictator =  GetCharacter();
		Ast(pCharacterDictator);
		Ast(pCharacterDictator->GetFighter());
		float fDistance = pCharacterDictator->GetEntityDist(pTarget->GetEntity());
		float fAttackScope = m_fAttackScope*eGridSpanForObj;
		//cout<<"当前距离： "<<fDistance<<"   攻击距离： "<<fAttackScope<<endl;
		return fDistance < fAttackScope;
	}
	return false;
}

void CNpcAI::ReSetNpcGridAttackScope(float fScope)
{
	m_fAttackScope = fScope;
}

void CNpcAI::RevertNpcAttackScope()
{
	m_fAttackScope = m_pBaseData->m_pFightBaseData->m_fAttackScope;
}

void CNpcAI::SetPersonalityHandler(CNpcNatureMgr* pNatureMgr)
{
	if (pNatureMgr->IsEventListend(eNpcEvent_OnObjectInSight))
		AddEventHandler(eNpcEvent_OnObjectInSight, pNatureMgr);
	
	if (pNatureMgr->IsEventListend(eNpcEvent_OnSleepyIdle))
		AddEventHandler(eNpcEvent_OnSleepyIdle, pNatureMgr);

	if (pNatureMgr->IsEventListend(eNpcEvent_OnOtherDead))
		AddEventHandler(eNpcEvent_OnOtherDead, pNatureMgr);

	if (pNatureMgr->IsEventListend(eNpcEvent_OnNpcInSight))
		AddEventHandler(eNpcEvent_OnNpcInSight, pNatureMgr);
}


void CNpcAI::DelPersonalityHandler(CNpcNatureMgr* pNatureMgr)
{
	for (MapEvent2Handler::iterator it = m_mapEventHandler.begin(); it != m_mapEventHandler.end(); ++it)
	{	
		switch(it->first)
		{
		case eNpcEvent_OnObjectInSight:
		case eNpcEvent_OnBeAttacked:
		case eNpcEvent_OnSuspend:
		case eNpcEvent_OnOtherDead:
		case eNpcEvent_OnNpcInSight:
			RemoveEventHandler(it->first, pNatureMgr);
			break;
		}
	}
}

void CNpcAI::MoveToBirthPos()
{
	MoveToPixel(m_InitPos,false,0);
}

//有生存周期的NPC是不允许改变AI的
bool CNpcAI::CanAIBeChange()
{
	Ast (GetCharacter());
	ENpcAIType eAIType = GetRealAIType();
	EClass eClassType = GetCharacter()->CppGetClass();
	ECamp eCamp = GetCharacter()->CppGetCamp();
	if (!NpcInfoMgr::CanBeChangeAI(eAIType, eClassType, eCamp))
		return false;
	return m_pBaseData->m_bCanBeChangeAI;
}

bool CNpcAI::CanBeRavin()
{
	Ast (GetCharacter());
	return m_pBaseData->m_bCanBeRavin;
}

bool CNpcAI::CanBeSelected()
{
	return m_pBaseData->m_bCanBeSelected;
}

void CNpcAI::SetCharacterHolder(CCharacterDictator* pCharacter)
{
	pCharacter ? m_CharacterHandler.Attach(pCharacter->GetRefByNpcAI()) : m_CharacterHandler.Detach();
}

void CNpcAI::SetNpcEnmityMgrHolder(CEnmityMgr* pEnmityMgr)
{
	pEnmityMgr ? m_pEnmityMgrHandler.Attach(pEnmityMgr->GetRefsByNpcAI()) : m_pEnmityMgrHandler.Detach();
}

void CNpcAI::SetNpcNatureMgrHolder(CNpcNatureMgr* pNpcNatureMgr)
{
	pNpcNatureMgr ? m_pNpcNatureMgrHandler.Attach(pNpcNatureMgr->GetRefByNpcAI()) : m_pNpcNatureMgrHandler.Detach();
}

void CNpcAI::SetSkillRuleMgrHolder(CSkillRuleMgr* pSkillRuleMgr)
{
	pSkillRuleMgr ? m_pSkillRuleMgrHandler.Attach(pSkillRuleMgr->GetRefsByNpcAI()) : m_pSkillRuleMgrHandler.Detach();
}

void CNpcAI::SetNpcAlertMgrHolder(CNpcAlertMgr* pNpcAlertMgr)
{
	pNpcAlertMgr ? m_pNpcAlertMgrtHandler.Attach(pNpcAlertMgr->GetRefsByNpcAI()) : m_pNpcAlertMgrtHandler.Detach();
}

void CNpcAI::SetStateMachineCheckValidHolder(StateMachineCheckValid* pCheckValid)
{
	pCheckValid ? m_ReferByStateMachineCheckValid.Attach(pCheckValid->GetRefByNpcAI()) : m_ReferByStateMachineCheckValid.Detach();
}

void CNpcAI::SetIsDoingSkillRule(bool bState) 
{
	m_isDoingSkill = bState;
}

void CNpcAI::SetNormalAttackFirstTime(bool bSet)
{
	m_bNormalAttackFirstTime = bSet;
}

void CNpcAI::ReSetTarget()
{
	CCharacterDictator* pFirstEnemy = GetFirstEnmityEnemy();
	if(pFirstEnemy)
		SetTarget(pFirstEnemy->GetFighter());
	else
		SetTarget(NULL);
}

void CNpcAI::FaceToEnemy()
{
	CCharacterDictator* pCharacterDictator = GetCharacter();
	Ast(pCharacterDictator);
	CFighterDictator* pFighter = pCharacterDictator->GetFighter();
	Ast(pFighter);
	CActorServer* pActorServer = pCharacterDictator->GetActor();
	Ast(pActorServer);

	if (pFighter && pFighter->GetCtrlState(eFCS_ForbitTurnAround))
	{
		pFighter->RemoveFacingPair();
		return;
	}
	
	if(pCharacterDictator->CppIsLive() && GetFirstEnenmyFighter() && GetFirstEnenmyFighter()->CppIsAlive())
	{
		pCharacterDictator->SetAndSyncActionDirByPos(GetFirstEnenmyFighter()->GetPixelPos());	
	}		
}

void CNpcAI::FaceToTarget(CCharacterDictator* pCharacter)
{
	CCharacterDictator* pCharacterDictator = GetCharacter();
	Ast(pCharacterDictator);
	CFighterDictator* pFighter = pCharacterDictator->GetFighter();
	Ast(pFighter);
	CActorServer* pActorServer = pCharacterDictator->GetActor();
	Ast(pActorServer);

	if (pCharacterDictator == pCharacter)
		return;

	if (pFighter && pFighter->GetCtrlState(eFCS_ForbitTurnAround))
	{
		pFighter->RemoveFacingPair();
		return;
	}
	
	CFPos curPos,targetCurPos;
	CVector2f vecDir;
	if(pCharacterDictator->CppIsLive() && pCharacter)
	{
		pCharacterDictator->GetPixelPos(curPos);
		pCharacter->GetPixelPos(targetCurPos);
		vecDir.x = targetCurPos.x - curPos.x;
		vecDir.y = targetCurPos.y - curPos.y;
		CDir dirCur;
		dirCur.Set(vecDir);
		pCharacterDictator->SetAndSyncActionDir(dirCur.uDir);	
	}
}

void CNpcAI::FaceToPos(CFPos targetCurPos)
{
	CCharacterDictator* pCharacterDictator = GetCharacter();
	Ast(pCharacterDictator);
	CFighterDictator* pFighter = pCharacterDictator->GetFighter();
	Ast(pFighter);
	CActorServer* pActorServer = pCharacterDictator->GetActor();
	Ast(pActorServer);

	if (pFighter && pFighter->GetCtrlState(eFCS_ForbitTurnAround))
	{
		pFighter->RemoveFacingPair();
		return;
	}

	CFPos curPos;
	CVector2f vecDir;
	if(pCharacterDictator->CppIsLive())
	{
		pCharacterDictator->GetPixelPos(curPos);
		if (targetCurPos == curPos)
			return;
		vecDir.x = targetCurPos.x - curPos.x;
		vecDir.y = targetCurPos.y - curPos.y;
		CDir dirCur;
		dirCur.Set(vecDir);
		pCharacterDictator->SetAndSyncActionDir(dirCur.uDir);	
	}
}

float CNpcAI::GetWeaponSpeed()
{
	CCharacterDictator* pCharacter = GetCharacter();
	if (pCharacter)
	{
		CFighterDictator * pFighter = pCharacter->GetFighter();
		if(pFighter)
		{
			float fSpeed = (float)pFighter->m_MainHandWeaponInterval.Get(pFighter);
			return fSpeed;	
		}
			
	}
	return GetFightBaseData()->m_fAttackSpeed;
}

uint32 CNpcAI::GetRandMaxAttackSpeed()
{
	return GetFightBaseData()->m_uRandMaxAttackSpeed;
}

void CNpcAI::SetRealAIType(ENpcAIType eRealAIType)
{
	m_eRealAIType = eRealAIType;	
}

ENpcAIType CNpcAI::GetRealAIType()
{
	return m_eRealAIType;
}

ENpcAIType CNpcAI::GetOldAIType()
{
	return m_pBaseData->m_eAIType;
}

void CNpcAI::SetRealNpcType(ENpcType eRealNpcType)
{
	m_eRealNpcType = eRealNpcType;	
}

ENpcType CNpcAI::GetRealNpcType()
{
	return m_eRealNpcType;
}

const string& CNpcAI::GetNpcName() const
{
	return m_pBaseData->GetName();
}

void CNpcAI::SetMoveType(uint8 uMoveType)
{
	m_eMoveType = (ENpcMoveType) uMoveType;
}

void CNpcAI::ClearDebufferAndSettingImmunity()
{
	CCharacterDictator* pCharacter = GetCharacter();
	if(NULL == pCharacter)
		return;
	NpcDoSkill(CNpcSpecialSkillCfg::ms_vecNpcSpecialSkill[eSSN_EraseDecreaseState], pCharacter);
	NpcDoSkill(CNpcSpecialSkillCfg::ms_vecNpcSpecialSkill[eSSN_SetupImmune], pCharacter);

}
void CNpcAI::ClearImmunity()
{
	CCharacterDictator* pCharacter = GetCharacter();
	if(NULL == pCharacter)
		return;
	NpcDoSkill(CNpcSpecialSkillCfg::ms_vecNpcSpecialSkill[eSSN_CancelImmune], pCharacter);
}

void CNpcAI::SetBornTrigger()
{
	CCharacterDictator* pCharacter = GetCharacter();
	if(!pCharacter)
		return;
	if(GetFightBaseData()->m_uBornTriggerType == 1)
		NpcDoSkill(CNpcSpecialSkillCfg::ms_vecNpcSpecialSkill[eSSN_DieBurstSoul],pCharacter);
	else if(GetFightBaseData()->m_uBornTriggerType == 2)
		NpcDoSkill(CNpcSpecialSkillCfg::ms_vecNpcSpecialSkill[eSSN_NormalDieBurstSoul],pCharacter);
	const LstBornSkillType &lstBornSkill = GetFightBaseData()->GetNpcBornSkill();
	if(!lstBornSkill.empty())
	{
		for (LstBornSkillType::const_iterator iter = lstBornSkill.begin(); iter != lstBornSkill.end();iter++)
		{
			NpcDoSkill(*iter, pCharacter);
		}
	}
}

CCharacterDictator* CNpcAI::GetOneEnemy()
{
	if (m_mapEntity.empty())
		return NULL;
	CCharacterDictator* pCharacter = NULL;
	for (MapEntityIDInNpcSight::iterator iter = m_mapEntity.begin(); iter != m_mapEntity.end(); ++iter)
	{
		if (iter->second == eCOHT_Character)
		{
			pCharacter = CCharacterDictator::GetCharacterByID(iter->first);
			if (CanFight(GetCharacter()))
			{
				return pCharacter;
			}
		}
	}
	return NULL;
}

CCharacterDictator* CNpcAI::GetNearestVisionEnemy()
{
	if(m_mapEntity.empty())
		return NULL;
	CCharacterDictator* pOwnCharacter = GetCharacter();
	Ast(pOwnCharacter);
	CFighterDictator* pOwnFighter = pOwnCharacter->GetFighter();
	Ast(pOwnFighter);
	CCharacterDictator* pTempCharacter = NULL;
	float fVisionDistance = GetAIBaseData()->m_uEyeSize;
	float fTempDistance = fVisionDistance;	
	for (MapEntityIDInNpcSight::iterator iter = m_mapEntity.begin();iter != m_mapEntity.end();)
	{
		if(eCOHT_Character == iter->second)
		{
			CCharacterDictator* pTargetCharacter = CCharacterDictator::GetCharacterByID(iter->first);
			if (CanFight(pTargetCharacter))
			{
				CPos ownerPos,targetPos;
				pOwnCharacter->GetGridPos(ownerPos);
				pTargetCharacter->GetGridPos(targetPos);
				float fDst = ownerPos.Dist(targetPos);
				if(fDst>fVisionDistance)
				{
					//cout<<"删除错误视野内敌人ID== "<<iter->first<<endl;;
					m_mapEntity.erase(iter++);
					continue;
				}
				if(fDst<fTempDistance)
				{
					pTempCharacter = pTargetCharacter;
					fTempDistance = fDst;
				}
			}
		}
		++iter;
	}
	return pTempCharacter;
}

void CNpcAI::ClearNearestVisionEnemy(CCharacterDictator* pEnemy)
{
	if(m_mapEntity.empty())
		return;
	if(!pEnemy)
		return;
	Ast(GetCharacter()->GetFighter());
	uint32 uEnemyID = pEnemy->GetEntityID();
	for (MapEntityIDInNpcSight::iterator iter = m_mapEntity.begin();iter != m_mapEntity.end();)
	{
		if(uEnemyID == iter->first)
		{
			m_mapEntity.erase(iter);
			return;
		}
		++iter;
	}
}

bool CNpcAI::CanDoNormalAttack()
{
	return GetFightBaseData()->m_sNormalAttack != "";
}

void CNpcAI::SetNpcSize()
{
	m_fSize = GetResBaseData()->m_fBaseSize;
}

float CNpcAI::GetNpcSize()
{
	return m_fSize;
}

void CNpcAI::NpcSinglePatrolStateEnd()
{
	if (m_eMoveType == ENpcMove_Single2Random)
	{
		m_eMoveType = ENpcMove_Random;
		if (m_uCurrentPoint != 0 && m_uCurrentPoint <= m_mapMovePath.size())
			m_RandomMoveBeginPos = *m_mapMovePath[m_uCurrentPoint];
	}
	CNpcEventMsg::Create(this,eNpcEvent_OnPatrolStateEnd);
}

uint32 CNpcAI::GetLockEnemyDis()
{
	return GetAIBaseData()->GetLockEnemyDis();
}

void CNpcAI::SetLifeTimeEnd()
{
	m_bLifeTimeEnd = true;
}

bool CNpcAI::BeLifeTimeEnd()
{
	return m_bLifeTimeEnd;
}

bool CNpcAI::BeInBattle()
{
	CFighterDictator* pFighter = GetCharacter()->GetFighter();
	return pFighter->GetCtrlState(eFCS_InBattle); 
}

void CNpcAI::TalkWithPlayerStart(uint32 uEntityID)
{
	if(m_eMoveType == ENpcMove_Cycle || m_eMoveType == ENpcMove_Trip)
		CNpcEventMsg::Create(this,eNpcEvent_OnTaskDialogBegin,eNECI_Zero,reinterpret_cast<void*>(uEntityID));
}
void CNpcAI::TalkWithPlayerEnd()
{
	CNpcEventMsg::Create(this,eNpcEvent_OnTaskDialogAllEnd);
}

uint32 CNpcAI::GetMovePathPointCount()
{
	return m_mapMovePath.size();
}

void CNpcAI::NotifyAccompanierEnterBattle(float fEyeSight, uint32 uNumber, bool bShowEffect)
{
	vector<CCoreObjectServer* > vecObject;
	if (NULL == GetCharacter()->GetScene())
		return;
	uint32 uExitNum = 0;
	CFPos pixelPos = GetCharacter()->GetPixelPos();
	GetCharacter()->GetScene()->QueryObject(vecObject, pixelPos, fEyeSight);
	for(vector<CCoreObjectServer* >::iterator it = vecObject.begin(); it != vecObject.end(); ++it)
	{
		if ( *it == NULL ) 
			break;
		ICoreObjectServerHandler* pHandler = (*it)->GetHandler();
		if ( pHandler == NULL )
			break;
		size_t eType = (size_t)(pHandler->GetTag());
		switch(eType)
		{
		case eCOHT_Character:
			CCharacterDictator *pCharacter = CCharacterDictator::GetCharacterByCoreObj(*it);
			Ast (pCharacter);
			if (!pCharacter->GetNpcAIHandler() || !pCharacter->CppIsLive() || pCharacter == GetCharacter())
				break;
			CNpcAI* pRangeAI = pCharacter->GetNpcAIHandler()->CastToNpcAI();
			if (!pRangeAI->BeInBattle())
			{
				if(pCharacter->CppGetCamp() == GetCharacter()->CppGetCamp() && IsExclaimAlert())
				{
					ShareEnmityWithFriend(pRangeAI);
					if (bShowEffect)
						pCharacter->DoNPCSkill(CNpcSpecialSkillCfg::ms_vecNpcSpecialSkill[eSSN_ByExclaim], pCharacter);
				}
			}
			uExitNum++;
			if (uExitNum == uNumber)
				return;
		}
	}
}

void CNpcAI::ShareEnmityWithFriend(CNpcAI* pFriendAI)
{
	if(!pFriendAI)
		return;
	CEnmityMgr* pOwnEnmity = GetEnmityMgr();
	pOwnEnmity->ShareEnmityWithFriend(pFriendAI);
}

void CNpcAI::DropAdscriptionJudgement(CCharacterDictator* pAttacker)
{
	if (!pAttacker)
		return;
	if( pAttacker->GetNpcAIHandler() && !pAttacker->GetMaster())
		return;
	if(GetExpOwnerID() == 0 && pAttacker->GetEntityID())
	{
		if(!pAttacker->GetNpcAIHandler())
			SetExpOwnerID( pAttacker->GetEntityID());
		else if(pAttacker->GetMaster())
			SetExpOwnerID( pAttacker->GetMaster()->GetEntityID());
	}
}

void CNpcAI::DealExpOwnerOnEnemyLost(CCharacterDictator* pEnemy)
{
	if (pEnemy && pEnemy->GetEntityID() == GetExpOwnerID())
	{
		SetExpOwnerID(0);
		const CEnmityMgr::EnemyListType& lstEnemy = GetEnmityMgr()->GetEnemyList();
		for (CEnmityMgr::EnemyListType::const_iterator iter = lstEnemy.begin(); iter != lstEnemy.end(); iter++)
		{
			uint32 uTargetID = (*iter)->GetEnemyEntityID();
			CCharacterDictator* pEnemyChar = CCharacterDictator::GetCharacterByID(uTargetID);
			if (pEnemyChar && pEnemyChar->GetTeamID() == pEnemy->GetTeamID())
			{
				SetExpOwnerID(uTargetID);
				break;
			}
		}
	}
}

void CNpcAI::NeedToChangeTarget(uint32 uID)
{
	CNpcEventMsg::Create(this,eNpcEvent_OnNeedToChangeTarget,eNECI_Zero,reinterpret_cast<void*>(uID));
}

uint32 CNpcAI::GetNpcLockEnemyDis()
{
	return GetAIBaseData()->m_uLockEnemyDis;
}

uint32 CNpcAI::GetNpcChaseEnemyDis()
{
	return GetAIBaseData()->m_uChaseDistance;
}

float CNpcAI::GetNpcRunSpeed()
{
	return GetAIBaseData()->m_uRunSpeed;
}

void CNpcAI::AddPlayToSleepDim()
{
	if ( !m_bSleep)
		return;
	Ast (m_uPlayNumInSleepDim >= 0);
	m_uPlayNumInSleepDim ++; 
	CNpcEventMsg::Create(this,eNpcEvent_OnNpcSleepEnd);
}

void CNpcAI::DelPlayToSleepDim()
{
	if ( !m_bSleep)
		return;
	Ast (m_uPlayNumInSleepDim >= 1)	//必须大于等于1，不然没法再减了
	m_uPlayNumInSleepDim --;
	if (m_uPlayNumInSleepDim == 0)
	{
		CNpcEventMsg::Create(this,eNpcEvent_OnNpcSleepBegin);
	}
}

void CNpcAI::SetNpcSleepState(bool bSleep)
{
	m_bSleep = bSleep;
}

void CNpcAI::OnEnterWanderState()
{
	if (GetCharacter()->CppIsLive())
	{
		if(GetSkillRuleMgr())
			GetSkillRuleMgr()->InstallWanderSkillRule();
		CNpcEventMsg::Create(this,eNpcEvent_OnEnterWander);
	}
}

void CNpcAI::OnLeaveWanderState()
{
	if (GetSkillRuleMgr())
		GetSkillRuleMgr()->UnInstallWanderSkillRule();
}

void CNpcAI::CheckNpcMoveState(const CFPos& pos, const TCHAR* sMsg)
{
	if (!CGameConfigServer::Inst()->BePreventStateMoveCyc())
		return;
	uint32 uStep = CGameConfigServer::Inst()->GetStateMoveCycCheckStep();
	uint32 uCheckTime = CGameConfigServer::Inst()->GetStateMoveCycTimeLimit();
	bool bMayDeadCyc;	
	uint64 uCurTime = GetHDProcessTime();
	(uCurTime - m_uLasteTime) < uCheckTime ? bMayDeadCyc = true : bMayDeadCyc = false;
	if (m_bMayDeadCycState && bMayDeadCyc)
	{
		m_uCycNum ++;
		if (m_uCycNum > uStep && m_uCycNum <= (uStep + 10))
		{
			ostringstream strm;
			strm<<"调用步骤：【"<<m_uCycNum - uStep<<"】 "<<sMsg<<" 移动目标坐标：【"<<pos.x<<"，"<<pos.y<<"】"<<endl;
			m_sErrLog = m_sErrLog + strm.str();
		}
		if (m_uCycNum > (uStep + 11))
		{
			CFPos ownerpos = GetCharacter()->GetPixelPos();
			ostringstream strm1;
			strm1<<"场景 【"<<GetCharacter()->GetScene()->GetSceneName()<<"】中名为【"<<m_pBaseData->GetName()<<"】的Npc在【"<<GetCurrentStateName()<<"】状态调用移动频率过高!当前坐标是：【"<<ownerpos.x<<"，"<<ownerpos.y<<"】可能已经进入死循环状态！"<<endl;
			LogErr("Npc移动死循环！", (strm1.str() + m_sErrLog).c_str());
			GetCharacter()->SetOnDead();
		}
	}
	else
	{
		m_uCycNum = 0;
		m_sErrLog.clear();
	}
	m_bMayDeadCycState = bMayDeadCyc;
	m_uLasteTime = uCurTime;
}

bool CNpcAI::RegulateNpcPlace()
{
	Ast (GetCharacter());
	CCharacterDictator* pTarget = GetFirstEnmityEnemy();
	if (pTarget)
	{
		CFPos pos;
		GetCharacter()->GetPixelPos(pos);
		pTarget->GetRegulateNpcPlace(GetCharacter()->GetEntityID(), pos);
		CheckNpcMoveState(pos, "【void CNpcAI::RegulateNpcPlace()】函数里面调用【GetRegulateNpcPlace】调整Npc追击位置");
		return MoveToPixelInLinePath(pos, false);
	}
	return false;
}

void CNpcAI::CheckStateMachineValid(CNpcEventMsg* pEvent)
{
	if (m_ReferByStateMachineCheckValid.Get())
	{
		StateMachineCheckValid* pChecker = m_ReferByStateMachineCheckValid.Get();
		if (pChecker)
			pChecker->CheckValid(pEvent);
	}
}

void CNpcAI::OnEnemyDead()
{
	CNpcEventMsg::Create(this,eNpcEvent_OnEnemyDead);
}

void CNpcAI::OnChangeSpeedBySkill()
{
	CNpcEventMsg::Create(this,eNpcEvent_OnSpeedChange);
}

uint32 CNpcAI::GetAlertTime(uint32 uNumber)
{
	return GetAIBaseData()->GetAlertTime(uNumber);
}

bool CNpcAI::BeAlertNpc(uint32 uTargetID)
{
	CCharacterDictator* pTarget = CCharacterDictator::GetCharacterByID(uTargetID);
	if (!pTarget || !pTarget->CppIsLive())
		return false;	
	uint32 uTargetEnemyNum = pTarget->GetChaserTargetNum();
	uint32 uAlertTime = GetAlertTime(uTargetEnemyNum);
	if (uAlertTime != 0)
		return true;
	return false;
}

bool CNpcAI::TargetCanBeAlert(uint32 uTargetID)
{
	CCharacterDictator* pTarget = CCharacterDictator::GetCharacterByID(uTargetID);
	if (!pTarget || !pTarget->CppIsLive())
		return false;	
	if (pTarget->GetNpcType() == ENpcType_Summon || pTarget->GetNpcType() == ENpcType_Shadow)
	{
		return false;
	}
	return true;
}

void CNpcAI::CreateAlertMgr(uint32 uTargetID, bool bNormal, uint32 uAlertTime)
{
	if (TargetCanBeAlert(uTargetID))
	{
		new CNpcAlertMgr(this, uTargetID, bNormal, uAlertTime);	
	}
}

void CNpcAI::DestoryNpcAlertMgr()
{
	if (m_pNpcAlertMgrtHandler.Get())
	{
		delete m_pNpcAlertMgrtHandler.Get();
	}
}

bool CNpcAI::NeedOpenTargetPKState()
{
	return GetFightBaseData()->m_bOpenTargetPKState;
}

uint32 CNpcAI::GetRandMoveTime()
{
	return Random::Rand(GetAIBaseData()->m_uMinRandMoveTime, GetAIBaseData()->m_uMaxRandMoveTime);
}

void CNpcAI::GetNpcBirthPos(CFPos& pos)
{
	pos = m_InitPos;
}

int32 CNpcAI::GetLeftLifeTime()
{
	if (GetAIBaseData()->m_fExistTime != 0)
	{
		uint32 uAliveTime = uint32((GetCharacter()->GetDistortedProcessTime() - GetBirthTime())/1000);
		if (uAliveTime > uint32(GetAIBaseData()->m_fExistTime) )
		{
			return 0;
		}
		return uint32(GetAIBaseData()->m_fExistTime) - uAliveTime;
	}
	return -1;
}

bool CNpcAI::IsInExclaimAlertDist(CCharacterDictator* pEneny)
{
	CFPos ownpos, enenypos;
	GetCharacter()->GetPixelPos(ownpos);
	pEneny->GetPixelPos(enenypos);
	return ownpos.Dist(enenypos) < GetAlertDist()*eGridSpanForObj ? true : false;
}

ENpcTargetType CNpcAI::GetEnmityTargetType() const
{
	if (GetEnmityTargetFilter())
		return GetEnmityTargetFilter()->GetTargetType();
	return ENpcTargetType_AllTarget;	
}

bool CNpcAI::IsEnmityTargetNpc(const string& strNpcName) const
{
	Ast (GetEnmityTargetType() == ENpcTargetType_AppointNpc);
	return GetEnmityTargetFilter()->IsInTargetList(strNpcName);
}

bool CNpcAI::CanFight(CCharacterDictator* pTarget)
{
	CCharacterDictator* pOwnCharacter = GetCharacter();
	if (!pTarget  || pOwnCharacter == pTarget)
		return false;
	if (!GetCharacter()->CppIsLive() || !pTarget->CppIsLive())
		return false;

	bool bTargetBePlyer = !pTarget->GetNpcAIHandler();
	bool bPlayerServant = NpcInfoMgr::BeServantType(pTarget->GetNpcType()) && pTarget->GetMaster() && !pTarget->GetMaster()->GetNpcAIHandler();

	ENpcTargetType eTargetType = GetEnmityTargetType();
	switch(eTargetType)
	{
	case ENpcTargetType_AllPlayer:		//只打玩家和玩家的召唤兽
		{
			if (!bTargetBePlyer && !bPlayerServant)
				return false;
		}
		break;

	case ENpcTargetType_AllNpc:			//只打Npc，不打玩家和玩家的召唤兽
		{
			if (bTargetBePlyer || bPlayerServant)
				return false;
		}
		break;

	case ENpcTargetType_AppointNpc:		//打特定名字的Npc
		{
			if (!IsEnmityTargetNpc(pTarget->GetNpcName()))
				return false;
		}
		break;

	default:
		break;
	}

	CRelationMgrServer& relationMgr = CRelationMgrServer::Inst();
	if(relationMgr.IsEnemy(pOwnCharacter->GetFighter(), pTarget->GetFighter()))
		return true;
	return false;
}

void CNpcAI::CheckAndDoAction()
{
	if ((m_eMoveType == ENpcMove_None && GetCharacter()->ExistSpecialActionAni()) 
		|| m_eMoveType == ENpcMove_Random)
	{
		if (!m_pRandomMoveOrActionTick)
			m_pRandomMoveOrActionTick = new DoActionOrRandomMoveTick(this);
		uint32 uRandomMoveInterval = GetRandMoveTime()*1000;
		GetCharacter()->RegistDistortedTick(m_pRandomMoveOrActionTick, uRandomMoveInterval);
	}
}

void CNpcAI::DestoryActionTick()
{
	if (m_pRandomMoveOrActionTick)
	{
		GetCharacter()->UnRegistDistortedTick(m_pRandomMoveOrActionTick);
		SafeDelete(m_pRandomMoveOrActionTick);
	}
}

bool CNpcAI::AttackTargetHaveHighBarrier()
{
	CCharacterDictator* pCharacter = GetCharacter();
	Ast (pCharacter);
	CFighterDictator* pFighter = pCharacter->GetFighter();
	Ast(pFighter);
	CCharacterDictator* pEnemy = GetFirstEnmityEnemy();
	if (!pEnemy)
	{
		CNpcEventMsg::Create(this,eNpcEvent_OnEnemyLost, eNECI_One);
		return true;
	}
	CFPos selfPos;
	CFPos targetPos;
	pCharacter->GetPixelPos(selfPos);
	pEnemy->GetPixelPos(targetPos);	
	CPixelPath* pPath =pFighter->GetScene()->CreatePath(selfPos, targetPos, eFPT_Line, eBT_MidBarrier);
	if (pPath == NULL && selfPos != targetPos)
	{
		return true;
	}
	SafeRelease(pPath);
	return false;
}

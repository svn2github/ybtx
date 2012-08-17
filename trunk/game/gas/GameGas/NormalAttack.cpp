#include "stdafx.h"
#include "NormalAttack.h"
#include "CNormalAttackCfg.h"
#include "CFighterDictator.h"
#include "CFighterMediator.h"
#include "TimeHelper.h"
#include "CSkillServer.h"
#include "CoreCommon.h"
#include "CRelationMgrServer.h"
#include "PtrlGas2GacCPPDef.h"
#include "CDir.h"
#include "IFighterServerHandler.h"
#include "CCharacterDictator.h"
#include "MagicOps_Damage.h"
#include "CSkillInstServer.h"
#include "CTriggerEvent.h"
#include "ErrLogHelper.h"
#include "CActorServer.h"
#include "CAniKeyFrameCfg.h"
#include "Random.h"
#include "CPixelPath.h"
#include "CCoreSceneServer.h"
#include "BaseHelper.h"
#include "FighterProperty.inl"
#include "TStringRef.inl"

enum EAHAttackDelayTime { eAHAttackDelayTime = 1000 };
//可重用的CDistortedTick类,用于普攻的伤害结算和双持延迟攻击,避免多次new和delete
class CReusableTick
	:public CDistortedTick
	,public CFighterMallocObject
{
protected:
	CReusableTick():m_bUsed(false){}
	virtual ~CReusableTick(){}
	void Use(uint32 uTime, CFighterDictator* pFighter)
	{
		//if(m_bUsed)
		//	return;
		pFighter->RegistDistortedTick(this, uTime);
		m_bUsed = true;
	}
	virtual void Reset()=0;
	void Unuse(CFighterDictator* pFighter)
	{
		if(!m_bUsed)
			return;
		Reset();
		pFighter->UnRegistDistortedTick(this);
		m_bUsed = false;
	}
	bool IsUsed()const { return m_bUsed; }
private:
	bool m_bUsed;
};

class CNormalAttackAniTick
	:public CReusableTick
{
	friend class CNormalSkillServer;
	friend class CSingleHandNATick;
public:
	void Init(CSkillInstServer* pSkillInst, const CNormalAttackCfgSharedPtr& pCfg, CSingleHandNATick* pNATick, uint32 uTime)
	{
		//这里不需要对skillinst进行addref,因为普攻的inst不会删除,并且在每次伤害结算之前都会重新初始化
		m_pSkillInst = pSkillInst;
		m_pCfg = pCfg;
		m_pNATick = pNATick;
		m_bTimeIsUp = false;
		Use(uTime, m_pNATick->GetFighter());
	}
	void	Delete();
	void	OnTick();	//执行一次伤害运算
private:
	CNormalAttackAniTick():m_pSkillInst(NULL),m_pCfg(NULL),m_pNATick(NULL),m_bTimeIsUp(false),m_uRegisterTickTime(0){}
	virtual ~CNormalAttackAniTick() { Delete(); }

	virtual void Reset();

	CSkillInstServer*		m_pSkillInst;
	CNormalAttackCfgSharedPtr		m_pCfg;
	CSingleHandNATick*		m_pNATick;

	bool					m_bTimeIsUp;
	uint64					m_uRegisterTickTime;
};

//由于主副手攻击动作播放时间太接近需要把后面的一次攻击延后
class CDelayedAttack
	:public CReusableTick
{
	friend class CNormalSkillServer;
	friend class CSingleHandNATick;
public:
	virtual void OnTick();
private:
	CDelayedAttack():m_pNATick(NULL){}
	virtual ~CDelayedAttack();

	virtual void Reset();
	void Init(CSingleHandNATick* pNATick, uint32 uDelayedTime);

	CSingleHandNATick*		m_pNATick;
};

void CNormalAttackAniTick::OnTick()
{
	Ast(!m_bTimeIsUp);
	m_bTimeIsUp =  true;
	const CMagicEffServerSharedPtr& pMagicEffServer=m_pCfg->m_pMagicEff->Get(); 
	m_pSkillInst->SetSkillName(m_pCfg->m_strName);

	CSingleHandNATick* pNATick = m_pNATick;
	CFighterDictator* pFrom = pNATick->GetFighter();
	CNormalAttackMgr* pNAMgr = pNATick->m_pMgr;
	float fDistRatio = 0.0f;	//当前距离与普攻距离的比率

	CFighterDictator* pTo = pFrom->GetLockingTarget();

	float fNADist = pNAMgr->GetAttackDistance() * eGridSpanForObj;
	float fCurDist = pFrom->GetFighterDist(pTo);
	fDistRatio = fNADist == 0.0f ? 0.0f : fCurDist / fNADist;
	//cout << "fDistRatio " << fDistRatio << " curdis " << fCurDist << " fNADist " << fNADist << endl;

	if(fDistRatio > 4.0f)	//超出两倍普攻距离
	{
		pNAMgr->Pause(false);	//mediator会把this给删除掉, npc则不会
		//if(pNATick->m_pCurNormalAttack->IsUsed())//正常情况下应调用Delete()
			this->Delete();
		//cout << "2.0" << endl;
		return;
	}
	CFPos pos;
	IFighterServerHandler* pHandler = pFrom->GetHandler();
	//cout << pHandler->GetEntityID() << " 普攻伤害结算" << endl;
	pHandler->GetEntity()->GetPixelPos(pos);
	CFighterMediator* pMedFighter = pFrom->CastToFighterMediator();
	if(pMedFighter)
		pMedFighter->SetFighterPos(pos);

	pMagicEffServer->Do(m_pSkillInst, pFrom, pTo);// 目标如果被打死了, 那么在这会delete this
	CTriggerEvent::MessageEvent(eTST_Trigger, pFrom, pTo, eSET_AttackByEnemyObjectSkill);

	//if(pNATick->m_pCurNormalAttack->IsUsed())//正常情况下应调用Delete()
		this->Delete();
	//else//当目标被打死了, 就不需要再调用Delete()
	//{
	//}

	if(fDistRatio > 1.0f && fDistRatio <= 4.0f)
	{
		//cout << "1.0-2.0" << endl;
		pNAMgr->Pause(false);
	}
}

void CNormalAttackAniTick::Reset()
{
	m_pSkillInst = NULL;
	m_pCfg.reset();
	m_pNATick = NULL;
	m_bTimeIsUp = false;
}

void CNormalAttackAniTick::Delete()
{
	if(m_pNATick)
		Unuse(m_pNATick->GetFighter());	
	//CAppServer::Inst()->UnRegisterTick(this);

	//if(m_pNATick->GetCurNormalAttack() == this)
	//{
	//	m_pNATick->ClearCurNormalAttack();
	//}
	//else
	//{
	//	string str("this指针竟然与自己的mgr中指向自己的指针不一样, 有问题");
	//	CConnServer* pConn = m_pNATick->GetFighter()->GetConnection();
	//	if (pConn)
	//		LogErr(str.c_str(), pConn);
	//	else
	//		LogErr(str.c_str());
	//}

	//delete this;
}

void CDelayedAttack::Init(CSingleHandNATick* pNATick, uint32 uDelayedTime)
{
	Ast(pNATick);
	m_pNATick = pNATick;

	//cout << (m_pNATick->m_bIsMainHand ? "主" : "副") << " 攻击延迟 " << uDelayedTime << " CurTime = " << m_pNATick->GetFighter()->GetDistortedFrameTime() << endl;
	m_pNATick->m_uLastDelayAttackTime = m_pNATick->GetFighter()->GetDistortedFrameTime();
	Use(uDelayedTime, m_pNATick->GetFighter());
	m_pNATick->StopOwnTick();
}

CDelayedAttack::~CDelayedAttack()
{
	if(m_pNATick)
		Unuse(m_pNATick->GetFighter());
}

void CDelayedAttack::Reset()
{
	m_pNATick = NULL;
}

void CDelayedAttack::OnTick()
{
	CSingleHandNATick* pNATick = m_pNATick;
	//Ast(this == pNATick->m_pDelayedAttack);
	//cout << (pNATick->m_bIsMainHand ? "主" : "副") << " 攻击延迟ontick CurTime = " << pNATick->GetFighter()->GetDistortedFrameTime() << endl;
	pNATick->ClearDelayedAttack();
	pNATick->TickOnce();
	pNATick->RestoreOwnTick();
	pNATick->m_uLastDelayTickOnceTime = pNATick->GetFighter()->GetDistortedFrameTime();
	return;
}

CSingleHandNATick::CSingleHandNATick(CFighterDictator* pAttacker, CNormalAttackMgr* pMgr, bool bIsMainHand)
:m_pAttacker(pAttacker)
,m_pCfg(new CNormalAttackCfgSharedPtr)
,m_pCurNormalAttack(NULL)
,m_eAttackType(eATT_None)
,m_uPausetime(0)
,m_NormalAttackLastStartTime(0)
,m_NoneCDTime(0)
,m_uDelayedTime(0)
,m_uNextAttackTime(uint32(-1))
,m_pDelayedAttack(NULL)
,m_pMgr(pMgr)
,m_uLastAniStartTime(0)
,m_bIsMainHand(bIsMainHand)
,m_fAttackScope(0.0f)
{
	m_pSkillInst = CSkillInstServer::CreateNAOrigin();
	m_pCurNormalAttack = new CNormalAttackAniTick;
	m_pDelayedAttack = new CDelayedAttack;
	if(m_bIsMainHand)
	{
		m_pSkillInst->GetNAInstServer()->SetMainHandNA(true);
	}
}

CSingleHandNATick::~CSingleHandNATick()
{
	delete m_pCurNormalAttack;
	delete m_pDelayedAttack;
	m_pSkillInst->DestroyNAOrigin();
	UnRegisterTick();
	SafeDelete(m_pCfg);
}

EAttackType CSingleHandNATick::GetCfgAttackType()const
{
	if(GetCfg())
		return GetCfg()->GetAttackType();

	return eATT_None;
}

bool CSingleHandNATick::HasWeapon()
{
	return !GetCfg() ? false : true;
}

void CSingleHandNATick::UnarmWeapon()
{
	m_pCfg->reset();
}

void CSingleHandNATick::SetAttackScope(float fAttackScope)
{
	m_fAttackScope = fAttackScope;
}

void CSingleHandNATick::SetWeaponInterval(float fWeaponInterval)
{
	if(fWeaponInterval <= 0.0f )//|| fWeaponInterval >= 10.0f)
	{
		string str("设置非法的普攻攻击间隔");
		CConnServer* pConn = GetFighter()->GetConnection();
		if (pConn)
			LogErr(str.c_str(), pConn);
		else
			LogErr(str.c_str());
	}
	else
	{
		if(m_fWeaponInterval > fWeaponInterval)	//提高攻速时为了避免在下次Restart时计算时间差时会与上次的普攻冲突，在这马上进行伤害结算
		{
			if(IsDoingCurNormalAttack())
			{
				GetCurNormalAttack()->OnTick();
			}
		}
		m_fWeaponInterval = fWeaponInterval;
	}
}

void CSingleHandNATick::SetCfg(const TCHAR* szName)
{
	if(m_pAttacker->CastToFighterMediator())
		*m_pCfg = CNormalAttackCfg::GetNormalAttack(szName);
	else
		*m_pCfg = CNormalAttackCfg::GetNPCNormalAttack(szName);
	m_pSkillInst->GetNAInstServer()->SetNACfgId(m_pCfg->get()->GetId());
}

bool CSingleHandNATick::IsDoingCurNormalAttack()
{
	//return m_pCurNormalAttack != NULL && !m_pCurNormalAttack->m_bTimeIsUp;
	return m_pCurNormalAttack->IsUsed() && !m_pCurNormalAttack->m_bTimeIsUp;
}

CNormalAttackAniTick* CSingleHandNATick::GetCurNormalAttack()
{
	return m_pCurNormalAttack;
}

EHurtResult CSingleHandNATick::PreDice()
{
	CFighterDictator* pTarget = GetFighter()->GetLockingTarget();
	EHurtResult eHurtResult = eHR_Hit;

	if(m_eAttackType == eATT_None||m_eAttackType == eATT_Puncture || m_eAttackType == eATT_Chop || m_eAttackType == eATT_BluntTrauma)
	{
		CCalculateAssistantPhyicsHurtMop mop;
		eHurtResult = mop.DicePhysicalHurt(m_pSkillInst, GetFighter(), pTarget);
	}
	else if(m_eAttackType == eATT_Nature || m_eAttackType == eATT_Destroy || m_eAttackType == eATT_Evil)
	{
		CCalNatureMagHurtMop mop;
		eHurtResult = mop.DiceMagicHurt(GetFighter(), pTarget);
	}
	else
	{
		string str("普攻EAttackType类型错误");
		CConnServer* pConn = GetFighter()->GetConnection();
		if (pConn)
			LogErr(str.c_str(), pConn);
		else
			LogErr(str.c_str());
	}
	return eHurtResult;
}

void CSingleHandNATick::ClearDelayedAttack()
{
	if(!m_pDelayedAttack)
		return;
	//CAppServer::Inst()->UnRegisterTick(m_pDelayedAttack);
	//delete m_pDelayedAttack;
	//m_pDelayedAttack = NULL;
	m_pDelayedAttack->Unuse(GetFighter());
	m_uDelayedTime = 0;
}

CNormalAttackCfgSharedPtr& CSingleHandNATick::GetCfg()const
{
	return *m_pCfg;
}

uint8 CSingleHandNATick::DoNormalAttackOnce()
{
	if(!GetCfg())
	{
		UnRegisterTick();
		return 0;
	}
	CFighterDictator* pTarget = GetFighter()->GetLockingTarget();

	if(pTarget == NULL || !CRelationMgrServer::IsEnemy(GetFighter(), pTarget))
	{
		GetFighter()->CancelNormalAttackAutoTracking();
		GetFighter()->CancelAttack();
		return 0;
	}
	// 普通坐骑上攻击触发下马
	CTriggerEvent::MessageEvent(eTST_Trigger, GetFighter(), NULL, eSET_Attack);

	pTarget = GetFighter()->GetLockingTarget();
	if(pTarget == NULL)	//上面的触发器事件有可能把目标打死,导致普攻取消
	{
		GetFighter()->CancelNormalAttackAutoTracking();
		GetFighter()->CancelAttack();
		return 0;
	}

	//if(m_pCurNormalAttack != NULL)
	if(m_pCurNormalAttack->IsUsed())
	{
		float fRealWeaponInterval;
		if(m_bIsMainHand)
		{
			fRealWeaponInterval = GetFighter()->m_MainHandWeaponInterval.Get(GetFighter());
		}
		else
		{
			fRealWeaponInterval = GetFighter()->m_AssistantWeaponInterval.Get(GetFighter());
		}

		uint32 uAniTickLeftTime=m_pCurNormalAttack->GetLeftTime();

		uint64 uCurTime = m_pAttacker->GetDistortedFrameTime();
		stringstream strm;
		strm << "m_pCurNormalAttack != NULL bOnTicked = " << m_pCurNormalAttack->m_bTimeIsUp << " " << m_pCurNormalAttack->m_pCfg->GetName() << " CurTime = " << uCurTime << " TimeExpiredFromLastAttack = " << uCurTime - GetNormalAttackLastStartTime() << " m_fWeaponInterval = " << m_fWeaponInterval << " fRealWeaponInterval = " << fRealWeaponInterval << " SingleHandNATick Interval = " << GetInterval() << " << SingleHandNATick uLastOnTickTime = " << m_uLastOnTickTime << " SingleHandNATick uLastDelayTickOnceTime = " << m_uLastDelayTickOnceTime << " SingleHandNATick uLastDelayAttackTime = " << m_uLastDelayAttackTime << " CurNAAniTick Interval = " << m_pCurNormalAttack->GetInterval() << " AniTick RegisterTime = " << m_pCurNormalAttack->m_uRegisterTickTime << " AniTick LeftTime = " << uAniTickLeftTime << endl;
		CConnServer* pConn = GetFighter()->GetConnection();
		if (pConn)
			LogErr("CSingleHandNATick::DoNormalAttackOnce error !", strm.str().c_str(), pConn);
		else
			LogErr("CSingleHandNATick::DoNormalAttackOnce error !", strm.str().c_str());
		m_pCurNormalAttack->Delete();
	}

	m_pSkillInst->SetCreatorID(GetFighter()->GetEntityID());

	CCharacterDictator* pChar = GetFighter()->GetCharacter();
	if(pChar && pChar->GetNpcSkillLevel() > 0)
	{
		m_pSkillInst->InitSkillLevel(pChar->GetNpcSkillLevel());
	}
	else
	{
		m_pSkillInst->InitSkillLevel(GetFighter()->CppGetLevel());   //普攻等级暂时改成人物等级
	}

	m_pSkillInst->SetAttackType(m_eAttackType);
	m_pSkillInst->SetTargetAliveCheckIsIgnored(false);

	//m_pCurNormalAttack->Init(m_pSkillInst, m_pCfg, this);

	pTarget = GetFighter()->GetLockingTarget();
	if(pTarget == NULL)
	{
		string str("普攻无目标222");
		LogErr(str.c_str());
		GetFighter()->CancelNormalAttackAutoTracking();
		GetFighter()->CancelAttack();
		return 0;
	}

	GetFighter()->Notify(eCE_Attack, reinterpret_cast<void*>(pTarget));

	//播放攻击动作
	return PlayAttackAni();
}

void CSingleHandNATick::TickOnce()
{
	if(m_pMgr->IsTargetInAttackScope()&&GetFighter()->GetLockingTarget())
	{
		//cout << (m_bIsMainHand ? "M" : "A") << " TickOnce" << endl;
		CFPos posDes = GetFighter()->GetLockingTarget()->GetPixelPos();

		DoNormalAttackOnce();

		if( Registered() )
		{
			if(GetInterval() >= m_fWeaponInterval * 1000)
				return;
			UnRegisterTick();
		}

		RegisterTick( uint32(m_fWeaponInterval * 1000) );
	}
	else
	{
		m_pMgr->Pause();
	}
}

void CSingleHandNATick::OnTick()
{
	if(m_pMgr->IsTargetUnreachableInLine(GetFighter()->GetLockingTarget()))
	{
		m_pMgr->Pause();
		return;
	}
	SetNormalAttackLastStartTime();

	float fRealWeaponInterval;
	if(m_bIsMainHand)
	{
		fRealWeaponInterval = GetFighter()->m_MainHandWeaponInterval.Get(GetFighter());
	}
	else
	{
		fRealWeaponInterval = GetFighter()->m_AssistantWeaponInterval.Get(GetFighter());
	}
	if(m_fWeaponInterval != fRealWeaponInterval)
	{
		SetWeaponInterval(fRealWeaponInterval);
		//if(!m_pMgr->IsPaused() && m_pMgr->IsAttacking())
		//{
		UnRegisterTick();
		RegisterTick(uint32(m_fWeaponInterval * 1000));
		//}
	}

	if(m_uDelayedTime > 0)	//攻击被延后
	{
		m_pDelayedAttack->Init(this, m_uDelayedTime);
		//new CDelayedAttack(this, m_uDelayedTime);
		return;
	}

	TickOnce();

	m_uLastOnTickTime = m_pAttacker->GetDistortedFrameTime();
}

bool CSingleHandNATick::NextAttackRequireToBeWaited(uint32& uWaitingTime)const
{
	int32 iLeftTime = 0;
	int32 iDiff = 0;

	iDiff = int32(m_pAttacker->GetDistortedFrameTime() - GetNormalAttackLastStartTime() - m_NoneCDTime);//(bExcludeNoneCDTime ? m_NoneCDTime : 0));

	if(iDiff < 0)
	{
		stringstream strm;
		strm << "iDiff < 0" << " CurTime = " << m_pAttacker->GetDistortedFrameTime() << " LastAttackTime = " << GetNormalAttackLastStartTime() << " m_NoneCDTime = " << m_NoneCDTime;
		strm << " " << GetCfg()->GetName();

		CConnServer* pConn = GetFighter()->GetConnection();
		if (pConn)
			LogErr("CSingleHandNATick::NextAttackRequireToBeWaited", strm.str().c_str(), pConn);
		else
			LogErr("CSingleHandNATick::NextAttackRequireToBeWaited", strm.str().c_str());
	}


	iLeftTime = (int32)(m_fWeaponInterval * 1000) - iDiff;
	//cout << "CalcLeftTime  m_NoneCDTime = " << m_NoneCDTime << "  iDiff = " << iDiff << "  iLeftTime = " << iLeftTime << endl;
	if(iLeftTime > 0)
	{
		uWaitingTime = uint32(iLeftTime);
		return true;
	}
	return false;
}

void CSingleHandNATick::StopOwnTick()
{
	this->UnRegisterTick();
}

void CSingleHandNATick::RestoreOwnTick()
{
	this->RegisterTick( uint32(m_fWeaponInterval * 1000) );
}

void CSingleHandNATick::RegisterTick(uint32 uTime)
{
	if(!m_pMgr->IsAttacking() || m_pMgr->IsPaused())
	{
		return;
	}
	//cout << (m_bIsMainHand ? "M" : "A") << " regist " << uTime << endl;
	m_pAttacker->RegistDistortedTick(this, uTime);
}

void CSingleHandNATick::UnRegisterTick()
{
	//cout << (m_bIsMainHand ? "M" : "A") << " unregist" << endl;
	m_pAttacker->UnRegistDistortedTick(this);
}


bool CSingleHandNATick::IsShortNormalAttack()
{
	if(m_eWeaponType > eWT_None && m_eWeaponType < eWT_Bow)
	{
		return true;
	}

	if(m_eWeaponType > eWT_LongStick && m_eWeaponType < eWT_End)
	{
		return false;
	}

	if (!m_pAttacker->CastToFighterMediator())
	{
		CCharacterDictator* pCharacter = m_pAttacker->GetCharacter();
		if (pCharacter && pCharacter->GetNpcAIHandler() && pCharacter->GetNpcAIHandler()->GetAttackScope() > 3.0f)
			return false;
	}

	return true;
}


void CSingleHandNATick::AddNoneCDTime(uint64 uAccumulatedNoneCDTime)
{
	m_NoneCDTime += uAccumulatedNoneCDTime;
}

void CSingleHandNATick::ClearNoneCDTime()
{
	m_NoneCDTime = 0;	
}

void CSingleHandNATick::SetNormalAttackLastStartTime()
{
	m_NormalAttackLastStartTime = m_pAttacker->GetDistortedFrameTime();// - m_uDelayedTime;
	ClearNoneCDTime();
}

void CSingleHandNATick::SetDelayedTime(uint32 uDelayedTime)
{
	m_uDelayedTime = uDelayedTime;
}

uint8 CSingleHandNATick::PlayAttackAni()
{
	EHurtResult eHurtResult = PreDice();
	EActionState eActionState;
	uint32 uIndex = 0;
	CCharacterDictator* pCharacter=GetFighter()->GetCharacter();
	Ast(pCharacter);

	if(!pCharacter->CastToCharacterMediator() || (HasWeapon() && GetWeaponType() != eWT_None))
	{
		EClass eClass = pCharacter->CppGetClass();
		if(eHurtResult == eHR_Strike && eClass != eCL_NatureElf && eClass != eCL_EvilElf && eClass != eCL_Priest)
		{
			eActionState = eAS_Strike;
			uIndex = eAS_Attack_All;
		}
		else
		{
			eActionState = EActionState(Random::Rand(eAS_Attack_Right, eAS_Attack_All));
			uIndex = eActionState;
		}
	}
	else
	{
		eActionState = eAS_Attack_Fist;
	}

	if(m_bIsMainHand)
	{
		float fInterval = m_pMgr->GetFighter()->m_MainHandWeaponInterval.GetMultiplier();
		//根据副手攻击动作的到来时间计算主手动作播放的速率
		CSingleHandNATick* pAHTick = m_pMgr->GetAHNormalAttack();
		uint32 uAniLen = uint32(m_pMgr->GetEndFrameByActionState(eActionState) * fInterval * 33);	//攻击动作时长，单位毫秒
		float fNARhythm = 1.0f;

		uint32  uExpectedAHAniTime = pAHTick->Registered()?pAHTick->GetLeftTime():uint32(-1);
		
		if(uExpectedAHAniTime == uint32(-1) && pAHTick->m_uNextAttackTime != uint32(-1))
		{
			//cout << "修正下次副手攻击时间为 " << pAHTick->m_uNextAttackTime << endl;
			uExpectedAHAniTime = pAHTick->m_uNextAttackTime;
			pAHTick->m_uNextAttackTime = uint32(-1);
		}

		if(uExpectedAHAniTime != uint32(-1))
		{
			fNARhythm = max(1.3f, float(uAniLen) / float(uExpectedAHAniTime == 0 ? 1 : uExpectedAHAniTime));

			if(fNARhythm > 2.0f)
			{
				pAHTick->SetDelayedTime(min(uAniLen / 2 - uExpectedAHAniTime, uAniLen / 2));
				fNARhythm = 2.0f;
			}
			//cout << "AH left time = " << uExpectedAHAniTime << endl;
		}
		//

		//cout << m_pAttacker->GetDistortedFrameTime() << "|" << m_pAttacker->GetDistortedFrameTime() - m_uLastAniStartTime << " 主手加速 " << fNARhythm << " 主手动作长度 " << uAniLen << " 下次副手攻击 " << uExpectedAHAniTime << endl;
		
		if(pCharacter->GetMaster() && pCharacter->IsBattleHorse())
		{
			eActionState = eAS_Attack_Ride;
			uIndex = eAS_Attack_All;
			pCharacter->GetMaster()->SetAndSyncActionState(eAS_Attack_Ride, fNARhythm);
		}
		else
		{
			pCharacter->SetAndSyncActionState(eActionState, fNARhythm, eHurtResult);
		}

		CFighterDictator* pTarget = m_pAttacker->GetLockingTarget();
		if(pCharacter && pTarget)
		{
			bool bCanTurn = !pCharacter->CppGetCtrlState(eFCS_ForbitTurnAround);
			if (bCanTurn)
				pCharacter->SetAndSyncActionDirByPos(pTarget->GetPixelPos());
		}

		// 根据关键帧来注册伤害结算TICK
		m_pCurNormalAttack->m_uRegisterTickTime = m_pAttacker->GetDistortedFrameTime();
		m_pCurNormalAttack->Init(m_pSkillInst, GetCfg(), this, (uint32)(fInterval * m_pMgr->GetHitFrameByActionState(eActionState) * 33 / fNARhythm));
		//CAppServer::Inst()->RegisterTick(m_pCurNormalAttack, (uint32)(fInterval * m_pMgr->GetHitFrameByActionState(eActionState) * 33 / fNARhythm));
	}
	else
	{
		float fInterval = m_pMgr->GetFighter()->m_AssistantWeaponInterval.GetMultiplier();
		//根据主手攻击动作的到来时间计算副手动作播放的速率
		CSingleHandNATick* pMHTick = m_pMgr->GetMHNormalAttack();
		uint32 uExpectedMHAniTime = uint32(-1);
		uint32 uAniLen = uint32(m_pMgr->GetEndFrameByActionState(eAS_Attack_Assist) * fInterval * 33);	//攻击动作时长，单位毫秒
		float fNARhythm = 1.0f;
		if( pMHTick->Registered() )
		{
			uExpectedMHAniTime=pMHTick->GetLeftTime();
			fNARhythm = max(1.3f, float(uAniLen) / float(uExpectedMHAniTime == 0 ? 1 : uExpectedMHAniTime));

			if(fNARhythm > 2.0f)
			{
				pMHTick->SetDelayedTime(min(uAniLen / 2 - uExpectedMHAniTime, uAniLen / 2));
				fNARhythm = 2.0f;
			}
			//cout << "MH left time = " << uExpectedMHAniTime << endl;
		}
		//
		//cout << m_pAttacker->GetDistortedFrameTime() << "|" << m_pAttacker->GetDistortedFrameTime() - m_uLastAniStartTime << " 副手加速 " << fNARhythm << " 副手动作长度 " << uAniLen << " 下次主手攻击 " << uExpectedMHAniTime << endl;
		pCharacter->SetAndSyncActionState(eAS_Attack_Assist, fNARhythm, eHurtResult);

		// 根据关键帧来注册伤害结算TICK
		m_pCurNormalAttack->m_uRegisterTickTime = m_pAttacker->GetDistortedFrameTime();
		m_pCurNormalAttack->Init(m_pSkillInst, GetCfg(), this, (uint32)(fInterval * m_pMgr->GetHitFrameByActionState(eAS_Attack_Assist) * 33 / fNARhythm));
		//CAppServer::Inst()->RegisterTick(m_pCurNormalAttack, (uint32)(fInterval * m_pMgr->GetHitFrameByActionState(eAS_Attack_Assist) * 33 / fNARhythm));
	}

	m_uLastAniStartTime = m_pAttacker->GetDistortedFrameTime();

	m_pSkillInst->GetNAInstServer()->SetAniIndex(uIndex);
	m_pSkillInst->GetNAInstServer()->SetHurtResult(eHurtResult);

	if (m_bIsMainHand && !m_pMgr->GetFighter()->CastToFighterMediator())
		return uint8(m_pMgr->GetEndFrameByActionState(eActionState) * m_pAttacker->m_MainHandWeaponInterval.GetMultiplier());
	else
		return 0;
}


CNormalAttackMgr::CNormalAttackMgr(CFighterDictator* pAttacker)
: m_pAttacker(pAttacker)
, m_bIsAttacking(false)
, m_bIsPaused(false)
//, m_bIsAuto(true)
, m_bCanBeRestarted(true)
, m_bIsForbitAutoNA(false)
{
	m_pMainhand = new CSingleHandNATick(pAttacker, this, true);
	m_pAssistanthand = new CSingleHandNATick(pAttacker, this, false);
}

float CNormalAttackMgr::GetAttackDistance() const
{
	if(m_pMainhand->m_fAttackScope != 0.0f)
		return m_pMainhand->m_fAttackScope;

	if (m_pMainhand->GetCfg() && m_pMainhand->GetCfg()->m_pMaxAttackDis)
		return float(m_pMainhand->GetCfg()->m_pMaxAttackDis->GetDblValue(GetFighter()));
	else
		return 0.0f;
}

bool CNormalAttackMgr::IsInAttackScope(CFighterDictator* pTarget)
{
	//CFPos posAttacker, posTarget;
	//posAttacker=GetFighter()->GetPixelPos();
	//posTarget=pTarget->GetPixelPos();
	//float x = posAttacker.x - posTarget.x;
	//float y = posAttacker.y - posTarget.y;
	//float fDistance = sqrt(x*x + y*y);
	float fDistance = m_pAttacker->GetFighterDist(pTarget);
	float fMaxAttackDist = GetAttackDistance() * eGridSpanForObj;
	//cout << "check NA dist fDist = " << fDistance << " fMaxDist = " << fMaxAttackDist << endl;
	return fDistance-fMaxAttackDist <= 1.0f;
}

bool CNormalAttackMgr::IsTargetUnreachableInLine(const CFighterDictator* pTarget)const
{
	if(pTarget == NULL || GetFighter() == NULL)
	{
		return true;
	}

	CCoreSceneServer* pScene = GetFighter()->GetScene();
	CPixelPath* pPath = NULL;
	CFPos posFromInPixel, posToInPixel;
	CPos posFromInGrid, posToInGrid;
	GetFighter()->GetPixelPos(posFromInPixel);
	pTarget->GetPixelPos(posToInPixel);
	GetFighter()->GetGridPos(posFromInGrid);
	pTarget->GetGridPos(posToInGrid);
	if(pScene)
		pPath = pScene->CreatePath(posFromInPixel, posToInPixel, eFPT_Line, eBT_MidBarrier);
	if(pPath == NULL && posFromInGrid != posToInGrid)
		return true;

	if(pPath)
		SafeRelease(pPath);

	return false;
}

bool CNormalAttackMgr::IsTargetInAttackScope()
{
	if(GetFighter()->GetLockingTarget() == NULL || GetFighter() == NULL)
	{
		return false;
	}
	//if (GetFighter()->CastToFighterMediator()==NULL)
	//{
	//	return true;
	//}
	return IsInAttackScope(GetFighter()->GetLockingTarget());
}

void CNormalAttackMgr::InitAutoAttacking(CFighterDictator* pTarget)
{
	//m_bIsPaused = false;
	//m_pAttacker->GetEntity()->LogEntityMsg("InitAutoAttacking");

	//注册事件
	RegisterEvent(pTarget);
}

//这个函数没用，可以删掉
void CNormalAttackMgr::OnTargetChanged(CFighterDictator* pNewTarget)
{
	UnRegisterEvent();

	InitAutoAttacking(pNewTarget);
}

void CNormalAttackMgr::CalDamageImmediately()
{
	if(m_pMainhand->IsDoingCurNormalAttack())
	{
		m_pMainhand->GetCurNormalAttack()->OnTick();
	}

	if(m_pAssistanthand->IsDoingCurNormalAttack())
	{
		m_pAssistanthand->GetCurNormalAttack()->OnTick();
	}
}

//void CNormalAttackMgr::SetAutoNormalAttack(bool bIsAuto)
//{
//	if (!m_bIsForbitAutoNA)
//		m_bIsAuto = bIsAuto;
//}

bool CNormalAttackMgr::Start()
{
	if(GetFighter()->GetCtrlState(eFCS_ForbitNormalAttack))
	{
		return false;
	}

	Ast(!m_bIsAttacking);
	Ast(!m_bIsPaused);

	m_bIsAttacking = true;
	//m_pAttacker->GetEntity()->LogEntityMsg("StartNA");

	//cout << GetFighter()->GetEntityID() << " Start普攻" << endl;
	m_pMainhand->m_uPausetime = 0;
	m_pAssistanthand->m_uPausetime = 0;
	InitAutoAttacking((CFighterDictator*)(GetFighter()->GetLockingTarget()));
	m_bCanBeRestarted = true;

	if(IsTargetInAttackScope() && !IsTargetUnreachableInLine(GetFighter()->GetLockingTarget()))
	{
		if (!GetFighter()->GetCtrlState(eFCS_InDrawWeaponMode))
			GetFighter()->CppSetCtrlState(eFCS_InDrawWeaponMode, true);

		uint32 uAssistanthandDelayTime = 0;
		//进行一次普通攻击
		if(m_pMainhand->HasWeapon())
		{
			uint32 uLeftTime = 0;

			if(!m_pMainhand->NextAttackRequireToBeWaited(uLeftTime))
			{
				//cout << "Start NA immediately" << endl;
				m_pMainhand->SetNormalAttackLastStartTime();
				m_pMainhand->DoNormalAttackOnce();
				m_pMainhand->RegisterTick((uint32)(m_pMainhand->m_fWeaponInterval * 1000));
				uAssistanthandDelayTime = eAHAttackDelayTime;
			}
			else
			{
				//Ast(iLeftTime <= (int32)((GetFighter()->Get(&CFighterCalInfo::m_MainHandWeaponInterval)) * 1000));
				if(uLeftTime > uint32(m_pMainhand->m_fWeaponInterval * 1000))
				{
					stringstream strm;
					strm << uLeftTime << " " << m_pMainhand->m_fWeaponInterval;
					GenErr("uLeftTime > uint32(m_pMainhand->m_fWeaponInterval * 1000)", strm.str().c_str());
				}

				m_pMainhand->RegisterTick(uLeftTime);
				//uAssistanthandDelayTime += uLeftTime;
			}
		}
		if(m_pAssistanthand->HasWeapon())
		{
			uint32 uLeftTime = 0;
			if(!m_pAssistanthand->NextAttackRequireToBeWaited(uLeftTime))
			{
				if(uAssistanthandDelayTime == 0)
				{
					m_pAssistanthand->SetNormalAttackLastStartTime();
					m_pAssistanthand->DoNormalAttackOnce();
					m_pAssistanthand->RegisterTick((uint32)(m_pAssistanthand->m_fWeaponInterval * 1000));
				}
				else
				{
					m_pAssistanthand->RegisterTick(uAssistanthandDelayTime);
				}
			}
			else
			{
				m_pAssistanthand->RegisterTick(uLeftTime);
			}
		}
		return true;
	}
	else
	{
		Pause();
		return false;
	}
	return false;
}

void CNormalAttackMgr::Cancel(bool bCalDamageImmediately, bool bUnRegistEvent)
{
	//if(!m_bIsAttacking) return;
	m_pAttacker->UnRegistDistortedTick(this);

	if(bUnRegistEvent)
	{
		UnRegisterEvent();
		m_pAttacker->GetEntity()->StopWatching(this);
	}

	m_bIsPaused = false;
	m_bIsAttacking = false;

	HaltNormalAttack(bCalDamageImmediately);


	//m_pAttacker->GetEntity()->LogEntityMsg("CancelNA");
	//cout << GetFighter()->GetEntityID() << " Cancel普攻" << endl;
}

void CNormalAttackMgr::ObserveTarget(CFighterDictator* pTarget)
{
	if(!pTarget || !IsAttacking())
		return;
	CEntityServer* pEntityTarget = pTarget->GetEntity();
	Ast(pEntityTarget);

	CEntityServer* pEntityFrom = m_pAttacker->GetEntity();
	Ast(pEntityFrom);
	pEntityFrom->Watch(this, pEntityTarget, GetAttackDistance() * eGridSpanForObj);
}

void CNormalAttackMgr::HaltNormalAttack(bool bCalDamageImmediately)
{
	if(bCalDamageImmediately)
	{
		this->CalDamageImmediately();
	}

	//if(m_pMainhand->IsDoingCurNormalAttack())
		m_pMainhand->m_pCurNormalAttack->Delete();

	//if(m_pAssistanthand->IsDoingCurNormalAttack())
		m_pAssistanthand->m_pCurNormalAttack->Delete();

	if(m_pMainhand->m_pDelayedAttack)
	{
		m_pMainhand->ClearDelayedAttack();
	}
	if(m_pAssistanthand->m_pDelayedAttack)
	{
		m_pAssistanthand->ClearDelayedAttack();
	}

	m_pMainhand->UnRegisterTick();
	m_pAssistanthand->UnRegisterTick();
}

//如果本次普攻正在进行，则延迟到本次普攻结束后再暂停
void CNormalAttackMgr::Pause(bool bCalDamageImmediately)
{
	if(!IsAttacking() || IsPaused())
	{
		return;
	}

	//m_pAttacker->GetEntity()->LogEntityMsg("PauseNA");
	m_bIsPaused = true;

	m_pMainhand->m_uPausetime = m_pAttacker->GetDistortedFrameTime();
	m_pAssistanthand->m_uPausetime = m_pMainhand->m_uPausetime;
	//m_pMainhand->ClearNoneCDTime();
	//m_pAssistanthand->ClearNoneCDTime();

	//cout << "Pause NA CalDamgageImm = " << bCalDamageImmediately << endl;

	HaltNormalAttack(bCalDamageImmediately);

	m_pAttacker->RegistDistortedTick(this, 500);
	//cout << GetFighter()->GetEntityID() << " Pause普攻" << endl;
	// 通知客户端取消攻击动作
}

void CNormalAttackMgr::OnTick()
{
	if(IsPaused() && IsAttacking() && !IsTargetUnreachableInLine(GetFighter()->GetLockingTarget()))
	{
		GetFighter()->RestartAttack();
	}
}

bool CNormalAttackMgr::IsPaused()const
{
	return m_bIsPaused;
}

bool CNormalAttackMgr::Restart()
{
	//cout << "Restart NA" << endl;
	if(!m_bCanBeRestarted)
		return false;

	if(!IsAttacking() || !IsPaused())
	{
		return false;
	}

	if(GetFighter()->GetCtrlState(eFCS_ForbitNormalAttack))
	{
		return false;
	}

	//m_pAttacker->GetEntity()->LogEntityMsg("RestartNA");
	m_bIsPaused = false;

	CFighterDictator* pTarget = GetFighter()->GetLockingTarget();
	if (pTarget)
		GetFighter()->Notify(eCE_Attack, reinterpret_cast<void*>(pTarget));

	if(m_pMainhand->HasWeapon())
	{
		m_pMainhand->SetWeaponInterval(GetFighter()->m_MainHandWeaponInterval.Get(GetFighter()));
	}

	if(m_pAssistanthand->HasWeapon())
	{

		m_pAssistanthand->SetWeaponInterval(GetFighter()->m_AssistantWeaponInterval.Get(GetFighter()));
	}

	uint32 uAssistanthandDelayTime = 0;
	if(m_pAssistanthand->HasWeapon())	//因为主手攻击时需要知道下次副手攻击的到来时间，所以在主手攻击之前先计算副手攻击到来时间
	{
		if(!m_pAssistanthand->NextAttackRequireToBeWaited(uAssistanthandDelayTime))
		{
			//cout << "Restart AHAttack Immediately" << endl;
			m_pAssistanthand->m_uNextAttackTime = 0;
		}
		else
		{
			//cout << "Restart AHAttack after " << uAssistanthandDelayTime << endl;
			m_pAssistanthand->m_uNextAttackTime = uAssistanthandDelayTime;
		}
	}

	if(m_pMainhand->HasWeapon())
	{
		uint32 uLeftTime = 0;

		if(!m_pMainhand->NextAttackRequireToBeWaited(uLeftTime))	//马上进行普通攻击
		{
			//cout << "Restart NA immediately" << endl;
			m_pMainhand->SetNormalAttackLastStartTime();
			m_pMainhand->TickOnce();
			m_pMainhand->RegisterTick((uint32)(m_pMainhand->m_fWeaponInterval * 1000));
			uAssistanthandDelayTime = eAHAttackDelayTime;
		}
		else
		{
			//Ast(uLeftTime <= (int32)((GetFighter()->Get(&CFighterCalInfo::m_MainHandWeaponInterval)) * 1000));
			if(uLeftTime > uint32(m_pMainhand->m_fWeaponInterval * 1000))
			{
				stringstream strm;
				strm << uLeftTime << " " << m_pMainhand->m_fWeaponInterval;
				GenErr("uLeftTime > uint32(m_pMainhand->m_fWeaponInterval * 1000)", strm.str().c_str());
			}

			//cout << "下次普攻将在 " << uLeftTime << endl;
			m_pMainhand->RegisterTick(uLeftTime);
		}
	}

	if(m_pAssistanthand->HasWeapon())
	{
		if(uAssistanthandDelayTime == 0)
		{
			m_pAssistanthand->SetNormalAttackLastStartTime();
			m_pAssistanthand->TickOnce();
			m_pAssistanthand->RegisterTick((uint32)(m_pAssistanthand->m_fWeaponInterval * 1000));
		}
		else
		{
			m_pAssistanthand->RegisterTick(uAssistanthandDelayTime);
		}
	}

	if(!IsPaused() && IsAttacking())
	{
		m_pMainhand->m_uPausetime = 0;
		m_pAssistanthand->m_uPausetime = 0;
		m_pAttacker->UnRegistDistortedTick(this);
	}
	//cout << GetFighter()->GetEntityID() << " Restart普攻" << endl;
	return true;
}

CNormalAttackMgr::~CNormalAttackMgr()
{
	Cancel();

	m_pAttacker->GetEntity()->StopWatching(this);
	delete m_pMainhand;
	delete m_pAssistanthand;
}

void CNormalAttackMgr::OnObserveeEntered()
{
	if(IsPaused() && IsAttacking())
	{
		GetFighter()->RestartAttack();
	}
}

void CNormalAttackMgr::OnObserveeLeft()
{
	if(!IsPaused() && IsAttacking())
	{
		if(!m_pMainhand->IsDoingCurNormalAttack() && !m_pAssistanthand->IsDoingCurNormalAttack() && !IsTargetInAttackScope())
			GetFighter()->PauseAttack();
	}
}

void CNormalAttackMgr::OnCOREvent(CPtCORSubject* pSubject, uint32 uEvent,void* pArg)
{
	if( pSubject == GetFighter()->GetLockingTarget() )
	{
		switch(uEvent)
		{
		case eCE_Die:
		case eCE_Offline:
		case eCE_ChangeMapEnd:
			GetFighter()->CancelAttack();
			break;
		default:
			break;
		}
	}
	else if (pSubject == GetFighter())
	{
		switch(uEvent)
		{
		case eCE_FinishDuel:
			GetFighter()->CancelAttack();
			break;
		default:
			break;
		}
	}
}

void CNormalAttackMgr::RegisterEvent(CFighterDictator* pTarget)
{
	//cout<<"CNormalAttackMgr::RegisterEvent = "<<pTarget<<endl;
	pTarget->Attach(this, eCE_Die);
	pTarget->Attach(this, eCE_Offline);
	pTarget->Attach(this, eCE_ChangeMapEnd);
	GetFighter()->Attach(this, eCE_FinishDuel);
}

void CNormalAttackMgr::UnRegisterEvent()
{
	this->DetachAll();
}

bool CNormalAttackMgr::DoAttack(CFighterDictator* pTarget)
{
	CFighterMediator* pMed = GetFighter()->CastToFighterMediator();
	Ast(pMed);
	if(GetFighter()->GetDoSkillCtrlState(eDSCS_InDoingSkill))
	{
		//eDSR_NormalAttackNotAllowed 
		pMed->SendFailureMsgToGac((uint32)(eDSR_NormalAttackNotAllowed));
		return false;
	}

	if(GetFighter()->GetCtrlState(eFCS_ForbitNormalAttack))
	{
		return false;
	}

	if(pTarget)
	{
		if(!pTarget->CppIsAlive() || !GetFighter()->CppIsAlive())
			return false;

		// 判断是否敌对
		CRelationMgrServer& relationMgr = CRelationMgrServer::Inst();
		if(!relationMgr.IsEnemy(GetFighter(), pTarget))
		{
			uint32 uResult = 0;
			switch(relationMgr.GetRelationType(GetFighter(), pTarget))
			{
			case eRT_Friend:
				uResult = eDSR_TargetIsFriend;
				break;
			case eRT_Neutral:
				uResult = eDSR_TargetIsNeutral;
				break;
			case eRT_SelfUnderPKLevel:
				uResult = eDSR_SelfUnderPKLevel;
				break;
			case eRT_TargetUnderPKLevel:
				uResult = eDSR_TargetUnderPKLevel;
				break;
			case eRT_SelfBeyondLv:
				uResult = eDSR_SelfBeyondLv;
				break;
			case eRT_TargetBeyondLv:
				uResult = eDSR_TargetBeyondLv;
				break;
			case eRT_SelfSafetyState:
				uResult = eDSR_SelfSafetyState;
				break;
			case eRT_TargetSafetyState:
				uResult = eDSR_TargetSafetyState;
				break;
			case eRT_TargetInDuel:
				uResult = eDSR_TargetInDuel;
				break;
			case eRT_DuelTargetWrong:
				uResult = eDSR_DuelTargetWrong;
				break;
			default:
				break;
			}
			if (uResult != 0)
				pMed->SendFailureMsgToGac(uResult);
			return false;
		}

		//判断和目标间是否有障碍
		//bool bTargetIsReachable = true;
		//CCoreSceneServer* pScene = GetFighter()->GetScene();
		//CPixelPath* pPath = NULL;
		//CFPos posFromInPixel, posToInPixel;
		//CPos posFromInGrid, posToInGrid;
		//GetFighter()->GetPixelPos(posFromInPixel);
		//pTarget->GetPixelPos(posToInPixel);
		//GetFighter()->GetGridPos(posFromInGrid);
		//pTarget->GetGridPos(posToInGrid);

		////int32 iMaxStep = (int32)(GetNpcLockEnemyDis()*GetNpcLockEnemyDis());
		//if(pScene)
		//	pPath = pScene->CreatePath(posFromInPixel, posToInPixel, eFPT_Line, eBT_MidBarrier);
		//if(pPath == NULL && posFromInGrid != posToInGrid)
		//	bTargetIsReachable = false;
		//if(pPath)
		//	SafeRelease(pPath);

		if(IsTargetUnreachableInLine(pTarget))
		{
			pMed->SendFailureMsgToGac((uint32)(eDSR_ToTargetHaveBarrier));
			return false;
		}
	}

	if(GetFighter()->GetLockingTarget() != pTarget && IsAttacking())//更换攻击目标
	{
		this->CalDamageImmediately();	//切换目标时需要马上结算伤害，有可能新目标已经不在攻击范围内了
		OnTargetChanged(pTarget);
		GetFighter()->SetLockingTarget(pTarget);
		
		//Cancel(true);
		m_pAttacker->RestartAttack();
		ObserveTarget(pTarget);
	}
	else if(!IsAttacking())//开始普通攻击
	{
		//cout << GetFighter()->GetEntityID() << " 开始普通攻击" << endl;
		if(m_pMainhand->HasWeapon())
		{
			m_pMainhand->SetWeaponInterval(GetFighter()->m_MainHandWeaponInterval.Get(GetFighter()));
		}

		if(m_pAssistanthand->HasWeapon())
		{
			m_pAssistanthand->SetWeaponInterval(GetFighter()->m_AssistantWeaponInterval.Get(GetFighter()));
		}

		if(m_pMainhand->HasWeapon())
		{
			//cout << GetFighter()->GetEntityID() << " 开始普通攻击->切换目标至 " << pTarget->GetEntityID() << endl;
			//OnTargetChanged(pTarget);
			GetFighter()->SetLockingTarget(pTarget);
			Start();
			ObserveTarget(pTarget);
		}
		else
		{
			return false;
		}
	}
	return true;
}

void CNormalAttackMgr::InitMHNA(const TCHAR* szMHName, EAttackType MHAttackType, EWeaponType MHWeaponType)
{
	CCharacterDictator* pCharacter = m_pAttacker->GetCharacter();

	if(szMHName != NULL)
	{
		m_pMainhand->SetWeaponInterval(m_pAttacker->m_MainHandWeaponInterval.Get(m_pAttacker));
		m_pMainhand->SetCfg(szMHName);
		EAttackType eOldType = m_pMainhand->GetAttackType();
		m_pMainhand->SetAttackType(MHAttackType);
		if(eOldType != MHAttackType)
		{
			m_pAttacker->OnChangeAttackType();
		}
		if (pCharacter)
			pCharacter->SetWalkBackRadius((uint8)GetAttackDistance() + 2);
	}

	m_pMainhand->SetWeaponType(MHWeaponType);

	//TODO:1.根据男女要区分, 2.判断是否有战斗坐骑（暂时fky文件不全，没法做&测）
	//uint32 a0 = (uint32)GetFighter()->GetCharacter()->GetViewVariant()->GetMember("Attribute")->GetMember("Sex")->GetNumber<uint8>();
	if (pCharacter)
	{
		CActorServer* pActor = pCharacter->GetActor();
		Ast(pActor);
		if (m_pAttacker->CastToFighterMediator())
		{
			const string& strKeyFrameFile = m_pMainhand->GetCfg()->GetKFrameFileMan();
			if (strKeyFrameFile != "")
				pActor->OnChangeNAAniKeyFrame(strKeyFrameFile.c_str());
		}
		else
		{
			const string& strKeyFrameFile = m_pAttacker->GetCharacter()->GetNpcName();
			if (strKeyFrameFile != "")
				pActor->OnChangeNAAniKeyFrame(strKeyFrameFile.c_str());
		}
	}
}

void CNormalAttackMgr::InitAHNA(const TCHAR* szAHName, EAttackType AHAttackType, EWeaponType AHWeaponType)
{
	if(szAHName == NULL)
	{
		m_pAssistanthand->UnarmWeapon();
	}
	else
	{
		m_pAssistanthand->SetWeaponInterval(m_pAttacker->m_AssistantWeaponInterval.Get(m_pAttacker));
		m_pAssistanthand->SetCfg(szAHName);
		m_pAssistanthand->SetAttackType(AHAttackType);
	}
	m_pAssistanthand->SetWeaponType(AHWeaponType);
}

void CNormalAttackMgr::InitNPCNormalAttack(const TCHAR* szMHName)
{
	InitMHNA(szMHName, eATT_None, eWT_None);
	InitAHNA(NULL, eATT_None, eWT_None);
	m_pMainhand->SetAttackType(m_pMainhand->GetCfg()?m_pMainhand->GetCfg()->GetAttackType():eATT_None);
}

void CNormalAttackMgr::InitNormalAttack(const TCHAR* szMHName, EAttackType MHAttackType, EWeaponType MHWeaponType, 
										const TCHAR* szAHName, EAttackType AHAttackType, EWeaponType AHWeaponType)
{
	InitMHNA(szMHName, MHAttackType, MHWeaponType);
	InitAHNA(szAHName, AHAttackType, AHWeaponType);
}

uint8 CNormalAttackMgr::GetHitFrameByActionState(EActionState eActionState)
{
	CCharacterDictator* pCharacter = m_pAttacker->GetCharacter();
	Ast(pCharacter);
	CActorServer* pActor = pCharacter->GetActor();
	Ast(pActor);
	return pActor->GetHitFrameByActionState(eActionState);
}

uint8 CNormalAttackMgr::GetEndFrameByActionState(EActionState eActionState)
{
	CCharacterDictator* pCharacter = m_pAttacker->GetCharacter();
	Ast(pCharacter);
	CActorServer* pActor = pCharacter->GetActor();
	Ast(pActor);
	return pActor->GetEndFrameByActionState(eActionState);
}

void CNormalAttackMgr::AddNoneCDTime(uint64 uAccumulatedNoneCDTime)
{
	uint64 uCurTime = m_pAttacker->GetDistortedFrameTime();
	if(uCurTime - m_pMainhand->GetNormalAttackLastStartTime() < uAccumulatedNoneCDTime)
	{
		ostringstream strm;
		strm << GetFighter()->GetCharacter()->GetGlobalID();
		strm << " " << uAccumulatedNoneCDTime << " " << uCurTime << " " << m_pMainhand->GetNormalAttackLastStartTime() << endl;
		GenErr(" 流程过程中执行了主手普攻 ID casttime curTime lastattacktime ", strm.str());
	}
	if(uCurTime - m_pAssistanthand->GetNormalAttackLastStartTime() < uAccumulatedNoneCDTime)
	{
		ostringstream strm;
		strm << GetFighter()->GetCharacter()->GetGlobalID();
		strm << " " << uAccumulatedNoneCDTime << " " << uCurTime << " " << m_pAssistanthand->GetNormalAttackLastStartTime() << endl;
		GenErr(" 流程过程中执行了副手普攻 ID casttime curTime lastattacktime ", strm.str());
	}
	m_pMainhand->AddNoneCDTime(uAccumulatedNoneCDTime);
	m_pAssistanthand->AddNoneCDTime(uAccumulatedNoneCDTime);
}


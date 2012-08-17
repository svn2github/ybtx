#pragma once
#include "CNpcServerBaseData.h"
#include "CNpcAIBaseData.h"
#include "INpcAIHandler.h"
#include "CNpcStateMachine.h"
#include "CDistortedTick.h"
#include "PatternCOR.h"
#include "CNpcAINature.h"
#include "CNpcFightBaseData.h"
#include "CEnmityMgr.h"
#include "StateMachineCheckValid.h"
#include "IObjPosObserverHandler.h"
#include "CNpcAIMallocObject.h"
#include "TNpcAIAllocator.h"
#include "ErrLogHelper.h"

namespace sqr
{
	template < class HolderType, class PointeeHolderType >class TPtRefer;
	class CCoreObjectCalculator;
}

class CEntityServer;
class INpcEventHandler;
class CTotemMagicCfgServer;
class CSkillInstServer;
class CFighterDictator;
class CNormalSkillServerSharedPtr;
class CNpcEventMsg;
class CNpcAlertMgr;

//辅助声明状态宏
#define DECLARE_NPCSTATE(name, base) \
    class CNpcState##name : public CNpcState##base \
    { \
	public: \
	CNpcState##name(CNpcAI *pNpc, uint32 uID) : CNpcState##base(pNpc, uID) { }\
	static CNpcStateBase* Create(CNpcAI *pNpc, uint32 uID) { return new CNpcState##name(pNpc, uID); } \
	protected:

#define END_DECLARE_NPCSTATE };

//辅助声明重载方法
#define OVERRIDE_NPCSTATE_METHOD(name) \
	virtual CNpcEventMsg* name(CNpcEventMsg *pEvent)

//辅不带异常捕获的重载方法，主要用于状态响应
#define DEFINE_NPCSTATE_METHOD(statename, methodname) \
	CNpcEventMsg* CNpcAI::CNpcState##statename::methodname(CNpcEventMsg *pEvent) \
	{ \

#define END_DEFINE_NPCSTATE_METHOD \
		return pEvent; \
	} \

class CCharacterDictator;
class CNpcStateTransit;
class CNpcEnmityMgr;
class CNpcAI;
class CTotemAI;
class CDummyAI;
class CMemberAI;
class CServantAI;
class CNpcNatureMgr;
class CNpcEventMsg;
class StateMachineCheckValid;
class CSkillRuleMgr;
class CSkillRule;
class CTruckAI;
class CTruckFollowMaster;
class CDoSceneSkillRule;
class CGoBackExclaimRule;
class CExclaimRule;
class CAlertExclaimRule;
class CNpcEnmityTargetFilterData;

/*
 * =====================================================================================
 *        Class:  CNpcAI
 *  Description:  Npc
 * =====================================================================================
 */
class CNpcAI 
	: public INpcAIHandler
	, public CPtCORHandler
	, public IWatchHandler
	, public CNpcAIMallocObject 
{
	typedef map<uint32, uint16, less<uint32>, TNpcAIAllocator<pair<uint32, uint16> > >	MapEntityIDInNpcSight;
	typedef set<INpcEventHandler*, less<INpcEventHandler*>, TNpcAIAllocator<INpcEventHandler*> > SetEventHandler;
	typedef map<uint32, SetEventHandler, less<uint32>, TNpcAIAllocator<pair<uint32, SetEventHandler > > >		MapEvent2Handler;
	typedef list<CNpcStateMachine*, TNpcAIAllocator<CNpcStateMachine*> > ListStateMachine;	
	typedef map<uint8, CFPos*, less<uint8>, TNpcAIAllocator<pair<uint8, CFPos*> > >	MapMovePath;
	friend class CNpcServerCreator;
	friend class CCharacterDictator;
	friend class CNpcSubMachineCreator;
	friend class CEnmityMgr;
	friend class CDummyEnmityMgr;
	friend class CMemberAI;
	friend class CDummyAI;
	friend class CServantAI;
	friend class CSkillRule;
	friend class CSkillRuleMgr;
	friend class CNpcAlertMgr;
	friend class CTruckAI;
	friend class CTruckFollowMaster;
	friend class CSkillAroundRule;
	friend class CLittlePetAI;
	friend class CLittlePetDoActTick;
	friend class CDoSceneSkillRule;
	friend class CGoBackExclaimRule;
	friend class CExclaimRule;
protected:
	/* ====================  LIFECYCLE     ======================================= */
	CNpcAI( CCharacterDictator* pCharacter, const CNpcServerBaseData* pBaseData, const CNpcStateTransit* pStateTransit, ENpcType eRealNpcType, ENpcAIType eRealAIType, EObjectAoiType eObjAoiType);
	virtual ~CNpcAI();

public:
	/* ====================  OPERATIONS    ======================================= */
	//注册NPC事件
	static bool RegistAllEvent();
	//注册NPC状态
	static bool RegistAllState();

	TPtRefee<CNpcAI, CNpcEventMsg>& GetRefByEventMsg();

	//重载Handler函数
	virtual void OnCreateEnded();
	virtual void OnDead();
	virtual void OnDestoryBegin();
	virtual void OnCaughtViewSightOf(uint32 uObjGlobalId, uint32 uDimension);
	virtual void OnLostViewSightOf(uint32 uObjGlobalId, uint32 uDimension);
	void RedirectForCaughtViewSightOf(CCharacterDictator *pCharacter, uint32 uDimension);
	void RedirectForLostViewSightOf(CCharacterDictator *pCharacter, uint32 uDimension);
	virtual void OnMoveBegan();
	virtual void OnMoveEnded(uint32 uMoveID =0);
	virtual void OnCreateMovePath(uint8 uPoint, float x, float y);
	virtual void OnMoveToPathPoint(uint8 point);
	virtual void OnBeAttacked(CCharacterDictator* pAttacker, bool bHurtEffect = true);
	virtual void OnMemberBeAttacked(CCharacterDictator* pAttacker) {}
	virtual void OnOtherDead(uint32 EntityID, const string& NpcName);
	virtual void OnAgileValueChanged(uint32 uValueTypeID, float fValue);
	virtual void OnCtrlValueChanged(uint32 uValue, uint32 uValueTypeID, bool bSet);
	virtual void OnDoSkillCtrlStateChanged(uint32 uState, bool bSet);
	virtual void OnSkillBegin();
	virtual void OnSkillSuccessEnd();
	virtual void OnSkillFailEnd();
	virtual void OnMasterDestroyed() {};
	virtual void OnGroupDestroyed() {};
	virtual void CheckLifeTimeByLeaveBattle();
	virtual void SetDeadAfterBattle();
	virtual void SetDefaultWalkState(bool walkState);
	virtual void SetMoveRange(uint32 range);
	virtual void OnBeginRandomMove();
	virtual void OnSetState(uint32 global_id);
	virtual void OnDeleteState(uint32 global_id);
	virtual void OnFacialAction(uint32 uEntityID, const string& szFacialName);
	virtual void OnSetNatureData(const string& szNatureName, const string& szNatureArg, uint32 uNatureExtraArg, 
								const string& szNatureAction, uint16 uColdTime, const string& szAction, const string& szActionArg, uint16 uLevel);
	virtual void SendNpcEvent(ENpcEvent eEvent);
	virtual uint32 GetOneActorAround();
	virtual uint32 GetOneNpcAround();
	virtual uint32 GetRandomOneAround();
	virtual void ClearAllEnmity();
	void ReSetNpcExistTick(uint32 uExitTime);
	virtual void StopFollowMaster(){}
	virtual EAttackType GetNpcAttackType();
	virtual void SetGroupMemberID(uint32 id);
	virtual CNpcNatureMgr* GetNatureMgr();
	virtual ECharacterInnerHandlerType GetType() const { return ECharacterInnerHandlerType_Npc;}
	virtual bool CanDelInBattle() { return m_bDelInBattle;}
	virtual void SetCanDelInBattle(bool bCanDel) { m_bDelInBattle = bCanDel;}
	virtual void CheckPlayInNpcSleepDim();
	virtual	bool HaveBeSetPath();
	virtual void RemoveOneEnemy(CCharacterDictator *pEnemy);
	virtual void SetNpcAoiAndNature();
	virtual void SetNpcBirthDir(uint8 uDir);
	virtual void OnEnterBattleState();
	virtual void OnLeaveBattleState();
	virtual void MasterNotifyLeaveBattle();
	virtual void NpcMoveInPath();
	virtual float GetAttackScope() { return m_fAttackScope;}
	virtual CEnmityMgr* GetEnmityMgr() const{ return m_pEnmityMgrHandler.Get(); } 
	virtual uint32 GetNpcDoSkillLevel();
	virtual void OnBeHurt(int32 iHpChange);
	virtual void SetDoSpecialActionProbability(uint32 uProbability);

	void RevertNpcBirthDir();
	ENpcType GetNpcType() const { return m_eRealNpcType; }
	const string& GetName() const { return m_pBaseData->GetName(); }
	ENpcAIType GetAITypeID() const {return m_pBaseData->m_eAIType;}
	uint32 GetMoveRang() {return m_uMoveRange;};
	bool GetIsDoingAttack() { return m_isDoingAttack;}
	void SetIsDoingAttack(bool bState) { m_isDoingAttack = bState;}
	bool GetIsDoingSkillRule() { return m_isDoingSkill;}
	void SetIsDoingSkillRule(bool bState);
	bool GetNormalAttackFirstTime() { return m_bNormalAttackFirstTime; }
	void SetNormalAttackFirstTime(bool bSet);
	void ReSetTarget();
	void FaceToEnemy();
	void FaceToTarget(CCharacterDictator* pCharacter);
	void FaceToPos(CFPos targetCurPos);
	float GetWeaponSpeed(); 
	uint32 GetRandMaxAttackSpeed();
	void NotifyAccompanierEnterBattle(float fEyeSight, uint32 uNumber, bool bShowEffect);
	void ShareEnmityWithFriend(CNpcAI* pFriendAI);
	void DropAdscriptionJudgement(CCharacterDictator* pAttacker);
	void DealExpOwnerOnEnemyLost(CCharacterDictator* pEnemy);
	//NPC追击返回时清除所有BUFF及免疫伤害
	void ClearDebufferAndSettingImmunity();
	void ClearImmunity();
	void SetBornTrigger();						//NPC出生释放爆魂触发器
	//下面两个函数提任务巡逻NPC使用
	void TalkWithPlayerStart(uint32 uEntityID);
	void TalkWithPlayerEnd();
	uint32 GetMovePathPointCount();
	//lua 调用的移动到目的地的消息响应函数
	virtual void MoveToDest(uint32 uX,uint32 uY);
	virtual void MoveToDest(float uX,float uY);
	void MoveToDestEnded();
	void AddEventHandler(uint32 uEventID, INpcEventHandler* pHandler);
	void RemoveEventHandler(uint32 uEventID, INpcEventHandler* pHandler);
	void ForceLock(CCharacterDictator* pTarget, uint32 uChaosTime);
	
	CCharacterDictator* GetTargetDictator() const;
	CFighterDictator* GetTarget() const;
	virtual uint32 GetExpOwnerID();
	CCharacterDictator* GetFirstEnmityEnemy();				//得到仇恨列表第一位的敌人
	CFighterDictator* GetFirstEnenmyFighter();
	inline CCharacterDictator* GetCharacter() const{ return m_CharacterHandler.Get(); }
	void SetTarget(CFighterDictator *pEnemy, bool bCalNAImmediately = false);
	bool CanAIBeChange();
	bool CanBeRavin();
	bool CanBeSelected();
	void OnEnterWanderState();
	void OnLeaveWanderState();
	void SetCharacterHolder(CCharacterDictator* pCharacter);
	void SetNpcNatureMgrHolder(CNpcNatureMgr* pNpcNatureMgr);
	void SetSkillRuleMgrHolder(CSkillRuleMgr* pSkillRuleMgr);
	void SetNpcEnmityMgrHolder(CEnmityMgr* pEnmity);
	void SetNpcAlertMgrHolder(CNpcAlertMgr* pNpcAlertMgr);
	void SetStateMachineCheckValidHolder(StateMachineCheckValid* pCheckValid);
	uint32 GetExistTime(){return GetAIBaseData()->m_fExistTime;}
	uint32 GetEnmityDist(){return GetAIBaseData()->m_uEnmityDist;}
	void SetRealAIType(ENpcAIType eRealAIType);
	ENpcAIType GetRealAIType();
	ENpcAIType GetOldAIType();
	void SetRealNpcType(ENpcType eRealNpcType);
	ENpcType GetRealNpcType();
	const string& GetNpcName() const;
	void SetMoveType(uint8 uMoveType);
	void StopMoving();
	uint32 GetLockEnemyDis();
	void SetLifeTimeEnd();
	bool BeLifeTimeEnd();
	bool BeInBattle();
	virtual bool IsInLockEnemyRange(CFighterDictator* pTarget);
	virtual bool IsInVisionRange(CFighterDictator* pTarget);
	void NeedToChangeTarget(uint32 uID);
	uint32 GetNpcLockEnemyDis();
	uint32 GetNpcChaseEnemyDis();
	float GetNpcRunSpeed();
	void SetNpcSleepState(bool bSleep);
	bool RegulateNpcPlace();
	void CheckStateMachineValid(CNpcEventMsg* pEvent);
	const string&	GetCurrentStateName() const;
	virtual void OnObserveeEntered();
	virtual void OnObserveeLeft();
	virtual void OnWatched(EWatchResult eResult);
	void TraceEnemy(CCharacterDictator* pTarget,EBarrierType eBarrierType, float fDist);			//只为战斗中追击使用
	void StopTraceEnemy();
	virtual void BeginChaseOriginTrace();
	virtual void EndChaseOriginTrace();
	void OnEnemyDead();
	void OnChangeSpeedBySkill();
	void OnTraceEnded(ETraceEndedType eType);	
	bool BeAlertNpc(uint32 uTargetID);
	bool TargetCanBeAlert(uint32 uTargetID);
	uint32 GetAlertTime(uint32 uNumber);
	void BeginAttack(uint32 uEntityID);
	bool NeedOpenTargetPKState();
	void DispatchEventToNpcAI(CNpcEventMsg* pEvent);
	uint32 GetRandMoveTime();
	MapEntityIDInNpcSight& GetEntityInSightMap() {return m_mapEntity;};
	uint32 GetEscapeTime() const {return GetAIBaseData()->m_uEscapeTime;}
	uint32 GetEscapeDistace() const {return GetAIBaseData()->m_uEscapeDistace;}
	void GetNpcBirthPos(CFPos& pos);
	bool IsChaosEnmity(){return GetFightBaseData()->m_bChaosEnmity;};
	bool BeRegulate(){return GetFightBaseData()->m_bRegulate;}
	uint32 GetAlertDist() const {return GetAIBaseData()->m_uAlertDist;}
	bool IsExclaimAlert() const {return GetFightBaseData()->m_bExclaimAlert;}
	uint32 GetPatrolPointStayTime() const {return GetAIBaseData()->m_uPatrolPointStayTime;}
	bool IsInExclaimAlertDist(CCharacterDictator* pEneny);
	ENpcTargetType GetEnmityTargetType() const;
	bool IsEnmityTargetNpc(const string& strNpcName) const;
	bool CanFight(CCharacterDictator* pTarget);
	int32 GetMaxStep(const CFPos& curPos, const CFPos& targetPos);
private:
	void OnCOREvent(CPtCORSubject* pSubject, uint32 uEvent,void* pArg);
	void DispatchEventToLisner(CNpcEventMsg* pEvent);
	void DispatchEventToStateMachine(CNpcEventMsg* pEvent);
	void NpcDoSkill(const CNormalSkillServerSharedPtr* pCfg,CCharacterDictator* pTarget);
	void NpcDoPosSkill(const TCHAR* sName, const CFPos& Pos);
	void NpcDoPosSkill(const CNormalSkillServerSharedPtr* pCfg, const CFPos& Pos);
	void OnAddEnemyToEnmityMgr(CCharacterDictator *pEnemy, bool bHurtEffect);
	void SetExpOwnerID(uint32 uExpOwnerID);
	void CheckAndDoAction();
	void DestoryActionTick();
private:
	inline const CNpcFightBaseData*	GetFightBaseData() const
		{ return m_pBaseData->m_pFightBaseData;  }
	inline const CNpcAIBaseData*	GetAIBaseData() const
		{ return  m_pBaseData->m_pAIBaseDataOne;  }
	inline const CNpcResServerData* GetResBaseData() const
		{ return m_pBaseData->m_pResBaseData;}
	inline const CNpcEnmityTargetFilterData* GetEnmityTargetFilter() const
		{ return m_pBaseData->m_pEnmityTargetFilter;}
	inline const uint32 GetNpcBirthLevel() const
		{ return m_pBaseData->m_uLevel;} 
	const bool GetSynToClient()
		{ return m_pBaseData->m_bSynToClient; }
	//获取当前坐标到目标坐标连线上距离目标iDist的坐标
	CPos GetPosToTarget(const CPos& TargetPos, int32 iDist) const;
	bool IsInAttackRange(CFighterDictator* pTarget);
	CNpcStateMachine* GetCurrentStateMachine() const;
	CNpcEventMsg* PushSubMachine(CNpcStateMachine* pStateMachine, CNpcEventMsg* pEvent);
	CNpcEventMsg* PopSubMachine(CNpcEventMsg* pEvent);
	void ReSetNpcGridAttackScope(float fScope);
	void RevertNpcAttackScope();
	void UnRegisterNpCDistortedTick(CDistortedTick* pTick);
	void SetPersonalityHandler(CNpcNatureMgr* pNatureMgr);
	void DelPersonalityHandler(CNpcNatureMgr* pNatureMgr);
	CSkillRuleMgr* GetSkillRuleMgr() {return m_pSkillRuleMgrHandler.Get();}
	CNpcAlertMgr* GetNpcAlertMgr() {return m_pNpcAlertMgrtHandler.Get();}
	CCharacterDictator* GetOneEnemy();
	CCharacterDictator* GetNearestVisionEnemy();                  //提供主动怪脱离战斗后选取视野内的敌人
	void ClearNearestVisionEnemy(CCharacterDictator* pEnemy);
	bool CanDoNormalAttack();
	void SetNpcSize();
	float GetNpcSize();
	void NpcSinglePatrolStateEnd();
	void MoveToBirthPos();
	void AddPlayToSleepDim();
	void DelPlayToSleepDim();
	void CheckNpcMoveState(const CFPos& pos, const TCHAR* sMsg);
	void SetInChaseBackState(bool bInChaseBack)  { m_bInChaseBackState = bInChaseBack;}
	bool BeInChaseBackState(){ return m_bInChaseBackState;}
	void CreateAlertMgr(uint32 uTargetID, bool bNormal = true, uint32 uAlertTime = 0);
	void DestoryNpcAlertMgr();
	void DeleteNpcAIStateMachine();
	void InitVariant();
	int32 GetLeftLifeTime();
	uint64 GetBirthTime()		{ return m_uBirthTime; }
	bool ExitSkillRuleMgr(){ return GetFightBaseData()->m_lSkillRule.size() != 0;}
	bool AttackTargetHaveHighBarrier();

	/************************************************************************/
	//			NpcAI上面封装的直接调用引擎对象进行移动的四个函数
	/************************************************************************/
private:
	void TheaterMoveToGird(const CPos& pos, bool bWalkState);
	bool MoveToGrid(const CPos& pos, bool bWalkState, float fReachDist);
	bool MoveToPixel(const CFPos& pos, bool bWalkState, float fReachDist);
	bool MoveToPixelInLinePath(const CFPos& pos, bool bWalkState);
	bool MoveToPixel(const CFPos& pos, float fSpeed, bool bWalkState, float fReachDist);
	bool MoveToPixelInLinePath(const CFPos& pos, float fSpeed, bool bWalkState);
	bool MoveToTargetPos(const CFPos& targetPos, bool bWalkState);
protected:
	class PatrolTick 
		: public CDistortedTick
		, public CNpcAIMallocObject
	{
	public:
		PatrolTick(CNpcAI* pAI):m_pAI(pAI){};
		virtual void OnTick();
	private:
		CNpcAI* m_pAI;
	};

	class SpecialActionTick 
		: public CDistortedTick
		, public CNpcAIMallocObject
	{
	public:
		SpecialActionTick(CNpcAI* pAI):m_pAI(pAI){};
		virtual void OnTick();
	private:
		CNpcAI* m_pAI;
	};

	class ExistTick 
		: public CDistortedTick
		, public CNpcAIMallocObject
	{
	public:
		ExistTick(CNpcAI *pAI) : m_pAI(pAI){}
		void OnTick();
	private:
		CNpcAI* m_pAI;
	};

	class DoActionOrRandomMoveTick 
		: public CDistortedTick
		, public CNpcAIMallocObject
	{
	public:
		DoActionOrRandomMoveTick(CNpcAI* pAI) : m_pAI(pAI){}
		void OnTick();
	private:
		CNpcAI*		m_pAI;
	};

	class  CNpcAlertDelayAttackTick
		: public CDistortedTick
		, public CNpcAIMallocObject
	{
	public:
		CNpcAlertDelayAttackTick(CNpcAI* pAI, uint32 uTargetID):m_pAI(pAI), m_uTargetID(uTargetID){}
		void OnTick();
	private:
		CNpcAI*			m_pAI;
		uint32			m_uTargetID;
	};

	class  DelaySetEyeSizeAndNatureTick
		: public CDistortedTick
		, public CNpcAIMallocObject
	{
	public:
		DelaySetEyeSizeAndNatureTick(CNpcAI* pAI):m_pAI(pAI){}
		void OnTick();
	private:
		CNpcAI*			m_pAI;
	};

	//------------------------------------------------------------------------------------------------------------------------------
	//*
	//*												NpcAI状态定义
	//*
	//------------------------------------------------------------------------------------------------------------------------------
protected:
	DECLARE_NPCSTATE(MoveBase, Base);
	OVERRIDE_NPCSTATE_METHOD(LeaveState);
	END_DECLARE_NPCSTATE;

	DECLARE_NPCSTATE(Idle, Base);
	OVERRIDE_NPCSTATE_METHOD(EnterState);
	OVERRIDE_NPCSTATE_METHOD(LeaveState);
	CDistortedTick* m_pDoSpecialActionTick;
	END_DECLARE_NPCSTATE;
	
	DECLARE_NPCSTATE(Task, Base);
	OVERRIDE_NPCSTATE_METHOD(EnterState);
	OVERRIDE_NPCSTATE_METHOD(OnEvent);
	OVERRIDE_NPCSTATE_METHOD(LeaveState);
	private:
		CDistortedTick* m_pRemainTimeTick;
	END_DECLARE_NPCSTATE;

	DECLARE_NPCSTATE(Wander, MoveBase);
	OVERRIDE_NPCSTATE_METHOD(EnterState);
	OVERRIDE_NPCSTATE_METHOD(OnEvent);
	OVERRIDE_NPCSTATE_METHOD(LeaveState);
	END_DECLARE_NPCSTATE;

	//主动怪的主动状态
	DECLARE_NPCSTATE(ActiveState, Base);
	OVERRIDE_NPCSTATE_METHOD(EnterState);
	OVERRIDE_NPCSTATE_METHOD(OnEvent);
	OVERRIDE_NPCSTATE_METHOD(LeaveState);
	END_DECLARE_NPCSTATE;

	//物件Npc（建筑，刷怪器等等）
	DECLARE_NPCSTATE(Building, Base);
	OVERRIDE_NPCSTATE_METHOD(EnterState);
	END_DECLARE_NPCSTATE;

	DECLARE_NPCSTATE(ActiveBossInit, Base);
	OVERRIDE_NPCSTATE_METHOD(EnterState);
	OVERRIDE_NPCSTATE_METHOD(OnEvent);
	OVERRIDE_NPCSTATE_METHOD(LeaveState);
	END_DECLARE_NPCSTATE;

	DECLARE_NPCSTATE(PassivityBossInit, Base);
	OVERRIDE_NPCSTATE_METHOD(EnterState);
	OVERRIDE_NPCSTATE_METHOD(OnEvent);
	OVERRIDE_NPCSTATE_METHOD(LeaveState);
	END_DECLARE_NPCSTATE;

	DECLARE_NPCSTATE(BossPatrol, Base);
	OVERRIDE_NPCSTATE_METHOD(EnterState);
	OVERRIDE_NPCSTATE_METHOD(OnEvent);
	OVERRIDE_NPCSTATE_METHOD(LeaveState);
	END_DECLARE_NPCSTATE;

	DECLARE_NPCSTATE(Theater, Base);
	OVERRIDE_NPCSTATE_METHOD(EnterState);
	OVERRIDE_NPCSTATE_METHOD(OnEvent);
	END_DECLARE_NPCSTATE;

	DECLARE_NPCSTATE(DoNothingState, Base);
	END_DECLARE_NPCSTATE;

	DECLARE_NPCSTATE(NpcSleep, Base);
	OVERRIDE_NPCSTATE_METHOD(EnterState);
	OVERRIDE_NPCSTATE_METHOD(LeaveState);
	END_DECLARE_NPCSTATE;

	DECLARE_NPCSTATE(DeathOrDestoryState, Base);
	OVERRIDE_NPCSTATE_METHOD(EnterState);
	END_DECLARE_NPCSTATE;

	DECLARE_NPCSTATE(TotemBase, Base);
	CTotemAI* GetOwner() const;
	END_DECLARE_NPCSTATE;

	DECLARE_NPCSTATE(TotemCaughtCharacter, TotemBase);
	OVERRIDE_NPCSTATE_METHOD(EnterState);
	END_DECLARE_NPCSTATE;

	DECLARE_NPCSTATE(TotemLostCharacter, TotemBase);
	OVERRIDE_NPCSTATE_METHOD(EnterState);
	END_DECLARE_NPCSTATE;

	DECLARE_NPCSTATE(DummyBase, Base);
	CDummyAI* GetOwner() const;
	END_DECLARE_NPCSTATE;

	DECLARE_NPCSTATE(PatrolForGroupLeader, DummyBase);
	OVERRIDE_NPCSTATE_METHOD(EnterState);
	OVERRIDE_NPCSTATE_METHOD(OnEvent);
	OVERRIDE_NPCSTATE_METHOD(LeaveState);
	CDistortedTick* m_pRelayTimeTick;
	bool m_bIsMemberFighting;
	END_DECLARE_NPCSTATE;

	DECLARE_NPCSTATE(MemberBase, Base);
	CMemberAI* GetOwner() const;
	END_DECLARE_NPCSTATE;

	DECLARE_NPCSTATE(PatrolForGroupMember, MemberBase);
	OVERRIDE_NPCSTATE_METHOD(EnterState);
	OVERRIDE_NPCSTATE_METHOD(OnEvent);
	OVERRIDE_NPCSTATE_METHOD(LeaveState);
	END_DECLARE_NPCSTATE;

	DECLARE_NPCSTATE(ServantBase, Base);
	CServantAI* GetOwner() const;
	END_DECLARE_NPCSTATE;

	DECLARE_NPCSTATE(ServantDefenseState, ServantBase)
	OVERRIDE_NPCSTATE_METHOD(EnterState);
	OVERRIDE_NPCSTATE_METHOD(OnEvent);
	OVERRIDE_NPCSTATE_METHOD(LeaveState);
	CDistortedTick*	m_pMoveBackToMaster;	
	END_DECLARE_NPCSTATE;

	DECLARE_NPCSTATE(ServantActiveState, ServantBase)
	OVERRIDE_NPCSTATE_METHOD(EnterState);
	OVERRIDE_NPCSTATE_METHOD(OnEvent);
	OVERRIDE_NPCSTATE_METHOD(LeaveState);
	END_DECLARE_NPCSTATE;

	DECLARE_NPCSTATE(ServantPassivityState, ServantBase)
	OVERRIDE_NPCSTATE_METHOD(EnterState);
	OVERRIDE_NPCSTATE_METHOD(OnEvent);
	OVERRIDE_NPCSTATE_METHOD(LeaveState);
	END_DECLARE_NPCSTATE;

	DECLARE_NPCSTATE(ChaseEnemy, Base);
	OVERRIDE_NPCSTATE_METHOD(EnterState);
	OVERRIDE_NPCSTATE_METHOD(OnEvent);
	OVERRIDE_NPCSTATE_METHOD(LeaveState);
	private:
		void StartTrace();
		EBarrierType			m_eBarrierType;
		bool					m_bInRegulate;
	END_DECLARE_NPCSTATE;

	DECLARE_NPCSTATE(AttackEnemy, Base);
	OVERRIDE_NPCSTATE_METHOD(EnterState);
	OVERRIDE_NPCSTATE_METHOD(OnEvent);
	OVERRIDE_NPCSTATE_METHOD(LeaveState);
	private:
		bool CheckAttackDis();			//检查是否超过攻击距离就跳转去追击
		void CreateAttackAniTick();
		void CreateNormalAttackCoolDownTick();
		void CreateDelayDoAttackTick();
		void CancelNormalAttack(bool bHurt);
		bool CanDoSkillRule();
		CDistortedTick*		m_pAttackEnemyAnimationTick;			//攻击动作tick
		CDistortedTick*		m_pNormalAttackEnemyCoolDownTick;	//普攻冷却时间tick
		CDistortedTick*		m_pDelayAttackEnemyTick;				//延时攻击
		bool		m_bInNormalAttackEnemyCoolDown;
	END_DECLARE_NPCSTATE;

	DECLARE_NPCSTATE(ChaseBack, MoveBase);
	OVERRIDE_NPCSTATE_METHOD(EnterState);
	OVERRIDE_NPCSTATE_METHOD(OnEvent);
	OVERRIDE_NPCSTATE_METHOD(LeaveState);
	private:
		uint32	m_uMoveID;
	END_DECLARE_NPCSTATE;

	DECLARE_NPCSTATE(LockEnemy, Base);
	OVERRIDE_NPCSTATE_METHOD(EnterState);
	END_DECLARE_NPCSTATE;
	
	DECLARE_NPCSTATE(PatrolForNpc, MoveBase);
	OVERRIDE_NPCSTATE_METHOD(EnterState);
	OVERRIDE_NPCSTATE_METHOD(OnEvent);
	OVERRIDE_NPCSTATE_METHOD(LeaveState);
	END_DECLARE_NPCSTATE;

	DECLARE_NPCSTATE(ExitSubMachine, Base);
	OVERRIDE_NPCSTATE_METHOD(EnterState);
	OVERRIDE_NPCSTATE_METHOD(Suspend) { return pEvent; }
	OVERRIDE_NPCSTATE_METHOD(Resume) { return pEvent; }
	END_DECLARE_NPCSTATE;

	DECLARE_NPCSTATE(TowerWander, Base);
	OVERRIDE_NPCSTATE_METHOD(EnterState);
	OVERRIDE_NPCSTATE_METHOD(OnEvent);
	OVERRIDE_NPCSTATE_METHOD(LeaveState);
	END_DECLARE_NPCSTATE;

	DECLARE_NPCSTATE(TowerLockEnemy, Base);
	OVERRIDE_NPCSTATE_METHOD(EnterState);
	END_DECLARE_NPCSTATE;

	DECLARE_NPCSTATE(TowerLeaveBattle, Base);
	OVERRIDE_NPCSTATE_METHOD(EnterState);
	OVERRIDE_NPCSTATE_METHOD(LeaveState);
	END_DECLARE_NPCSTATE;

	DECLARE_NPCSTATE(EscapeState, Base);
	OVERRIDE_NPCSTATE_METHOD(EnterState);
	OVERRIDE_NPCSTATE_METHOD(OnEvent);
	OVERRIDE_NPCSTATE_METHOD(LeaveState);
	private:
		void OnBeginEscape();
		CDistortedTick*	m_pEscapeTick;
	END_DECLARE_NPCSTATE;

	DECLARE_NPCSTATE(LittlePet, Base);
	OVERRIDE_NPCSTATE_METHOD(EnterState);
	OVERRIDE_NPCSTATE_METHOD(LeaveState);
	private:
		CDistortedTick*			m_pDoActionTick;
	END_DECLARE_NPCSTATE;

	/*========个性==========*/
	//个性状态
	DECLARE_NPCSTATE(InitPersonality, Base);
	OVERRIDE_NPCSTATE_METHOD(EnterState);
	END_DECLARE_NPCSTATE;

	//怯懦状态
	DECLARE_NPCSTATE(CowardState, Base);
	OVERRIDE_NPCSTATE_METHOD(EnterState);
	OVERRIDE_NPCSTATE_METHOD(LeaveState);
	END_DECLARE_NPCSTATE;

	//嗜睡状态
	DECLARE_NPCSTATE(SleepyState, Base);
	OVERRIDE_NPCSTATE_METHOD(EnterState);
	OVERRIDE_NPCSTATE_METHOD(OnEvent);
	OVERRIDE_NPCSTATE_METHOD(LeaveState);
	private:
		uint32 m_pLeastTime;
		uint32 m_pMostTime;
		CDistortedTick* m_pWakeUpTick;
		CDistortedTick* m_pSleepStateEndTick;
	END_DECLARE_NPCSTATE;

	//吸引
	DECLARE_NPCSTATE(AttractState, Base);
	OVERRIDE_NPCSTATE_METHOD(EnterState);
	OVERRIDE_NPCSTATE_METHOD(LeaveState);
	OVERRIDE_NPCSTATE_METHOD(OnEvent);
	private:
		CDistortedTick* m_pAttractTick;
	END_DECLARE_NPCSTATE;
	
	//喜欢状态
	DECLARE_NPCSTATE(LikeState, Base);
	OVERRIDE_NPCSTATE_METHOD(EnterState);
	OVERRIDE_NPCSTATE_METHOD(LeaveState);
	OVERRIDE_NPCSTATE_METHOD(OnEvent);
	OVERRIDE_NPCSTATE_METHOD(DoMove);
	private:
		CDistortedTick* m_DoLikeThingsTick;
	END_DECLARE_NPCSTATE;
	
	//厌恶状态
	DECLARE_NPCSTATE(AbhorState, Base);
	OVERRIDE_NPCSTATE_METHOD(EnterState);
	OVERRIDE_NPCSTATE_METHOD(OnEvent);
	OVERRIDE_NPCSTATE_METHOD(LeaveState);
	OVERRIDE_NPCSTATE_METHOD(DoMove);
	private:
		//CDistortedTick* m_pDoActionTick;
		CDistortedTick* m_pMoveBackTick;
	END_DECLARE_NPCSTATE;

	DECLARE_NPCSTATE(GreetState, Base);
	OVERRIDE_NPCSTATE_METHOD(EnterState);
	OVERRIDE_NPCSTATE_METHOD(LeaveState);
	END_DECLARE_NPCSTATE;

	//表情动作相应
	DECLARE_NPCSTATE(FacialState, Base);
	OVERRIDE_NPCSTATE_METHOD(EnterState);
	END_DECLARE_NPCSTATE;

	//表情冷却状态
	DECLARE_NPCSTATE(FacialColdState, Base);
	OVERRIDE_NPCSTATE_METHOD(EnterState);
	OVERRIDE_NPCSTATE_METHOD(OnEvent);
	OVERRIDE_NPCSTATE_METHOD(LeaveState);
	private:
		CDistortedTick* m_pFacialColdTick;
	END_DECLARE_NPCSTATE;

	//邪魔塔杨任务 之警戒
	DECLARE_NPCSTATE(AlertState, Base);
	OVERRIDE_NPCSTATE_METHOD(EnterState);
	OVERRIDE_NPCSTATE_METHOD(LeaveState);
	private:
		CDistortedTick* m_pLeaveWarningStateTick;
	END_DECLARE_NPCSTATE;

private:
	/* ====================  DATA MEMBERS  ======================================= */
	ListStateMachine				m_lstStateMachineStack;		
	MapMovePath					m_mapMovePath;
	const CNpcServerBaseData*	m_pBaseData;
	CDistortedTick*		m_pExistTick;
	CDistortedTick*		m_pPatrolTick;
	CDistortedTick*		m_pSpecialActionTick;
	CDistortedTick*		m_pRandomMoveOrActionTick;
	CDistortedTick*		m_pSetNpcAoiTick;	//有些Npc的EyeSize需要单独设置
	CDistortedTick*		m_pNpcAlertDelayAttackTick;
	uint32				m_uExpOwnerID;
	bool				m_bDefaultWalkState;
	uint8				m_uCurrentPoint;	//当前巡逻点
	bool				m_bMoveDir;			//往返巡逻时的移动方向(true 正向移动   false 逆向移动)
	bool				m_isDoingAttack;
	bool				m_isDoingSkill;
	ENpcAIType			m_eRealAIType;					
	ENpcType			m_eRealNpcType;
	uint32				m_uMoveRange;
	ENpcMoveType		m_eMoveType;		//巡逻方式(往返,循环)
	EObjectAoiType		m_eObjAoiType;
	bool				m_bLifeTimeEnd;	
	bool				m_bDelInBattle;
	uint32				m_uGroupSelfID;
	float				m_fSize;
	uint32				m_uPlayNumInSleepDim;	//Npc在休眠层能够看见玩家的总数
	bool				m_bSleep;						//是否需要休眠
	bool				m_bInSleepState;
	bool				m_bAlertState;
	bool				m_bInChaseBackState;	//在撤退过程中不允许对Npc造成伤害

	//这三个变量主要是为了解决Npc死循环问题
	uint64				m_uLasteTime;
	uint32				m_uCycNum;
	bool				m_bMayDeadCycState;
	string				m_sErrLog;

	sqr::CFPos			m_InitPos;
	sqr::CFPos			m_ChaseBeginPos;
	sqr::CFPos			m_DestPos;
	sqr::CFPos			m_RandomMoveBeginPos;
	float				m_fAttackScope;
	uint8				m_uBirthDir;
	uint64				m_uBirthTime;
	uint32				m_uSpecialActionProbability;
	bool				m_bNormalAttackFirstTime;
	MapEvent2Handler				m_mapEventHandler;
	MapEntityIDInNpcSight						m_mapEntity;
	uint32										m_uOriginObjID;
	bool										m_bTheaterMove; 
	TPtRefee<CNpcAI, CNpcEventMsg>				m_RefsByMsg;
	TPtRefer<CNpcAI, StateMachineCheckValid>	m_ReferByStateMachineCheckValid;
	TPtRefer<CNpcAI, CCharacterDictator>		m_CharacterHandler;
	TPtRefer<CNpcAI, CEnmityMgr>				m_pEnmityMgrHandler;
	TPtRefer<CNpcAI, CNpcNatureMgr>				m_pNpcNatureMgrHandler;
	TPtRefer<CNpcAI, CSkillRuleMgr>				m_pSkillRuleMgrHandler;
	TPtRefer<CNpcAI, CNpcAlertMgr>				m_pNpcAlertMgrtHandler;
}; /* -----  end of class  CNpcAI  ----- */

#undef DECLARE_NPCSTATE
#undef END_DECLARE_NPCSTATE
#undef OVERRIDE_NPCSTATE_METHOD


#pragma once
#include "CDistortedTick.h"
#include "PatternCOR.h"
#include "FightDef.h"
#include "ActorDef.h"
#include "TimeHelper.h"
#include "IObjPosObserverHandler.h"
#include "CFighterMallocObject.h"

class CSkillInstServer;
class CFighterDictator;
class CSingleHandNATick;
class CNormalAttackMgr;
class CNormalAttackCfg;
class CNormalAttackCfgSharedPtr;

class CNormalAttackAniTick;
//由于主副手攻击动作播放时间太接近需要把后面的一次攻击延后
class CDelayedAttack;

class CSingleHandNATick
	:public CDistortedTick
	,public CFighterMallocObject
{
public:
	friend class CNormalAttackMgr;
	friend class CNormalSkillServer;
	friend class CNormalAttackAniTick;
	friend class CDelayedAttack;

	CSingleHandNATick(CFighterDictator* pAttacker, CNormalAttackMgr* pMgr, bool bIsMainHand);
	virtual ~CSingleHandNATick();

	void SetAttackType(EAttackType eAttackType)					{ m_eAttackType = eAttackType; }
	void SetWeaponType(EWeaponType eWeaponType)					{m_eWeaponType = eWeaponType;}
	void UnarmWeapon();
	void SetCfg(const TCHAR* szName);
	void SetWeaponInterval(float fWeaponInterval);
	void SetAttackScope(float fAttackScope);
	CNormalAttackAniTick* GetCurNormalAttack();
	void ClearCurNormalAttack();
	EWeaponType GetWeaponType()const{return m_eWeaponType;}
	EAttackType GetAttackType()const{return m_eAttackType;}
	EAttackType GetCfgAttackType()const;
	bool HasWeapon();

	uint8 DoNormalAttackOnce();
	virtual void OnTick();

	bool IsShortNormalAttack();
	CFighterDictator*	GetFighter()const{return m_pAttacker;}
	bool IsMainHand()const;

private:
	// ReferToPauseTime表示拿上次pause和当前restart的时间差，ConsiderPauseTime表示是否将上次pause和当前restart之间的时间差也算作普攻的cd时间
	bool NextAttackRequireToBeWaited(uint32& uWaitingTime)const;
	void StopOwnTick();			//由于延迟攻击,需要暂停原来的普攻Tick
	void RestoreOwnTick();		//执行完延迟攻击,需要恢复原来的普攻Tick
	void RegisterTick(uint32 uTime);
	void UnRegisterTick();	

	void TickOnce();
	uint8 PlayAttackAni();
	EHurtResult PreDice();

	void SetNormalAttackLastStartTime();
	uint64 GetNormalAttackLastStartTime()const { return m_NormalAttackLastStartTime ; }
	void SetDelayedTime(uint32 uDelayedTime);
	bool IsDoingCurNormalAttack();
	void AddNoneCDTime(uint64 uAccumulatedNoneCDTime);
	void ClearNoneCDTime();
	void ClearDelayedAttack();
	CNormalAttackCfgSharedPtr& GetCfg()const;

private:
	CSkillInstServer*			m_pSkillInst;
	CFighterDictator*			m_pAttacker;
	CNormalAttackCfgSharedPtr*	m_pCfg;
	float						m_fWeaponInterval;		//武器攻击间隔
	CNormalAttackAniTick*		m_pCurNormalAttack;		//一次普通攻击
	EAttackType					m_eAttackType;			//武器攻击类型
	EWeaponType					m_eWeaponType;			//武器类型
	uint64						m_uPausetime;			//暂停一次普通攻击的时间
	uint64						m_NormalAttackLastStartTime;
	uint64						m_NoneCDTime;			//在当前时刻和上次攻击时刻之间不能被计算为cd的时间量
	uint32						m_uDelayedTime;			//被延迟的时间
	uint32						m_uNextAttackTime;		//下次攻击预计发生的时间，只是用于Restart的时候记录副手的下次攻击的时间
	CDelayedAttack*				m_pDelayedAttack;		

	CNormalAttackMgr*			m_pMgr;

	uint64						m_uLastAniStartTime;
	bool						m_bIsMainHand;
	float						m_fAttackScope;			//攻击距离,主要是npc的普攻使用

	//用于调试
	uint64						m_uLastOnTickTime;
	uint64						m_uLastDelayAttackTime;
	uint64						m_uLastDelayTickOnceTime;

};

class CNormalAttackMgr
	:public CPtCORHandler
	,public IWatchHandler
	,public CDistortedTick
	,public CFighterMallocObject
{
public:
	friend class CNormalSkillServer;

	CNormalAttackMgr(CFighterDictator* pAttacker);
	~CNormalAttackMgr();

	void OnCOREvent(CPtCORSubject* pSubject,uint32 uEvent,void* pArg);

	void RegisterEvent(CFighterDictator* pTarget);
	void UnRegisterEvent();
	bool DoAttack(CFighterDictator* pTarget);
	void InitNPCNormalAttack(const TCHAR* szMHName); 
	void InitNormalAttack(const TCHAR* szMHName, EAttackType MHAttackType, EWeaponType MHWeaponType, 
						const TCHAR* szAHName, EAttackType AHAttackType, EWeaponType AHWeaponType);
	void InitMHNA(const TCHAR* szMHName, EAttackType MHAttackType, EWeaponType MHWeaponType);
	void InitAHNA(const TCHAR* szAHName, EAttackType AHAttackType, EWeaponType AHWeaponType);
	
	void CalDamageImmediately();

	bool Start();								//开始自动进行普通攻击
	void Cancel(bool bCalDamageImmediately = false, bool bUnRegistEvent = true);				//停止自动进行普通攻击
	void Pause(bool bCalDamageImmediately = true);		//暂停自动进行普通攻击, 例如目标超出攻击范围
	bool Restart();								//恢复自动进行普通攻击, 例如目标再次进入攻击范围
	void OnTargetChanged(CFighterDictator* pNewTarget);	//转换攻击目标, 只能在一次Start()和Cancel()之间调用
	bool IsPaused()const;
	bool IsAttacking()const								{ return m_bIsAttacking; }
	void SetIsForbitAutoNA(bool bIsForbitAutoNA)		{m_bIsForbitAutoNA = bIsForbitAutoNA;}

	bool IsTargetUnreachableInLine(const CFighterDictator* pTarget)const;
	bool IsTargetInAttackScope();
	float GetAttackDistance() const;
	CSingleHandNATick* GetMHNormalAttack()const	{ return m_pMainhand;}
	CSingleHandNATick* GetAHNormalAttack()const	{ return m_pAssistanthand;}

	EAttackType GetAttackType()				{ return m_pMainhand->GetAttackType(); }
	
	void SetCanBeRestarted(bool bCanBeRestarted) { m_bCanBeRestarted = bCanBeRestarted; }
	bool GetCanBeRestarted()const { return m_bCanBeRestarted;}
	CFighterDictator*	GetFighter()const{return m_pAttacker;}

	uint8 GetHitFrameByActionState(EActionState eActionState);
	uint8 GetEndFrameByActionState(EActionState eActionState);

	virtual void OnObserveeEntered();
	virtual void OnObserveeLeft();

	void AddNoneCDTime(uint64 uAccumulatedNoneCDTime);	//更新非普攻cd时间

private:
	void InitAutoAttacking(CFighterDictator* pTarget);	//初始化自动攻击
	bool IsInAttackScope(CFighterDictator* pTarget);
	void ObserveTarget(CFighterDictator* pTarget);
	void HaltNormalAttack(bool bCalDamageImmediately);

	virtual void OnTick();	//普攻暂停时用该tick来判断攻击者和目标之间是否不存在障碍了,如果没有障碍则restart普攻

	CFighterDictator*		m_pAttacker;
	bool					m_bIsAttacking;		// 是否进行普通攻击
	bool					m_bIsPaused;		// 是否已暂停普通攻击，还包含之前已经进行普攻的语义
	//bool					m_bIsAuto;			// 是否自动普通攻击
	bool					m_bCanBeRestarted;	// 是否能重新普通攻击
	bool					m_bIsForbitAutoNA;	// 是否禁止自动普攻
	//bool					m_bIsAniDelayed;	//是否在播放攻击动作时暂停普通攻击并终止了动作, 这样就需要在恢复时重新播放动作.
	CSingleHandNATick*		m_pMainhand;
	CSingleHandNATick*		m_pAssistanthand;
};

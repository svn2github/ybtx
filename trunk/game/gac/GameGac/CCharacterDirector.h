#pragma once
#include "CCharacterFollower.h"
#include "PatternCOR.h"
#include "CTick.h"
#include "CActorDirector.h"
#include "CEntityMallocObject.h"

namespace sqr
{
	class CCoreObjectDirector;
}

class CMoveAccelerator;
class CAutoTracker;
class IAutoTrackerHandler;
class CFighterViewInfo;
class CClientCharacterMsgDispatcher;

class CCharacterDirector
	:public CCharacterFollower
	,public CPtCORHandler
{
public:
	friend class CMoveAccelerator;
	friend class CClientCharacterMsgDispatcher;

	CCharacterDirector(CCoreObjectDirector* pCoreObj, uint32 uID);
	virtual ~CCharacterDirector();

	virtual CCharacterDirector* CastToCharacterDirector() { return this; }

	CActorDirector* GetActor() const;
	CCharacterFollower* GetCharacterFromAdapter(ICoreObjectFollowerHandler* pHandler)const;

	virtual EGameEntityType GetGameEntityType()const { return eGET_CharacterDirector; }
	void SetCurSpeed(float fSpeed);
	float GetCurSpeed()const;
	float GetMaxSpeed()const;
	EMoveToResult MoveToInLinePath(const CFPos& PixelPosDes, EFindPathType eFindPathType,EBarrierType eBarrierType);
	EMoveToResult MoveToInAStarPath(const CFPos& PixelPosDes, EFindPathType eFindPathType, EBarrierType eBarrierType, float fReachDist, int32 iMaxStep);
	void StopMoving();

	virtual void OnPosChanged();
	virtual void DoMoveState(EMoveState eMoveState, bool bForce = false);
	virtual void DoActionState(EActionState eActionState);
	virtual EMoveState GetMoveState();
	virtual EActionState GetActionState();

	//added by gao
	virtual void DoFunActionState(TCHAR* actionName);
	virtual void EndFunActionState();
	//end

	void CreateAccelerator();
	virtual void OnMoveBegan();
	virtual void OnMoveEnded(EMoveEndedType eMEType);
	
	virtual void OnDestroy();
	void OnTransfered(CCoreObjectFollower* pCoreObj);

	// 注册给lua
	bool		IsEnemy(CCharacterFollower* pChar);
	bool		IsFriend(CCharacterFollower* pChar);
	bool		IsNeutral(CCharacterFollower* pChar);
	void		SetMHNACfg(const TCHAR* szName,float fMainHandWeaponInterval,const TCHAR* MHWeaponType,const TCHAR* szAttackType);
	void		SetAHNACfg(const TCHAR* szName, float fAssistantWeaponInterval, const TCHAR* szWeaponType);

	EConsumeType	GetConsumeType(const TCHAR* SkillName);
	uint32		GetConsumeValue(const TCHAR* SkillName, uint32 SkillLevel);
	uint32		GetSkillDistance(const TCHAR* SkillName, uint32 SkillLevel);
	ECastingProcessType		GetCastingProcessType(const TCHAR* SkillName);
	float		GetCastingProcessTime(const TCHAR* SkillName, uint32 SkillLevel);
	float		GetCoolDownTime(const TCHAR* SkillName, uint32 SkillLevel);
	uint32		GetSkillCoolDownType(const TCHAR* SkillName);
	uint32		GetSkillLeftCoolDownTime(const TCHAR* SkillName);
	bool		IsInCommonCD(const TCHAR* SkillName);
	bool		IsSwitchEquipSkill(const TCHAR* SkillName);
	uint32		GetFireCondition(const TCHAR* SkillName);
	float		GetNormalAttackDistance() const;
	uint32		GetTabDistance();
	void		CancelCastingProcess();

	void		AddLearnSkill(const TCHAR* SkillName,uint32 SkillLevel);
	void		DelLearnSkill(const TCHAR* SkillName,uint32 SkillLevel);
	void		ClearAllLearnSkill();
	EDoSkillResult	FightSkill(const TCHAR* szName, uint8 uSkillLevel);
	void		UseTempSkill(const TCHAR* szName, uint8 uSkillLevel, uint32 uPos);
	void		DoAttack(const TCHAR* szSkillName, uint8 uSkillLevel, uint32 uPos, uint32 uDir, uint32 uMagicDir);
	void		CancelCoerceMove(uint32 uID);
	bool		DoNormalAttack(uint32 uTargetID, bool bIsNeedAutoTracker = true);
	bool		CheckDoNormalAttack(uint32 uTargetID);
	//void		SetAutoNormalAttack(bool bIsAutoNormalAttack);
	//bool		IsAutoNormalAttack();
	void		SetTarget(uint32 uTargetID);
	CEntityClient* GetTarget()const;
	bool		IsNormalAttacking() const;
	void		CancelNormalAttack();
	CCharacterFollower*	GetNormalAttackTarget();
	void		SetTabDistance(uint32 uTabDistance);
	bool		IsCastingTypeSkill(const TCHAR* szSkillName);
	bool		CanUseNonFightSkill(const TCHAR* szSkillName, uint8 uSkillLevel);
	bool		CanUseSkill(const TCHAR* szSkillName, uint8 uSkillLevel);
	void		TurnAroundByNonFightSkill(const TCHAR* szSkillName);
	void		SetTargetPos(const CFPos& PixelPos);

	void		LockObj(uint32 uTargetID);
	void		OpenObjPanel(uint32 uObjID, CFighterViewInfo* pViewInfo, bool bIsTargetPanel, bool IsOpen);
	void		CancelState(uint32 uID);
	void		DestroyServant();
	bool		IsCanCancelState(const TCHAR*  uName);
	bool		ExistState(const TCHAR*  uName);
	void		SortTargetList();
	void		SelectFriendTarget();
	void		SelectAimTarget();
	void		NotifyServerChangeDir(const CFPos& PixelPosDes);
	void		GetTargetCastingProcess(uint32 uTargetID);
	void		GetTargetOfTarget(uint32 uTargetID);

	void		ChangeShowHurtMOD();
	uint32		GetMoveDir();
	bool		ActionIsTurnOver();
	
	virtual CFighterFollower* GetSelectedFighter()const;

	virtual void OnDead();
	virtual void OnMagicCondFail(uint32 uMessageID);

	void RefreshRelationList();
	void SortTargetListOnList(list<uint32>&listCharacterFollower);
	void InsertTargetToFollwerList(CCharacterFollower* pCharacter, list<uint32>&listCharacterFollower);
	void AddCharacterFollowerToSet(CCharacterFollower* pCharacter);
	void DelCharacterFollowerFromSet(CCharacterFollower* pCharacter);
	void ChangeCharacterFollowerList(CCharacterFollower* pCharacter);
	void OnCOREvent(CPtCORSubject* pSubject,uint32 uEvent,void* pArg);

	void TurnAndSyncDir(uint8 uDir);
	void ChangeMoveMode(bool bIsWalkMode);
	void ChangeDrawWeaponMode(bool bForceDraw = false);
	void ChangeNormalAttacking(bool bIsNormalAttacking);
	uint8 GetActionDir();

	void CreateFacingPair(CCharacterFollower* pCharSrc, CCharacterFollower* pCharDst);
	void RemoveFacingPair();

	bool InitAutoTracker(uint32 uAttacker, uint32 uTarget, float fReachGridDist, IAutoTrackerHandler* pHandler, bool bIsNormalAttackTracker=false);
	void CancelAutoTracker(bool bCancelNormalAttackOnly);
	bool IsNormalAttackTracker();
	bool IsAutoTracking()const;
	void SetTransfer(bool bTransfer){m_bTransfer=bTransfer;}
	void ShowHeadBloodDialog(bool bShow);
	bool BeInTransfer();
	void SetMovingByKeyboard(bool bMovingByKeyboard);
	bool IsMovingByKeyboard() const {return m_bMovingByKeyboard;}
	CNPCGazedAtMgr*	GetNPCGazedAtMgr()	{ return m_pNPCGazedAtMgr; }

	void CppSetDoSkillCtrlStateForLua(EDoSkillCtrlState eState,bool bSet);
	bool CppGetDoSkillCtrlState(EDoSkillCtrlState eState);

	//只有director才有召唤兽管理器
	CCharacterFollower* GetServant(ENpcType eType) const; 
	CCharacterFollower* GetServantByID(uint32 uServant);
	void AddServant(ENpcType eType, uint32 uServantID);
	void RemoveServant(ENpcType eType, uint32 uServantID);
	void RemoveServantByType( ENpcType eType );
	uint32 GetFirstServantClient();
	CCharacterFollower* GetServantByType( ENpcType eType ) const;

	void AddFXCache(const TCHAR* szSkillName);
	void RemoveFXCache(const TCHAR* szSkillName);
	void ClearAllFXCache();
private:
	ESetSpeedResult SetCoreObjSpeed(float fSpeed);

	// 主角开始移动时会由慢变快，m_fRealCurMoveSpeed是由慢变快过程中每时每刻的真实速度，
	float m_fRealCurMoveSpeed;	// 真实速度
	CMoveAccelerator*			m_pMoveAccelerator;
	CNPCGazedAtMgr*				m_pNPCGazedAtMgr;
	CAutoTracker*				m_pAutoTracker;
	list<uint32>				m_FriendCharacterFollower;
	list<uint32>				m_AimCharacterFollower;
	list<uint32>				m_OtherCharacterFollower; //其他关系Follower且非中立
	uint32						m_uTargetID;
	uint32						m_uTabDistance;
	bool						m_bTransfer;
	bool						m_bMovingByKeyboard;
	CPos						m_LastPosition;
	CServantClientMgr*			m_pServantMgr;
};


//主角的移动速度在由静止到移动的过程中会由小变大，
//由于Director被剥夺自主权后不能通过SetSpeed函数来设置速度了，只能通过额外的RPC来设置速度
class CMoveAccelerator
	:public CTick 
	,public CEntityMallocObject
{
public:
	explicit CMoveAccelerator(CCharacterDirector* pCharacter);
	~CMoveAccelerator();
	void OnTick();
	bool AccelerateSpeed();
private:
	static uint32 ms_uCount;

	uint32 m_uCount;
	CCharacterDirector* m_pCharacter;
};

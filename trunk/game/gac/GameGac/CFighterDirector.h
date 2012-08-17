#pragma once
#include "CFighterFollower.h"
#include "TFighterAllocator.h"

class CGas2GacCPP_OnMagicCondFail;
class CGas2GacCPP_OnCoolDownBegin;
class CGas2GacCPP_OnCoolDownEnd;
class CGas2GacCPP_OnAllCoolDownEnd;
class CGas2GacCPP_OnClearCommonCD;
class CGas2GacCPP_OnCancelNormalAttack;
class CGas2GacCPP_OnSetAutoNormalAttack;
class CGas2GacCPP_OnDoNormalAttack;
class CGas2GacCPP_OnCalValueChanged;
class CGas2GacCPP_OnArrayValueChange;
class CGas2GacCPP_OnArrayCreate;
class CGas2GacCPP_OnPrintFightInfo;
class CGas2GacCPP_OnAttackTypeChanged;
class CGas2GacCPP_OnSetNoSingTime;
class CGas2GacCPP_UpdateSmashRate;
class CGas2GacCPP_OnSyncToSelfCalValueChanged;
class CGas2GacCPP_OnReplaceSkill;
class CGas2GacCPP_OnUseSkillStateChanged;
class CGas2GacCPP_OnRevertSkillCtrlState;
class CGas2GacCPP_OnTalentChanged;
class CGas2GacCPP_OnRemoveTalent;
class CGas2GacCPP_OnBurstSoulTimesChanged;
class CGas2GacCPP_OnAddServant;
class CGas2GacCPP_OnDestroyServant;

class ICharacterDirectorCallbackHandler;
class CNormalAttackClient;
class CCoolDownMgrClient;
class IFighterDirectorHandler;
class CFighterViewInfo;
class CSkillCondMgrClient;
class CCharacterDirector;
class CFightInfoData;
class CSkillMgrClient;
class CSkillClientSharedPtr;

class CFighterDirector
	: public CFighterFollower
{
public:
	friend class CCharacterDirector;
	CFighterDirector();
	virtual ~CFighterDirector();
	virtual CFighterDirector* CastToFighterDirector() { return this; }
	typedef map<uint32, CFighterViewInfo*,less<uint32>, TFighterAllocator<pair<uint32,CFighterViewInfo*> > >		MapCFighterViewInfoById;
	typedef map<string, uint32,less<string>, TFighterAllocator<pair<string,uint32*> > >					MapLearnSkill;
	typedef set<uint32,less<uint32>,TFighterAllocator<uint32> >	SetNoSingTimeSkill;
	ICharacterDirectorCallbackHandler*			GetCallBackHandler()const;
	IFighterDirectorHandler*	GetHandler()const;

	void				SetMHNACfg(const TCHAR* szName,float fMainHandWeaponInterval,EWeaponType MHWeaponType, EAttackType eAttackType);
	void				SetAHNACfg(const TCHAR* szName,float fAssistantWeaponInterval, EWeaponType AHWeaponType);
	CFighterFollower*	GetNATarget()const;
	void				CancelNormalAttackAutoTracking();
	bool				IsNormalAttacking();
	//bool				IsAutoNormalAttack();
	//void				SetAutoNormalAttack(bool bIsAutoNormalAttack);
	void				CancelNormalAttack();
	CCharacterFollower*	GetNormalAttackTarget();
	uint32				CheckNACondAndSetNATarget(CFighterFollower* pTarget);
	bool				IsShortDistNA()const;
	CNormalAttackClient*GetNormalAttack()const;

	uint32				PreUseSkill(const CSkillClientSharedPtr& pSkillClient, uint32 uSkillLevel);
	uint32				PreAttack(const TCHAR* SkillName,uint32 SkillLevel);
	EDoSkillResult		FightSkill(const TCHAR* szName, uint8 uSkillLevel,uint8 uPos);
	void				DoAttack(const TCHAR* szSkillName, uint8 uSkillLevel,uint8 uPos,uint8 uDir=0, uint8 uMagicDir = 0);
	bool				IsCastingTypeSkill(const TCHAR* szSkillName);
	void				DoNormalSkill( uint32 uSkillID, uint8 uSkillLevel, const CFPos& fPosFrom, uint8 uPos, uint8 uDir, uint8 uMagicDir);
	bool				DoNormalAttack(uint32 uTargetID,bool bIsNeedAutoTracker);
	bool				CheckDoNormalAttack(uint32 uTargetID);
	void				TurnAround(const TCHAR* szSkillName, CFPos posFrom, CFPos posTo, CFighterFollower* pTarget);
	void				TurnAroundByNonFightSkill(const TCHAR* szSkillName);

	float				NewCoolDownTime(const TCHAR* SkillName,uint32 SkillLevel, uint32 uLeftTime, bool bIsSwitchEquipSkill);
	uint32				ResetSingleCoolDownTime(const TCHAR* SkillName);

	EConsumeType		GetConsumeType(const TCHAR* SkillName);
	uint32				GetConsumeValue(const TCHAR* SkillName,uint32 SkillLevel);
	uint32				GetSkillDistance(const TCHAR* SkillName,uint32 SkillLevel);
	ECastingProcessType	GetCastingProcessType(const TCHAR* SkillName);
	float				GetCastingProcessTime(const TCHAR* SkillName,uint32 SkillLevel);
	uint32				GetSkillCoolDownType(const TCHAR* SkillName);
	float		 		GetCoolDownTime(const TCHAR* SkillName,uint32 SkillLevel);
	uint32				GetSkillLeftCoolDownTime(const TCHAR* SkillName);
	bool				IsInCommonCD(const TCHAR* SkillName);
	bool				IsSwitchEquipSkill(const TCHAR* SkillName);
	uint32				GetFireCondition(const TCHAR* SkillName);
	float				GetNormalAttackDistance() const;

	uint32				GetPositionArea(const TCHAR* SkillName,uint32 SkillLevel);
	void				CancelState(uint32 uID);
	void				DestroyServant();
	void				CancelCoerceMove(uint32 uID);
	void				SetTargetPos(const CFPos& Pos);
	void				SaveObjPanelByID(uint32 uObjID, CFighterViewInfo* pViewInfo, bool bIsTargetPanel);
	void				DeleteObjPanelByID(uint32 uObjID, bool bIsTargetPanel);
	void				OnOpenObjPanel(uint32 uObjID, EAttackType eAttackType, uint32 uPropertyID);
	void				AddLearnSkill(const TCHAR* SkillName,uint32 SkillLevel);
	void				DelLearnSkill(const TCHAR* SkillName,uint32 SkillLevel);
	void				ClearAllLearnSkill();
	void				CondTestOnTick();
	void				CancelCastingProcess();
	void				GetTargetCastingProcess(uint32 uTargetID);
	void				GetTargetOfTarget(uint32 uTargetID);
	void				NotifyBurstSoulArrived(EBurstSoulType eBurstSoulType);
	uint32				GetBurstSoulTimes(){return m_uBurstSoulTimes;}
	float				GetMainHandWeaponInterval()const;
	float				GetAssistantWeaponInterval()const;
    void				StopNormalAttack(); 
	void				SetCurrentSkillAttackType(EAttackType eAttackType){ m_pCurrentSkillAttackType = eAttackType; };
	EAttackType			GetCurrentSkillAttackType()const{ return m_pCurrentSkillAttackType; };
	void				SetDoSkillCtrlState(EDoSkillCtrlState eSkillCtrlState, bool bSet);
	bool				GetDoSkillCtrlState(EDoSkillCtrlState eSkillCtrlState);
	void				RevertSkillCtrlState();
	void AddFXCache(const string& strSkillName);
	void RemoveFXCache(const string& strSkillName);

	virtual void OnCtrlVariantChanged(uint32 uValue, uint32 uValueTypeID, bool bSet, bool bIsAlive);

	template<typename CmdType>
	bool						SendToGas(const CmdType* pCmd)const;
	void OnFighterCommand(const CGas2GacCPP_OnMagicCondFail* pCmd);
	void OnFighterCommand(const CGas2GacCPP_OnCoolDownBegin* pCmd);
	void OnFighterCommand(const CGas2GacCPP_OnCoolDownEnd* pCmd);
	void OnFighterCommand(const CGas2GacCPP_OnAllCoolDownEnd* pCmd);
	void OnFighterCommand(const CGas2GacCPP_OnClearCommonCD* pCmd);
	void OnFighterCommand(const CGas2GacCPP_OnCancelNormalAttack* pCmd);
	void OnFighterCommand(const CGas2GacCPP_OnDoNormalAttack* pCmd);
	void OnFighterCommand(const CGas2GacCPP_OnSetAutoNormalAttack* pCmd);
	void OnFighterCommand(const CGas2GacCPP_OnCalValueChanged* pCmd);
	void OnFighterCommand(const CGas2GacCPP_UpdateSmashRate* pCmd);
	virtual void OnFighterCommand(const CGas2GacCPP_OnSetNormalSkillTarget* pCmd);
	virtual void OnFighterCommand(const CGas2GacCPP_OnLevelChanged* pCmd);
	virtual void OnFighterCommand(const CGas2GacCPP_OnCampChanged* pCmd);
	virtual void OnFighterCommand(const CGas2GacCPP_OnPKStateChanged* pCmd);
	virtual void OnFighterCommand(const CGas2GacCPP_OnGameCampChanged* pCmd);
	void OnFighterCommand(const CGas2GacCPP_OnArrayValueChange* pCmd);
	void OnFighterCommand(const CGas2GacCPP_OnArrayCreate* pCmd);
	void OnFighterCommand(const CGas2GacCPP_OnPrintFightInfo* pCmd);
	void OnFighterCommand(const CGas2GacCPP_OnAttackTypeChanged* pCmd);
	void OnFighterCommand(const CGas2GacCPP_OnSetNoSingTime* pCmd);
	void OnFighterCommand(const CGas2GacCPP_OnSyncToSelfCalValueChanged* pCmd);
	void OnFighterCommand(const CGas2GacCPP_OnReplaceSkill* pCmd);
	void OnFighterCommand(const CGas2GacCPP_OnUseSkillStateChanged* pCmd);
	void OnFighterCommand(const CGas2GacCPP_OnRevertSkillCtrlState* pCmd);
	void OnFighterCommand(const CGas2GacCPP_OnTalentChanged* pCmd);
	void OnFighterCommand(const CGas2GacCPP_OnRemoveTalent* pCmd);
	void OnFighterCommand(const CGas2GacCPP_OnBurstSoulTimesChanged* pCmd);
	void OnFighterCommand(const CGas2GacCPP_OnAddServant* pCmd);
	void OnFighterCommand(const CGas2GacCPP_OnDestroyServant* pCmd);
private:
	void TellMedDoNormalAttack(uint32 uTargetID,bool bIsAutoNormalAttack, bool bToBeStarted);
	void TellMedStartOrRestartNADueToSkillFailure(uint32 uSkillID)const;


private:
	CCoolDownMgrClient*		m_pCoolDownMgr;
	CNormalAttackClient*	m_pNormalAttack;
	CSkillCondMgrClient*	m_pSkillCondMgrClient;
	
	EAttackType				m_pCurrentSkillAttackType;
	MapCFighterViewInfoById	m_mapCFighterViewInfoById;	//角色对象信息map,保存客户端所有打开面板的对象
	MapLearnSkill			m_mapLearnSkill;
	CFightInfoData*			m_pFightInfoData;
	//吟唱时间相关变量
	bool					m_bNoSingTimeAll;				//下一次所有吟唱时间为0
	SetNoSingTimeSkill				m_setNoSingTimeSkill;			//下一次吟唱时间为0的技能
	uint32					m_uBurstSoulTimes;
	CSkillMgrClient*		m_pSkillMgrClient;
	bool					IsNoSingTime(uint32 uSkillID)	{return m_bNoSingTimeAll || m_setNoSingTimeSkill.find(uSkillID) != m_setNoSingTimeSkill.end();}	//是否吟唱时间为0

	void					YieldTick();
	void					ResumeTick();
};

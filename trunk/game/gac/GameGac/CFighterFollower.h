#pragma once
#include "CPos.h"
#include "CFighterAgileInfoClient.h"
#include "TFighterCtrlInfo.h"
#include "CFighterCalInfo.h"
#include "TPropertyHolder.h"
#include "PatternCOR.h"
#include "CDistortedTick.h"
#include "CFighterMallocObject.h"

//class CGas2GacCPP_OnCtrlValueChanged;
class CGas2GacCPP_OnLevelChanged;
class CGas2GacCPP_OnClassChanged;
class CGas2GacCPP_OnCampChanged;
class CGas2GacCPP_OnPKStateChanged;
//class CGas2GacCPP_OnZoneTypeChanged;
class CGas2GacCPP_OnDisplayFightInfo;
class CGas2GacCPP_OnDisplayMagicFightInfo;
class CGas2GacCPP_OnDisplayMsg;
class CGas2GacCPP_OnDoMagicOp;
class CGas2GacCPP_OnClearState;
class CGas2GacCPP_OnDeleteState;
class CGas2GacCPP_OnSetState;
class CGas2GacCPP_OnSetNormalSkillTarget;
class CGas2GacCPP_OnSkillBegin;
class CGas2GacCPP_OnGameCampChanged;
class CGas2GacCPP_OnCanBeTakeOver;
class CGas2GacCPP_OnCanNotTakeOver;
class CGas2GacCPP_OnCastMagicSkill;
class CGas2GacCPP_OnCancelMagicSkill;
class CGas2GacCPP_OnLostTarget;
class CGas2GacCPP_OnRealNameChange;
class CGas2GacCPP_OnSyncCastingProcess;

class IFighterFollowerHandler;
class ICharacterFollowerCallbackHandler;
class CMagicStateMgrClient;
template<typename StateType>
class COtherStateMgrClient;
class CTriggerStateClient;
class CDamageChangeStateClient;
class CCumuliTriggerStateClient;
class CSpecialStateClient;
class CTalentHolder;
class CFighterDirector;
class CAllStateMgrClient;
class CPlayerFX;
class CObjectShake;
class CLeadingFX;
class CCharacterFollower;
class CEntityClient;
class CFighterHealthPointVariantCallback;
class CFighterConsumePointVariantCallback;
class CFighterCastingBeginVariantCallback;
class CFighterCastingEndVariantCallback;
class CFighterSetStateVariantCallback;
class CFighterDelStateVariantCallback;
class CFighterClearAllStateVariantCallback;
class CFighterCtrlValueVariantCallback;

namespace sqr{
class CCypherVariant;
}


class CFighterFollower
	: public CFighterAgileInfo
	, public TFighterCtrlInfo<BoolType>
	, public CFighterSyncToDirectorCalInfo
	, public TPropertyHolder<CFighterFollower>
	, public CPtCORSubject
	, public CPtCORHandler
	, public CFighterMallocObject
{
	//friend class SetZoneTypeVariantCallback;
public:
	typedef CFighterAgileInfo	AgileInfo_t;
	CFighterFollower();
	virtual ~CFighterFollower();
	virtual CFighterDirector* CastToFighterDirector() { return NULL; }
	void SetHandler(IFighterFollowerHandler* pHandler);
	IFighterFollowerHandler* GetHandler() const { return m_pHandler; }
	ICharacterFollowerCallbackHandler*	GetCallBackHandler()const;

	CTalentHolder*				GetTalentHolder()const		{return m_pTalentHolder;}

	CMagicStateMgrClient*							GetMagicStateMgrClient()const		{return m_pMagicStateMgrClient;}
	COtherStateMgrClient<CTriggerStateClient>*		GetTriggerStateMgrClient()const		{return m_pTriggerStateMgrClient;}
	COtherStateMgrClient<CDamageChangeStateClient>*	GetDamageChangeStateMgrClient()const{return m_pDamageChangeStateMgrClient;}
	COtherStateMgrClient<CCumuliTriggerStateClient>*		GetCumuliTriggerStateMgrClient()const		{return m_pCumuliTriggerStateMgrClient;}
	COtherStateMgrClient<CSpecialStateClient>*		GetSpecialStateMgrClient()const		{return m_pSpecialStateMgrClient;}
	CAllStateMgrClient*					GetAllStateMgr() const		{return m_pAllStateMgrClient;}

	void								SetTarget( CFighterFollower* pTarget );
	void								SetTargetPos(const CFPos& pos ){m_TargetPos = pos;}
	CFighterFollower*					GetTarget()const			{return	m_Target.Get();}
	void								GetTargetPos( CFPos& pos )const	{pos = m_TargetPos;}
	
	CCharacterFollower* GetCharacter()const;
	uint32 GetEntityID()const;
	CEntityClient* GetEntity()const;

	uint32								GetTalentPoint(const string&  szName);
	
	void								OnCOREvent(CPtCORSubject* pSubject, uint32 uEvent,void* pArg);
	void								SetTargetBuff();
	float								GetRemainTime(const TCHAR* sName, uint32 uId);
	bool								ExistState(const TCHAR* sName);
	bool								IsCanCancelState(const TCHAR*  uName);
	void								ClearAllState();

	CPlayerFX* GetCastingFX()const {return m_pCastingFX;}
	CPlayerFX* GetMagicFX()const {return m_pMagicFX;}
	CLeadingFX* GetLeadingFX()const {return m_pLeadingFX;}

	void CppSetClass(EClass eClass);
	void CppSetCamp(ECamp eCamp);
	void CppSetLevel(uint32 uLevel);
	void CppSetBirthCamp(ECamp eCamp);
	void CppSetPKState(bool bPKState);
	//void CppSetPKState(EPKState ePKState);
	//void CppSetZoneType(EZoneType eZoneType);

	void OnCastBegin(uint32 uFromID, uint32 uID, uint32 uTargetID, float fCastTime, float fExpiredTime, bool bIsSync);
	void OnCastEnd(uint32 uSkillID, uint32 uId, bool bIsFinish);
	void InitVariantCallback();
	void InitAgileValue();
	void CppSetGameCamp(int32 iGameCamp);
	void OnSetState(uint32 uCategoryId, uint32 uDynamicId, uint32 uCount, int64 iTime, int64 iGuessStartTime, uint32 uSkillLevel, uint32 uInstallerID, int32 iCalcValue);
	void OnDeleteState(uint32 uCategoryId, uint32 uDynamicId);
	void OnClearAllState();
	virtual void OnCtrlVariantChanged(uint32 uValue, uint32 uValueTypeID, bool bSet, bool bIsAlive);
	void RegistDistortedTick(CDistortedTick* pTick, uint32 uCyc);
	void UnRegistDistortedTick(CDistortedTick* pTick);
	uint64 GetDistortedProcessTime();
	uint64 GetDistortedFrameTime();
	uint64 GetDistortedServerFrameTime();

	//void OnFighterCommand(const CGas2GacCPP_OnCtrlValueChanged* pCmd);
	virtual void OnFighterCommand(const CGas2GacCPP_OnLevelChanged* pCmd);
	void OnFighterCommand(const CGas2GacCPP_OnClassChanged* pCmd);
	virtual void OnFighterCommand(const CGas2GacCPP_OnCampChanged* pCmd);
	virtual void OnFighterCommand(const CGas2GacCPP_OnPKStateChanged* pCmd);
	//void OnFighterCommand(const CGas2GacCPP_OnZoneTypeChanged* pCmd);
	void OnFighterCommand(const CGas2GacCPP_OnDisplayFightInfo* pCmd);
	void OnFighterCommand(const CGas2GacCPP_OnDisplayMagicFightInfo* pCmd);
	void OnFighterCommand(const CGas2GacCPP_OnDisplayMsg* pCmd);
	void OnFighterCommand(const CGas2GacCPP_OnDoMagicOp* pCmd);
	void OnFighterCommand(const CGas2GacCPP_OnClearState* pCmd);
	void OnFighterCommand(const CGas2GacCPP_OnDeleteState* pCmd);
	void OnFighterCommand(const CGas2GacCPP_OnSetState* pCmd);
	virtual void OnFighterCommand(const CGas2GacCPP_OnGameCampChanged* pCmd);
	void OnFighterCommand(const CGas2GacCPP_OnCanBeTakeOver* pCmd);
	void OnFighterCommand(const CGas2GacCPP_OnCanNotTakeOver* pCmd);
	virtual void OnFighterCommand(const CGas2GacCPP_OnSetNormalSkillTarget* pCmd);
	virtual void OnFighterCommand(const CGas2GacCPP_OnSkillBegin* pCmd);
	void OnFighterCommand(const CGas2GacCPP_OnCastMagicSkill* pCmd);
	void OnFighterCommand(const CGas2GacCPP_OnCancelMagicSkill* pCmd);
	void OnFighterCommand(const CGas2GacCPP_OnLostTarget* pCmd);
	void OnFighterCommand(const CGas2GacCPP_OnRealNameChange* pCmd);
	void OnFighterCommand(const CGas2GacCPP_OnSyncCastingProcess* pCmd);

	static bool OnSetStateForInit(const void* pHandler, const TCHAR* sName, CCypherVariant& pVariant);

protected:
	IFighterFollowerHandler*		m_pHandler;
	CFPos												m_TargetPos;
	TPtRefer<CFighterFollower, CFighterFollower>	m_Target;
	TPtRefee<CFighterFollower, CFighterFollower>	m_RefsByFighter;
	CTalentHolder*										m_pTalentHolder;
	CAllStateMgrClient*									m_pAllStateMgrClient;
	CMagicStateMgrClient*								m_pMagicStateMgrClient;			// 魔法状态管理器
	COtherStateMgrClient<CTriggerStateClient>*			m_pTriggerStateMgrClient;		// 触发器魔法状态管理器
	COtherStateMgrClient<CDamageChangeStateClient>*		m_pDamageChangeStateMgrClient;	// 伤害变更魔法状态管理器
	COtherStateMgrClient<CCumuliTriggerStateClient>*			m_pCumuliTriggerStateMgrClient;		// 触发器魔法状态管理器
	COtherStateMgrClient<CSpecialStateClient>*			m_pSpecialStateMgrClient;		// 特殊魔法状态管理器
	CPlayerFX*		m_pCastingFX;
	CLeadingFX*	m_pLeadingFX;
	CPlayerFX*		m_pMagicFX;

	CFighterHealthPointVariantCallback*					m_pHealthPointVariantCallback;
	CFighterConsumePointVariantCallback*				m_pConsumePointVariantCallback;
	CFighterCastingBeginVariantCallback*				m_pCastBeginVariantCallback;
	CFighterCastingEndVariantCallback*					m_pCastEndVariantCallback;
	CFighterCtrlValueVariantCallback*					m_CtrlValueVariantCallback;

public:
	CFighterSetStateVariantCallback*					m_pSetStateVariantCallback;
	CFighterDelStateVariantCallback*					m_pDelStateVariantCallback;
	CFighterClearAllStateVariantCallback*				m_pClearAllStateVariantCallback;
};

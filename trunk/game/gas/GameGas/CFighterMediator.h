#pragma once
#include "CFighterDictator.h"

class CPropertyTransactionMgr;
class CBurstSoulMgrServer;
class ICharacterMediatorCallbackHandler;
class CPlayerSkillMgrServer;
class CSkillReplaceServer;
class CFightStatisticMgr;
class CActivityValueManager;

class CGac2GasCPP_DoSkill;
class CGac2GasCPP_StopNormalAttack;
class CGac2GasCPP_DoNormalAttack;
class CGac2GasCPP_StartOrRestartNADueToSkillFailure;
class CGac2GasCPP_SetTargetPos;
class CGac2GasCPP_CancelState;
class CGac2GasCPP_CancelCoerceMove;
class CGac2GasCPP_OpenObjPanel;
class CGac2GasCPP_DestroyServant;
class CGac2GasCPP_SetAutoNormalAttack;
class CGac2GasCPP_CancelCastingProcess;
class CGac2GasCPP_GetTargetCastingProcess;
class CGac2GasCPP_GetTargetOfTarget;
class CGac2GasCPP_NotifyBurstSoulArrived;


///////////////////////////////////////////////////////////////////////////////////////////////
//	CFighterMediator类是玩家对象的战斗系统的最终派生类
///////////////////////////////////////////////////////////////////////////////////////////////

class CFighterMediator	
	:public CFighterDictator
{
public:
	CFighterMediator();
	~CFighterMediator();
	void Init();
	CFighterMediator* CastToFighterMediator() { return this; }
	ICharacterMediatorCallbackHandler*			GetCallBackHandler()const;

	void CppInit(EClass eClass, uint32 uLevel, ECamp eCamp);
	void CppInitHPMP(uint32 uCurHP, uint32 uCurMp);
	void CppFullRage(bool bFull);
	void CppDie(CFighterDictator* pAttacker);			// 死亡
	void CppReborn();		// 重生
	void OnLevelUp();
	void TellDirectorToDoNA(const CFighterDictator* pTarget, bool bAutoTrack);

	void OnCaughtViewSightOf(CFighterDictator* pFighter, uint32 uDimension, bool bMultiCast);
	void OnLostViewSightOf(CFighterDictator* pFighter, uint32 uDimension, bool bMultiCast);
	void OnCaughtSyncSightOf(CFighterDictator* pFighter, uint32 uDimension, bool bMultiCast);
	void OnLostSyncSightOf(CFighterDictator* pFighter, uint32 uDimension, bool bMultiCast);
	void ReSendToSelf();
	void ReSendHPMPToSelf();
	void ReSendSkillCoolDownTimeToSelf();
	void OnBeAttacked();
	uint32 GetBeAttackTimes();
	void SetCloseDurabilityChange(bool bClose);
	bool GetCloseDurabilityChange();

	virtual void OnMoveSpeedChange(float fSpeed, const char* szSpeedChangeReason, const char* szSkillName)const;

	virtual void PauseAttack(bool bLockRestart = false, bool bCalDamageImmediately = true);
	virtual bool RestartAttack(bool bUnlockRestart = false);
	virtual void CancelAttack(bool bCalDamageImmediately = false);
	virtual void CancelNormalAttackAutoTracking();

	EDoSkillResult UseFightSkill(const string& szName, uint8 uSkillLevel, uint8 uPos, uint8 uDir, 
uint8 uMagicDir);
	EDoSkillResult DoSkill(const string& szName, uint8 uSkillLevel, uint8 uDir = 0, uint8 uMagicDir = 0);
	EDoSkillResult DoPassiveSkill(const string& szName, uint8 uSkillLevel, uint8 uDir = 0);
	EDoSkillResult PlayerDoPassiveSkill(const TCHAR* sName, uint32 uSkillLevel);

	void SendFailureMsgToGac(uint32 uMsg, const TCHAR* szName = "");
	uint32 GetSkillCoolDownType(const TCHAR* SkillName);
	
	void ReturnArray();
	bool AddTempSkill(const string& strName, uint8 uSkillLevel);
	void RemoveTempSkill(const string& strName, uint8 uSkillLevel);
	void RemoveAllTempSkill();
	void CancelCastingProcess();
	
	uint32 CreateTransactionID();
	void DestroyTransaction(uint32 uTransactionID);
	void EnableTransaction(uint32 uTransactionID);
	void DisableTransaction(uint32 uTransactionID);
	void CommitTransaction(uint32 uTransactionID);
	void ChangeTransactionPropertyValue(uint32 uTransactionID, const string& strPropertyName, float fValue);
	float GetPropertyValue(const string& strPropertyName);
	CPropertyTransactionMgr* GetPropertyTransactionMgr()const {return m_pEquipPropertyMgr;}
	

	void SetArmorValueRate(float fBluntRate,float fPunctureRate,float fChopRate);
	CSkillReplaceServer*	GetSkillReplaceServer() const	{return m_pSkillReplaceServer;}
	CBurstSoulMgrServer*	GetBurstSoulMgrServer() const	{return m_pBurstSoulMgrServer;}
	void OnFightHurt(EFightInfoIndex eFightInfoIndex, uint32 uHurt);
	void OnBeSubHp(EFightInfoIndex eFightInfoIndex, uint32 uHurt);
	void OnBattleStateChanged(bool bEnterBattle);
	void BeginStatistic(const TCHAR* sName);
	void EndStatistic(const TCHAR* sName);
	uint32 GetStatisticFightHurt(const TCHAR* sName);
	uint32 GetStatisticFightHeal(const TCHAR* sName);
	uint32 GetStatisticBeSubHurt(const TCHAR* sName);
	float GetStatisticDps(const TCHAR* sName);
	float GetStatisticHps(const TCHAR* sName);
	uint32 GetStatisticDeadTimes(const TCHAR* sName);
	void ClearAllStatisticData();
	uint32 GetBurstSoulTimes();
	void SetBurstSoulTimes(uint32 uBurstSoulTimes);
	uint32 GetContinuousBurstTimes();

	bool SendToGac(const void* pData,uint32 uSize,uint32 uRange=0)const;

	void OnClientCommand(const CGac2GasCPP_DoSkill* pCmd);
	void OnClientCommand(const CGac2GasCPP_StopNormalAttack* pCmd);
	void OnClientCommand(const CGac2GasCPP_DoNormalAttack* pCmd);
	void OnClientCommand(const CGac2GasCPP_StartOrRestartNADueToSkillFailure* pCmd);
	void OnClientCommand(const CGac2GasCPP_SetTargetPos* pCmd);
	void OnClientCommand(const CGac2GasCPP_CancelState* pCmd);
	void OnClientCommand(const CGac2GasCPP_DestroyServant* Cmd);
	void OnClientCommand(const CGac2GasCPP_CancelCoerceMove* pCmd);
	void OnClientCommand(const CGac2GasCPP_OpenObjPanel* pCmd);
	void OnClientCommand(const CGac2GasCPP_SetAutoNormalAttack* Cmd);
	void OnClientCommand(const CGac2GasCPP_CancelCastingProcess* pCmd);
	void OnClientCommand(const CGac2GasCPP_GetTargetCastingProcess* pCmd);
	void OnClientCommand(const CGac2GasCPP_GetTargetOfTarget* pCmd);
	void OnClientCommand(const CGac2GasCPP_NotifyBurstSoulArrived* pCmd);

	void SetFighterPos(const CFPos& pos);
	const CFPos& GetFighterPos()const;

	void ChangeTargetHP(int32 uHP);

	uint32 GetActivityValue();
	void ClearActivityValue();

private:
	CPropertyTransactionMgr*	m_pEquipPropertyMgr;
	CBurstSoulMgrServer*		m_pBurstSoulMgrServer;
	CSkillReplaceServer*		m_pSkillReplaceServer;
	CFightStatisticMgr*			m_pFightStatisticMgr;
	CActivityValueManager*	m_pActivityValueMgr;
	uint32						m_BeAttackTimes;
	bool						m_bCloseDurabilityChange;
	CFPos						m_FighterPos;
	mutable bool				m_bSpeedIsSuspicious;
};


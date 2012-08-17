#pragma once
#include "CCharacterDictator.h"
#include "CConnServer.h"

namespace sqr
{
	class CCoreObjectMediator;
}

class CSkillCoolDownDBDataMgr;
class CAureMagicDBDataMgr;

class CGac2GasCPP_LockObj;
class CGac2GasCPP_MoveByDirection;
class CGac2GasCPP_SyncActionDir;
class CGac2GasCPP_ChangeMoveMode;
class CGac2GasCPP_ChangeDrawWeaponMode;
class CGac2GasCPP_ChangeNormalAttacking;

class CCharacterMediator : public CCharacterDictator
{
public:
	CCharacterMediator(CCoreObjectMediator *pCoreObj);
	virtual CCharacterMediator* CastToCharacterMediator() { return this; }
	static CCharacterMediator* GetCharacterMediatorByConn(CConnServer* pConn);
	void InitPlayerAoi();
	
	virtual EGameEntityType GetGameEntityType()const { return eGET_CharacterMediator; }

	virtual void Release();

	void CppInit(EClass eClass, uint32 uLevel, ECamp eCamp);
	void CppLevelUp();
	void CppInitHPMP(uint32 uCurHP, uint32 UCurMp);
	void CppFullRage(bool bFull);
	void CppInitByDead();
	void ResetAllCoolDown();
	void AddListeningWnd();
	void DelListeningWnd();
	void SetInGatherProcess(bool bInProcess);
	bool IsInGatherProcess();
	virtual void OnMoveBegan();
	void SetTarget(CCharacterDictator*  pTarget);
	void CppInitMHNA(const TCHAR* szMHName, const TCHAR* MHAttackType, const TCHAR* MHWeaponType, float fAttackSpeed, bool bInitPlayer);
	void CppInitAHNA(const TCHAR* szAHName, const TCHAR* AHAttackType, const TCHAR* AHWeaponType, float fAttackSpeed, bool bInitPlayer);
	void CancelNormalAttack();
	void CancelAutoTracking();

	EDoSkillResult	PlayerDoItemSkill(const TCHAR* sName, uint32 uLevel);
	EDoSkillResult	PlayerDoPosSkill(const TCHAR* sName, uint32 uLevel, const CFPos& Pos);
	EDoSkillResult	PlayerDoFightSkill(const TCHAR* sName, uint32 uLevel);
	EDoSkillResult	PlayerDoPosSkillWithoutLevel(const TCHAR* sName, const CFPos& Pos);
	EDoSkillResult	PlayerDoFightSkillWithoutLevel(const TCHAR* sName);
	EDoSkillResult	PlayerDoPassiveSkill(const TCHAR* sName, uint32 uSkillLevel);

	virtual void OnCaughtViewSightOf(uint32 uObjGlobalId, uint32 uDimension);
	virtual void OnLostViewSightOf(uint32 uObjGlobalId, uint32 uDimension);

	void InitFolBattleHorseViewInfo(CCharacterDictator* pCharBattleHorse, uint32 uObjGlobalId, uint32 uDimension, bool bMultiCast)const;
	void InitFolSyncInfo(CCharacterDictator* pCharacter, uint32 uGlobalID, uint32 uDimension, bool bMultiCast)const;

	virtual void OnTransfered(CCoreSceneServer* pOldScene,const CFPos& OldPos);
	void ReSendSyncInfoToSelf();
	void ReSendSkillCoolDownTimeToSelf();
	void ReSendHPMPToSelf();
	void ReSendToSelf();
	void InitSpeed() const;
	void ReSendSpeedToSelf() const;

	void RemoteInit() const;
	void RemoteInitBattleHorse(CCharacterDictator* pBattleHorse) const;
	void RemoteInitBattleHorseToSelf()const;
	void RemoteUnitBattleHorse(CCharacterDictator* pBattleHorse) const;

	void InsertTalent(const TCHAR* szName, uint32 uPoint);
	void ClearAllTalent();

	void AddEquipSuiteEff(const TCHAR* szEquipSuiteName, uint32 uTalentPoint);
	bool RemoveEquipSuiteEff(const TCHAR* szEquipSuiteName, uint32 uTalentPoint);

	void CreateBattleArray(const TCHAR* szArrayName, const TCHAR* szArrayPos,const TCHAR* szEntityID);
	bool AddTempSkill(const TCHAR* szName, uint8 uSkillLevel);
	void RemoveTempSkill(const TCHAR* szName, uint8 uSkillLevel);
	void RemoveAllTempSkill();

	bool IsAttacking()const;

	void OnCharacterCommand(const CGac2GasCPP_LockObj* pCmd);
	void OnCharacterCommand(const CGac2GasCPP_MoveByDirection* pCmd);
	void OnCharacterCommand(const CGac2GasCPP_SyncActionDir* pCmd);
	void OnCharacterCommand(const CGac2GasCPP_ChangeMoveMode* pCmd);
	void OnCharacterCommand(const CGac2GasCPP_ChangeDrawWeaponMode* pCmd);
	void OnCharacterCommand(const CGac2GasCPP_ChangeNormalAttacking* pCmd);

	CAureMagicDBDataMgr* SerializeAureMagicToDB();
	void SaveAureMagicToDBEnd();
	void LoadAureMagicFromDB(const TCHAR* szAureMagicName, uint8 uSkillLevel, const TCHAR* sSkillName, EAttackType eAttackType);
	CSkillCoolDownDBDataMgr* GetSkillCoolDownTime();
	uint32 GetSkillCoolDownTimeByName(const TCHAR* szSkillName);
	void SaveSkillCoolDownTimeToDBEnd();
	void CPPInitSkillCoolDownTime(CSkillCoolDownDBDataMgr* pSkillCoolDownDBDataMgr);
	uint32 GetSkillCoolDownType(const TCHAR* SkillName);
	void CancelCastingProcess();

	bool IsInPlayerSet(CCharacterMediator* pCharacterMediator);
	//一下两个函数专用于切场景，其他地方不要调用
	void OffBattleHorse();
	void CreateBattleHorse(uint32 uMasterID, const TCHAR* sBattleHorseName, const TCHAR* sNpcType, uint8 uLevel, CFPos* pPos);
	void ClearQuestBeckonNpc();

	virtual void OnEnterBattleState();
	virtual void OnLeaveBattleState();

	void InsertOtherPlayer(CCharacterMediator* pCharacter);
	void EraseOtherPlayer(CCharacterMediator* pCharacter);

	void OnStartDuel(CCharacterMediator* pCharacter);
	void OnFinishDuel(bool bResult);
	CCharacterMediator* GetDuelTarget() const;
	void ChangeMoveSpeedByGMCommand(float fSpeed);

	uint32 GetBeAttackTimes();
	bool GetCloseDurabilityChange();
	void SetCloseDurabilityChange(bool bClose);
	uint32 GetBurstSoulTimes();
	void SetBurstSoulTimes(uint32 uBurstSoulTimes);
	uint32 GetContinuousBurstTimes();
	void SetArmorValueRate(float fBluntRate,float fPunctureRate,float fChopRate);

	uint32 CreateTransactionID();
	void DestroyTransaction(uint32 uTransactionID);
	void EnableTransaction(uint32 uTransactionID);
	void DisableTransaction(uint32 uTransactionID);
	void CommitTransaction(uint32 uTransactionID);
	void ChangeTransactionPropertyValue(uint32 uTransactionID, const TCHAR* szPropertyName, float fValue);
	float GetEquipPropertyValue(const TCHAR* szPropertyName);
	void DoAlertPhaseEffect(CCharacterDictator* pAlertNpc, uint32 uLasTime);
	void DoAlertEndEffect(CCharacterDictator* pAlertNpc);
	void OnMasterDisbandTruck(CCharacterDictator* pTruck);
	void OnMasterDestroyTruck(CCharacterDictator* pTruck);
	virtual void SetStealth(float fStealth);

	bool ChangeScene(const TCHAR* szSceneName,uint32 uNewSceneID, 
							CCoreSceneServer *pScene, const CFPos& PixelPos);
	//主角同服切场景召唤兽存数据库
	void SaveServantInfo();
	void SaveServantInfoOnServantChange();
	void BeginStatistic(const TCHAR * sName);
	void EndStatistic(const TCHAR * sName);
	uint32 GetStatisticFightHurt(const TCHAR* sName);
	uint32 GetStatisticFightHeal(const TCHAR* sName);
	uint32 GetStatisticBeSubHurt(const TCHAR* sName);
	float GetStatisticDps(const TCHAR* sName);
	float GetStatisticHps(const TCHAR* sName);
	uint32 GetStatisticDeadTimes(const TCHAR* sName);
	void ClearAllStatisticData();

	void TriggerSetupWeapon();
	void TriggerChangeEquip();

	float CppGetPropertyValueByName(const TCHAR* szName);

	void CppSuicide();

	uint32 GetActivityValue();
	void ClearActivityValue();

private:
	~CCharacterMediator();
	bool IsSetupWeapon();
	uint8 m_uListeningWndRef;
	bool  m_bInGatherProcess;//采集,开obj过程中
	set<CCharacterMediator*, less<CCharacterMediator*>, TEntityAllocator<CCharacterMediator*> > m_setPlayer;

	CCharacterMediator* m_pDuelTarget;
};

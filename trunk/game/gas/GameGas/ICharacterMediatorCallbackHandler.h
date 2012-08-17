#pragma once
#include "ICharacterDictatorCallbackHandler.h"

namespace sqr
{
	class CCoreSceneServer;
}
class CStateDBData;
class CStoredObjDBData;
class CCharacterDictator;

class ICharacterMediatorCallbackHandler 
	: public ICharacterDictatorCallbackHandler
{
public:
	virtual ICharacterMediatorCallbackHandler* CastToMediatorHandler() { return this; }
	virtual void AddStateToDB(uint32 uObjID, EStateGlobalType eType, CStateDBData* pState) {}
	virtual void AddStoredObjToDB(uint32 uObjID, uint32 uInstID, CStoredObjDBData* pStored) {}
	virtual void AddToDBEnd(uint32 uObjID) {}
	virtual bool ReadStateFromDB(uint32 uObjID, EStateGlobalType eType, CStateDBData* pState) {return false;}
	virtual bool ReadStoredObjFromDB(uint32 uObjID, uint32 uInstID, CStoredObjDBData* pStored) {return false;}
	virtual void ReadFromDBEnd(uint32 uObjID) {}
	
	virtual void OnSetLockObj(uint32 uObjID,uint32 uTargetObjID);
	
	virtual void OnDead(CCharacterDictator* pAttacker, CCharacterDictator* pBeAttacker);
	
	virtual void OnDoNonCombatStateMagicOp(const TCHAR* szMopName, double dArgValue, uint32 uFromEntityID, uint32 uToEntityID);

	
	virtual void OnCancelNonCombatStateMagicOp(const TCHAR* szMopName, double dArgValue, uint32 uFromEntityID, uint32 uToEntityID);

	virtual void ListeningOpenWnd(CCharacterDictator* pCharacter);
	
	virtual void OnCreateTempSkill(uint32 uObjID,const TCHAR* szSkillName, uint8 uSkillLevel, float fTime, uint32 uID);
	
	virtual void OnRemoveTempSkill(uint32 uObjID,uint32 uID);
	
	virtual void OnCastTempSkill(uint32 uObjID,uint32 uID);

	virtual void OnTransfered(CCharacterDictator* pCharacter, CCoreSceneServer* pOldScene, float OldPosX,float OldPosY, CCoreSceneServer* pNewScene);

	virtual void OnRidingHorseChanged( uint32 uEntityID, const TCHAR* szHorseName, const TCHAR* szHorseType );

	virtual void OnForbitUseWeapon( uint32 uEntityID, bool bForbit );

	virtual void OnBreakGatherProcess( uint32 uEntityID );

	virtual void OnItemSkillEnd(uint32 uObjID, const TCHAR* szSkillName);

	virtual void OnBurstSoulPrize(uint32 uObjID1, uint32 uObjID2, float uCount);

	virtual void OnChangeModulus(uint32 uObjID, uint32 uProperty, float fModulus);

	virtual void OnPermitReborn(uint32 uEntityID1, uint32 uEntityID2);

	virtual void OnChangeScene(uint32 uEntityID, const TCHAR* szSceneName, uint32 uPosX, uint32 uPosY);

	virtual void OnFinishDuel(CCharacterMediator* pCharacter, bool bResult);

	virtual void OnAttackTimesChanged(uint32 uObjID, uint32 uBeAttackTimes);

	virtual void OnDestroyed(CCharacterMediator* pPlayer);

	virtual void DoAlertPhaseEffect(CCharacterMediator* pPlayer, CCharacterDictator* pAlertNpc, uint32 uLasTime);

	virtual void DoAlertEndEffect(CCharacterMediator* pPlayer, CCharacterDictator* pAlertNpc);

	virtual void OnMasterDisbandTruck(CCharacterMediator* pMaster, CCharacterDictator* pTruck);

	virtual void OnMasterDestoryTruck(CCharacterMediator* pMaster, CCharacterDictator* pTruck);

	virtual void SaveServantInfoOnServantChange(CCharacterMediator* pMaster);

	virtual void SaveServantInfo(CCharacterMediator* pMaster);

	virtual bool BeClearServantByTransfer(const char* szNewSceneName);

	virtual bool InSceneArea(CCharacterMediator* pPlayer, const char* szSceneAreaName);

	virtual void OnPlayerEnterBattleState(CCharacterMediator* pPlayer);

	virtual void OnPlayerLeaveBattleState(CCharacterMediator* pPlayer);
	virtual void OnLogSpeedChangeReason(CCharacterMediator* pPlayer, float fSpeed, const char* szSpeedChangeReason);

	virtual void OnDoWorldSkill(CCharacterMediator* pPlayer, const char* szSkillName, uint32 uSkillLevel);

	virtual void OnDoCampSkill(CCharacterMediator* pPlayer, const char* szSkillName, uint32 uSkillLevel);

	virtual void OnDoArmyCorpsSkill(CCharacterMediator* pPlayer, const char* szSkillName, uint32 uSkillLevel);

	virtual void OnDoTongSkill(CCharacterMediator* pPlayer, const char* szSkillName, uint32 uSkillLevel);

};

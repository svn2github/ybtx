#include "stdafx.h"
#include "ICharacterMediatorCallbackHandler.h"
#include "CCharacterDictator.h"
#include "CCharacterMediator.h"
#include "CCoreSceneServer.h"
#include "CallBackX.h"

void ICharacterMediatorCallbackHandler::OnSetLockObj(uint32 uObjID,uint32 uTargetObjID)
{ 
	CALL_CLASS_CALLBACK_2(uObjID, uTargetObjID) 
}

void ICharacterMediatorCallbackHandler::OnDead(CCharacterDictator* pAttacker, CCharacterDictator* pBeAttacker)
{ 
	CALL_CLASS_CALLBACK_2(pAttacker, pBeAttacker) 
}

void ICharacterMediatorCallbackHandler::OnDoNonCombatStateMagicOp(const TCHAR* szMopName, double dArgValue, uint32 uFromEntityID, uint32 uToEntityID) 
{ 
	CALL_CLASS_CALLBACK_4(szMopName, dArgValue, uFromEntityID, uToEntityID) 
}

void ICharacterMediatorCallbackHandler::OnCancelNonCombatStateMagicOp(const TCHAR* szMopName, double dArgValue, uint32 uFromEntityID, uint32 uToEntityID) 
{ 
	CALL_CLASS_CALLBACK_4(szMopName, dArgValue, uFromEntityID, uToEntityID) 
}

void ICharacterMediatorCallbackHandler::ListeningOpenWnd(CCharacterDictator* pCharacter)
{
	CALL_CLASS_CALLBACK_1(pCharacter);
}

void ICharacterMediatorCallbackHandler::OnCreateTempSkill(uint32 uObjID,const TCHAR* szSkillName, uint8 uSkillLevel, float fTime, uint32 uID)
{ 
	CALL_CLASS_CALLBACK_5(uObjID, szSkillName, uSkillLevel, fTime, uID) 
}

void ICharacterMediatorCallbackHandler::OnRemoveTempSkill(uint32 uObjID,uint32 uID)
{
	CALL_CLASS_CALLBACK_2(uObjID, uID) 
}

void ICharacterMediatorCallbackHandler::OnCastTempSkill(uint32 uObjID,uint32 uID)
{
	CALL_CLASS_CALLBACK_2(uObjID, uID) 
}

void ICharacterMediatorCallbackHandler::OnTransfered(CCharacterDictator* pCharacter, CCoreSceneServer* pOldScene, float OldPosX,float OldPosY, CCoreSceneServer* pNewScene)
{
	CALL_CLASS_CALLBACK_5(pCharacter, pOldScene, OldPosX, OldPosY, pNewScene);
}

void ICharacterMediatorCallbackHandler::OnRidingHorseChanged( uint32 uEntityID, const TCHAR* szHorseName, const TCHAR* szHorseType ) 
{
	CALL_CLASS_CALLBACK_3(uEntityID, szHorseName, szHorseType) 
}

void ICharacterMediatorCallbackHandler::OnForbitUseWeapon( uint32 uEntityID, bool bForbit )
{
	CALL_CLASS_CALLBACK_2(uEntityID, bForbit) 
}

void ICharacterMediatorCallbackHandler::OnBreakGatherProcess( uint32 uEntityID ) 
{
	CALL_CLASS_CALLBACK_1(uEntityID) 
}

void ICharacterMediatorCallbackHandler::OnItemSkillEnd(uint32 uObjID, const TCHAR* szSkillName)
{
	CALL_CLASS_CALLBACK_2(uObjID, szSkillName)
}

void ICharacterMediatorCallbackHandler::OnBurstSoulPrize(uint32 uObjID1, uint32 uObjID2, float uCount)
{
	CALL_CLASS_CALLBACK_3(uObjID1, uObjID2, uCount)
}

void ICharacterMediatorCallbackHandler::OnChangeModulus(uint32 uObjID, uint32 uProperty, float fModulus)
{
	CALL_CLASS_CALLBACK_3(uObjID, uProperty, fModulus)
}

void ICharacterMediatorCallbackHandler::OnPermitReborn( uint32 uEntityID1, uint32 uEntityID2 )
{
	CALL_CLASS_CALLBACK_2(uEntityID1, uEntityID2)
}

void ICharacterMediatorCallbackHandler::OnChangeScene(uint32 uEntityID, const TCHAR* szSceneName, uint32 uPosX, uint32 uPosY)
{
	CALL_CLASS_CALLBACK_4(uEntityID, szSceneName, uPosX, uPosY)
}

void ICharacterMediatorCallbackHandler::OnFinishDuel(CCharacterMediator* pCharacter, bool bResult)
{
	CALL_CLASS_CALLBACK_2(pCharacter, bResult);	
}

void ICharacterMediatorCallbackHandler::OnAttackTimesChanged(uint32 uObjID, uint32 uBeAttackTimes)
{
	CALL_CLASS_CALLBACK_2(uObjID, uBeAttackTimes)
}

void ICharacterMediatorCallbackHandler::OnDestroyed(CCharacterMediator* pPlayer)
{
	CALL_CLASS_CALLBACK_1(pPlayer);
}

void ICharacterMediatorCallbackHandler::DoAlertPhaseEffect(CCharacterMediator* pPlayer, CCharacterDictator* pAlertNpc, uint32 uLasTime)
{
	CALL_CLASS_CALLBACK_3(pPlayer, pAlertNpc, uLasTime)
}

void ICharacterMediatorCallbackHandler::DoAlertEndEffect(CCharacterMediator* pPlayer, CCharacterDictator* pAlertNpc)
{
	CALL_CLASS_CALLBACK_2(pPlayer, pAlertNpc)
}

void ICharacterMediatorCallbackHandler::OnMasterDisbandTruck(CCharacterMediator* pMaster, CCharacterDictator* pTruck)
{
	CALL_CLASS_CALLBACK_2(pMaster, pTruck)
}

void ICharacterMediatorCallbackHandler::OnMasterDestoryTruck(CCharacterMediator* pMaster, CCharacterDictator* pTruck)
{
	CALL_CLASS_CALLBACK_2(pMaster, pTruck)
}

void ICharacterMediatorCallbackHandler::SaveServantInfoOnServantChange(CCharacterMediator* pMaster)
{
	CALL_CLASS_CALLBACK_1(pMaster)
}

void ICharacterMediatorCallbackHandler::SaveServantInfo(CCharacterMediator* pMaster)
{
	CALL_CLASS_CALLBACK_1(pMaster)
}

bool ICharacterMediatorCallbackHandler::BeClearServantByTransfer(const char* szNewSceneName)
{
	CALL_CLASS_CALLBACK_1_RET(bool, szNewSceneName)
	return true;
}

bool ICharacterMediatorCallbackHandler::InSceneArea(CCharacterMediator* pPlayer, const char* szSceneAreaName)
{
	CALL_CLASS_CALLBACK_2_RET(bool, pPlayer, szSceneAreaName)
	return true;
}

void ICharacterMediatorCallbackHandler::OnPlayerEnterBattleState(CCharacterMediator* pPlayer)
{
	CALL_CLASS_CALLBACK_1(pPlayer)
}

void ICharacterMediatorCallbackHandler::OnPlayerLeaveBattleState(CCharacterMediator* pPlayer)
{
	CALL_CLASS_CALLBACK_1(pPlayer)
}

void ICharacterMediatorCallbackHandler::OnLogSpeedChangeReason(CCharacterMediator* pPlayer, float fSpeed, const char* szSpeedChangeReason)
{
	CALL_CLASS_CALLBACK_3(pPlayer, fSpeed, szSpeedChangeReason)
}

void ICharacterMediatorCallbackHandler::OnDoWorldSkill(CCharacterMediator* pPlayer, const char* szSkillName, uint32 uSkillLevel)
{
	CALL_CLASS_CALLBACK_3(pPlayer, szSkillName, uSkillLevel)
}

void ICharacterMediatorCallbackHandler::OnDoCampSkill(CCharacterMediator* pPlayer, const char* szSkillName, uint32 uSkillLevel)
{
	CALL_CLASS_CALLBACK_3(pPlayer, szSkillName, uSkillLevel)
}

void ICharacterMediatorCallbackHandler::OnDoArmyCorpsSkill(CCharacterMediator* pPlayer, const char* szSkillName, uint32 uSkillLevel)
{
	CALL_CLASS_CALLBACK_3(pPlayer, szSkillName, uSkillLevel)
}

void ICharacterMediatorCallbackHandler::OnDoTongSkill(CCharacterMediator* pPlayer, const char* szSkillName, uint32 uSkillLevel)
{
	CALL_CLASS_CALLBACK_3(pPlayer, szSkillName, uSkillLevel)
}

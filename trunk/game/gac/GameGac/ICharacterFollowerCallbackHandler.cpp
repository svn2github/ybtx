#include "stdafx.h"
#include "ICharacterFollowerCallbackHandler.h"
#include "CallBackX.h"
#include "CCharacterFollower.h"
#include "CFightCallbackData.h"

void ICharacterFollowerCallbackHandler::OnCreated(CCharacterFollower *pObject, bool bIsBattleHorse) {
	CALL_CLASS_CALLBACK_2(pObject, bIsBattleHorse)
}

void ICharacterFollowerCallbackHandler::OnCreatedEnd(CCharacterFollower *pObject) {
	CALL_CLASS_CALLBACK_1(pObject)
}

void ICharacterFollowerCallbackHandler::OnDestroy(CCharacterFollower *pObject) {
	CALL_CLASS_CALLBACK_1(pObject)
}

void ICharacterFollowerCallbackHandler::OnIntoSyncAoi(CCharacterFollower* pObject) 
{
	CALL_CLASS_CALLBACK_1(pObject)
}

void ICharacterFollowerCallbackHandler::OnLeaveSyncAoi(CCharacterFollower* pObject) 
{
	CALL_CLASS_CALLBACK_1(pObject)
}

void ICharacterFollowerCallbackHandler::OnIntoScreen(CCharacterFollower* pObject) 
{
	CALL_CLASS_CALLBACK_1(pObject)
}

void ICharacterFollowerCallbackHandler::OnLeaveScreen(CCharacterFollower* pObject) 
{
	CALL_CLASS_CALLBACK_1(pObject)
}

void ICharacterFollowerCallbackHandler::OnAgileValueChanged(uint32 uEntityID)
{
	CALL_CLASS_CALLBACK_1(uEntityID)
}

void ICharacterFollowerCallbackHandler::OnCtrlValueChanged(uint32 uEntityID, uint32 uValueTypeID, bool bSet) 
{
	CALL_CLASS_CALLBACK_3(uEntityID, uValueTypeID, bSet)
}
void ICharacterFollowerCallbackHandler::OnCastingBarBegin(uint32 uEntityID,const TCHAR* szSkillName,ECastingProcessType eCastType,
														  float fCastTime,float fExpiredTime,bool bIsSync) 
{
	CALL_CLASS_CALLBACK_6(uEntityID, szSkillName, eCastType, fCastTime, fExpiredTime, bIsSync)
}

void ICharacterFollowerCallbackHandler::OnCastingBarEnd(uint32 uEntityID) 
{
	CALL_CLASS_CALLBACK_1(uEntityID)
}

void ICharacterFollowerCallbackHandler::OnStateBegin(uint32 uEntityID,uint32 uCategoryId, uint32 uDynamicId)	
{
	CALL_CLASS_CALLBACK_3(uEntityID, uCategoryId, uDynamicId)
}

void ICharacterFollowerCallbackHandler::OnStateEnd(uint32 uEntityID,uint32 uCategoryId, uint32 uDynamicId)
{
	CALL_CLASS_CALLBACK_3(uEntityID, uCategoryId, uDynamicId)
}

void ICharacterFollowerCallbackHandler::OnBuffIconUpdate(CBuffData* pBuffData)
{
	CALL_CLASS_CALLBACK_1(pBuffData)
}

void ICharacterFollowerCallbackHandler::OnPhysicalDamageChanged(uint32 uObjID, float minData, float maxData, float fStrikeRate, 
																float fPhysicDefenceRate, float fPhysicDodgeRate, float fParryRate)
{
	CALL_CLASS_CALLBACK_7(uObjID, minData, maxData, fStrikeRate, fPhysicDefenceRate, fPhysicDodgeRate, fParryRate)
}

void ICharacterFollowerCallbackHandler::OnMagicDamageChanged(uint32 uObjID,float fMagicResistRate,float fCompleteResistanceRate, 
														float fMagicDodgeRate, float fNatureResistanceRate ,float fDestructionResistanceRate ,float fEvilResistanceRate)
{
	CALL_CLASS_CALLBACK_7(uObjID, fMagicResistRate, fCompleteResistanceRate, fMagicDodgeRate, fNatureResistanceRate ,fDestructionResistanceRate ,fEvilResistanceRate)
}

void ICharacterFollowerCallbackHandler::OnCalValueChanged(uint32 uObjID)
{
	CALL_CLASS_CALLBACK_1(uObjID)
}

void ICharacterFollowerCallbackHandler::OnDead(uint32 uEntityID)
{
	CALL_CLASS_CALLBACK_1(uEntityID)
}

void ICharacterFollowerCallbackHandler::OnReborn(uint32 uEntityID)
{
	CALL_CLASS_CALLBACK_1(uEntityID)
}

void ICharacterFollowerCallbackHandler::SetLockObj(uint32 uObjID, uint32 uTargetID)
{
	CALL_CLASS_CALLBACK_2(uObjID, uTargetID)
}

bool ICharacterFollowerCallbackHandler::CheckIsCanLock(CCharacterFollower* pObject)
{
	CALL_CLASS_CALLBACK_1_RET(bool,pObject)
		return false;
}

void ICharacterFollowerCallbackHandler::OnRelationChanged(uint32 uEntityID)
{
	CALL_CLASS_CALLBACK_1(uEntityID)
}

void ICharacterFollowerCallbackHandler::OnFireActionEnd(uint32 uEntityID) 
{
	CALL_CLASS_CALLBACK_1(uEntityID)
}

void ICharacterFollowerCallbackHandler::OnRealNameChange(uint32 uEntityID, ENpcType eType,const TCHAR* szRealName)
{
	CALL_CLASS_CALLBACK_3(uEntityID, eType, szRealName)
}

void ICharacterFollowerCallbackHandler::OnTransfered(CCharacterFollower* pObject)
{
	CALL_CLASS_CALLBACK_1(pObject)
}

void ICharacterFollowerCallbackHandler::OnBeginTransfer(CCharacterFollower* pObject, const TCHAR* szSceneName)
{
	CALL_CLASS_CALLBACK_2(pObject,szSceneName)
}

void ICharacterFollowerCallbackHandler::OnResLoaded(CCharacterFollower* pObject)
{
	CALL_CLASS_CALLBACK_1(pObject)
}

const TCHAR* ICharacterFollowerCallbackHandler::GetWeaponTypeString(uint32 uEntityID, bool bIsMainHandWeapon)
{
	CALL_CLASS_CALLBACK_2_RET(const TCHAR*, uEntityID, bIsMainHandWeapon)
	return NULL;
}

const TCHAR* ICharacterFollowerCallbackHandler::GetArmorTypeString(uint32 uEntityID)
{
	CALL_CLASS_CALLBACK_1_RET(const TCHAR*, uEntityID)
	return NULL;
}

const TCHAR* ICharacterFollowerCallbackHandler::GetNpcAniFileName(const TCHAR* szNpcName)
{
	CALL_CLASS_CALLBACK_1_RET(const TCHAR*, szNpcName)
	return NULL;
}

void ICharacterFollowerCallbackHandler::OnMaterialNumChange(CCharacterFollower* pFollower, uint32 uMaterialNum, bool bDirector)
{
	CALL_CLASS_CALLBACK_3(pFollower, uMaterialNum, bDirector)
}

void ICharacterFollowerCallbackHandler::OnExpOwnerChange(CCharacterFollower* pFollower)
{
	CALL_CLASS_CALLBACK_1(pFollower)
}

void ICharacterFollowerCallbackHandler::OnDisplayMsg(CCharacterFollower* pObject,uint32 uMsgID,const TCHAR* szMsgFX) 
{
	CALL_CLASS_CALLBACK_3(pObject,uMsgID,szMsgFX)
}
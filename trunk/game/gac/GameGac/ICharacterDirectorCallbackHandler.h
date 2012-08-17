#pragma once
#include "ICharacterFollowerCallbackHandler.h"
#include "CFightCallbackData.h"

class CCharacterDirector;

class ICharacterDirectorCallbackHandler : public ICharacterFollowerCallbackHandler
{
public:
	virtual ~ICharacterDirectorCallbackHandler(){}
	virtual ICharacterDirectorCallbackHandler* CastToDirectorHandler() { return this; }

	virtual float GroundSelector_GetPosX() { 
		CALL_CLASS_CALLBACK_RET(float)	
		return 0.0f; 
	};
	virtual float GroundSelector_GetPosY() { 
		CALL_CLASS_CALLBACK_RET(float)	
		return 0.0f; 
	};
	virtual bool GroundSelector_IsActive() { 
		CALL_CLASS_CALLBACK_RET(bool)
		return false;
	};
	virtual void GroundSelector_CancelSelectGround() {
		CALL_CLASS_CALLBACK()
	};
	virtual void BeginSelectGround(uint8 uSelectType, const TCHAR* szSkillName, uint8 uSkillLevel, 
		uint8 uPos, uint32 uPositionArea, uint32 uMaxRange) {
		CALL_CLASS_CALLBACK_6(uSelectType, szSkillName, uSkillLevel, uPos, uPositionArea, uMaxRange)
	};
	virtual void OnMagicCondFail( uint32 MessageId, const TCHAR* szSkillName) {
		CALL_CLASS_CALLBACK_2(MessageId, szSkillName)
	};
	virtual void OnMagicCondFailOnTick(const TCHAR* szSkillName, bool bSucc) {
		CALL_CLASS_CALLBACK_2(szSkillName, bSucc)
	};
	virtual void OnCoolDownBegin(const TCHAR* szSkillName, ESkillCoolDownType eCoolDownType, uint32 uLeftTime) {
		CALL_CLASS_CALLBACK_3(szSkillName, eCoolDownType, uLeftTime)
	};
	virtual void OnCoolDownEnd(const TCHAR* szSkillName) {
		CALL_CLASS_CALLBACK_1(szSkillName)
	};
	virtual void OnAllCoolDownEnd() {
		CALL_CLASS_CALLBACK()
	};
	virtual void OnClearCommonCD(){
		CALL_CLASS_CALLBACK()
	};
	virtual void OnStanceSkill() {
		CALL_CLASS_CALLBACK()
	};
	virtual void OnLostTarget() {
		CALL_CLASS_CALLBACK()
	};
	
	virtual void OnArrayValueChange(const TCHAR* szName,uint32 uTotalValue,uint32 uCurValue,uint32 uSpeed){
		CALL_CLASS_CALLBACK_4(szName, uTotalValue, uCurValue, uSpeed)
	};
	virtual void OnGridPosChange() {
		CALL_CLASS_CALLBACK()
	};
	virtual void OnSetAutoNormalAttack(bool bAutoNA){
		CALL_CLASS_CALLBACK_1(bAutoNA)
	};
	virtual void OnPrintFightInfo(CFightInfoData* pFightInfoData){
		CALL_CLASS_CALLBACK_1(pFightInfoData)
	}

	virtual void OnMoveSuccessed(){
		CALL_CLASS_CALLBACK()
	}

	virtual void OnMoveStopped(){
		CALL_CLASS_CALLBACK()
	}


	virtual void OnUpdateSmashRate(uint32 uTargetObjID, EAttackType eAttackType, uint32 uSmashValue, float fSmashRate, bool bTargetIsDead){
		CALL_CLASS_CALLBACK_5(uTargetObjID, eAttackType, uSmashValue, fSmashRate, bTargetIsDead)
	}
	virtual void OnLevelUp(int32 iLevel){
		CALL_CLASS_CALLBACK_1(iLevel)
	};

	virtual void OnReplaceSkill(const TCHAR* szSkillName, const TCHAR* szReplaceSkillName, bool bReplace){
		CALL_CLASS_CALLBACK_3(szSkillName, szReplaceSkillName, bReplace)
	};

	virtual void OnBurstSoulTimesChanged(uint32 uBurstSoulTimes){
		CALL_CLASS_CALLBACK_1(uBurstSoulTimes)
	};

	virtual void ChangeServant(ENpcType eType, uint32 uMasterID, uint32 uServantID, bool uCreate)
	{
		CALL_CLASS_CALLBACK_4(eType, uMasterID, uServantID, uCreate)
	};
};

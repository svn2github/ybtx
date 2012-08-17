#pragma once
#include "CallBackX.h"
#include "FightDef.h"
#include "GameDef.h"
#include "CEntityMallocObject.h"

class ICharacterDirectorCallbackHandler;
class CCharacterFollower;
class CBuffData;

// CallbackHandler上面不能写纯虚的函数，并且不能注册成Abstract

class ICharacterFollowerCallbackHandler : public virtual CDynamicObject, public CEntityMallocObject
{
public:
	virtual ~ICharacterFollowerCallbackHandler() {}
	virtual ICharacterDirectorCallbackHandler* CastToDirectorHandler() { return NULL; }

	virtual void OnCreated(CCharacterFollower *pObject, bool bIsBattleHorse) ;
	virtual void OnCreatedEnd(CCharacterFollower *pObject) ;
	virtual void OnDestroy(CCharacterFollower *pObject) ;
	virtual void OnIntoSyncAoi(CCharacterFollower* pObject) ;
	virtual void OnLeaveSyncAoi(CCharacterFollower* pObject) ;
	virtual void OnIntoScreen(CCharacterFollower* pObject);
	virtual void OnLeaveScreen(CCharacterFollower* pObject);
	virtual void OnAgileValueChanged(uint32 uEntityID);
	virtual void OnCtrlValueChanged(uint32 uEntityID, uint32 uValueTypeID, bool bSet) ;
	virtual void OnCastingBarBegin(uint32 uEntityID,const TCHAR* szSkillName,ECastingProcessType eCastType,
		float fCastTime,float fExpiredTime,bool bIsSync);
	virtual void OnCastingBarEnd(uint32 uEntityID);
	virtual void OnStateBegin(uint32 uEntityID,uint32 uCategoryId, uint32 uDynamicId);
	virtual void OnStateEnd(uint32 uEntityID,uint32 uCategoryId, uint32 uDynamicId);
	virtual void OnBuffIconUpdate(CBuffData* pBuffData);
	virtual	void OnPhysicalDamageChanged(uint32 uObjID, float minData, float maxData, float fStrikeRate, 
		float fPhysicDefenceRate, float fPhysicDodgeRate, float fParryRate);
	virtual	void OnMagicDamageChanged(uint32 uObjID,float fMagicResistRate,float fCompleteResistanceRate, 
		float fMagicDodgeRate, float fNatureResistanceRate ,float fDestructionResistanceRate ,float fEvilResistanceRate);
	virtual void OnCalValueChanged(uint32 uObjID);
	virtual void OnDead(uint32 uEntityID);
	virtual void OnReborn(uint32 uEntityID);
	virtual void SetLockObj(uint32 uObjID, uint32 uTargetID);
	virtual bool CheckIsCanLock(CCharacterFollower* pObject);
	virtual void OnRelationChanged(uint32 uEntityID);
	virtual void OnFireActionEnd(uint32 uEntityID) ;
	virtual void OnRealNameChange(uint32 uEntityID, ENpcType eType,const TCHAR* szRealName);
	virtual void OnTransfered(CCharacterFollower* pObject);
	virtual void OnBeginTransfer(CCharacterFollower* pObject,const TCHAR* szSceneName);
	virtual void OnResLoaded(CCharacterFollower* pObject);
	virtual const TCHAR* GetWeaponTypeString(uint32 uEntityID, bool bIsMainHandWeapon);
	virtual const TCHAR* GetArmorTypeString(uint32 uEntityID);
	virtual const TCHAR* GetNpcAniFileName(const TCHAR* szNpcName);
	virtual void OnMaterialNumChange(CCharacterFollower* pFollower, uint32 uMaterialNum, bool bDirector);
	virtual void OnExpOwnerChange(CCharacterFollower* pFollower);
	virtual void OnDisplayMsg(CCharacterFollower* pObject,uint32 uMsgID,const TCHAR* szMsgFX);
};

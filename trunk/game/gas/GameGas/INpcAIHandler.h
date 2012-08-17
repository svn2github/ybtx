#pragma once
#include "FightDef.h"
#include "CNpcEventMsg.h"
#include "PatternRef.h"
#include "GameDef.h"
#include "NpcDefs.h"

class CCharacterDictator;
class CNpcAI;
class CMemberAI;
class CDummyAI;
class CServantAI;
class CTruckAI;
class CLittlePetAI;
class CEnmityMgr;

enum ECharacterInnerHandlerType
{
	ECharacterInnerHandlerType_Npc,
	ECharacterInnerHandlerType_Dummy,
	ECharacterInnerHandlerType_Member,
};
class INpcAIHandler
{
public:
	INpcAIHandler(){m_RefByCharacter.SetHolder(this);}
	typedef TPtRefee<INpcAIHandler, CCharacterDictator> RefNpcAIHandlerByCharacter;
	RefNpcAIHandlerByCharacter& GetRefByCharacter(){return m_RefByCharacter;}
	virtual void OnCreateEnded() = 0;
	virtual void OnDead() = 0;
	virtual void OnDestoryBegin() = 0;
	virtual void OnCaughtViewSightOf(uint32 uObjGlobalId, uint32 uDimension) = 0;
	virtual void OnLostViewSightOf(uint32 uObjGlobalId, uint32 uDimension) = 0;
	virtual void OnMoveBegan()= 0;
	virtual void OnMoveEnded(uint32 uMoveID =0)= 0;
	virtual void OnMoveToPathPoint(uint8 point) = 0;
	virtual void OnCreateMovePath(uint8 uPoint, float x, float y) = 0;
	virtual void OnBeAttacked(CCharacterDictator* pAttacker, bool bHurtEffect = true) =0;
	virtual void OnMemberBeAttacked(CCharacterDictator* pAttacker) =0;
	virtual void OnOtherDead(uint32 EntityID, const string& NpcName) = 0;
	virtual void OnMasterDestroyed() =0;
	virtual void OnGroupDestroyed() = 0;
	virtual void OnAgileValueChanged(uint32 uValueTypeID, float fValue)= 0;
	virtual void OnCtrlValueChanged(uint32 uValue, uint32 uValueTypeID, bool bSet)=0;
	virtual void OnDoSkillCtrlStateChanged(uint32 uState, bool bSet)=0;
	virtual void MoveToDest(uint32 uX,uint32 uY)=0;
	virtual void MoveToDest(float uX,float uY)=0;
	virtual void ForceLock(CCharacterDictator* pTarget, uint32 uChaosTime)=0;
	virtual CNpcAI*	CastToNpcAI();
	virtual CServantAI* CastToServantAI();
	virtual CDummyAI*	CastToDummyAI(); 
	virtual CMemberAI*	CastToMemberAI(); 
	virtual CTruckAI* CastToTruckAI();
	virtual CLittlePetAI* CastToLittlePetAI();
	virtual ECharacterInnerHandlerType GetType() const = 0;
	virtual void SetDefaultWalkState(bool bWalkState)=0;
	virtual void SetMoveRange(uint32 range)=0;
	virtual void OnBeginRandomMove()=0;
	virtual void OnSetState(uint32 uEntityID) = 0;
	virtual void OnDeleteState(uint32 uEntityID) = 0;
	virtual void OnFacialAction(uint32 uEntityID, const string& szFacialName) = 0;
	virtual void OnSetNatureData(const string& szNatureName, const string& szNatureArg, uint32 uNatureExtraArg, 
								const string& szNatureAction, uint16 uColdTime, const string& szAction, const string& szActionArg, uint16 uLevel) = 0;
	virtual void SendNpcEvent(ENpcEvent eEvent) = 0;
	virtual uint32 GetOneActorAround() = 0;
	virtual uint32 GetOneNpcAround() = 0;
	virtual uint32 GetRandomOneAround() = 0;
	virtual uint32 GetExpOwnerID() =0;
	virtual void ClearAllEnmity() = 0;
	virtual bool CanAIBeChange() = 0;
	virtual bool CanBeRavin() = 0;
	virtual bool CanBeSelected() = 0;
	virtual void SetCharacterHolder(CCharacterDictator* pCharacter)=0;
	virtual void SetRealAIType(ENpcAIType eRealAIType) = 0;
	virtual ENpcAIType GetRealAIType() = 0;
	virtual ENpcAIType GetOldAIType() = 0;
	virtual void SetRealNpcType(ENpcType eRealNpcType) = 0;
	virtual ENpcType GetRealNpcType() = 0;
	virtual void OnSkillBegin() = 0;
	virtual void OnSkillSuccessEnd() = 0;
	virtual void OnSkillFailEnd() = 0;
	virtual void SetMoveType(uint8 uMoveType) = 0;
	virtual void CheckLifeTimeByLeaveBattle() = 0;
	virtual void ReSetNpcExistTick(uint32 uExitTime) = 0;
	virtual void TalkWithPlayerStart(uint32 uEntityID) = 0;
	virtual void TalkWithPlayerEnd() = 0;
	virtual void SetGroupMemberID(uint32 id) = 0;
	virtual uint32 GetExistTime() = 0;
	virtual int32 GetLeftLifeTime() = 0;
	virtual void StopFollowMaster() = 0;
	virtual void ReSetNpcGridAttackScope(float fScope) = 0;
	virtual void RevertNpcAttackScope() = 0;
	virtual EAttackType GetNpcAttackType() = 0;
	virtual bool CanDelInBattle() = 0;
	virtual void SetCanDelInBattle(bool bCanDel) = 0;
	virtual void SetDeadAfterBattle() = 0;
	virtual void CheckPlayInNpcSleepDim() = 0;
	virtual void SetNpcSleepState(bool bSleep) = 0;
	virtual	bool HaveBeSetPath() = 0;
	virtual void RemoveOneEnemy(CCharacterDictator *pEnemy) = 0;
	virtual void SetNpcBirthDir(uint8 uDir) = 0;
	virtual float GetNpcSize() = 0;
	virtual void OnEnterBattleState() = 0;
	virtual void OnLeaveBattleState() = 0;
	virtual void MasterNotifyLeaveBattle() = 0;
	virtual void NpcMoveInPath() = 0;
	virtual float GetAttackScope() = 0;
	virtual CEnmityMgr* GetEnmityMgr() const = 0;
	virtual uint32 GetNpcDoSkillLevel() = 0;
	virtual void OnBeHurt(int32 iHpChange) = 0;
	virtual void SetDoSpecialActionProbability(uint32 uProbability) = 0;
	
	virtual ~INpcAIHandler(){}
private:
	RefNpcAIHandlerByCharacter	m_RefByCharacter;
};


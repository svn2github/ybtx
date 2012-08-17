#pragma once
#include "FightDef.h"
#include "CDistortedTick.h"
#include "CPos.h"
#include "CDoSkillCtrlMgr.h"

class CNormalSkillServerSharedPtr;
class CSkillInstServer;
class CCastingProcess;
class CAureMagicServer;
class CFighterDictator;
class CGenericTarget;
class CTempSkill;
class CNormalSkillServer;
class CAniKeyFrameMgrServer;

class CSkillMgrServer
	:public CDoSkillCtrlMgr
{
	friend class CTestBulletMagic;
public:
	CSkillMgrServer(CFighterDictator* pFigter);
	virtual ~CSkillMgrServer(void);
	void DoNPCSkill(const string& szName, uint8 uSkillLevel,CGenericTarget* pTarget,bool bDoSkillRule);
	void DoNPCSkill(const CNormalSkillServerSharedPtr& pSkill, uint8 uSkillLevel,CGenericTarget* pTarget,bool bDoSkillRule);
	void DoIntObjSkill(const string& szName, uint8 uSkillLevel,CGenericTarget* pTarget);
	void SyncCastingProcess(CFighterDictator* pFighter);		//同步施法流程给客户端
	void SetCurCasting(CCastingProcess* pCastingProcess);
	void CancelCurCasting(CCastingProcess* pCastingProcess);
	CCastingProcess* GetCurCasting()const;
	bool CurCastingProcessCanBeInterrupted();
	void CancelCastingProcess();
	void InterruptCastingProcessByType(ECastingInterruptType eCastingInterruptType);

	//RPC
	void OnSkillBegin(uint32 uSkillID, uint8 uSkillLevel, ETargetType eTargetType, uint32 uTargetID);
	void OnSkillEnd(uint32 uId, bool bSucceed);
	void OnMagicCondFail(uint32 MessageId, const TCHAR* sSkillName)const;
	void OnCoolDownBegin(const TCHAR* szSkillName, uint8 uSkillLevel, uint32 uLeftTime, bool bIsSwitchEquipSkill = false);
	void OnCoolDownEnd(const TCHAR* szSkillName);
	void OnClearCommonCD();
	void OnAllCoolDownEnd();
	void OnDoMagicOp(uint32 uMagicOpID, bool bNotPosition, uint32 uCreatorID, uint32 uTargetID, const CFPos& Pos, uint32 uHurtResult = 1, bool bPlayFX = true);
	CFighterDictator*			GetFighter()const{return m_pFighter;}
	//流程
	void OnCastBegin(uint32 uId, float CastTime, uint64 uStartTime, uint32 uTargetID);
	void OnCastEnd(CCastingProcess* pCastingProcess, uint32 uSkillID, uint32 uId,bool bIsFinish);

	//战斗信息
	void DisplayFightInfo(CFighterDictator* pCreator, CFighterDictator* pFrom, int32 iValue, EAgileType eAglieType,EHurtResult eHurtResult);
	void DisplayFightInfo(CFighterDictator* pCreator, CFighterDictator* pFrom, int32 iValue, EHurtResult eHurtResult, bool bIsSkill, uint32 uCoreObjID);
	void DisplayFightInfo(CFighterDictator* pCreator, CFighterDictator* pFrom, int32 iValue, EHurtResult eHurtResult, bool bIsSkill);
	void DisplayFightInfo(CFighterDictator* pCreator, CFighterDictator* pFrom, int32 iValue, EHurtResult eHurtResult, bool bOverTime,bool bIsSkill,bool bIsArea);

	void SetDoSkillCtrlState(EDoSkillCtrlState eSkillCtrlState, bool bSet);

	CAniKeyFrameMgrServer* GetAniKeyMgr()const {return m_pAniKeyMgr;}
	void RevertSkillCtrlState();
protected:
	void NoConditionDoSkill(const CNormalSkillServerSharedPtr& pSkill, uint8 uSkillLevel,CGenericTarget* pTarget,bool 
bDoSkillRule);

	CFighterDictator*		m_pFighter;
	CCastingProcess*		m_pCastingProcess;

public:
	static uint32 GetPlayerDoSkillSuccNum();
	static uint32 GetPlayerDoSkillFailNum();
	static uint32 GetNpcDoSkillNum();

private:
	CAniKeyFrameMgrServer* m_pAniKeyMgr;
	static uint32	ms_uPlayerDoSkillSuccNum;
	static uint32	ms_uPlayerDoSkillFailNum;
	static uint32	ms_uNpcDoSkillNum;
};


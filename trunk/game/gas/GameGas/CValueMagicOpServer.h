#pragma once
#include "CMagicOpServer.h"
#include "FightDef.h"
#include "CValueData.h"

class CCfgCalc;
class CCfgArg;
class CSkillInstServer;
class CMagicOpArg;
class CFighterDictator;

//公共基类

enum EValueMagicOpType
{ 
	eVMOT_Normal, 
	eVMOT_Physical, 
	eVMOT_Nature, 
	eVMOT_Destruction, 
	eVMOT_Evil,
	eVMOT_TaskNonBattle,
	eVMOT_TaskBomb,
	eVMOT_TaskSpecial,
	eVMOT_TaskNonNationBattleBuild,
};

class CValueMagicOpServer
	:public CMagicOpServer
{
public:	
	bool CanExecuteWithAttackerIsDead()const
	{
		return true;
	}
	virtual uint32 Do(CSkillInstServer* pSkillInst,const CCfgArg* Arg,CFighterDictator* pFrom,CFighterDictator* pTo,int32 *MajorMopValue=0);
	virtual uint32 Do(CSkillInstServer* pSkillInst,const CCfgCalc& Arg,CFighterDictator* pFrom,CFighterDictator* pTo,int32 *MajorMopValue=0)=0;
	virtual int32 MagicOpCalc(CSkillInstServer* pSkillInst,const CCfgArg* Arg,CFighterDictator* pFrom,CFighterDictator* pTo,CValueData *pValue=0,uint32* pExtraValue=0);
	virtual int32 MagicOpCalc(CSkillInstServer* pSkillInst,const CCfgCalc& Arg,CFighterDictator* pFrom,CFighterDictator* pTo,CValueData *pValue=0,uint32* pExtraValue=0)=0;
	virtual EMagicOpClassType GetMagicOpType()const
	{
		return eMOT_Value;
	}
	virtual bool Cancelable()const
	{
		return false;
	}
	virtual bool CalcNeedTarget(CCfgArg* pArg) const;
	virtual EValueMagicOpType GetValueMagicOpType()const { return eVMOT_Normal; }
	virtual bool IsNonTypeAttack() const {return false;}
	virtual void MagicOpExec(CSkillInstServer* pSkillInst,CFighterDictator* pFrom,CFighterDictator* pTo,const CValueData& Value,EHurtResult eResult,uint32 uExtraValue=0);
	virtual ~CValueMagicOpServer(){}
	virtual void GetImmuneResultAtExec(const CFighterDictator* pTo, EHurtResult& eHurtResult, CSkillInstServer* pSkillInst, int32 iValue) {};
	virtual EAttackType GetAttackType(CSkillInstServer* pSkillInst);
	virtual ESkillType GetSkillType(CSkillInstServer* pSkillInst);
	virtual void StatisticFightHPChange(EFightInfoIndex eFightInfoIndex, CSkillInstServer* pSkillInst, CFighterDictator* pFrom, CFighterDictator* pTo, uint32 uRealChangeValue);
protected:
	void PrintFightInfo(CSkillInstServer* pSkillInst,CFighterDictator* pFrom,CFighterDictator* pTo,int32 iFinalValue,int32 iValue,EHurtResult eResult,uint32 uExtraValue,uint32 uRealChangeValue);
	void OnPrintFightInfo(EFightInfoIndex eFightInfoIndex, CSkillInstServer* pSkillInst, CFighterDictator* pFrom,CFighterDictator* pTo,int32 iValue,uint32 uExtraValue,uint32 uRealChangeValue,const TCHAR* szArgName = "",uint16 uArgLen = 0);
	void HurtResultToActionState(CSkillInstServer* pSkillInst,CFighterDictator* pFrom,CFighterDictator* pTo,int32 iValue,EHurtResult eResult, bool bPTOIsFirstEnterBattle);
	void NotifyMessageEvent(CSkillInstServer* pSkillInst,CFighterDictator* pFrom,CFighterDictator* pTo,int32 iValue,int32 iFinalValue,EHurtResult eResult,uint32 uExtraValue);
	float GetLevelPressRate(CFighterDictator* pFrom, CFighterDictator* pTo);
};

class CValueCancelableMagicOp
	:public CValueMagicOpServer
{
public:
	virtual void Cancel(CSkillInstServer* pSkillInst,const CCfgArg* Arg,CFighterDictator* pFrom,CFighterDictator* pTo);
	virtual void Cancel(CSkillInstServer* pSkillInst,const CCfgCalc& Arg,CFighterDictator* pFrom,CFighterDictator* pTo)=0;
	virtual void CancelExec(CSkillInstServer* pSkillInst,CFighterDictator* pFrom,CFighterDictator* pTo,const CValueData& Value)=0;
	virtual bool Cancelable()const
	{ 
		return true;
	}
};

class CCalcNeedTargetValueMagicOpServer
	: public CValueMagicOpServer
{
public:
	bool CalcNeedTarget(CCfgArg* pArg) const
	{
		return true;
	}
};


#pragma once
#include "CValueMagicOpServer.h"

class CChangeHPMOP
	:public CValueMagicOpServer
{
public:
	bool CanExecuteWithAttackerIsDead()const
	{
		return true;
	}
	uint32 Do(CSkillInstServer* pSkillInst,const CCfgCalc& Arg,CFighterDictator* pFrom,CFighterDictator* pTo,int32* MajorMopValue=0);
	int32 MagicOpCalc(CSkillInstServer* pSkillInst,const CCfgCalc& Arg,CFighterDictator* pFrom,CFighterDictator* pTo,CValueData *pValue=0,uint32* pExtraValue=0);
	void GetImmuneResultAtExec(const CFighterDictator* pTo, EHurtResult& eHurtResult, CSkillInstServer* pSkillInst, int32 iValue);
};

class CTargetChangeHPMOP
	:public CChangeHPMOP
{
public:
	uint32 Do(CSkillInstServer* pSkillInst,const CCfgCalc& Arg,CFighterDictator* pFrom,CFighterDictator* pTo,int32* MajorMopValue=0);
};

class CSuckBloodMOP
	: public CChangeHPMOP
{
public:
	uint32 Do(CSkillInstServer* pSkillInst,const CCfgCalc& Arg,CFighterDictator* pFrom,CFighterDictator* pTo,int32* MajorMopValue=0);
};

class CIgnoreImmuneChangeHPMOP
	:public CChangeHPMOP
{
public:
	void GetImmuneResultAtExec(const CFighterDictator* pTo, EHurtResult& eHurtResult, CSkillInstServer* pSkillInst, int32 iValue);
};

class CCalculatePetHurtMop
	:public CValueMagicOpServer
{
	uint32 Do(CSkillInstServer* pSkillInst,const CCfgCalc& Arg,CFighterDictator* pFrom,CFighterDictator* pTo,int32* MajorMopValue=0);
	int32 MagicOpCalc(CSkillInstServer* pSkillInst,const CCfgCalc& Arg,CFighterDictator* pFrom,CFighterDictator* pTo,CValueData *pValue=0,uint32* pExtraValue=0);
	virtual uint32 GetAdditionalHurt(CFighterDictator* pFrom)=0;
	void GetImmuneResultAtExec(const CFighterDictator* pTo, EHurtResult& eHurtResult, CSkillInstServer* pSkillInst, int32 iValue);
};

class CCalPetPhysicHurtMop
	:public CCalculatePetHurtMop
{
	uint32 GetAdditionalHurt(CFighterDictator* pFrom);
};

class CCalPetMagicHurtMop
	:public CCalculatePetHurtMop
{
	uint32 GetAdditionalHurt(CFighterDictator* pFrom);
};

enum EPhysicalAttackType
{
	ePAT_Normal,	// MainHand & Skill
	ePAT_AssistHand,
	ePAT_TwoHand,
};

struct PropertyOfPhysicsCal
	:public CConfigMallocObject
{
	// Attacker Property
	EPhysicalAttackType	m_ePhysicalAttackType;
	uint32	m_uAttackerLevel;
	float	m_fMissRate;
	uint32	m_uAccuratenessValue;
	uint32	m_uStrikeValue;
	float	m_fExtraStrikeRate;

	// Target Property
	uint32	m_uTargetLevel;
	int32	m_uPhysicalDodgeValue;
	float	m_fExtraPhysicDodgeRate;
	int32	m_uParryValue;
	int32	m_uStrikeResistanceValue;
	float	m_fExtraParryRate;
	float	m_fBlockRate;
};

// 物理伤害运算
class CCalculatePhysicsHurtMop
	:public CCalcNeedTargetValueMagicOpServer
{
public:
	virtual EValueMagicOpType GetValueMagicOpType()const { return eVMOT_Physical; }
	uint32 Do(CSkillInstServer* pSkillInst,const CCfgCalc& Arg,CFighterDictator* pFrom,CFighterDictator* pTo,int32* MajorMopValue=0);
	int32 MagicOpCalc(CSkillInstServer* pSkillInst,const CCfgCalc& Arg,CFighterDictator* pFrom,CFighterDictator* pTo,CValueData *pValue=0,uint32* pExtraValue=0);
	
	virtual EHurtResult DicePhysicalHurt(CSkillInstServer* pSkillInst, CFighterDictator* pFrom,CFighterDictator* pTo);

	virtual double CalInitialDamage(const CFighterDictator* pFrom, const CFighterDictator* pTo) = 0; //计算初步的伤害，即公式中的 “伤害1”
	virtual int32 GetSkillPhyicsDamage(const CFighterDictator* pFrom);
	virtual void CalMainAssistHandDamage(CSkillInstServer* pSkillInst, const CFighterDictator* pFrom, double& hurt, double& uExtraHurt, EHurtResult eHurtResult) = 0;		//计算主、副手附加伤害
	virtual EAttackType GetAttackType(const CFighterDictator* pFrom)const = 0;

	static EHurtResult CalProbability(PropertyOfPhysicsCal Pro, bool bIgnoreMiss = false, bool bIgnorePhyDodge = false, bool bIgnoreParry = false, bool bIgnoreBlock = false, bool bIgnoreStrike = false);

protected:
	virtual uint32 GetStrikeValue(const CFighterDictator* pFrom)const;
	virtual float GetExtraStrikeRate(const CFighterDictator* pFrom)const;

};

// 计算主手伤害
class CCalculateMainHandPhysicsHurtMop
	:public	CCalculatePhysicsHurtMop
{
public:
	double GenerateWeaponDamage(const CFighterDictator* pFrom);

	virtual double CalInitialDamage(const CFighterDictator* pFrom, const CFighterDictator* pTo);
	virtual void CalMainAssistHandDamage(CSkillInstServer* pSkillInst, const CFighterDictator* pFrom, double& hurt, double& uExtraHurt, EHurtResult eHurtResult);
	virtual EAttackType GetAttackType(const CFighterDictator* pFrom)const;
};

// 计算主手暴击伤害
class CCalculateMainHandPhysicsStrikeMop
	:public CCalculateMainHandPhysicsHurtMop
{
public:
	virtual EHurtResult DicePhysicalHurt(CSkillInstServer* pSkillInst, CFighterDictator* pFrom, CFighterDictator* pTo) { return eHR_Strike;}
};

class CCalculateMainHandPhysicsNoStrikeHurtMop
	:public CCalculateMainHandPhysicsHurtMop
{
protected:
	virtual uint32 GetStrikeValue(const CFighterDictator* pFrom)const;
	virtual float GetExtraStrikeRate(const CFighterDictator* pFrom)const;
};

class CCalculateMainHandPhysicsOnlyHitAndStrikeHurtMop
	:public CCalculateMainHandPhysicsHurtMop
{
protected:
	virtual EHurtResult DicePhysicalHurt(CSkillInstServer* pSkillInst, CFighterDictator* pFrom, CFighterDictator* pTo);
};

// 计算副手伤害
class CCalculateAssistantPhyicsHurtMop
	:public	CCalculatePhysicsHurtMop
{
public:
	double GenerateWeaponDamage(const CFighterDictator* pFrom);

	virtual double CalInitialDamage(const CFighterDictator* pFrom, const CFighterDictator* pTo);
	virtual void CalMainAssistHandDamage(CSkillInstServer* pSkillInst, const CFighterDictator* pFrom, double& hurt, double& uExtraHurt, EHurtResult eHurtResult);
	virtual EAttackType GetAttackType(const CFighterDictator* pFrom)const;
	virtual int32 GetSkillPhyicsDamage(const CFighterDictator* pFrom);
};

struct PropertyOfMagicCal
	:public CConfigMallocObject
{
	// Attacker Property
	uint32	m_uAttackerLevel;
	uint32	m_uMagicHitValue;
	uint32	m_uStrikeValue;
	float	m_fExtraStrikeRate;

	// Target Property
	uint32	m_uTargetLevel;
	int32	m_uMagicDodgeValue;
	int32	m_uStrikeResistanceValue;
	float	m_fExtraMagicDodgeRate;

	int32	m_uNatureResistanceValue;
	float	m_fExtraNatureResistanceRate;
	int32	m_uDestructionResistanceValue;
	float	m_fExtraDestructionResistanceRate;
	int32	m_uEvilResistanceValue;
	float	m_fExtraEvilResistanceRate;

	float	m_fExtraMagicResistanceRate;
	float	m_fExtraCompleteResistanceRate;
};

// 计算法术伤害的基类
class CCalculateMagicHurtMop 
	:public CCalcNeedTargetValueMagicOpServer
{
public:
	EHurtResult DiceMagicHurt(CFighterDictator* pFrom,CFighterDictator* pTo);
	uint32 Do(CSkillInstServer* pSkillInst,const CCfgCalc& Arg,CFighterDictator* pFrom,CFighterDictator* pTo,int32* MajorMopValue=0);
	int32 MagicOpCalc(CSkillInstServer* pSkillInst,const CCfgCalc& Arg,CFighterDictator* pFrom,CFighterDictator* pTo,CValueData *pValue=0,uint32* pExtraValue=0);

	virtual float GetResistRate(const CFighterDictator* pTo)=0;
	virtual void GetImmuneResultByType(const CFighterDictator* pTo, EHurtResult& eHurtResult) = 0;
	virtual uint32 GetCertainMagicDamageValue(const CFighterDictator* pFrom)=0;
	virtual double GetExtraCertainMagicDamageValue(const CFighterDictator* pFrom, double uExtraValue, EHurtResult eHurtResult, double& hurt)=0;
	virtual float GetPenetrationFinalRate(CFighterDictator* pFrom,CFighterDictator* pTo)=0;
	virtual float GetSmashRate(const CFighterDictator* pFrom, const CFighterDictator* pTo)const=0;
	virtual ESkillType GetSkillType(CSkillInstServer* pSkillInst)	{return eST_Magic;}
	void GetImmuneResultAtExec(const CFighterDictator* pTo, EHurtResult& eHurtResult, CSkillInstServer* pSkillInst, int32 iValue);

	static EHurtResult CalProbability(PropertyOfMagicCal Pro);

protected:
	virtual uint32 GetStrikeValue(const CFighterDictator* pFrom)const;
	virtual float GetExtraStrikeRate(const CFighterDictator* pFrom)const;
	virtual uint32 GetNatureResistanceValue(const CFighterDictator* pTo)const;
	virtual float GetExtraNatureResistanceRate(const CFighterDictator* pTo)const;
	virtual uint32 GetDestructionResistanceValue(const CFighterDictator* pTo)const;
	virtual float GetExtraDestructionResistanceRate(const CFighterDictator* pTo)const;
	virtual uint32 GetEvilResistanceValue(const CFighterDictator* pTo)const;
	virtual float GetExtraEvilResistanceRate(const CFighterDictator* pTo)const;
	virtual float GetExtraMagicResistanceRate(const CFighterDictator* pTo)const;
	virtual float GetExtraCompleteResistanceRate(const CFighterDictator* pTo)const;
};

// 计算自然系魔法伤害
class CCalNatureMagHurtMop
	:public CCalculateMagicHurtMop
{
public:
	virtual EValueMagicOpType GetValueMagicOpType()const { return eVMOT_Nature; }
	virtual float GetResistRate(const CFighterDictator* pTo);
	virtual void GetImmuneResultByType(const CFighterDictator* pTo, EHurtResult& eHurtResult);
	virtual uint32 GetCertainMagicDamageValue(const CFighterDictator* pFrom);
	virtual double GetExtraCertainMagicDamageValue(const CFighterDictator* pFrom, double uExtraValue, EHurtResult eHurtResult, double& hurt);
	virtual float GetPenetrationFinalRate(CFighterDictator* pFrom,CFighterDictator* pTo);
	virtual float GetSmashRate(const CFighterDictator* pFrom, const CFighterDictator* pTo)const;
	virtual EAttackType GetAttackType(CSkillInstServer* pSkillInst)	{return eATT_Nature;}
};

class CCalNatureMagNoStrikeHurtMop
	:public CCalNatureMagHurtMop
{
protected:
	virtual uint32 GetStrikeValue(const CFighterDictator* pFrom)const;
	virtual float GetExtraStrikeRate(const CFighterDictator* pFrom)const;
};

// 计算破坏系魔法伤害
class CCalDestructionMagHurtMop
	:public CCalculateMagicHurtMop
{
public:
	virtual EValueMagicOpType GetValueMagicOpType()const { return eVMOT_Destruction; }
	virtual float GetResistRate(const CFighterDictator* pTo);
	virtual void GetImmuneResultByType(const CFighterDictator* pTo, EHurtResult& eHurtResult);
	virtual uint32 GetCertainMagicDamageValue(const CFighterDictator* pFrom);
	virtual double GetExtraCertainMagicDamageValue(const CFighterDictator* pFrom, double uExtraValue, EHurtResult eHurtResult, double& hurt);
	virtual float GetPenetrationFinalRate(CFighterDictator* pFrom,CFighterDictator* pTo);
	virtual float GetSmashRate(const CFighterDictator* pFrom, const CFighterDictator* pTo)const;
	virtual EAttackType GetAttackType(CSkillInstServer* pSkillInst)	{return eATT_Destroy;}
};

class CCalDestructionMagNoStrikeHurtMop
	:public CCalDestructionMagHurtMop
{
protected:
	virtual uint32 GetStrikeValue(const CFighterDictator* pFrom)const;
	virtual float GetExtraStrikeRate(const CFighterDictator* pFrom)const;
};

class CCalDestructionMagNoResistHurtMop
	:public CCalDestructionMagHurtMop
{
protected:
	virtual uint32 GetNatureResistanceValue(const CFighterDictator* pTo)const;
	virtual float GetExtraNatureResistanceRate(const CFighterDictator* pTo)const;
	virtual uint32 GetDestructionResistanceValue(const CFighterDictator* pTo)const;
	virtual float GetExtraDestructionResistanceRate(const CFighterDictator* pTo)const;
	virtual uint32 GetEvilResistanceValue(const CFighterDictator* pTo)const;
	virtual float GetExtraEvilResistanceRate(const CFighterDictator* pTo)const;
	virtual float GetExtraMagicResistanceRate(const CFighterDictator* pTo)const;
	virtual float GetExtraCompleteResistanceRate(const CFighterDictator* pTo)const;
};

// 计算暗黑系魔法伤害
class CCalEvilMagHurtMop
	:public CCalculateMagicHurtMop
{
public:
	virtual EValueMagicOpType GetValueMagicOpType()const { return eVMOT_Evil; }
	virtual float GetResistRate(const CFighterDictator* pTo);
	virtual void GetImmuneResultByType(const CFighterDictator* pTo, EHurtResult& eHurtResult);
	virtual uint32 GetCertainMagicDamageValue(const CFighterDictator* pFrom);
	virtual double GetExtraCertainMagicDamageValue(const CFighterDictator* pFrom, double uExtraValue, EHurtResult eHurtResult, double& hurt);
	virtual float GetPenetrationFinalRate(CFighterDictator* pFrom,CFighterDictator* pTo);
	virtual float GetSmashRate(const CFighterDictator* pFrom, const CFighterDictator* pTo)const;
	virtual EAttackType GetAttackType(CSkillInstServer* pSkillInst)	{return eATT_Evil;}
};

class CCalEvilMagNoStrikeHurtMop
	:public CCalEvilMagHurtMop
{
protected:
	virtual uint32 GetStrikeValue(const CFighterDictator* pFrom)const;
	virtual float GetExtraStrikeRate(const CFighterDictator* pFrom)const;
};

//Dot伤害计算
class CCalDOTHurtMop
	:public CCalcNeedTargetValueMagicOpServer
{
public:
	uint32 Do(CSkillInstServer* pSkillInst,const CCfgCalc& Arg,CFighterDictator* pFrom,CFighterDictator* pTo,int32* MajorMopValue=0);
	int32 MagicOpCalc(CSkillInstServer* pSkillInst,const CCfgCalc& Arg,CFighterDictator* pFrom,CFighterDictator* pTo,CValueData *pValue=0,uint32* pExtraValue=0);
	void GetImmuneResultAtExec(const CFighterDictator* pTo, EHurtResult& eHurtResult, CSkillInstServer* pSkillInst, int32 iValue);
};

class CCalDOTNoStrikeHurtMop
	:public CCalDOTHurtMop
{
protected:
	virtual uint32 GetStrikeValue(const CFighterDictator* pFrom)const;
	virtual float GetExtraStrikeRate(const CFighterDictator* pFrom)const;
};

//即时伤害计算
class CCalInstantHurtMop
	:public CChangeHPMOP
{
public:
	int32 MagicOpCalc(CSkillInstServer* pSkillInst,const CCfgCalc& Arg,CFighterDictator* pFrom,CFighterDictator* pTo,CValueData *pValue=0,uint32* pExtraValue=0);
};

//无类型伤害计算
class CCalNonTypeHurtMop
	:public CCalInstantHurtMop
{
public:
	bool IsNonTypeAttack() const {return true;}
};

// 治疗魔法操作
class CCureMop
	:public CCalcNeedTargetValueMagicOpServer
{
public:
	uint32 Do(CSkillInstServer* pSkillInst,const CCfgCalc& Arg,CFighterDictator* pFrom,CFighterDictator* pTo,int32* MajorMopValue=0);
	int32 MagicOpCalc(CSkillInstServer* pSkillInst,const CCfgCalc& Arg,CFighterDictator* pFrom,CFighterDictator* pTo,CValueData *pValue=0,uint32* pExtraValue=0);
	void GetImmuneResultAtExec(const CFighterDictator* pTo, EHurtResult& eHurtResult, CSkillInstServer* pSkillInst, int32 iValue);
};

//改变移动速度百分比
class CChangePercentMoveSpeedMOP
	:public CValueCancelableMagicOp
{
public:
	uint32 Do(CSkillInstServer* pSkillInst,const CCfgCalc& Arg,CFighterDictator* pFrom,CFighterDictator* pTo,int32* MajorMopValue=0);
	int32 MagicOpCalc(CSkillInstServer* pSkillInst,const CCfgCalc& Arg,CFighterDictator* pFrom,CFighterDictator* pTo,CValueData *pValue=0,uint32* pExtraValue=0);
	void MagicOpExec(CSkillInstServer* pSkillInst,CFighterDictator* pFrom,CFighterDictator* pTo,const CValueData& Value,EHurtResult eResult,uint32 uExtraValue=0);
	void CancelExec(CSkillInstServer* pSkillInst,CFighterDictator* pFrom,CFighterDictator* pTo,const CValueData& Value);
	void Cancel(CSkillInstServer* pSkillInst,const CCfgCalc& Arg,CFighterDictator* pFrom,CFighterDictator* pTo);
};

//改变步行速度百分比
class CChangePercentWalkSpeedMOP
	:public CValueCancelableMagicOp
{
public:
	uint32 Do(CSkillInstServer* pSkillInst,const CCfgCalc& Arg,CFighterDictator* pFrom,CFighterDictator* pTo,int32* MajorMopValue=0);
	int32 MagicOpCalc(CSkillInstServer* pSkillInst,const CCfgCalc& Arg,CFighterDictator* pFrom,CFighterDictator* pTo,CValueData *pValue=0,uint32* pExtraValue=0);
	void MagicOpExec(CSkillInstServer* pSkillInst,CFighterDictator* pFrom,CFighterDictator* pTo,const CValueData& Value,EHurtResult eResult,uint32 uExtraValue=0);
	void CancelExec(CSkillInstServer* pSkillInst,CFighterDictator* pFrom,CFighterDictator* pTo,const CValueData& Valuee);
	void Cancel(CSkillInstServer* pSkillInst,const CCfgCalc& Arg,CFighterDictator* pFrom,CFighterDictator* pTo);
};

//改变普通攻击速度百分比
class CChangePercentNormalAttackSpeedMOP
	:public CValueCancelableMagicOp
{
public:
	uint32 Do(CSkillInstServer* pSkillInst,const CCfgCalc& Arg,CFighterDictator* pFrom,CFighterDictator* pTo,int32* MajorMopValue=0);
	int32 MagicOpCalc(CSkillInstServer* pSkillInst,const CCfgCalc& Arg,CFighterDictator* pFrom,CFighterDictator* pTo,CValueData *pValue=0,uint32* pExtraValue=0);
	void MagicOpExec(CSkillInstServer* pSkillInst,CFighterDictator* pFrom,CFighterDictator* pTo,const CValueData& Value,EHurtResult eResult,uint32 uExtraValue=0);
	void CancelExec(CSkillInstServer* pSkillInst,CFighterDictator* pFrom,CFighterDictator* pTo,const CValueData& Value);
	void Cancel(CSkillInstServer* pSkillInst,const CCfgCalc& Arg,CFighterDictator* pFrom,CFighterDictator* pTo);
};

class CTaskNonBattleHurtMOP
	:public CCalInstantHurtMop
{
	EValueMagicOpType GetValueMagicOpType()const { return eVMOT_TaskNonBattle; }
	void GetImmuneResultAtExec(const CFighterDictator* pTo, EHurtResult& eHurtResult, CSkillInstServer* pSkillInst, int32 iValue);
};

class CTaskBombHurtMOP
	:public CCalInstantHurtMop
{
	EValueMagicOpType GetValueMagicOpType()const { return eVMOT_TaskBomb; }
	void GetImmuneResultAtExec(const CFighterDictator* pTo, EHurtResult& eHurtResult, CSkillInstServer* pSkillInst, int32 iValue);
};

class CTaskSpecialHurtMOP
	:public CCalInstantHurtMop
{
	EValueMagicOpType GetValueMagicOpType()const { return eVMOT_TaskSpecial; }
	void GetImmuneResultAtExec(const CFighterDictator* pTo, EHurtResult& eHurtResult, CSkillInstServer* pSkillInst, int32 iValue);
};

class CTaskNonNationBattleBuildHurtMOP
	:public CCalInstantHurtMop
{
	EValueMagicOpType GetValueMagicOpType()const { return eVMOT_TaskNonNationBattleBuild; }
	void GetImmuneResultAtExec(const CFighterDictator* pTo, EHurtResult& eHurtResult, CSkillInstServer* pSkillInst, int32 iValue);
};

class CChangeNpcAttackScopeToMOP
	:public CValueCancelableMagicOp
{
public:
	uint32 Do(CSkillInstServer* pSkillInst,const CCfgCalc& Arg,CFighterDictator* pFrom,CFighterDictator* pTo,int32* MajorMopValue=0);
	int32 MagicOpCalc(CSkillInstServer* pSkillInst,const CCfgCalc& Arg,CFighterDictator* pFrom,CFighterDictator* pTo,CValueData *pValue=0,uint32* pExtraValue=0);
	void MagicOpExec(CSkillInstServer* pSkillInst,CFighterDictator* pFrom,CFighterDictator* pTo,const CValueData& Value,EHurtResult eResult,uint32 uExtraValue=0);
	void CancelExec(CSkillInstServer* pSkillInst,CFighterDictator* pFrom,CFighterDictator* pTo,const CValueData& Valuee);
	void Cancel(CSkillInstServer* pSkillInst,const CCfgCalc& Arg,CFighterDictator* pFrom,CFighterDictator* pTo);
};

class CNoEventChangeHPMOP
	:public CChangeHPMOP
{
public:
	bool CanExecuteWithAttackerIsDead()const
	{
		return true;
	}
	void MagicOpExec(CSkillInstServer* pSkillInst,CFighterDictator* pFrom,CFighterDictator* pTo,const CValueData& Value,EHurtResult eResult,uint32 uExtraValue=0);
};

class CDrugChangeHPMOP
	:public CChangeHPMOP
{
public:
	void MagicOpExec(CSkillInstServer* pSkillInst,CFighterDictator* pFrom,CFighterDictator* pTo,const CValueData& Value,EHurtResult eResult,uint32 uExtraValue=0);
	void StatisticFightHPChange(EFightInfoIndex eFightInfoIndex, CSkillInstServer* pSkillInst, CFighterDictator* pFrom, uint32 uRealChangeValue);
};

class CChangeExpRateMOP
	:public CValueCancelableMagicOp
{
	uint32 Do(CSkillInstServer* pSkillInst, const CCfgCalc& Arg, CFighterDictator* pFrom, CFighterDictator* pTo,int32* MajorMopValue=0);
	int32 MagicOpCalc(CSkillInstServer* pSkillInst,const CCfgCalc& Arg,CFighterDictator* pFrom,CFighterDictator* pTo,CValueData *pValue=0,uint32* pExtraValue=0);
	void MagicOpExec(CSkillInstServer* pSkillInst,CFighterDictator* pFrom,CFighterDictator* pTo,const CValueData& Value,EHurtResult eResult,uint32 uExtraValue=0);
	void CancelExec(CSkillInstServer* pSkillInst,CFighterDictator* pFrom,CFighterDictator* pTo,const CValueData& Valuee);
	void Cancel(CSkillInstServer* pSkillInst, const CCfgCalc& Arg, CFighterDictator* pFrom, CFighterDictator* pTo);
	bool CanExecuteWithTargetIsDead()const
	{
		return true;
	}
};

class CChangeExpRateInFBMOP
	:public CValueCancelableMagicOp
{
	uint32 Do(CSkillInstServer* pSkillInst, const CCfgCalc& Arg, CFighterDictator* pFrom, CFighterDictator* pTo,int32* MajorMopValue=0);
	int32 MagicOpCalc(CSkillInstServer* pSkillInst,const CCfgCalc& Arg,CFighterDictator* pFrom,CFighterDictator* pTo,CValueData *pValue=0,uint32* pExtraValue=0);
	void MagicOpExec(CSkillInstServer* pSkillInst,CFighterDictator* pFrom,CFighterDictator* pTo,const CValueData& Value,EHurtResult eResult,uint32 uExtraValue=0);
	void CancelExec(CSkillInstServer* pSkillInst,CFighterDictator* pFrom,CFighterDictator* pTo,const CValueData& Valuee);
	void Cancel(CSkillInstServer* pSkillInst, const CCfgCalc& Arg, CFighterDictator* pFrom, CFighterDictator* pTo);
	bool CanExecuteWithTargetIsDead()const
	{
		return true;
	}
};

class CChangeFetchRateMOP
	:public CValueCancelableMagicOp
{
	uint32 Do(CSkillInstServer* pSkillInst, const CCfgCalc& Arg, CFighterDictator* pFrom, CFighterDictator* pTo,int32* MajorMopValue=0);
	int32 MagicOpCalc(CSkillInstServer* pSkillInst,const CCfgCalc& Arg,CFighterDictator* pFrom,CFighterDictator* pTo,CValueData *pValue=0,uint32* pExtraValue=0);
	void MagicOpExec(CSkillInstServer* pSkillInst,CFighterDictator* pFrom,CFighterDictator* pTo,const CValueData& Value,EHurtResult eResult,uint32 uExtraValue=0);
	void CancelExec(CSkillInstServer* pSkillInst,CFighterDictator* pFrom,CFighterDictator* pTo,const CValueData& Valuee);
	void Cancel(CSkillInstServer* pSkillInst, const CCfgCalc& Arg, CFighterDictator* pFrom, CFighterDictator* pTo);
	bool CanExecuteWithTargetIsDead()const
	{
		return true;
	}
};

class CChangeStealthMOP
	:public CValueCancelableMagicOp
{
	uint32 Do(CSkillInstServer* pSkillInst, const CCfgCalc& Arg, CFighterDictator* pFrom, CFighterDictator* pTo,int32* MajorMopValue=0);
	int32 MagicOpCalc(CSkillInstServer* pSkillInst,const CCfgCalc& Arg,CFighterDictator* pFrom,CFighterDictator* pTo,CValueData *pValue=0,uint32* pExtraValue=0);
	void MagicOpExec(CSkillInstServer* pSkillInst,CFighterDictator* pFrom,CFighterDictator* pTo,const CValueData& Value,EHurtResult eResult,uint32 uExtraValue=0);
	void CancelExec(CSkillInstServer* pSkillInst,CFighterDictator* pFrom,CFighterDictator* pTo,const CValueData& Valuee);
	void Cancel(CSkillInstServer* pSkillInst, const CCfgCalc& Arg, CFighterDictator* pFrom, CFighterDictator* pTo);
	bool CanExecuteWithTargetIsDead()const
	{
		return true;
	}
};

class CChangeKeennessMOP
	:public CValueCancelableMagicOp
{
	uint32 Do(CSkillInstServer* pSkillInst, const CCfgCalc& Arg, CFighterDictator* pFrom, CFighterDictator* pTo,int32* MajorMopValue=0);
	int32 MagicOpCalc(CSkillInstServer* pSkillInst,const CCfgCalc& Arg,CFighterDictator* pFrom,CFighterDictator* pTo,CValueData *pValue=0,uint32* pExtraValue=0);
	void MagicOpExec(CSkillInstServer* pSkillInst,CFighterDictator* pFrom,CFighterDictator* pTo,const CValueData& Value,EHurtResult eResult,uint32 uExtraValue=0);
	void CancelExec(CSkillInstServer* pSkillInst,CFighterDictator* pFrom,CFighterDictator* pTo,const CValueData& Valuee);
	void Cancel(CSkillInstServer* pSkillInst, const CCfgCalc& Arg, CFighterDictator* pFrom, CFighterDictator* pTo);
	bool CanExecuteWithTargetIsDead()const
	{
		return true;
	}
};

class CDisableStealthMOP
	:public CValueCancelableMagicOp
{
	uint32 Do(CSkillInstServer* pSkillInst, const CCfgCalc& Arg, CFighterDictator* pFrom, CFighterDictator* pTo,int32* MajorMopValue=0);
	int32 MagicOpCalc(CSkillInstServer* pSkillInst,const CCfgCalc& Arg,CFighterDictator* pFrom,CFighterDictator* pTo,CValueData *pValue=0,uint32* pExtraValue=0);
	void MagicOpExec(CSkillInstServer* pSkillInst,CFighterDictator* pFrom,CFighterDictator* pTo,const CValueData& Value,EHurtResult eResult,uint32 uExtraValue=0);
	void CancelExec(CSkillInstServer* pSkillInst,CFighterDictator* pFrom,CFighterDictator* pTo,const CValueData& Valuee);
	void Cancel(CSkillInstServer* pSkillInst, const CCfgCalc& Arg, CFighterDictator* pFrom, CFighterDictator* pTo);
	bool CanExecuteWithTargetIsDead()const
	{
		return true;
	}
};

class CDisableKeennessMOP
	:public CValueCancelableMagicOp
{
	uint32 Do(CSkillInstServer* pSkillInst, const CCfgCalc& Arg, CFighterDictator* pFrom, CFighterDictator* pTo,int32* MajorMopValue=0);
	int32 MagicOpCalc(CSkillInstServer* pSkillInst,const CCfgCalc& Arg,CFighterDictator* pFrom,CFighterDictator* pTo,CValueData *pValue=0,uint32* pExtraValue=0);
	void MagicOpExec(CSkillInstServer* pSkillInst,CFighterDictator* pFrom,CFighterDictator* pTo,const CValueData& Value,EHurtResult eResult,uint32 uExtraValue=0);
	void CancelExec(CSkillInstServer* pSkillInst,CFighterDictator* pFrom,CFighterDictator* pTo,const CValueData& Valuee);
	void Cancel(CSkillInstServer* pSkillInst, const CCfgCalc& Arg, CFighterDictator* pFrom, CFighterDictator* pTo);
	bool CanExecuteWithTargetIsDead()const
	{
		return true;
	}
};

class CAbsorbManaPointMOP
	:public CValueMagicOpServer
{
public:
	bool CanExecuteWithAttackerIsDead()const
	{
		return true;
	}
	uint32 Do(CSkillInstServer* pSkillInst, const CCfgCalc& Arg, CFighterDictator* pFrom, CFighterDictator* pTo,int32* MajorMopValue=0);
	int32 MagicOpCalc(CSkillInstServer* pSkillInst,const CCfgCalc& Arg,CFighterDictator* pFrom,CFighterDictator* pTo,CValueData *pValue=0,uint32* pExtraValue=0);
	void MagicOpExec(CSkillInstServer* pSkillInst,CFighterDictator* pFrom,CFighterDictator* pTo,const CValueData& Value,EHurtResult eResult,uint32 uExtraValue=0);
};

class CAbsorbRagePointMOP
	:public CValueMagicOpServer
{
public:
	bool CanExecuteWithAttackerIsDead()const
	{
		return true;
	}
	uint32 Do(CSkillInstServer* pSkillInst, const CCfgCalc& Arg, CFighterDictator* pFrom, CFighterDictator* pTo,int32* MajorMopValue=0);
	int32 MagicOpCalc(CSkillInstServer* pSkillInst,const CCfgCalc& Arg,CFighterDictator* pFrom,CFighterDictator* pTo,CValueData *pValue=0,uint32* pExtraValue=0);
	void MagicOpExec(CSkillInstServer* pSkillInst,CFighterDictator* pFrom,CFighterDictator* pTo,const CValueData& Value,EHurtResult eResult,uint32 uExtraValue=0);
};


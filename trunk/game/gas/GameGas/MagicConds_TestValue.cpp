#include "stdafx.h"
#include "MagicConds_TestValue.h"
#include "CFighterDictator.h"
#include "CMagicOpArg.h"
#include "CSkillInstServer.h"
#include "TPropertyHolder.inl"
#include "TPropertyHolder_inl.inl"
#include "CoreCommon.h"


uint32 CProbabilityLesserCond::Test(CSkillInstServer* pSkillInst,const CCfgCalc& Arg,const CFighterDictator* pFrom,const CFighterDictator* pTo) const
{
	CValueData Value = MagicCondCalc(pSkillInst,Arg,pFrom,pTo);
	bool bResult = MagicCondExec(pSkillInst,pFrom,pTo,Value);
	if (bResult)
		return eDSR_Success;
	else
		return eDSR_Fail;
}

CValueData CProbabilityLesserCond::MagicCondCalc(CSkillInstServer* pSkillInst,const CCfgCalc& Arg,const CFighterDictator* pFrom,const CFighterDictator* pTo) const
{
	CValueData Value = float(Arg.GetDblValue(pTo));
	return Value;
}

bool CProbabilityLesserCond::MagicCondExec(CSkillInstServer* pSkillInst,const CFighterDictator* pFrom,const CFighterDictator* pTo,const CValueData& Value) const
{

	float ArgValue=Value.GetBy<float>();
	float fDice	= GetRandFloat();
	//cout << "¸ÅÂÊÐ¡ÓÚ test " << fDice << " < " << ArgValue << endl; 
	return fDice<ArgValue;
}

uint32 CDistOfAttackerAndTargetLesserCond::Test(CSkillInstServer* pSkillInst,const CCfgCalc& Arg,const CFighterDictator* pFrom,const CFighterDictator* pTo) const
{
	CValueData Value = MagicCondCalc(pSkillInst,Arg,pFrom,pTo);
	bool bResult = MagicCondExec(pSkillInst,pFrom,pTo,Value);
	if (bResult)
		return eDSR_Success;
	else
		return eDSR_Fail;
}

CValueData CDistOfAttackerAndTargetLesserCond::MagicCondCalc(CSkillInstServer* pSkillInst,const CCfgCalc& Arg,const CFighterDictator* pFrom,const CFighterDictator* pTo) const
{
	CValueData Value = float(Arg.GetDblValue(pTo));
	return Value;
}

bool CDistOfAttackerAndTargetLesserCond::MagicCondExec(CSkillInstServer* pSkillInst,const CFighterDictator* pFrom,const CFighterDictator* pTo,const CValueData& Value) const
{
	float ArgValue=Value.GetBy<float>();

	//CFPos posAttacker, posTarget;
	//posAttacker=pFrom->GetPixelPos();
	//posTarget=pTo->GetPixelPos();
	//float x = posAttacker.x - posTarget.x;
	//float y = posAttacker.y - posTarget.y;
	//float fDistance = sqrt(x*x + y*y);
	float fDistance = pFrom->GetFighterDistInGrid(pTo);

	return fDistance<ArgValue;
}

uint32 CDistOfAttackerAndTargetGreaterOrEqualCond::Test(CSkillInstServer* pSkillInst,const CCfgCalc& Arg,const CFighterDictator* pFrom,const CFighterDictator* pTo) const
{
	CValueData Value = MagicCondCalc(pSkillInst,Arg,pFrom,pTo);
	bool bResult = MagicCondExec(pSkillInst,pFrom,pTo,Value);
	if (bResult)
		return eDSR_Success;
	else
		return eDSR_Fail;
}

CValueData CDistOfAttackerAndTargetGreaterOrEqualCond::MagicCondCalc(CSkillInstServer* pSkillInst,const CCfgCalc& Arg,const CFighterDictator* pFrom,const CFighterDictator* pTo) const
{
	CValueData Value = float(Arg.GetDblValue(pTo));
	return Value;
}

bool CDistOfAttackerAndTargetGreaterOrEqualCond::MagicCondExec(CSkillInstServer* pSkillInst,const CFighterDictator* pFrom,const CFighterDictator* pTo,const CValueData& Value) const
{
	float ArgValue=Value.GetBy<float>();

	//CFPos posAttacker, posTarget;
	//posAttacker=pFrom->GetPixelPos();
	//posTarget=pTo->GetPixelPos();
	//float x = posAttacker.x - posTarget.x;
	//float y = posAttacker.y - posTarget.y;
	//float fDistance = sqrt(x*x + y*y);
	float fDistance = pFrom->GetFighterDistInGrid(pTo);

	return fDistance>=ArgValue;
}

uint32 CDistOfMagicAndTargetLesserCond::Test(CSkillInstServer* pSkillInst,const CCfgCalc& Arg,const CFighterDictator* pFrom,const CFighterDictator* pTo) const
{
	CValueData Value = MagicCondCalc(pSkillInst,Arg,pFrom,pTo);
	bool bResult = MagicCondExec(pSkillInst,pFrom,pTo,Value);
	if (bResult)
		return eDSR_Success;
	else
		return eDSR_Fail;
}

CValueData CDistOfMagicAndTargetLesserCond::MagicCondCalc(CSkillInstServer* pSkillInst,const CCfgCalc& Arg,const CFighterDictator* pFrom,const CFighterDictator* pTo) const
{
	CValueData Value = float(Arg.GetDblValue(pTo));
	return Value;
}

bool CDistOfMagicAndTargetLesserCond::MagicCondExec(CSkillInstServer* pSkillInst,const CFighterDictator* pFrom,const CFighterDictator* pTo,const CValueData& Value) const
{
	float ArgValue=Value.GetBy<float>();

	CFPos posMagicInitPos, posTarget;
	posMagicInitPos=pSkillInst->GetMagicBeginPos();

	if(posMagicInitPos == CFPos(0.0f,0.0f))
		return false;

	posTarget=pTo->GetPixelPos();
	float x = posMagicInitPos.x - posTarget.x;
	float y = posMagicInitPos.y - posTarget.y;
	float fDistance = sqrt(x*x + y*y);
	fDistance = fDistance / eGridSpanForObj;

	return fDistance<ArgValue;
}

uint32 CDistOfMagicAndTargetGreaterOrEqualCond::Test(CSkillInstServer* pSkillInst,const CCfgCalc& Arg,const CFighterDictator* pFrom,const CFighterDictator* pTo) const
{
	CValueData Value = MagicCondCalc(pSkillInst,Arg,pFrom,pTo);
	bool bResult = MagicCondExec(pSkillInst,pFrom,pTo,Value);
	if (bResult)
		return eDSR_Success;
	else
		return eDSR_Fail;
}

CValueData CDistOfMagicAndTargetGreaterOrEqualCond::MagicCondCalc(CSkillInstServer* pSkillInst,const CCfgCalc& Arg,const CFighterDictator* pFrom,const CFighterDictator* pTo) const
{
	CValueData Value = float(Arg.GetDblValue(pTo));
	return Value;
}

bool CDistOfMagicAndTargetGreaterOrEqualCond::MagicCondExec(CSkillInstServer* pSkillInst,const CFighterDictator* pFrom,const CFighterDictator* pTo,const CValueData& Value) const
{
	float ArgValue=Value.GetBy<float>();

	CFPos posMagicInitPos, posTarget;
	posMagicInitPos=pSkillInst->GetMagicBeginPos();

	if(posMagicInitPos == CFPos(0.0f,0.0f))
		return false;

	posTarget=pTo->GetPixelPos();
	float x = posMagicInitPos.x - posTarget.x;
	float y = posMagicInitPos.y - posTarget.y;
	float fDistance = sqrt(x*x + y*y);
	fDistance = fDistance / eGridSpanForObj;

	return fDistance>=ArgValue;
}

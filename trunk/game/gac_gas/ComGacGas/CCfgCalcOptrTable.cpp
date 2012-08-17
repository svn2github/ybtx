#include "stdafx.h"
#include "CCfgCalcOptrTable.h"
#include "ExpHelper.h"
#include <ctime>
#include "TSqrAllocator.inl"

uint32 COptrTbl::m_uOptrEyes[];
MapStr2Optr COptrTbl::m_mapOptrNo;
MapStr2VarType COptrTbl::m_mapVarTypeNo;
MapStr2CalcTargetType COptrTbl::m_mapCalcTargetTypeNo;
bool COptrTbl::__init = COptrTbl::Init();



COptrUnit::COptrUnit(uint32 t, uint32 e, uint32 g, ECalcTargetType target)
: theta(t), 
  eyes(e), 
  grade(g),
  targettype(target)
{

}

COpndUnit::COpndUnit()
: type(eUT_Null)
{

}

bool COptrTbl::Init()
{
	if(g_sOptrStr[eOI_End] != "#e")
	{
		GenErr("g_sOptrStr数组映射错误");
		//return false;
	}
	if(g_sVarTypeStr[eVT_End] != "#e")
	{
		GenErr("g_sVarTypeStr数组映射错误");
		//return false;
	}
	if(g_sCalcTargetTypeStr[eCTT_End] != "#e")
	{
		GenErr("g_sCalcTargetTypeStr数组映射错误");
		//return false;
	}

	for(uint32 i = eOI_Start2Eyes; i < eOI_Start1Eyes; i++)
	{
		m_uOptrEyes[i] = 2;
	}
	for(uint32 i = eOI_Start1Eyes; i < eOI_End; i++)
	{
		m_uOptrEyes[i] = 1;
	}
	for(uint32 i = 0; i < eOI_End; i++)
	{
		m_mapOptrNo[g_sOptrStr[i]] = EOptrID(i);
	}
	for(uint32 i = eVT_Arg; i < eVT_End; i++)
	{
		m_mapVarTypeNo[g_sVarTypeStr[i]] = EFunType(i);
	}
	for(uint32 i = 0; i < eCTT_End; i++)
	{
		m_mapCalcTargetTypeNo[g_sCalcTargetTypeStr[i]] = ECalcTargetType(i);
	}
	srand(uint32(time(0)));
	rand();
	rand();
	rand();	
	return true;
}

uint32 COptrTbl::OptrPrecede(const COptrUnit& optr_a, uint32 b, uint32 grade_b)
{
	//uint32 a = optr_a.theta;
	if(optr_a.theta == eOI_Shell) return 1;
	if(b == eOI_Shell) return 2;
	uint32 j = 0;
	switch(optr_a.eyes)
	{
	case 1:
		j = EYES1_GRADE;
		break;
	case 2:
		j = g_uOptrGrade[optr_a.theta];
		break;
	default:
		{
			stringstream ExpStr;
			ExpStr << "表达式运算符" << g_sOptrStr[optr_a.theta] << "目数未知\n";
			GenErr(ExpStr.str());
			//return 0;
		}
	}
	if(j + optr_a.grade < g_uOptrGrade[b] + grade_b) return 1;
	else return 2;
}

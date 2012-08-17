#include "stdafx.h"
#include "CCfgCalcFunction.h"
#include "CTxtTableFile.h"
#include "LoadSkillCommon.h"
#include "CCfgCalc.h"
#include "FightDef.h"
#include <cmath>
#include "BaseHelper.h"
#include "CCfgColChecker.inl"
#include "CQuickRand.h"
#include "StringHelper.h"

using namespace std;
using namespace CfgChk;

#define MAX_INDEX_SIZE 150

namespace sqr
{
	extern const wstring PATH_ALIAS_CFG;
}

inline static void CreateExpStr(stringstream& ExpStr, const int32& Row, TCHAR* ColumeName)
{
	ExpStr << "第" << Row << "行" << ColumeName << "数值错误";
}

//bool CCfgGlobal::__init = CCfgGlobal::Init();
CCfgGlobal::MapGlobalSkillParam CCfgGlobal::m_mapVar;
CCfgGlobal::VecGlobalSkillParam CCfgGlobal::m_vecVar;
bool CCfgGlobal::m_bRandValueIsFixed = false;
int32 CCfgGlobal::m_iRandFixedValue = 0;
bool CCfgGlobal::m_bRandfValueIsFixed = false;
double CCfgGlobal::m_dRandfFixedValue = 0.0;




//以下是CCfgGlobal操作：
//bool CCfgGlobal::Init()
//{
//	m_dVecResult.resize(MAX_INDEX_SIZE);
//	for(size_t i = 0; i < MAX_INDEX_SIZE; ++i)
//	{
//		m_bCalculated[i] = false;
//	}
//	return true;
//}


bool CCfgGlobal::LoadConfig(const TCHAR* cfgFile)
{
	CTxtTableFile TabFile;

	CfgChk::SetTabFile(TabFile, "全局参数");
	if (!TabFile.Load(PATH_ALIAS_CFG.c_str(), cfgFile)) return false;
	UnloadConfig();
	m_vecVar.resize(TabFile.GetHeight());

	for(int32 i=1; i<TabFile.GetHeight(); i++)
	{
		stringstream strName;
		string strName1 = TabFile.GetString(i, szGlobalParam_Name1);
		tolower(strName1);
		trimend(strName1);
		string strName2 = TabFile.GetString(i, szGlobalParam_Name2);
		tolower(strName2);
		trimend(strName2);
		strName << strName1 << "." << strName2;

		if(strName.str() == ".")
		{
			continue;
		}

		CCfgCalc* pCfgCalc = new CCfgCalc(TabFile.GetString(i, szGlobalParam_CfgCalc));
		pCfgCalc->SetTypeExpression();

		CCfgGlobal* pCfg = new CCfgGlobal;
		pCfg->m_pCfgCalc = pCfgCalc;

		SetLineNo(i);
		ReadItem(pCfg->m_bIsArrayConst, szGlobalParam_IsArrayConst, CANEMPTY);
		if(pCfg->m_bIsArrayConst)
		{
			pCfg->m_bDeqCalculated.assign(MAX_INDEX_SIZE, false);
			pCfg->m_dVecResult.assign(MAX_INDEX_SIZE, 0.0);
		}
			
		pair<MapGlobalSkillParam::iterator, bool> pr = m_mapVar.insert(make_pair(strName.str(), pCfg));
		pCfg->m_uId = i;
		m_vecVar[i] = pCfg;
		if(!pr.second)
		{
			SafeDelete(pCfg);
			stringstream ExpStr;
			ExpStr << "第" << i << "行的" << "全局技能参数 " << strName.str() << " 重复";
			GenErr(ExpStr.str());
			//return false;
		}
	}
	return true;
}

void CCfgGlobal::UnloadConfig()
{
	ClearMap(m_mapVar);
	m_vecVar.clear();
}

CCfgGlobal::~CCfgGlobal()
{
	delete m_pCfgCalc;
	m_dVecResult.clear();
	m_bDeqCalculated.clear();
}


template<>
double CCfgGlobal::Get<CFighterNull>(const CFighterNull* pFrom, const CFighterNull* pTo, uint32 index)
{
	if(m_bIsArrayConst)
	{
		if(m_bDeqCalculated[index])
		{
			return m_dVecResult[index];
		}
		else
		{
			m_bDeqCalculated[index] = true;
			return m_dVecResult[index] = m_pCfgCalc->GetDblValue(pFrom, pTo, index);
		}
	}
	else
	{
		return m_pCfgCalc->GetDblValue(pFrom, pTo, index);
	}
}

pair<bool, CCfgGlobal*> CCfgGlobal::GetCfgValue(const string& name)
{
	MapGlobalSkillParam::iterator itr = m_mapVar.find(name);
	if(itr != m_mapVar.end())
	{
		return make_pair(true, itr->second);
	}
	else
	{
		return make_pair(false, (CCfgGlobal*)(NULL));
	}
}

bool CCfgGlobal::ExistCfgGlobal(const string& name)
{
	return m_mapVar.find(name) != m_mapVar.end();
}

const TCHAR* CCfgGlobal::GetCalcChars(const TCHAR* sName)
{
	static string sEmpty = "";
	MapGlobalSkillParam::iterator itr = m_mapVar.find(sName);
	if(itr != m_mapVar.end())
	{
		if(itr->second->m_pCfgCalc)
			return itr->second->m_pCfgCalc->GetTestString().c_str();
	}
	return sEmpty.c_str();
}

uint32 CCfgGlobal::GetVarId(const string& name)
{
	MapGlobalSkillParam::iterator itr = m_mapVar.find(name);
	if(itr != m_mapVar.end())
	{
		return itr->second->m_uId;
	}
	else
	{
		return 0;
	}
}

pair<bool, CCfgGlobal*> CCfgGlobal::GetCfgValue(uint32 uId)
{
	if(uId >= m_vecVar.size())
	{
		stringstream str;
		str << "全局变量下标" << uId << "超过变量上限" << m_vecVar.size();
		GenExpInfo(str.str());
		return make_pair(false, (CCfgGlobal*)(NULL));
	}
	
	return make_pair(true, m_vecVar[uId]);
}



void CCfgGlobal::SetRandFixedValue(int32 iValue)
{
	m_bRandValueIsFixed = true;
	m_iRandFixedValue = iValue;
}

void CCfgGlobal::SetRandfFixedValue(double dValue)
{
	m_bRandfValueIsFixed = true;
	m_dRandfFixedValue = dValue;
}

void CCfgGlobal::ClearRandFixedValue()
{
	m_bRandValueIsFixed = false;
}

void CCfgGlobal::ClearRandfFixedValue()
{
	m_bRandfValueIsFixed = false;
}






//以下是CAttrVarMap操作：
CAttrVarMap::MapAttrVarName2Id CAttrVarMap::m_mapVarId;
bool CAttrVarMap::__init = CAttrVarMap::InitMapVarId();

bool CAttrVarMap::InitMapVarId()
{
	//注意：如果名字中有字母则必须用小写
	m_mapVarId["当前生命值"]	= ePID_HealthPoint * ePFT_Count + ePFT_Agile;
	m_mapVarId["生命值上限"]	= ePID_HealthPoint * ePFT_Count + ePFT_Value;
	m_mapVarId["当前魔法值"]	= ePID_ManaPoint * ePFT_Count + ePFT_Agile;
	m_mapVarId["魔法值上限"]	= ePID_ManaPoint * ePFT_Count + ePFT_Value;
	m_mapVarId["当前怒气值"]	= ePID_RagePoint * ePFT_Count + ePFT_Agile;

	m_mapVarId["治疗量"]		= ePID_MagicDamageValue*ePFT_Count + ePFT_Value;

	m_mapVarId["格挡减免值"]	= ePID_BlockDamage * ePFT_Count + ePFT_Value;
	m_mapVarId["格挡率"]		= ePID_BlockRate * ePFT_Count + ePFT_Value;
	
	m_mapVarId["主手武器最大伤害"]	= ePID_MainHandMaxWeaponDamage * ePFT_Count + ePFT_Value;
	m_mapVarId["主手武器最小伤害"]	= ePID_MainHandMinWeaponDamage * ePFT_Count + ePFT_Value;
	m_mapVarId["主手武器攻击间隔"]	= ePID_MainHandWeaponInterval * ePFT_Count + ePFT_Value;
	m_mapVarId["主手武器攻击间隔伤害增加系数"]	= ePID_MHWeaponIntervalExtraDamageRate * ePFT_Count + ePFT_Value;
	m_mapVarId["副手武器攻击间隔伤害增加系数"]	= ePID_AHWeaponIntervalExtraDamageRate * ePFT_Count + ePFT_Value;
	m_mapVarId["副手武器攻击间隔"]	= ePID_AssistantWeaponInterval * ePFT_Count + ePFT_Value;

	m_mapVarId["移动速度"] = ePID_RunSpeed * ePFT_Count + ePFT_Value;

	m_mapVarId["攻击力"] = ePID_PhysicalDPS * ePFT_Count + ePFT_Value;

	m_mapVarId["法伤"] = ePID_MagicDamageValue * ePFT_Count + ePFT_Value;

	m_mapVarId["生命值恢复速度"] = ePID_HPUpdateRate * ePFT_Count + ePFT_Value;
	m_mapVarId["魔法值恢复速度"] = ePID_MPUpdateRate * ePFT_Count + ePFT_Value;
	m_mapVarId["护甲值"] = ePID_Defence * ePFT_Count + ePFT_Value;
	m_mapVarId["招架值"] = ePID_ParryValue * ePFT_Count + ePFT_Value;
	m_mapVarId["物理躲避值"] = ePID_PhysicalDodgeValue * ePFT_Count + ePFT_Value;
	m_mapVarId["爆击值"] = ePID_StrikeValue * ePFT_Count + ePFT_Value;
	m_mapVarId["爆击倍数值"] = ePID_StrikeMultiValue * ePFT_Count + ePFT_Value;
	m_mapVarId["韧性值"] = ePID_ResilienceValue * ePFT_Count + ePFT_Value;
	m_mapVarId["免暴值"] = ePID_StrikeResistanceValue * ePFT_Count + ePFT_Value;
	m_mapVarId["精准值"] = ePID_AccuratenessValue * ePFT_Count + ePFT_Value;
	m_mapVarId["法术命中值"] = ePID_MagicHitValue * ePFT_Count + ePFT_Value;
	m_mapVarId["法术躲避值"] = ePID_MagicDodgeValue * ePFT_Count + ePFT_Value;
	m_mapVarId["自然系抗性值"] = ePID_NatureResistanceValue * ePFT_Count + ePFT_Value;
	m_mapVarId["破坏系抗性值"] = ePID_DestructionResistanceValue * ePFT_Count + ePFT_Value;
	m_mapVarId["暗黑系抗性值"] = ePID_EvilResistanceValue * ePFT_Count + ePFT_Value;
	m_mapVarId["破坏系抗性值百分比"] = ePID_DestructionResistanceValue * ePFT_Count + ePFT_Multiplier;
	
	m_mapVarId["普攻伤害"] = eMAID_NADamage;
	m_mapVarId["人物等级"] = eMAID_FighterLevel;
	m_mapVarId["主武是否单"] = eMAID_MainHandWeaponIsSingle;

	return true;
}

pair<bool, uint32> CAttrVarMap::GetVarId(const string& name)
{
	MapAttrVarName2Id::iterator itr = m_mapVarId.find(name);
	if(itr == m_mapVarId.end())
	{
		return make_pair(false, eMAID_Undefined);
	}
	else
	{
		return make_pair(true, itr->second);
	}
}

//以下是静态函数：
int32 CCfgGlobal::Rand(double fStart, double fEnd)
{
	if(m_bRandValueIsFixed && fStart <= m_iRandFixedValue && fEnd >= m_iRandFixedValue)
	{
		return m_iRandFixedValue;
	}
	double fMin = min(fStart, fEnd);
	double fMax = max(fStart, fEnd);
	double fFixMin = ceil(fMin);
	Ast(fFixMin == (int32)fFixMin);
	double fFixMax = floor(fMax + 1.0);
	Ast(fFixMax == (int32)fFixMax);
	//int32 iResult = (int32)(fFixMin + rand() / (double)(RAND_MAX + 1) * (fFixMax - fFixMin));
	int32 iResult = CQuickRand::Rand2((int32)fFixMin, (int32)fFixMax);
	if(iResult > fMax || iResult < fMin)
	{
		stringstream sExp;
		sExp << "整数结果" << iResult << "不在表达式Rand(" << fStart << "," << fEnd << ") 浮点数范围内\n";
		cout << sExp.str();
	}
	return iResult;
}






double CCfgGlobal::Randf(double fStart, double fEnd)
{
	if(m_bRandfValueIsFixed && fStart <= m_dRandfFixedValue && fEnd >= m_dRandfFixedValue)
	{
		return m_dRandfFixedValue;
	}
	return rand() / (double)RAND_MAX * (fEnd - fStart) + fStart;
}	

double CCfgGlobal::IfElse(double fIf, double fThen, double fElse)
{
	if(fIf)
	{
		return fThen;
	}
	else
	{
		return fElse;
	}
}

template<>
bool CCfgGlobal::ExistMagicState<CFighterNull>(string& name, const CFighterNull* pFighter)
{
	return false;
}

template<>
bool CCfgGlobal::ExistTriggerState<CFighterNull>(string& name, const CFighterNull* pFighter)
{
	return false;
}

template<>
bool CCfgGlobal::ExistDamageChangeState<CFighterNull>(string& name, const CFighterNull* pFighter)
{
	return false;
}

template<>
bool CCfgGlobal::ExistSpecialState<CFighterNull>(string& name, const CFighterNull* pFighter)
{
	return false;
}

template<>
bool CCfgGlobal::ExistState<CFighterNull>(string& name, const CFighterNull* pFighter)
{
	return false;
}

template<>
uint32 CCfgGlobal::StateCount<CFighterNull>(string& name, const CFighterNull* pFighter)
{
	return 0;
}

template<>
uint32 CCfgGlobal::CurRlserStateCount<CFighterNull>(string& name, const CFighterNull* pFrom, const CFighterNull* pFighter)
{
	return 0;
}

template<>
uint32 CCfgGlobal::TriggerCount<CFighterNull>(string& name, const CFighterNull* pFighter)
{
	return 0;
}

template<>
int32 CCfgGlobal::StateLeftTime<CFighterNull>(string& name, const CFighterNull* pFrom, const CFighterNull* pTo)
{
	return 0;
}

template<>
double CCfgGlobal::Distance<CFighterNull>(const CFighterNull* pFrom, const CFighterNull* pTo)
{
	return 0.0;
}

template<>
bool CCfgGlobal::TargetIsNPC<CFighterNull>(const CFighterNull* pTo)
{
	return 0.0;
}

bool CCfgGlobal::IsActiveSkill(string& name, const CFighterNull* pFighter)
{
	return true;
}


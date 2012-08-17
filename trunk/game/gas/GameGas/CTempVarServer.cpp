#include "stdafx.h"
#include "CTempVarServer.h"
#include "ExpHelper.h"
#include "TSqrAllocator.inl"
#include "CCfgCalcOptrTable.h"
#include "CTempVarMgr.h"

CTempVarMgrServer::MapTempVar	CTempVarMgrServer::m_mapPVar;
CTempVarMgrServer::VecTempVar	CTempVarMgrServer::m_vecPVar;
set<string> CTempVarMgrServer::m_setImmuneTaskAttackName;
bool CTempVarMgrServer::__init = CTempVarMgrServer::Init();


CTempVarMgrServer::CTempVarMgrServer(const CFighterDictator* pHolder)
: m_iMopCalcValue(0)
, m_bLockIncreaseHP(false)
, m_bLockDecreaseHP(false)
, m_bLockIncreaseAgile(false)
, m_bLockDecreaseAgile(false)
, m_fDamageShareProportion(0)
, m_pLinkFighter(NULL)
, m_iDisTriggerEvent(0)
, m_iPrintInfoOff(0)
, m_uSubHP(0)
, m_uBeSubHP(0)
, m_uUnchangeSubHP(0)
, m_uBeUnchangeSubHP(0)
, m_uSubMP(0)
, m_uBeSubMP(0)
, m_uHeal(0)
, m_uGetHeal(0)
, m_uUnchangeHealHP(0)
, m_uBeUnchangeHealHP(0)
, m_iNoSingTime(0)
, m_strDamageChangeStateName("")
, m_pHolder(pHolder)
{
	for(set<string>::iterator itr = m_setImmuneTaskAttackName.begin(); itr != m_setImmuneTaskAttackName.end(); ++itr)
	{
		m_iImmuneTaskAttack[*itr] = 0;
	}
}


bool CTempVarMgrServer::Init()
{
	//注意：如果名字中有字母则必须用小写
	
	//类成员指针问题2010-12-21
	//m_mapPVar["伤害"] = &CTempVarMgrServer::m_uSubHP;
	//m_mapPVar["被伤害"] = &CTempVarMgrServer::m_uBeSubHP;
	//m_mapPVar["未变更伤害"] = &CTempVarMgrServer::m_uUnchangeSubHP;
	//m_mapPVar["被未变更伤害"] = &CTempVarMgrServer::m_uBeUnchangeSubHP;
	//m_mapPVar["减蓝"] = &CTempVarMgrServer::m_uSubMP;
	//m_mapPVar["被减蓝"] = &CTempVarMgrServer::m_uBeSubMP;
	//m_mapPVar["治疗"] = &CTempVarMgrServer::m_uHeal;
	//m_mapPVar["被治疗"] = &CTempVarMgrServer::m_uGetHeal;
	//m_mapPVar["未变更治疗"] = &CTempVarMgrServer::m_uUnchangeHealHP;
	//m_mapPVar["被未变更治疗"] = &CTempVarMgrServer::m_uBeUnchangeHealHP;
	//m_mapPVar["魔法操作计算值"] = &CTempVarMgrServer::m_iMopCalcValue;

	m_vecPVar.push_back(0);		//0保留不用

#define RegisterTempVar(KeyName, MemberVar)	\
	m_mapPVar[KeyName] = m_vecPVar.size(); m_vecPVar.push_back(&CTempVarMgrServer::MemberVar);

	RegisterTempVar("伤害", m_uSubHP);
	RegisterTempVar("被伤害", m_uBeSubHP);
	RegisterTempVar("未变更伤害", m_uUnchangeSubHP);
	RegisterTempVar("被未变更伤害", m_uBeUnchangeSubHP);
	RegisterTempVar("减蓝", m_uSubMP);
	RegisterTempVar("被减蓝", m_uBeSubMP);
	RegisterTempVar("治疗", m_uHeal);
	RegisterTempVar("被治疗", m_uGetHeal);
	RegisterTempVar("未变更治疗", m_uUnchangeHealHP);
	RegisterTempVar("被未变更治疗", m_uBeUnchangeHealHP);
	RegisterTempVar("魔法操作计算值", m_iMopCalcValue);

#undef RegisterTempVar

	m_setImmuneTaskAttackName.insert("任务非战斗");
	m_setImmuneTaskAttackName.insert("任务炸弹");
	m_setImmuneTaskAttackName.insert("任务特殊");
	m_setImmuneTaskAttackName.insert("非国战建筑");

	CTempVarMgr::SetSetVarPtrFunc(CTempVarMgrServer::SetVarPtr);
	return true;
}

bool CTempVarMgrServer::SetVarPtr(UUnitValue* pVarPtr, const string& sVarName)
{
	//类成员指针问题2010-12-21
	//if(m_mapPVar.find(sVarName) == m_mapPVar.end())
	//{
	//	pVarPtr->pOff = NULL;
	//	return false;
	//}
	//else
	//{
	//	pVarPtr->pOff = m_mapPVar[sVarName];
	//	return true;
	//}

	MapTempVar::iterator itr = m_mapPVar.find(sVarName);	
	if(itr == m_mapPVar.end())
	{
		pVarPtr->lng = 0;
		return false;
	}
	else
	{
		pVarPtr->lng = itr->second;
		return true;
	}
}

int32 CTempVarMgrServer::GetVarValue(const string& sVarName)
{
	//类成员指针问题2010-12-21
	//if(m_mapPVar.find(sVarName) == m_mapPVar.end())
	//{
	//	stringstream str;
	//	str << sVarName << endl;
	//	GenErr("临时变量不存在", str.str());
	//	//return 0;
	//}
	//return this->*m_mapPVar[sVarName];

	MapTempVar::iterator itr = m_mapPVar.find(sVarName);	
	if(itr == m_mapPVar.end())
	{
		stringstream str;
		str << sVarName << endl;
		GenErr("临时变量不存在", str.str());
		//return 0;
	}
	else if (itr->second >= m_vecPVar.size())
	{
		stringstream str;
		str << itr->second << " >= " << m_vecPVar.size() << endl;
		GenErr("临时变量访问超出索引上限", str.str());
	}
	return this->*m_vecPVar[itr->second];
}

int32 CTempVarMgrServer::GetVarValue(UUnitValue pVarPtr)
{
	//类成员指针问题2010-12-21
	//if(!pVarPtr.pOff)
	//{
	//	stringstream str;
	//	GenErr("偏移值为空，临时变量不存在");
	//	//return 0;
	//}
	//return this->*pVarPtr.pOff;

	if(pVarPtr.lng == 0 || pVarPtr.lng >= m_vecPVar.size())
	{
		stringstream str;
		str << pVarPtr.lng << " >= " << m_vecPVar.size() << endl;
		GenErr("临时变量访问索引为0或超出索引上限", str.str());
		//return 0;
	}
	return this->*m_vecPVar[pVarPtr.lng];
}

void CTempVarMgrServer::SetVarValue(const string& sVarName, int32 iValue)
{
	//类成员指针问题2010-12-21
	//if(iValue < 0 && sVarName != "魔法操作计算值")
	//{
	//	stringstream str;
	//	str << sVarName << " = " << iValue << endl;
	//	GenErr("临时变量不能为负值", str.str());
	//}
	//this->*m_mapPVar[sVarName] = iValue;

	//类成员指针问题2010-12-21
	if(iValue < 0 && sVarName != "魔法操作计算值")
	{
		stringstream str;
		str << sVarName << " = " << iValue << endl;
		GenErr("临时变量不能为负值", str.str());
	}
	this->*m_vecPVar[m_mapPVar[sVarName]] = iValue;
}

string CTempVarMgrServer::GetPassVarName(const string& sVarName)	
{
	string sPassVarName;
	if(sVarName.substr(0, 2) != "被")
	{
		sPassVarName = "被" + sVarName;
		//sPassVarName.insert(0, "被");
		return sPassVarName;
	}
	else
	{
		return sVarName.substr(2);
	}
}

void CTempVarMgrServer::SetDamage(int32 iValue)		
{
	if(iValue < 0)
	{
		stringstream str;
		str << "伤害 = " << iValue << endl;
		GenErr("临时变量不能为负值", str.str());
	}
	//SetVarValue(string("伤害"), iValue);
	//SetVarValue(string("未变更伤害"), iValue);
	m_uSubHP = iValue;
	m_uUnchangeSubHP = iValue;
}
void CTempVarMgrServer::SetBeDamaged(int32 iValue)
{
	if(iValue < 0)
	{
		stringstream str;
		str << "被伤害 = " << iValue << endl;
		GenErr("临时变量不能为负值", str.str());
	}
	//SetVarValue(string("被伤害"), iValue);
	//SetVarValue(string("被未变更伤害"), iValue);
	m_uBeSubHP = iValue;
	m_uBeUnchangeSubHP = iValue;
}

void CTempVarMgrServer::SetSubMP(int32 iValue)
{
	//SetVarValue(string("减蓝"), iValue);
	m_uSubMP = iValue;
}

void CTempVarMgrServer::SetBeSubMP(int32 iValue)
{
	//SetVarValue(string("被减蓝"), iValue);
	m_uBeSubMP = iValue;
}



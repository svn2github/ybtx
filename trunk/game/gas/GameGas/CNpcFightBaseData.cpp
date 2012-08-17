#include "stdafx.h"
#include "CTxtTableFile.h"
#include "CNpcFightBaseData.h"
#include "CCfgColChecker.inl"
#include "TimeHelper.h"
#include "TSqrAllocator.inl"
#include "Random.h"
#include "CSkillServer.h"
#include "BaseHelper.h"

namespace sqr
{
	extern const wstring PATH_ALIAS_CFG;
}

CNpcFightBaseData::CNpcFightBaseData( const string& sName)
:m_sName(sName)
{

}

CNpcFightBaseData::~CNpcFightBaseData()
{
	ClearVector(m_lstBornSkill);
}

void CNpcFightBaseData::GetSkillRuleName(string& strSkillRuleName) const
{

	uint32 uRandomNum = Random::Rand(0,100);
	LstSkillRule::const_iterator iter = m_lSkillRule.begin();
	for (; iter != m_lSkillRule.end(); iter++)
	{
		if ((*iter).second >= uRandomNum)
		{
			strSkillRuleName = (*iter).first;
			return;
		}
	}
}

void CNpcFightBaseData::CreateNpcBornSkill(string &strSkill)
{
	if(strSkill.empty())
		return;
	erase(strSkill, " ");
	erase(strSkill,"\"");
	replace(strSkill, "，", ",");
	int32 startPos = -1;
	string strTemp;
	while (true)
	{
		if(strSkill.empty())
			return;
		startPos = strSkill.find(',');
		if(-1 == startPos)
		{
			const CNormalSkillServerSharedPtr* pSkillServer = new CNormalSkillServerSharedPtr(CNormalSkillServer::GetNPCSkill(strSkill));
			m_lstBornSkill.push_back(pSkillServer);
			return;
		}
		strTemp = strSkill.substr(0,startPos);
		if(strTemp.empty())
		{
			if(startPos < int32(strSkill.length())-1)
			{
				ostringstream strm;
				strm<<"Npc："<< GetName() <<" 的出生释放技能列格式不对，出现连续 ,, 的情况"<< endl;
				GenErr(strm.str());
			}
			else
				return;
		}
		const CNormalSkillServerSharedPtr* pSkillServer = new CNormalSkillServerSharedPtr(CNormalSkillServer::GetNPCSkill(strTemp));
		m_lstBornSkill.push_back(pSkillServer);
		strSkill = strSkill.substr(startPos+1,strSkill.length());
	}
}

void CNpcFightBaseData::CreateNpcSkillRule(string& strSkillRule)
{
	if(strSkillRule.empty())
		return;
	erase(strSkillRule, " ");
	erase(strSkillRule,"\"");
	replace(strSkillRule, "，", ",");
	replace(strSkillRule, "（","(");
	replace(strSkillRule, "）", ")");
	if (strSkillRule.find(",") == -1) //说明只有一个技能规则的Npc
	{
		m_lSkillRule.push_back(make_pair(strSkillRule, 100));
		return;
	}

	uint32 uNumber = 0;
	//下面是有多个技能规则的情况
	while (true)
	{
		if(strSkillRule.empty())
			return;
		string strSkillRuleNode;
		int32 uNext = strSkillRule.find(",");
		if (uNext != -1)
		{
			strSkillRuleNode = strSkillRule.substr(0, uNext);
			strSkillRule = strSkillRule.substr(uNext+1, strSkillRule.length());
		}
		else
		{
			strSkillRuleNode = strSkillRule;
			strSkillRule.clear();
		}
		Ast (!strSkillRuleNode.empty());
		int32 uStatrt = strSkillRuleNode.find("(");
		int32 uEnd = strSkillRuleNode.find(")");
		if (uStatrt == -1 || uEnd ==-1)
		{
			ostringstream strm;
			strm<<"NpcFightBaseData_Server表中 Npc："<< GetName() <<" 的技能规则填写错误！"<< endl;
			GenErr(strm.str());
		}
		string strSkillRuleName = strSkillRuleNode.substr(0, uStatrt);
		string strNum = strSkillRuleNode.substr(uStatrt+1, uEnd-uStatrt-1);
		uNumber = uNumber + static_cast<uint32>(atoi(strNum.c_str()));
		m_lSkillRule.push_back(make_pair(strSkillRuleName, uNumber));
	}
	if (uNumber != 100)
	{
		ostringstream strm;
		strm<<"NpcFightBaseData_Server表中 Npc："<< GetName() <<" 的技能规则总概率不为100，轻查实！"<< endl;
		GenErr(strm.str());
	}
}



CNpcFightBaseDataMgr::CNpcFightBaseDataMgr()
{
}

CNpcFightBaseDataMgr::~CNpcFightBaseDataMgr()
{
	DeleteAll();
}

const CNpcFightBaseData* CNpcFightBaseDataMgr::GetEntity( const string& sName)
{
	IndexMapType::const_iterator it = m_mapIndex.find(sName);
	return it == m_mapIndex.end()  ? NULL : it->second;
}
bool CNpcFightBaseDataMgr::AddEntity( CNpcFightBaseData* pEntity )
{
	if( GetEntity( pEntity->GetName()) != NULL )
		return false;
	m_mapIndex[pEntity->GetName()] = pEntity;
	return true;
}
void CNpcFightBaseDataMgr::DeleteAll()
{
	for ( IndexMapType::const_iterator it = m_mapIndex.begin(); it != m_mapIndex.end(); ++it)
	{
		delete(it->second);
	}
	m_mapIndex.clear();
}
bool CNpcFightBaseDataMgr::LoadConfig( const string& sFileName )
{
	using namespace CfgChk;
	CTxtTableFile TabFile;
	CNpcFightBaseData*  pValue;
	string   sLimit = "";
	string	 sCondition = "";
	string	 sSkill = "";
	string	sBornSkill;
	string	strSkillRule;
	SetTabFile(TabFile, "NpcFightBaseData_Server");
	uint64	uBeginTime = GetProcessTime();
	if (!TabFile.Load(PATH_ALIAS_CFG.c_str(), sFileName.c_str())) 
		return false;

	DeleteAll();

	for(int32 i=1; i<TabFile.GetHeight(); ++i)
	{
		SetLineNo(i);
		string sName = TabFile.GetString(i, "名字");
		if (sName.empty())
			continue;
		pValue = new CNpcFightBaseData(sName);
		ReadItem(pValue->m_sNormalAttack, "普通攻击",MUSTFILL);
		ReadItem(pValue->m_fAttackSpeed,"攻击速度",GE,0.0f);
		ReadItem(pValue->m_fAttackScope,"普攻距离", 1.f);
		ReadItem(strSkillRule,"技能规则",CANEMPTY);
		ReadItem(pValue->m_uBornTriggerType,"死亡爆魂触发器类型",CANEMPTY,0);
		ReadItem(pValue->m_bOpenTargetPKState, "是否打开目标PK开关",CANEMPTY,NO);
		ReadItem(pValue->m_bLevelPress, "等级压制", CANEMPTY, NO);
		ReadItem(pValue->m_bChaosEnmity, "乱仇恨顺序",CANEMPTY, NO);
		ReadItem(pValue->m_bRegulate, "是否排队",CANEMPTY, NO);
		ReadItem(pValue->m_bExclaimAlert, "是否能呼叫警戒", CANEMPTY, YES);
		ReadItem(sBornSkill,"出生释放技能",CANEMPTY);
		ReadItem(pValue->m_uRandMaxAttackSpeed, "最大随机攻击速度", 0);
		pValue->CreateNpcBornSkill(sBornSkill);
		pValue->CreateNpcSkillRule(strSkillRule);
		AddEntity(pValue);
	}
	SetTabNull();
	uint64 uEndTime = GetProcessTime();
	cout << "读取配置表：【NpcFightBaseData_Server】完毕！    耗时：" << (uEndTime - uBeginTime) << " 毫秒！\n";
	return true;
}


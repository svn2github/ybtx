#include "stdafx.h"
#include "CCfgNpcServerBaseData.h"
#include "NpcInfoDefs.h"
#include "CCfgNPCFighterBaseDataCheck.h"
#include "CCfgNPCResCommonCheck.h"
#include "CCfgNPCAIBaseDataCheck.h"
#include "ExpHelper.h"

set<string> CCfgNpcServerBaseData::m_setCfgNpcBaseData;

void CCfgNpcServerBaseData::Create(const TCHAR* szName, const TCHAR* szNpcType, const TCHAR* szAIType, 
							  const TCHAR* szAIData, uint32 uRace, uint32 uCamp, uint32 uClass, uint32 uSex, uint32 uLevel)
{
	bool bSuccess = true;
	if (szNpcType == NULL)
	{
		bSuccess = false;
		cout << "Npc类型为空\n";
	}

	if (szName == NULL)
	{
		bSuccess = false;
		cout << "Npc名为空\n";
	}

	if (szAIType == NULL)
	{
		bSuccess = false;
		cout << "Npc " << szName << " AI类型为空\n";
	}

	if (szAIData == NULL)
	{
		bSuccess = false;
		cout << "Npc " << szName << " AI数据为空\n";
	}

	string sNpcName(szName);
	set<string>::iterator iter = m_setCfgNpcBaseData.find(sNpcName);
	if (iter != m_setCfgNpcBaseData.end())
	{
		bSuccess = false;
		cout << "Npc " << szName <<" 在Npc_Common表中重复！\n";
	}
	else
	{
		m_setCfgNpcBaseData.insert(sNpcName);
	}

	if (!CCfgNPCFighterBaseDataCheck::BeExist(sNpcName))
	{
		bSuccess = false;
		cout << "Npc " << szName <<" 战斗信息不存在！\n";
	}

	string sAIDataName(szAIData);
	if (!CCfgNPCAIBaseDataCheck::BeExist(sAIDataName))
	{
		bSuccess = false;
		cout << "Npc " << szName << " AI数据 " << szAIData << " 不存在！\n";
	}

	if (!CCfgNPCResCommonCheck::BeExist(sNpcName))
	{
		bSuccess = false;
		cout << "Npc  " << szName << "在【NpcRes_Common】表中不存在！请查实！\n";
	}

	//索引Npc类型
	TypeMapRgst::mapNpcType_itr iter1 = NpcType.mapNpcTypeMap.find(szNpcType);
	if (iter1 == NpcType.mapNpcTypeMap.end())
	{
		bSuccess = false;
		cout << "Npc " << szName << "  Type： " << szNpcType << " 尚未注册\n";
	}

	//索引Aoi类型
	AITypeAoiRgst::mapObjectAoi_itr iter2 = sObjectAoiType.mapObjectAoiType.find(szAIType);
	if (iter2 == sObjectAoiType.mapObjectAoiType.end())
	{
		bSuccess = false;
		cout << "Npc " << szName << "  AIType： " << szAIType << " 尚未注册AOI类型！\n";
	}

	//索引AIType类型
	AITypeMap::mapNpcAIType_itr iter3 = sNpcAIType.mapNpcAIType.find( szAIType);
	if (iter3 == sNpcAIType.mapNpcAIType.end())
	{
		bSuccess = false;
		cout << "Npc " << szName << "  AIType：" << szAIType << " 尚未注册AI类型！\n";
	}

	if (!bSuccess)
	{
		string str1;
		str1 = "关联检测Npc基础信息表失败，请查实！";
		GenErr(str1, szName);
	}

	if (uClass == 29)
	{
		string strSkillRuleName("通用呼救红色警戒狂乱");
		string strNpcName(szName);
		bool bSuccess = CCfgNPCFighterBaseDataCheck::CheckSkillRule(strNpcName, strSkillRuleName);
		if (!bSuccess)
		{
			ostringstream strm;
			cout<<"【警告：】 Npc："<< szName<<" 的职业是29,但是在NpcFighterBaseData表中对应的技能规则为: [通用呼救红色警戒狂乱], 请检查是否正确！"<< endl;
		}
	}
}

bool CCfgNpcServerBaseData::FindNpc(string sNpcName)
{
	return m_setCfgNpcBaseData.find(sNpcName) != m_setCfgNpcBaseData.end();
}

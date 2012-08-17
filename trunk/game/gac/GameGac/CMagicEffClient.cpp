#include "stdafx.h"
#include "CMagicEffClient.h"
#include "LoadSkillCommon.h"
#include "CCfgCalc.h"
#include "CCfgCalc.inl"
#include "CTxtTableFile.h"
#include "MagicConds_TestValue.inl"
#include "MagicConds_Function.h"
#include "CFighterDirector.h"
#include "IFighterDirectorHandler.h"
#include "BaseHelper.h"
#include "DebugHelper.h"
#include "StringHelper.h"

namespace sqr
{
	extern const wstring PATH_ALIAS_CFG;
}

CMagicEffClient::MapMagicEffClient	CMagicEffClient::ms_mapMagicEffClient;
CMagicEffClient::MapMagicEffClientById	CMagicEffClient::ms_mapMagicEffClientById;
CMagicEffClient::MapMagicCond		CMagicEffClient::ms_mapMagicCond;
uint32 CMagicEffClient::ms_uMaxLineNum = 0;
CMagicEffClientSharedPtr CMagicEffClient::ms_NULL;

CMagicEffClient::CMagicEffClient(void)
{
}

CMagicEffClient::~CMagicEffClient(void)
{
	ClearVector(m_vecMagicCondData);
}

void CMagicEffClient::BuildMCondMap()
{
	ClearMap(ms_mapMagicCond);

	ms_mapMagicCond["生命值小于"]		= new CHealthPointLesserMCOND;
	ms_mapMagicCond["生命值大于等于"]	= new CHealthPointGreaterOrEqualMCOND;
	ms_mapMagicCond["魔法值小于"]		= new CManaPointLesserMCOND;
	ms_mapMagicCond["魔法值大于等于"]	= new CManaPointGreaterOrEqualMCOND;
	ms_mapMagicCond["能量值大于等于"]	= new CEnergyPointGreaterOrEqualMCOND;
	ms_mapMagicCond["怒气值大于等于"]	= new CRagePointGreaterOrEqualMCOND;
	ms_mapMagicCond["连击点大于等于"]	= new CComboPointGreaterOrEqualMCOND;
	ms_mapMagicCond["概率小于"]			= new CProbabilityLesserCond;

	ms_mapMagicCond["处于魔法状态"]		= new CTestMagicStateMCOND;
	ms_mapMagicCond["不处于魔法状态"]	= new CTestNotInMagicStateMCOND;
	ms_mapMagicCond["不处于不可重复魔法状态"]	= new CTestNotInRepeatedMagicStateMCOND;
	ms_mapMagicCond["处于触发器状态"]	= new CTestTriggerStateMCOND;
	ms_mapMagicCond["处于伤害变更状态"] = new CTestDamageChangeStateMCOND;
	ms_mapMagicCond["不处于伤害变更状态"]	= new CTestNotInDamageChangeStateMCOND;
	ms_mapMagicCond["处于特殊状态"]		= new CTestSpecialStateMCOND;
	ms_mapMagicCond["魔法状态叠加层数大于等于"]	= new CTestMagicStateCascadeGreaterOrEqualMCOND;
	ms_mapMagicCond["处于姿态技能"]		= new CTestAureMagicMCOND;
	ms_mapMagicCond["装备了盾牌"]		= new CTestShieldEquipMCOND;
	ms_mapMagicCond["主手装备了"]		= new CTestMainHandEquipMCOND;
	ms_mapMagicCond["副手装备了武器"]	= new CTestAssistHandEquipMCOND;
	ms_mapMagicCond["坐骑存在"]			= new CTestIsExistHorseMCOND;
	ms_mapMagicCond["坐骑不存在"]		= new CTestIsNotExistHorseMCOND;
	ms_mapMagicCond["目标是否为玩家"]			= new CTestTargetIsPlayerMCOND;
	ms_mapMagicCond["目标是否为NPC"]			= new CTestTargetIsNPCMCOND;
	ms_mapMagicCond["目标是否为召唤兽"]			= new CTestTargetIsSummonMCOND;
	ms_mapMagicCond["目标等级小于等于自身等级"] = new CTestTargetLevelLesserOrEqualMCOND;
	ms_mapMagicCond["不处于战斗状态"]			= new CTestNotInBattleMCOND;
	ms_mapMagicCond["不处于玩法状态"]			= new CTestNotOnMissionMCOND;
	ms_mapMagicCond["目标是否可被控制"]			= new CTestTargetCanBeControlledMCOND;
	ms_mapMagicCond["爆魂蓄力等级大于等于"]		= new CTestBurstSoulTimesGreaterOrEqualMCOND;
	ms_mapMagicCond["宠物存在"]					= new CTestIsExistPetMCOND;
}

CMagicCondClient* CMagicEffClient::GetMagicCond(const string& name)
{
	MapMagicCond::iterator it = ms_mapMagicCond.find(name);
	if(it == ms_mapMagicCond.end())
	{
		return NULL;
	}

	return it->second;
}

CMagicEffClientSharedPtr& CMagicEffClient::GetMagicEff(const string& name)
{
	MapMagicEffClient::iterator it = ms_mapMagicEffClient.find(name);

	if(it == ms_mapMagicEffClient.end())
	{
		stringstream str;
		str << "没有魔法效果: " << name;
		GenErr(str.str().c_str());
	}

	return *(it->second);
}

CMagicEffClientSharedPtr& CMagicEffClient::GetMagicEffById(uint32 uId)
{
	stringstream ExpStr;
	CMagicEffClient::MapMagicEffClientById::iterator mapCfgItr;
	mapCfgItr = ms_mapMagicEffClientById.find(uId);
	if (mapCfgItr == ms_mapMagicEffClientById.end())
	{
		ExpStr << "没有魔法效果: " << uId;
		GenErr(ExpStr.str());
		//return NULL;
	}

	return *(mapCfgItr->second);
}

bool CMagicEffClient::LoadMagicEff(const string& fileName, bool bFirstFile)
{
	if (bFirstFile)
	{
		UnloadMagicEff();
		BuildMCondMap();
	}

	CTxtTableFile MagicEffFile;
	if(!MagicEffFile.Load(PATH_ALIAS_CFG.c_str(), fileName.c_str()))
		return false;

	string curMagicEffName = "", lastMagicEffName;
	CMagicEffClient* pCurMagicEffCfg = NULL;
	MagicOpData* pCurMOPData = NULL;
	bool bNewSec = true;

	for( uint32 i = ms_uMaxLineNum+1; i < MagicEffFile.GetHeight()+ms_uMaxLineNum; ++i )
	{
		uint32 uLineNum = i-ms_uMaxLineNum;
		string name	= MagicEffFile.GetString( uLineNum, szMagicEff_Name);
		trimend(name);

		if( name.empty() )
		{
			bNewSec = true;
			continue;
		}

		if( bNewSec )
		{
			bNewSec = false;
			lastMagicEffName = curMagicEffName;
			curMagicEffName = name;
			pCurMagicEffCfg = new CMagicEffClient;
			pCurMagicEffCfg->m_uId = i;
			pCurMagicEffCfg->m_strName = name;
			if(curMagicEffName.compare(lastMagicEffName) != 0)	// 初始化下一个魔法效果
			{
				CMagicEffClientSharedPtr* pCurMagicEffCfgSharedPtr = new CMagicEffClientSharedPtr(pCurMagicEffCfg);
				ms_mapMagicEffClient.insert(pair<string, CMagicEffClientSharedPtr*>(name, pCurMagicEffCfgSharedPtr));	
				ms_mapMagicEffClientById.insert(make_pair(pCurMagicEffCfg->m_uId, pCurMagicEffCfgSharedPtr));
			}
			else
			{
				continue;
			}
		}

		//初始化每个魔法操作的数据
		pCurMOPData = new MagicOpData;	
		pCurMOPData->m_uId				= i;
		pCurMOPData->m_strMagicOpName	= MagicEffFile.GetString(uLineNum, szMagicEff_MagicOp);
		trimend(pCurMOPData->m_strMagicOpName);
		string strObjFilter				= MagicEffFile.GetString(uLineNum, szMagicEff_FilterPipe);
		trimend(strObjFilter);
		pCurMOPData->m_pMagicOpArg		= new CCfgCalc();
		string sArg = MagicEffFile.GetString(uLineNum, szMagicEff_MOPParam);
		trimend(sArg);
		pCurMOPData->m_pMagicOpArg->InputString(sArg);	
		string strMOPType				= MagicEffFile.GetString(uLineNum, szMagicEff_MOPType);
		trimend(strMOPType);
		string strExecCond				= MagicEffFile.GetString(uLineNum, szMagicEff_MOPExecCond);
		trimend(strExecCond);
		if ( strMOPType.find("条件") != -1 && strExecCond.compare("")==0 && strObjFilter=="自身" )
		{
			pCurMagicEffCfg->m_vecMagicCondData.push_back(pCurMOPData);
		}
		else
		{
			delete pCurMOPData;
		}
	}
	ms_uMaxLineNum = ms_uMaxLineNum + MagicEffFile.GetHeight();
	return true;
}

uint32 CMagicEffClient::DoMagicCond(uint32 SkillLevel, const CFighterDirector* pFighter)
{
	uint32 uResult = eDSR_Success;

	if(m_vecMagicCondData.empty())
		return uResult;

	VecMagicCondData::iterator it = m_vecMagicCondData.begin();
	for(; it != m_vecMagicCondData.end(); ++it)
	{
		if (CMagicEffClient::GetMagicCond((*it)->m_strMagicOpName) == NULL)
			continue;
		uResult = CMagicEffClient::GetMagicCond((*it)->m_strMagicOpName)->Test(SkillLevel, *((*it)->m_pMagicOpArg), pFighter);
		if (uResult != eDSR_Success)
			return uResult;
	}

	return uResult;
}

EConsumeType CMagicEffClient::GetEConsumeType()
{
	EConsumeType eConsumeType = eCT_None;

	if (m_vecMagicCondData.empty())
		return eConsumeType;

	VecMagicCondData::iterator it = m_vecMagicCondData.begin();
	for(; it != m_vecMagicCondData.end(); ++it)
	{
		if ((*it)->m_strMagicOpName == "魔法值大于等于")
		{
			eConsumeType = eCT_MP;
		}
		else if ((*it)->m_strMagicOpName == "怒气值大于等于")
		{
			eConsumeType = eCT_RG;
		}
		else if ((*it)->m_strMagicOpName == "能量值大于等于")
		{
			eConsumeType = eCT_EG;
		}
		else if ((*it)->m_strMagicOpName == "连击点大于等于")
		{
			eConsumeType = eCT_CP;
		}
		else if ((*it)->m_strMagicOpName == "生命值大于等于")
		{
			eConsumeType = eCT_HP;
		}
	}

	return eConsumeType;
}

CCfgCalc* CMagicEffClient::GetMagicOpArg(const string& name)
{
	if (m_vecMagicCondData.empty())
		return NULL;

	VecMagicCondData::iterator it = m_vecMagicCondData.begin();
	for(; it != m_vecMagicCondData.end(); ++it)
	{
		if ((*it)->m_strMagicOpName == name)
			return (*it)->m_pMagicOpArg;	
	}

	return NULL;
}

void CMagicEffClient::UnloadMagicEff()
{
	ClearMap(ms_mapMagicCond);
	ClearMap(ms_mapMagicEffClient);
	ms_mapMagicEffClientById.clear();
}

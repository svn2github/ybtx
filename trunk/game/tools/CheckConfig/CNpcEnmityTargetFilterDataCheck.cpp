#include "stdafx.h"
#include "CNpcEnmityTargetFilterDataCheck.h"
#include "CTxtTableFile.h"
#include "CCfgColChecker.inl"
//#include "CCfgSkillCheck.h"
#include "CCfgNpcServerBaseData.h"
#include "CScriptAppServer.h"

CNpcEnmityTargetFilterDataCheck::CNpcEnmityTargetFilterDataCheck()
{

}

CNpcEnmityTargetFilterDataCheck::~CNpcEnmityTargetFilterDataCheck()
{

}

void CNpcEnmityTargetFilterDataCheck::GetNpcArgs(SetAllNpc& lstNpcArgs, string& strNpcArgs)
{
	if (strNpcArgs.empty())
		return;
	erase(strNpcArgs, " ");
	erase(strNpcArgs,"\"");
	replace(strNpcArgs, "，", ",");
	while(true)
	{
		uint32 upoint = strNpcArgs.find(",");
		if (upoint != -1)
		{
			string sNpcName = strNpcArgs.substr(0, upoint);
			lstNpcArgs.insert(sNpcName);
			strNpcArgs = strNpcArgs.substr(upoint+1, strNpcArgs.length());
		}
		else
		{
			lstNpcArgs.insert(strNpcArgs);
			break;
		}
	}
}

bool CNpcEnmityTargetFilterDataCheck::Check()
{
	string strFiler = CScriptAppServer::Inst()->GetCfgFilePath("");
	strFiler = strFiler + "/npc/NpcEnmiyTargetFilter.txt";
	CTxtTableFile TableFile;
	Load(TableFile, strFiler.c_str(), "NpcEnmiyTargetFilter");

	SetAllNpc setAllNpc;

	for(int32 i = 1; i < TableFile.GetHeight(); ++i)
	{
		string sNpcName				= TableFile.GetString(i,"Name");
		string sTargetType			= TableFile.GetString(i, "TargetType");
		string sNpcArgs				= TableFile.GetString(i, "TargetArg");
		
		if (setAllNpc.find(sNpcName) != setAllNpc.end())
		{
			stringstream ExpStr;
			ExpStr << "在【NpcEnmiyTargetFilter】表中Npc:【"<<sNpcName<<"】填写重复！请查实！";
			CfgChk::GenExpInfo(ExpStr.str());
		}
		else
			setAllNpc.insert(sNpcName);

		if (!CCfgNpcServerBaseData::FindNpc(sNpcName))
		{
			stringstream ExpStr;
			ExpStr << "在【NpcEnmiyTargetFilter】表中Npc:【"<<sNpcName<<"】在【Npc_Common】表中不存在！请查实！";
			CfgChk::GenExpInfo(ExpStr.str());
			continue;
		}
		if (sTargetType == "所有目标" || sTargetType == "所有玩家" || sTargetType == "所有Npc")
		{
			continue;
		}
		else if (sTargetType == "指定Npc")
		{
			SetAllNpc lstNpcArgs;
			GetNpcArgs(lstNpcArgs, sNpcArgs);
			for (SetAllNpc::iterator iter = lstNpcArgs.begin(); iter != lstNpcArgs.end(); iter ++)
			{
				bool bFind = CCfgNpcServerBaseData::FindNpc(*iter);
				if (!bFind)
				{
					stringstream ExpStr;
					ExpStr << "在【NpcEnmiyTargetFilter】表中Npc:【"<<sNpcName<<"】的参数Npc【"<<(*iter)<<"】在【Npc_Common】表中不存在！请查实！";
					CfgChk::GenExpInfo(ExpStr.str());
				}
			}
		}
		else
		{
			stringstream ExpStr;
			ExpStr << "在【NpcEnmiyTargetFilter】表中Npc:【"<<sNpcName<<"】的目标类型【"<<sTargetType<<"】不存在！请查实！";
			CfgChk::GenExpInfo(ExpStr.str());
		}
	}
	return true;
}

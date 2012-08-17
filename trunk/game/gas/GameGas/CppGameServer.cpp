#include "stdafx.h"
#include "CppGameServer.h"
#include "LoadVariantCfgCommon.h"
#include "CScriptAppServer.h"
#include "CCoreObjectDictator.h"
#include "CNpcAI.h"
#include "CNpcAIBaseData.h"
#include "CNpcStateTransit.h"
#include "CNpcFightBaseData.h"
#include "CNpcServerBaseData.h"
#include "CCharacterDictator.h"
#include "CNpcResServerData.h"
#include "CppGameServer.h"
#include "CServantData.h"
#include <iostream>
#include "CTeamServerManager.h"
#include "CSkillRuleServer.h"
#include "CMagicEffServer.h"
#include "TSqrAllocator.inl"
#include "CGameConfigServer.h"
#include "CNpcSpecialSkillCfg.h"
#include "CodeCvs.h"
#include "CNpcEnmityTargetFilterData.h"


bool LoadNPCConfig()
{
	CfgChk::g_bExistError = false;

	string path;
	if ( CBaseScriptApp::Inst()->GetIsCfgPkgUsed()  )
	{
		path = "cfg";
	}
	else
	{
		path  =  utf8_to_gbk(CBaseScriptApp::Inst()->GetCfgFilePath(""));
	}

	string sNpcPath, sServantPath;
	sNpcPath = path + "/npc/";
	sServantPath = path + "/servant/";

	string sFileName;

	sFileName = sNpcPath + "NpcAIBaseData_Server.txt";
	if (!CNpcAIBaseDataMgr::GetInst()->LoadConfig( sFileName ))
	{
		ostringstream strm;
		strm<<"¼ÓÔØ£º"<< sFileName <<" Ê§°Ü";
		GenErr(strm.str());
	}

	sFileName = sServantPath + "ServantProperty_Server.txt";
	if (!CServantDataMgr::GetInst()->LoadConfig( sFileName ))
	{
		ostringstream strm;
		strm<<"¼ÓÔØ£º"<< sFileName <<" Ê§°Ü";
		GenErr(strm.str());
	}

	sFileName = sNpcPath + "NpcFightBaseData_Server.txt";
	if (!CNpcFightBaseDataMgr::GetInst()->LoadConfig( sFileName ))
	{
		ostringstream strm;
		strm<<"¼ÓÔØ£º"<< sFileName <<" Ê§°Ü";
		GenErr(strm.str());
	}

	sFileName = sNpcPath + "SkillRuleBaseData_Server.txt";
	if (!CSkillRuleServer::LoadSkillRuleCfg(sFileName))
	{
		ostringstream strm;
		strm<<"¼ÓÔØ£º"<< sFileName <<" Ê§°Ü";
		GenErr(strm.str());
	}

	sFileName = sNpcPath + "NpcEnmiyTargetFilter.txt";
	if (!CNpcEnmityTargetFilterDataMgr::GetInst()->LoadConfig(sFileName))
	{
		ostringstream strm;
		strm<<"¼ÓÔØ£º"<< sFileName <<" Ê§°Ü";
		GenErr(strm.str());
	}
	
	CNpcSpecialSkillCfg::LoadNpcSpecialSkill();
	CfgChk::SetTabNull();

	return !CfgChk::ExistError();
}

void CNpcAIServerInit()
{
	CNpcAI::RegistAllState();
	CNpcAI::RegistAllEvent();

	string sPath = "game/gas/xml/";
	string sFileName = sPath + "NpcAI_Server.xml";
	if (!CNpcStateTransitMgr::GetInst()->LoadConfig( sFileName ))
	{
		ostringstream strm;
		strm<<"¼ÓÔØ£º"<< sFileName <<" Ê§°Ü";
		GenErr(strm.str());
	}
}

bool CppGameServerInit()
{
	CTeamServerManager::Init();

	LoadVariantCfgCommon(CCoreObjectDictator::GetVarDefCfg(), "game/gac_gas/xml/");
	CNpcAIServerInit();
	CGameConfigServer::IntInst();

	return LoadNPCConfig();
}

void CppGameServerClear()
{
	CTeamServerManager::Unit();
	CNpcResServerDataMgr::GetInst()->DeleteAll();
	CNpcServerBaseDataMgr::GetInst()->DeleteAll();
	CNpcAIBaseDataMgr::GetInst()->DeleteAll();
	CNpcFightBaseDataMgr::GetInst()->DeleteAll();
	CServantDataMgr::GetInst()->DeleteAll();
	CNpcStateTransitMgr::GetInst()->DeleteAll();
	CSkillRuleServer::UnloadSkillRuleCfg();
	CNpcSpecialSkillCfg::UnLoadNpcSpecialSkill();
	CMagicOpTreeServer::DeleteAll();
	CNpcEnmityTargetFilterDataMgr::GetInst()->DeleteAll();
}

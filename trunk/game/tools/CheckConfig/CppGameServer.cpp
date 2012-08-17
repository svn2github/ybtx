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
#include "CTick.h"
#include "CAppServer.h"
#include <iostream>
//#include "CTeamServerManager.h"
#include "CSkillRuleServer.h"
#include "CMagicEffServer.h"
#include "TNedAllocator.inl"

bool CppGameServerInit()
{
	string sPath;
	sPath = CScriptAppServer::Inst()->GetXMLFilePath("game/gac_gas/xml/");
	sPath = CScriptAppServer::Inst()->GetCfgFilePath("");

	string sNpcPath, sServantPath;
	sNpcPath = sPath + "/npc/";
	sServantPath = sPath + "/servant/";

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
	return true;
}

void CppGameServerClear()
{
	//CTeamServerManager::Unit();
	CNpcResServerDataMgr::GetInst()->DeleteAll();
	//CNpcServerBaseDataMgr::GetInst()->DeleteAll();
	CNpcAIBaseDataMgr::GetInst()->DeleteAll();
	CNpcFightBaseDataMgr::GetInst()->DeleteAll();
	CServantDataMgr::GetInst()->DeleteAll();
	//CNpcStateTransitMgr::GetInst()->DeleteAll();
	CSkillRuleServer::UnloadSkillRuleCfg();
	//CMagicOpTreeServer::DeleteAll();
}

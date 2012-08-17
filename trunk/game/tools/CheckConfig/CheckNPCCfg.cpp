#include "stdafx.h"
#include "CheckNpcCfg.h"
#include "CCfgColChecker.inl"
#include "CCfgNPCAIBaseDataCheck.h"
#include "CCfgServantDataCheck.h"
#include "CCfgNPCFighterBaseDataCheck.h"
#include "CCfgSkillRuleCheck.h"
#include "CCfgNPCResCommonCheck.h"
#include "CScriptAppServer.h"

bool CheckNpcCfg()
{
	string strCfgFilePath = CScriptAppServer::Inst()->GetCfgFilePath("");

	string strNpcPath, strServantPath;
	strNpcPath = strCfgFilePath + "/npc/";
	strServantPath = strCfgFilePath + "/servant/";

	string strFileName;

	strFileName = strNpcPath + "NpcAIBaseData_Server.txt";
	if (!CCfgNPCAIBaseDataCheck::Check(strFileName.c_str()))
	{
		ostringstream strm;
		strm<<"¼ÓÔØ£º"<< strFileName <<" Ê§°Ü";
		CfgChk::GenExpInfo(strm.str());
	}

	strFileName = strServantPath + "ServantProperty_Server.txt";
	if (!CCfgServantDataCheck::Check(strFileName.c_str()))
	{
		ostringstream strm;
		strm<<"¼ÓÔØ£º"<< strFileName <<" Ê§°Ü";
		CfgChk::GenExpInfo(strm.str());
	}

	strFileName = strNpcPath + "SkillRuleBaseData_Server.txt";
	if (!CCfgSkillRuleCheck::Check(strFileName.c_str()))
	{
		ostringstream strm;
		strm<<"¼ÓÔØ£º"<< strFileName <<" Ê§°Ü";
		CfgChk::GenExpInfo(strm.str());
	}

	strFileName = strNpcPath + "NpcFightBaseData_Server.txt";
	if (!CCfgNPCFighterBaseDataCheck::Check(strFileName.c_str()))
	{
		ostringstream strm;
		strm<<"¼ÓÔØ£º"<< strFileName <<" Ê§°Ü";
		CfgChk::GenExpInfo(strm.str());
	}

	return !CfgChk::ExistError();
}

void EndCheckNpcCfg()
{
	CCfgNPCAIBaseDataCheck::EndCheck();
	CCfgServantDataCheck::EndCheck();
	CCfgNPCFighterBaseDataCheck::EndCheck();
	CCfgSkillRuleCheck::EndCheck();
	CCfgNPCResCommonCheck::EndCheck();
}

void CppCheckNpcResCommon(const TCHAR* sNpcName, float fBaseSize, bool bAdhereTerrain, const TCHAR* sAniFileName)
{
	CCfgNPCResCommonCheck::Check(sNpcName, fBaseSize, bAdhereTerrain, sAniFileName);
}

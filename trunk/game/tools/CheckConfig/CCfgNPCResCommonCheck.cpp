#include "stdafx.h"
#include "CCfgNPCResCommonCheck.h"
#include "CCfgColChecker.inl"

CCfgNPCResCommonCheck::SetNpcName CCfgNPCResCommonCheck::ms_setNpcName;

void CCfgNPCResCommonCheck::Check(const TCHAR* sNpcName, float fBaseSize, bool bAdhereTerrain, const TCHAR* sAniFileName)
{
	if (sNpcName == NULL)
	{
		stringstream ExpStr;
		ExpStr << "在【NpcRes_Common】表中【NpcName】项不能为空！请查实！";
		CfgChk::GenExpInfo(ExpStr.str());
	}

	if (fBaseSize <= 0)
	{
		stringstream ExpStr;
		ExpStr << "在【NpcRes_Common】表中叫【" << sNpcName<< "】的Npc【Scaling】【BaseSize】两项必须都大于0！请查实！";
		CfgChk::GenExpInfo(ExpStr.str());
	}

	SetNpcName::iterator iter = ms_setNpcName.find(sNpcName);
	if (iter != ms_setNpcName.end())
	{
		stringstream ExpStr;
		ExpStr << "在【NpcRes_Common】表中叫【" << sNpcName << "】的Npc重复！请查实！" ;
		CfgChk::GenExpInfo(ExpStr.str());
	}
	else
	{
		ms_setNpcName.insert(sNpcName);
	}
}

void CCfgNPCResCommonCheck::EndCheck()
{
	ms_setNpcName.clear();
}

bool CCfgNPCResCommonCheck::BeExist(string sNpcName)
{
	if (ms_setNpcName.find(sNpcName) != ms_setNpcName.end())
		return true;
	return false;
}



#include "stdafx.h"
#include "CTxtTableFile.h"
#include "CSkillReplaceServer.h"
#include "LoadSkillCommon.h"
#include "BaseHelper.h"
#include "CCfgColChecker.inl"
#include "CMagicStateCfg.h"
#include "CSkillServer.h"

namespace sqr
{
	extern const wstring PATH_ALIAS_CFG;
}

CSkillReplace::MultiMapCSkillReplace CSkillReplace::ms_multimapSkillReplace;

bool CSkillReplace::LoadConfig(const string& szFileName)
{
	using namespace CfgChk;
	CTxtTableFile TabFile;
	SetTabFile(TabFile, "¼¼ÄÜÌæ»»±í");
	if (!TabFile.Load(PATH_ALIAS_CFG.c_str(), szFileName.c_str())) return false;
	
	bool ret = true;
	for (int32 i = 1; i < TabFile.GetHeight(); i++)
	{
		SetLineNo(i);
		CSkillReplace* pSkillReplace = new CSkillReplace;
		ReadItem(pSkillReplace->m_sSkillName,			szSkillReplace_SkillName);
		ReadItem(pSkillReplace->m_sStateName,			szSkillReplace_StateName);
		ReadItem(pSkillReplace->m_sReplaceSkillName,			szSkillReplace_ReplaceName);
		pSkillReplace->m_uPriority = i;
		ms_multimapSkillReplace.insert(pair<string, CSkillReplace*>(pSkillReplace->m_sSkillName,pSkillReplace));
	}

	return ret;
}

void CSkillReplace::UnloadConfig()
{
	MultiMapCSkillReplace::iterator it = ms_multimapSkillReplace.begin();
	MultiMapCSkillReplace::iterator itEnd = ms_multimapSkillReplace.end();
	for (;it!=itEnd;it++)
	{
		delete (*it).second;
	}
	ms_multimapSkillReplace.clear();
}

CSkillReplace::CSkillReplace()
{
}

CSkillReplace::~CSkillReplace()
{
	
}


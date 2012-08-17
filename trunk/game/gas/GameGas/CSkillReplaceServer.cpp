#include "stdafx.h"
#include "CSkillReplaceServer.h"
#include "CFighterDictator.h"
#include "PtrlGas2GacCPPDef.h"

CSkillReplaceServer::CSkillReplaceServer(CFighterDictator* pFighter)
:m_pFighter(pFighter)
{
}

CSkillReplaceServer::~CSkillReplaceServer()
{
}

uint32 CSkillReplaceServer::ReplaceSkill(const TCHAR* szStateName)
{
	//规则:同一个技能的替换只能存在一个,优先级高的覆盖低的
	CSkillReplace::MultiMapCSkillReplace::iterator it = CSkillReplace::ms_multimapSkillReplace.begin();
	for (; it!=CSkillReplace::ms_multimapSkillReplace.end(); it++)
	{
		if (strcmp((*it).second->GetStateName().c_str(), szStateName) == 0)
		{
			const string& szSkillName = (*it).second->GetName();
			const string& szReplaceSkillName = (*it).second->GetReplaceSkillName();
			if (!GetFighter()->IsSkillActive(szSkillName, 1))
			{
				return eHR_Fail;
			}
			typedef CSkillReplace::MultiMapCSkillReplace::iterator it_SkillReplace;
			pair<it_SkillReplace,it_SkillReplace> pos = CSkillReplace::ms_multimapSkillReplace.equal_range(szSkillName);
			for (; pos.first != pos.second; ++pos.first)
			{
				if (strcmp((*pos.first).second->GetStateName().c_str(), szStateName) != 0 && GetFighter()->ExistState((*pos.first).second->GetStateName()))
				{
					if ((*pos.first).second->GetPriority() >= (*it).second->GetPriority())
					{
						//替换的优先级低，不替换
						GetFighter()->ClearState(szStateName);
						return eHR_Fail;
					}
					else
					{
						//有优先级低的替换，撤销上次优先级低的，使用这次的替换
						GetFighter()->ClearState((*pos.first).second->GetStateName().c_str());
						break;
					}
				}
			}
			CGas2GacCPP_OnReplaceSkill Cmd;
			Cmd.uObjID				 = GetFighter()->GetEntityID();
			Cmd.bReplace			 = true;
			Cmd.uSkillNameLen		 = szSkillName.size();
			Cmd.uReplaceSkillNameLen = szReplaceSkillName.size();
			GetFighter()->SendCmdToGac(&Cmd, eOnlyToDirector);
			GetFighter()->SendToGac(szSkillName.c_str(), szSkillName.size(), eOnlyToDirector);	
			GetFighter()->SendToGac(szReplaceSkillName.c_str(), szReplaceSkillName.size(), eOnlyToDirector);	
		
			GetFighter()->AddSkill(szReplaceSkillName,GetFighter()->GetActiveSkillLevel(szSkillName));
			GetFighter()->RemoveSkill(szSkillName);
			return eHR_Success;
		}
	}
	return eHR_Fail;
}
void CSkillReplaceServer::CancelReplaceSkill(const TCHAR* szStateName)
{
	CSkillReplace::MultiMapCSkillReplace::iterator it = CSkillReplace::ms_multimapSkillReplace.begin();
	for (; it!=CSkillReplace::ms_multimapSkillReplace.end(); it++)
	{
		if (strcmp((*it).second->GetStateName().c_str(), szStateName) == 0 && GetFighter()->IsSkillActive((*it).second->GetReplaceSkillName(), 1))
		{
			const string& szSkillName			= (*it).second->GetName();
			const string& szReplaceSkillName	= (*it).second->GetReplaceSkillName();
			//撤销
			CGas2GacCPP_OnReplaceSkill Cmd;
			Cmd.uObjID				 = GetFighter()->GetEntityID();
			Cmd.bReplace			 = false;
			Cmd.uSkillNameLen		 = szSkillName.size();
			Cmd.uReplaceSkillNameLen = szReplaceSkillName.size();
			GetFighter()->SendCmdToGac(&Cmd, eOnlyToDirector);
			GetFighter()->SendToGac(szSkillName.c_str(), szSkillName.size(), eOnlyToDirector);	
			GetFighter()->SendToGac(szReplaceSkillName.c_str(), szReplaceSkillName.size(), eOnlyToDirector);	
			
			GetFighter()->AddSkill(szSkillName.c_str(),GetFighter()->GetActiveSkillLevel(szReplaceSkillName));
			GetFighter()->RemoveSkill(szReplaceSkillName);
		}
	}
}


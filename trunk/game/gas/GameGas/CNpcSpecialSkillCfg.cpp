#include "stdafx.h"
#include "CNpcSpecialSkillCfg.h"
#include "CSkillServer.h"
#include "BaseHelper.h"

CNpcSpecialSkillCfg::VecNpcSpecialSkill CNpcSpecialSkillCfg::ms_vecNpcSpecialSkill;

bool CNpcSpecialSkillCfg::LoadNpcSpecialSkill()
{
	ms_vecNpcSpecialSkill.push_back( new CNormalSkillServerSharedPtr(CNormalSkillServer::GetNPCSkill("NPCÇå¸ºÃæ×´Ì¬")) );
	ms_vecNpcSpecialSkill.push_back( new CNormalSkillServerSharedPtr(CNormalSkillServer::GetNPCSkill("NPC°²×°È«ÃâÒß")) );
	ms_vecNpcSpecialSkill.push_back( new CNormalSkillServerSharedPtr(CNormalSkillServer::GetNPCSkill("NPC³·ÏúÈ«ÃâÒß")) );
	ms_vecNpcSpecialSkill.push_back( new CNormalSkillServerSharedPtr(CNormalSkillServer::GetNPCSkill("ËÀÍö±¬»ê")) );
	ms_vecNpcSpecialSkill.push_back( new CNormalSkillServerSharedPtr(CNormalSkillServer::GetNPCSkill("ÆÕÍ¨ËÀÍö±¬»ê")) );
	ms_vecNpcSpecialSkill.push_back( new CNormalSkillServerSharedPtr(CNormalSkillServer::GetNPCSkill("NPCºô¾È×´Ì¬")) );
	ms_vecNpcSpecialSkill.push_back( new CNormalSkillServerSharedPtr(CNormalSkillServer::GetNPCSkill("NPC±»ºô¾È×´Ì¬")) );

	return true;
}

void CNpcSpecialSkillCfg::UnLoadNpcSpecialSkill()
{
	ClearVector(ms_vecNpcSpecialSkill);
}


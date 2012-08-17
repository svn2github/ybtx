#include "stdafx.h"
#include "CheckSkill.h"


//CheckSkill::CheckSkill()
//{
//
//
//}
//
//
//
//CheckSkill::~CheckSkill()
//{
//
//}


//void CheckSkill::LoadSkillToList()
//{
//	for(CNormalSkillServer::MapNormalSkillServerById::iterator itr = CNormalSkillServer::ms_mapSkillById.begin();
//		itr != CNormalSkillServer::ms_mapSkillById.end(); itr++)
//	{
//		
//		m_SkillList.push_back(itr->second->GetName());//–¥»Î
//		//list<string>::iterator sks = m_SkillList.begin();//∂¡»°
//		//*sks//ƒ⁄»›
//	}
//}


bool CheckSkill::TestInSkillList(const TCHAR *cSkillName)
{
	//string& temp_skname = cSkillName;
	//for( list<string>::iterator itr_sk = m_SkillList.begin();
	//	 itr_sk != m_SkillList.end(); itr_sk ++)
	//{
	//	
	//	if ( *itr_sk == temp_skname)
	//	{
	//		return true;
	//	}
	//}
	//return false;
	const CNormalSkillServerSharedPtr& pNpcSkill = CNormalSkillServer::GetNPCSkill( cSkillName);
	if (!pNpcSkill)
		return false;
	return true;
}


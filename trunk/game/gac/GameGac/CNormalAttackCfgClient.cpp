#include "stdafx.h"
#include "CNormalAttackCfgClient.h"
#include "CTxtTableFile.h"
#include "LoadSkillCommon.h"
#include "CCfgCalc.inl"
#include "CCfgColChecker.inl"
#include "BaseHelper.h"

namespace sqr
{
	extern const wstring PATH_ALIAS_CFG;
}

CNormalAttackCfgClient::MapNormalAttack CNormalAttackCfgClient::ms_mapNormalAttack;
CNormalAttackCfgClient::MapNormalAttackById CNormalAttackCfgClient::ms_mapNormalAttackById;
CNormalAttackCfgClient::MapWeaponType CNormalAttackCfgClient::ms_mapWeaponType;
CNormalAttackCfgClient::MapAttackType CNormalAttackCfgClient::ms_mapAttackType;

bool CNormalAttackCfgClient::LoadNormalAttackConfig(const string& szFileName)
{
	using namespace CfgChk;
	
	InitMapWeaponType();
	InitMapAttackType();
	CTxtTableFile TabFile; 
	if (!TabFile.Load(PATH_ALIAS_CFG.c_str(), szFileName.c_str())) return false;
	SetTabFile(TabFile, "普通攻击");
	for( int i = 1; i < TabFile.GetHeight(); i++ )
	{
		SetLineNo(i);
		CNormalAttackCfgClient* pNormalAttack = new CNormalAttackCfgClient;
		pNormalAttack->m_uId=i;
		ReadItem(pNormalAttack->m_strName,			szNT_Name);
		ReadItem(pNormalAttack->m_pMaxAttackDis,	szNT_MaxAttackDis, GE, 0.0f);

		CNormalAttackCfgClientSharedPtr* pNormalAttackSharedPtr = new CNormalAttackCfgClientSharedPtr(pNormalAttack);
		InsertStringSharedPtrMap(ms_mapNormalAttack, pNormalAttackSharedPtr);
		InsertUint32SharedPtrMap(ms_mapNormalAttackById, pNormalAttackSharedPtr);
	}
	return true;
}

bool CNormalAttackCfgClient::LoadNpcNormalAttackConfig(const string& szFileName)
{
	using namespace CfgChk;

	CTxtTableFile TabFile; 
	if (!TabFile.Load(PATH_ALIAS_CFG.c_str(), szFileName.c_str())) return false;
	SetTabFile(TabFile, "NPC普通攻击");
	uint32 uNormalAttackCfgSize = ms_mapNormalAttackById.size();
	for( int i = 1; i < TabFile.GetHeight(); i++ )
	{
		SetLineNo(i);
		CNormalAttackCfgClient* pNormalAttack = new CNormalAttackCfgClient;
		pNormalAttack->m_uId = uNormalAttackCfgSize + i;
		ReadItem(pNormalAttack->m_strName,			szNT_Name);
		pNormalAttack->m_pMaxAttackDis = NULL;

		CNormalAttackCfgClientSharedPtr* pNormalAttackSharedPtr = new CNormalAttackCfgClientSharedPtr(pNormalAttack);
		InsertStringSharedPtrMap(ms_mapNormalAttack, pNormalAttackSharedPtr);
		InsertUint32SharedPtrMap(ms_mapNormalAttackById, pNormalAttackSharedPtr);
	}
	return true;
}

void CNormalAttackCfgClient::UnloadNormalAttackConfig()
{
	ClearMap(ms_mapNormalAttack);
}

CNormalAttackCfgClient::~CNormalAttackCfgClient()
{
	SafeDelete(m_pMaxAttackDis);
}

CNormalAttackCfgClientSharedPtr& CNormalAttackCfgClient::GetNormalAttack(const TCHAR* szName)
{
	MapNormalAttack::iterator it_Skill = ms_mapNormalAttack.find(szName);
	if(it_Skill == ms_mapNormalAttack.end())
	{
		stringstream str; 
		str << "没有这个普通攻击: " << szName;
		GenErr(str.str().c_str());
	}
	return *(it_Skill->second);
}

CNormalAttackCfgClientSharedPtr& CNormalAttackCfgClient::GetNormalAttackById(uint32 uID)
{
	MapNormalAttackById::iterator it_Skill = ms_mapNormalAttackById.find(uID);
	if(it_Skill == ms_mapNormalAttackById.end())
	{
		stringstream str; 
		str << "没有这个普通攻击: " << uID;
		GenErr(str.str().c_str());
	}
	return *(it_Skill->second);
}

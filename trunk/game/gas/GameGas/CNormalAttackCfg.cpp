#include "stdafx.h"
#include "CNormalAttackCfg.h"
#include "CTxtTableFile.h"
#include "LoadSkillCommon.h"
#include "CCfgColChecker.inl"
#include "CSkillServer.h"
#include "BaseHelper.h"
#include "TStringRef.inl"
#include "TUpdateCfgFunctions.h"
#include "CSkillCfg.h"

namespace sqr
{
	extern const wstring PATH_ALIAS_CFG;
}

// 普攻配置表加载
CNormalAttackCfg::MapWeaponType CNormalAttackCfg::ms_mapWeaponType;

CNormalAttackCfg::MapNormalAttack& CNormalAttackCfg::GetNormalAttackMap()
{
	static MapNormalAttack ms_mapNormalAttack;
	return ms_mapNormalAttack;
}

CNormalAttackCfg::MapNormalAttack& CNormalAttackCfg::GetNPCNormalAttackMap()
{
	static MapNormalAttack ms_mapNPCNormalAttack;
	return ms_mapNPCNormalAttack;
}

bool CNormalAttackCfg::LoadNormalAttackConfig(const string& szFileName)
{
	using namespace CfgChk;

	InitMapWeaponType();
	CTxtTableFile TabFile; 
	SetTabFile(TabFile, "普通攻击");
	if (!TabFile.Load(PATH_ALIAS_CFG.c_str(), szFileName.c_str())) return false;

	for( int32 i = 1; i < TabFile.GetHeight(); i++ )
	{
		SetLineNo(i);
		CNormalAttackCfg* pNormalAttack = new CNormalAttackCfg;
		CNormalAttackCfgSharedPtr* pNormalAttackSharedPtr = new CNormalAttackCfgSharedPtr(pNormalAttack);

		pNormalAttack->m_uId							= i;
		ReadItem(pNormalAttack->m_strName,				szNT_Name);
		ReadItem(pNormalAttack->m_pMaxAttackDis,		szNT_MaxAttackDis, GE, 0.0f);
		ReadItem(pNormalAttack, *(pNormalAttack->m_pMagicEff),			szNT_MagicEff);
		ReadItem(pNormalAttack->m_strKFrameFileMan,		szNT_KeyFrameFileMan);
		ReadItem(pNormalAttack->m_strKFrameFileWoman,	szNT_KeyFrameFileWoman);

		if(!InsertStringSharedPtrMap(GetNormalAttackMap(), pNormalAttackSharedPtr))
		{
			SafeDelete(pNormalAttackSharedPtr);
			continue;
		}
	}
	return true;
}

bool CNormalAttackCfg::LoadNPCNormalAttackConfig(const string& szFileName)
{
	using namespace CfgChk;

	InitMapWeaponType();
	CTxtTableFile TabFile; 
	SetTabFile(TabFile, "NPC普通攻击");
	if (!TabFile.Load(PATH_ALIAS_CFG.c_str(), szFileName.c_str())) return false;

	uint32 uNormalCfgSize = GetNormalAttackMap().size();
	for( int32 i = 1; i < TabFile.GetHeight(); i++ )
	{
		SetLineNo(i);
		CNormalAttackCfg* pNormalAttack = new CNormalAttackCfg;
		CNormalAttackCfgSharedPtr* pNormalAttackSharedPtr = new CNormalAttackCfgSharedPtr(pNormalAttack);

		pNormalAttack->m_uId							= uNormalCfgSize + i;
		ReadItem(pNormalAttack->m_strName,				szNT_Name);
		ReadItem(pNormalAttack->m_eAttackType,			szNT_AttackType,			CSkillCfg::ms_mapAttackType);
		//ReadItem(pNormalAttack->m_pMaxAttackDis,		szNT_MaxAttackDis, GE, 0.0f);
		ReadItem(pNormalAttack, *(pNormalAttack->m_pMagicEff),			szNT_MagicEff);
		ReadItem(pNormalAttack->m_strKFrameFileMan,		szNT_KeyFrameFileMan);
		ReadItem(pNormalAttack->m_strKFrameFileWoman,	szNT_KeyFrameFileWoman);

		if(!InsertStringSharedPtrMap(GetNPCNormalAttackMap(), pNormalAttackSharedPtr))
		{
			SafeDelete(pNormalAttackSharedPtr);
			continue;
		}
	}
	return true;
}

void CNormalAttackCfg::UnloadNormalAttackConfig()
{
	ClearMap(GetNormalAttackMap());
	ClearMap(GetNPCNormalAttackMap());
}

CNormalAttackCfg::CNormalAttackCfg()
:m_uId(0)
,m_pMaxAttackDis(NULL)
,m_eAttackType(eATT_None)
{
	m_pMagicEff = new MagicEffServerStringRefer(this);
}

CNormalAttackCfg::CNormalAttackCfg(const CNormalAttackCfg& cfg)
:m_uId(cfg.m_uId)
,m_strName(cfg.m_strName)
,m_pMaxAttackDis(cfg.m_pMaxAttackDis?new CCfgCalc(*(cfg.m_pMaxAttackDis)):NULL)
,m_eAttackType(cfg.m_eAttackType)
,m_strKFrameFileMan(cfg.m_strKFrameFileMan)
,m_strKFrameFileWoman(cfg.m_strKFrameFileWoman)
{
	string str;
	m_pMagicEff = new MagicEffServerStringRefer(this);
	cfg.m_pMagicEff->GetStringKey(str);
	m_pMagicEff->SetStringKey(str);
}

CNormalAttackCfg::~CNormalAttackCfg()
{
	SafeDelete(m_pMaxAttackDis);
	SafeDelete(m_pMagicEff);
}

CNormalAttackCfgSharedPtr& CNormalAttackCfg::GetNormalAttack(const TCHAR* szName)
{
	MapNormalAttack::iterator it_Skill = GetNormalAttackMap().find(szName);
	if(it_Skill == GetNormalAttackMap().end())
	{
		stringstream str; 
		str << szName;
		GenErr("没有这个普攻", str.str().c_str());
	}
	return *(it_Skill->second);
}

CNormalAttackCfgSharedPtr& CNormalAttackCfg::GetNPCNormalAttack(const TCHAR* szName)
{
	MapNormalAttack::iterator it_Skill = GetNPCNormalAttackMap().find(szName);
	if(it_Skill == GetNPCNormalAttackMap().end())
	{
		stringstream str; 
		str << szName;
		GenErr("没有这个NPC普攻", str.str().c_str());
	}
	return *(it_Skill->second);
}

void CNormalAttackCfg::UpdateCfg(const string& strName)
{
	if(!UpdateCfg::UpdateCfg<MapNormalAttack, CNormalAttackCfg, CNormalAttackCfgSharedPtr>(GetNormalAttackMap(), strName))
		UpdateCfg::UpdateCfg<MapNormalAttack, CNormalAttackCfg, CNormalAttackCfgSharedPtr>(GetNPCNormalAttackMap(), strName);
}

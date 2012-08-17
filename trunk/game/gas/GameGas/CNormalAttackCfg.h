#pragma once
#include "FightDef.h"
#include "CCfgCalc.h"
#include "TConfigAllocator.h"
#include "CMagicEffServer.h"
#include "TCfgSharedPtr.h"
#include "TStringRef.h"

class CCfgCalc;
class CMagicEffServer;

DefineCfgSharedPtr(CNormalAttackCfg)

class CNormalAttackCfg
	:public CConfigMallocObject
{
	friend class CNormalAttackAniTick;
	friend class CNormalAttackMgr;
	friend class CSingleHandNATick;

	typedef map<string, CNormalAttackCfgSharedPtr*, less<string>, TConfigAllocator<pair<string, CNormalAttackCfgSharedPtr*> > >			MapNormalAttack;
	typedef map<string, EWeaponType, less<string>, TConfigAllocator<pair<string, EWeaponType> > >						MapWeaponType;
	typedef TStringRefer<CNormalAttackCfg, CMagicEffServer>	MagicEffServerStringRefer;

public:
	static	MapNormalAttack& GetNormalAttackMap();
	static	MapNormalAttack& GetNPCNormalAttackMap();

	static	bool LoadNormalAttackConfig(const string& szFileName);
	static	bool LoadNPCNormalAttackConfig(const string& szFileName);
	static	void UnloadNormalAttackConfig();

	static	CNormalAttackCfgSharedPtr& GetNormalAttack(const TCHAR* szName);
	static	CNormalAttackCfgSharedPtr& GetNPCNormalAttack(const TCHAR* szName);
	static	void UpdateCfg(const string& strName);

	static	MapWeaponType	ms_mapWeaponType;
	inline static void InitMapWeaponType()					
	{
		ms_mapWeaponType[""]			= eWT_None;

		ms_mapWeaponType["盾牌"]		= eWT_Shield;

		ms_mapWeaponType["单手剑"]		= eWT_SHSword;
		ms_mapWeaponType["单手斧"]		= eWT_SHAxe;
		ms_mapWeaponType["单手锤"]		= eWT_SHHammer;
		ms_mapWeaponType["单手刀"]		= eWT_SHKnife;

		ms_mapWeaponType["主手剑"]		= eWT_SHSword;
		ms_mapWeaponType["主手斧"]		= eWT_SHAxe;
		ms_mapWeaponType["主手锤"]		= eWT_SHHammer;
		ms_mapWeaponType["主手自然杖"]	= eWT_SHWand;
		ms_mapWeaponType["主手暗黑杖"]	= eWT_SHWand;
		ms_mapWeaponType["主手破坏杖"]	= eWT_SHWand;

		ms_mapWeaponType["副手剑"]		= eWT_SHSword;
		ms_mapWeaponType["副手斧"]		= eWT_SHAxe;
		ms_mapWeaponType["副手锤"]		= eWT_SHHammer;

		ms_mapWeaponType["双手剑"]		= eWT_THSword;
		ms_mapWeaponType["双手斧"]		= eWT_THAxe;
		ms_mapWeaponType["双手锤"]		= eWT_THHammer;
		ms_mapWeaponType["双手弓"]		= eWT_Bow;
		ms_mapWeaponType["双手弩"]		= eWT_CrossBow;
		ms_mapWeaponType["双手自然杖"]	= eWT_THWand;
		ms_mapWeaponType["双手暗黑杖"]	= eWT_THWand;
		ms_mapWeaponType["双手破坏杖"]	= eWT_THWand;

		ms_mapWeaponType["骑士枪"]		= eWT_PaladinGun;
		ms_mapWeaponType["长柄矛"]		= eWT_Lance;
		ms_mapWeaponType["长柄刀"]		= eWT_LongKnife;
		ms_mapWeaponType["长柄棒"]		= eWT_LongStick;
	}

	CNormalAttackCfg();
	CNormalAttackCfg(const CNormalAttackCfg& cfg);
	~CNormalAttackCfg();
	string&			GetName()					{return m_strName;}
	uint32			GetId()	const				{return m_uId;}
	EAttackType		GetAttackType()const		{return m_eAttackType;}

	const string&	GetKFrameFileMan() 			{return m_strKFrameFileMan;}
	const string&	GetKFrameFileWoman() 		{return m_strKFrameFileWoman;}

	const CMagicEffServerSharedPtr&		GetMagicEff()		{return m_pMagicEff->Get();}

private:
	uint32					m_uId;				//技能编号
	string					m_strName;			//普通攻击名
	CCfgCalc*				m_pMaxAttackDis;	//最大攻击距离
	MagicEffServerStringRefer*	m_pMagicEff;		//魔法效果名
	EAttackType				m_eAttackType;		//攻击类型, 用于npc普攻表
	string					m_strKFrameFileMan;
	string					m_strKFrameFileWoman;
};


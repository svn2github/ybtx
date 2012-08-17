#pragma once
#include "FightDef.h"
#include "TCfgSharedPtr.h"
#include "CConfigMallocObject.h"
#include "TConfigAllocator.h"

DefineCfgSharedPtr(CNormalAttackCfgClient)


class CCfgCalc;
class CFighterDirector;

class CNormalAttackCfgClient
	: public CConfigMallocObject
{
	typedef map<string, CNormalAttackCfgClientSharedPtr*, less<string>, TConfigAllocator<pair<string, CNormalAttackCfgClientSharedPtr*> > >	MapNormalAttack;
	typedef map<uint32, CNormalAttackCfgClientSharedPtr*, less<uint32>, TConfigAllocator<pair<uint32, CNormalAttackCfgClientSharedPtr*> > >	MapNormalAttackById;
	typedef map<string, EWeaponType, less<string>, TConfigAllocator<pair<string, EWeaponType> > >				MapWeaponType;
	typedef map<string, EAttackType, less<string>, TConfigAllocator<pair<string, EAttackType> > >				MapAttackType;
public:
	static	MapWeaponType	ms_mapWeaponType;
	static	MapAttackType	ms_mapAttackType;
	static	bool LoadNormalAttackConfig(const string& szFileName);
	static	bool LoadNpcNormalAttackConfig(const string& szFileName);
	static	void UnloadNormalAttackConfig();
	static	CNormalAttackCfgClientSharedPtr& GetNormalAttack(const TCHAR* szName);
	static	CNormalAttackCfgClientSharedPtr& GetNormalAttackById(uint32 uID);
	inline static void InitMapAttackType()					
	{
		ms_mapAttackType[""]		= eATT_None;
		ms_mapAttackType["穿刺"]	= eATT_Puncture;
		ms_mapAttackType["砍斫"]	= eATT_Chop;
		ms_mapAttackType["钝击"]	= eATT_BluntTrauma;
		ms_mapAttackType["自然"]	= eATT_Nature;
		ms_mapAttackType["破坏"]	= eATT_Destroy;
		ms_mapAttackType["暗黑"]	= eATT_Evil;
	}

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

	~CNormalAttackCfgClient();

	uint32				GetId()							{return m_uId;}
	const string&		GetName()const					{return m_strName;};	
	CCfgCalc*			GetMaxAttackDis()const			{return m_pMaxAttackDis;}

private:
	static MapNormalAttack		ms_mapNormalAttack;
	static MapNormalAttackById	ms_mapNormalAttackById;
	uint32		m_uId;				//技能编号
	string		m_strName;			//普通攻击名
	CCfgCalc*	m_pMaxAttackDis;	//最大攻击距离
};

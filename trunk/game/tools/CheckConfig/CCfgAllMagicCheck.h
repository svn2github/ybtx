#pragma once
#include "CCfgMagicEffCheck.h"

#define DefineCfgMagicCheckClass(Name) \
class Name \
{ \
public: \
	static bool Check(const TCHAR* cfgFile); \
	static void	EndCheck(); \
	static bool CheckExist(const string& strName); \
	typedef set<string>	SetRowName; \
	static SetRowName	ms_setRowName; \
}; 

DefineCfgMagicCheckClass(CCfgBarrierMagicCheck);
DefineCfgMagicCheckClass(CCfgBattleArrayMagicCheck);
DefineCfgMagicCheckClass(CCfgBulletMagicCheck);
DefineCfgMagicCheckClass(CCfgShockWaveMagicCheck);
DefineCfgMagicCheckClass(CCfgTransMagicCheck);

class CCfgAureMagicCheck
{
public:
	CCfgAureMagicCheck();
	~CCfgAureMagicCheck();
	static bool Check(const TCHAR* cfgFile);
	static void	EndCheck();
	static bool CheckExist(const string& strName);

	string GetName() { return m_strName; }
	bool GetPersistent() { return m_bPersistent; }
	bool GetNeedSaveToDB() { return m_bNeedSaveToDB; }
	CCfgMagicEffCheck* GetMagicEff() { return m_pMagicEff; }
	bool CheckRelationAvail();

	typedef map<string, CCfgAureMagicCheck*>	MapAureMagic;
	static MapAureMagic		ms_mapAureMagic;

private:
	string				m_strName;
	bool				m_bPersistent;
	bool				m_bNeedSaveToDB;
	CCfgMagicEffCheck*	m_pMagicEff;
};

class CCfgPositionMagicCheck
{
public:
	CCfgPositionMagicCheck();
	~CCfgPositionMagicCheck();
	static bool Check(const TCHAR* cfgFile);
	static void	EndCheck();
	static bool CheckExist(const string& strName);

	string GetName() { return m_strName; }
	CCfgMagicEffCheck* GetTouchMagicEff() { return m_pTouchMagicEff; }
	bool IsMutex()	{ return m_bMutex; }
	bool CheckRelationAvail();

	typedef map<string, CCfgPositionMagicCheck*>	MapPositionMagic;
	static MapPositionMagic		ms_mapPositionMagic;

private:
	string				m_strName;
	CCfgMagicEffCheck*	m_pTouchMagicEff;
	bool				m_bMutex;
};

class CCfgTotemMagicCheck
{
public:
	CCfgTotemMagicCheck();
	~CCfgTotemMagicCheck();
	static bool Check(const TCHAR* cfgFile);
	static void	EndCheck();
	static bool CheckExist(const string& strName);

	string GetName() { return m_strName; }
	CCfgMagicEffCheck* GetTouchMagicEff() { return m_pTouchMagicEff; }
	bool CheckRelationAvail();

	typedef map<string, CCfgTotemMagicCheck*>	MapTotemMagic;
	static MapTotemMagic		ms_mapTotemMagic;

private:
	string				m_strName;
	CCfgMagicEffCheck*	m_pTouchMagicEff;
};

class CCfgBurstSoulCheck
{
public:
	static bool Check(const TCHAR* cfgFile);
	static void	EndCheck();
	static bool CheckExist(const string& strName);
	static bool CheckExistType(const string& strType);
	typedef set<string>	SetRowName;
	static SetRowName	ms_setRowName;

	typedef map<string, EBurstSoulType>	MapBurstSoulType;
	static MapBurstSoulType		ms_mapBurstSoulType;
	static bool	InitMapBurstSoulType()	
	{	
		ms_mapBurstSoulType["技能"]			= eBST_Skill;
		ms_mapBurstSoulType["任务"]			= eBST_Quest;
		return true;
	}
};

class CCfgMoveMagicCheck
{
public:
	CCfgMoveMagicCheck();
	~CCfgMoveMagicCheck();
	static bool Check(const TCHAR* cfgFile);
	static void	EndCheck();
	static bool CheckExist(const string& strName);

	string GetName() { return m_strName; }
	CCfgMagicEffCheck* GetCancelMagicEff() { return m_pCancelMagicEff; }
	bool CheckRelationAvail();

	typedef map<string, CCfgMoveMagicCheck*>	MapMoveMagic;
	static MapMoveMagic		ms_mapMoveMagic;

private:
	string				m_strName;
	CCfgMagicEffCheck*	m_pCancelMagicEff;
};

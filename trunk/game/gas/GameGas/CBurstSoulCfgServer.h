#pragma once
#include "CConfigMallocObject.h"
#include "CMagicEffServer.h"
#include "TCfgSharedPtr.h"
#include "TStringRef.h"

class CBurstSoulServer;
class CCfgCalc;

DefineCfgSharedPtr(CBurstSoulCfgServer)

class CBurstSoulCfgServer
	:public CConfigMallocObject 
{
	typedef TStringRefer<CBurstSoulCfgServer, CMagicEffServer>	MagicEffServerStringRefer;
public:
	static bool				LoadConfig(const TCHAR* cfgFile);		//读配置表
	static void				UnloadConfig();	
	static void				UpdateCfg(const string& strName);

	CBurstSoulCfgServer();
	CBurstSoulCfgServer(const CBurstSoulCfgServer& cfg);
	~CBurstSoulCfgServer();

	const string			GetName()				{return m_sName;}
	CCfgCalc*				GetDoubleBurst()		{return m_pDoubleBurst;}
	EBurstSoulType			GetBurstSoulType()		{return m_eBurstSoulType;}
	CCfgCalc*				GetSoulCount()			{return m_pSoulCount;}
	const CMagicEffServerSharedPtr&		GetMagicEff()const;
	const string			GetBulletMagicName()	{return m_sBulletMagicName;}
	CCfgCalc*				GetProbability()		{return m_fProbability;}
	CCfgCalc*				GetLevel()				{return m_pLevel;}
	CCfgCalc*				GetNeedBurstTimes()		{return m_pNeedBurstTimes;}
public:
	typedef map<string, CBurstSoulCfgServerSharedPtr*>		MapBurstSoulCfg;
	typedef map<string, EBurstSoulType, less<string>, 
		TConfigAllocator<pair<string, EBurstSoulType > > >			MapBurstSoulType;
	
	static MapBurstSoulCfg		ms_mapBurstSoulCfg;
	static MapBurstSoulType		ms_mapBurstSoulType;
private:
	static bool	InitMapBurstSoulType()	
	{	
		ms_mapBurstSoulType["技能"]			= eBST_Skill;
		ms_mapBurstSoulType["任务"]			= eBST_Quest;
		return true;
	}

	string						m_sName;
	CCfgCalc*					m_pDoubleBurst;
	EBurstSoulType				m_eBurstSoulType;
	CCfgCalc*					m_pSoulCount;
	string						m_sMagicEffName;
	string						m_sBulletMagicName;
	CCfgCalc*					m_fProbability;
	MagicEffServerStringRefer*	m_pExtraMagicEff;
	CCfgCalc*					m_pLevel;
	CCfgCalc*					m_pNeedBurstTimes;
};


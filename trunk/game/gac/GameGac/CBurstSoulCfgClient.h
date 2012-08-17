#pragma once
#include "CStaticObject.h"
#include "TCfgSharedPtr.h"
#include "TConfigAllocator.h"
#include "CConfigMallocObject.h"

DefineCfgSharedPtr(CBurstSoulCfgClient)


class CCfgCalc;

class CBurstSoulCfgClient
	: public virtual CStaticObject
	, public CConfigMallocObject
{
public:
	static bool				LoadConfig(const TCHAR* cfgFile);		//∂¡≈‰÷√±Ì
	static void				UnloadConfig();	
	~CBurstSoulCfgClient();

	const string			GetName()				{return m_sName;}
	CCfgCalc*				GetLevel()				{return m_pLevel;}
	CCfgCalc*				GetNeedBurstTimes()		{return m_pNeedBurstTimes;}
	static uint32			GetNeedBurstSoulTimesByLevel(uint32 uLevel);

public:
	typedef map<string, CBurstSoulCfgClientSharedPtr*, less<string>, TConfigAllocator<pair<string, CBurstSoulCfgClientSharedPtr*>> >		MapBurstSoulCfg;
	static MapBurstSoulCfg		ms_mapBurstSoulCfg;
	string						m_sName;
	CCfgCalc*					m_pLevel;
	CCfgCalc*					m_pNeedBurstTimes;
};
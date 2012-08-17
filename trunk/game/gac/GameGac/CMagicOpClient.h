#pragma once
#include "FXPlayer.h"
#include "CConfigMallocObject.h"
#include "TConfigAllocator.h"

class CMagicOpCfgClient
	: public CConfigMallocObject
{
public:
	typedef map<uint32, CMagicOpCfgClient*, less<uint32>, TConfigAllocator<pair<uint32, CMagicOpCfgClient*> > >		MapMagicOpCfgClient;
	typedef map<string, EFxType, less<string>, TConfigAllocator<pair<string, EFxType> > >		MapFxType;
	static bool									LoadConfig(const TCHAR* cfgFile, bool bFirstFile);
	static void									UnloadConfig();
	static CMagicOpCfgClient*					GetById(uint32 uId);				
	uint32	GetId()								{return m_uId;}
	string	GetFX()								{return m_sFX;}
	EFxType GetFxType()					{return m_eFxType;}
	static 	MapFxType	ms_mapFxType;
	static void							InitMapFxType()					
	{
		ms_mapFxType[""]	= eFT_None;
		ms_mapFxType["连接"]		= eFT_Leading;
		ms_mapFxType["受击"]		= eFT_Suffer;
		ms_mapFxType["本地"]		= eFT_Local;
		ms_mapFxType["连线方向"]		= eFT_LineDirection;
	}

private:
	static MapMagicOpCfgClient					m_mapCfgById;
	static uint32		ms_uMaxLineNum;
	uint32				m_uId;					//编号
	string				m_sName;				//名称
	string				m_sFX;					//特效
	EFxType				m_eFxType;		//是否播放受击特效
};
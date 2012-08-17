#pragma once
#include "CMagicClient.h"
#include "TCfgSharedPtr.h"
#include "TConfigAllocator.h"
#include "CConfigMallocObject.h"

DefineCfgSharedPtr(CPositionMagicCfgClient)


class CPositionMagicCfgClient
	: public CConfigMallocObject
{
public:
	typedef map<uint32, CPositionMagicCfgClientSharedPtr*, less<uint32>, TConfigAllocator<pair<uint32, CPositionMagicCfgClientSharedPtr*> > >		MapPositionMagicCfgClientById;
	static bool									LoadConfig(const TCHAR* cfgFile);
	static void									UnloadConfig();
	static CPositionMagicCfgClientSharedPtr&	GetById(uint32 uId);				
	const uint32		GetId()					{return m_uId;}
	const TCHAR*			GetFX()					{return m_sFX.c_str();};

private:
	static MapPositionMagicCfgClientById		m_mapCfgById;
	uint32				m_uId;					// ±àºÅ
	string				m_sFX;					// ÌØÐ§
};

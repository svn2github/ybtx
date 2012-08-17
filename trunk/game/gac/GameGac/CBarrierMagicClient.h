#pragma once
#include "CMagicClient.h"
#include "TCfgSharedPtr.h"
#include "TConfigAllocator.h"
#include "CConfigMallocObject.h"

DefineCfgSharedPtr(CBarrierMagicCfgClient)
class CBarrierMagicCfgClient
	: public CConfigMallocObject
{
public:
	typedef map<uint32, CBarrierMagicCfgClientSharedPtr*, less<uint32>, TConfigAllocator<pair<uint32, CBarrierMagicCfgClientSharedPtr*> > >		MapBarrierMagicCfgClientById;
	static bool									LoadConfig(const TCHAR* cfgFile);
	static void									UnloadConfig();
	static CBarrierMagicCfgClientSharedPtr&		GetById(uint32 uId);				
	const uint32		GetId()					{return m_uId;}
	const TCHAR*			GetFX()					{return m_sFX.c_str();}

private:
	static MapBarrierMagicCfgClientById		m_mapCfgById;
	uint32				m_uId;					// ±àºÅ
	string				m_sFX;					// ÌØÐ§
};

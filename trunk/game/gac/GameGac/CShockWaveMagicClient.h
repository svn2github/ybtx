#pragma once
#include "CMagicClient.h"
#include "TCfgSharedPtr.h"
#include "TConfigAllocator.h"
#include "CConfigMallocObject.h"

DefineCfgSharedPtr(CShockWaveMagicCfgClient)


class CShockWaveMagicCfgClient
	: public CConfigMallocObject
{
public:
	typedef map<uint32, CShockWaveMagicCfgClientSharedPtr*, less<uint32>, TConfigAllocator<pair<uint32, CShockWaveMagicCfgClientSharedPtr*> > >		MapShockWaveMagicCfgClientById;
	static bool									LoadConfig(const TCHAR* cfgFile);
	static void									UnloadConfig();
	static CShockWaveMagicCfgClientSharedPtr&	GetById(uint32 uId);				
	const uint32		GetId()					{return m_uId;}
	const TCHAR*			GetFX()					{return m_sFX.c_str();}

private:
	static MapShockWaveMagicCfgClientById		m_mapCfgById;
	uint32				m_uId;					// ±àºÅ
	string				m_sFX;					// ÌØÐ§
};

class CShockWaveMagicClient :public CMagicClient
{
public:
	CShockWaveMagicClient(CCoreObjectFollower* pCoreObjFol);
	~CShockWaveMagicClient(){}
private:
	void AdjustDirection();
};
#pragma once
#include "CMagicClient.h"
#include "TCfgSharedPtr.h"
#include "TConfigAllocator.h"
#include "CConfigMallocObject.h"

DefineCfgSharedPtr(CTransferableMagicCfgClient)

class CEntityClient;
class CTransferableMagicCfgClient
	: public CConfigMallocObject
{
public:
	typedef map<uint32, CTransferableMagicCfgClientSharedPtr*, less<uint32>, TConfigAllocator<pair<uint32, CTransferableMagicCfgClientSharedPtr*> > >		MapTransferableMagicCfgClientById;
	static bool									LoadConfig(const TCHAR* cfgFile);
	static void									UnloadConfig();
	static CTransferableMagicCfgClientSharedPtr&			GetById(uint32 uId);				
	const uint32		GetId()					{return m_uId;}
	const TCHAR*			GetFX()					{return m_sFX.c_str();};

private:
	static MapTransferableMagicCfgClientById	m_mapCfgById;
	uint32				m_uId;					// ±àºÅ
	string				m_sFX;					// ÌØÐ§
};

class CTransferableMagicClient:public CMagicClient
{
public:
	CTransferableMagicClient(CCoreObjectFollower* pCoreObjFol, uint32 uSrcEntityID);
	~CTransferableMagicClient();
	void PlayMagicFx(CEntityClient* pTargetObj, const string& strMagicFx);
	void OnValueChanged(CCypherVariant *pVariant);
private:
	uint32 m_uOwnerEntityID;
};

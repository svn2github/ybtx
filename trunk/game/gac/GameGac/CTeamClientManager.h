#pragma once
#include "CPos.h"
#include "TSingleton.h"
#include "TFighterAllocator.h"

class CGas2GacCPP_TeamSyncInfo;

class CSyncSceneInfo : public CFighterMallocObject
{
public:
	CSyncSceneInfo(uint32 uEntityID);
	const CFPos& GetPos()const{return m_fPos;}
	void UpdateInfo(const CGas2GacCPP_TeamSyncInfo* pCmd);
private:
	friend class CTeamClientManager;
	CFPos m_fPos;
	const uint32 m_uEntityID;
	uint32 m_uLevel;
	uint32 m_uBaseHP;
	uint32 m_uBaseMP;
	uint32 m_uBaseEP;
	uint32 m_uBaseRP;
	uint32 m_uHP;
	uint32 m_uMP;
	uint32 m_uEP;
	uint32 m_uRP;
	uint32 m_uHairResID;
	uint32 m_uFaceResID;
	uint32 m_uHeadIndexID;
	uint32 m_uBodyIndexID;
	bool m_bInBattle;
};

class ITeamClientManagerHandler;

class CTeamClientManager:public TSingleton<CTeamClientManager>, public CFighterMallocObject
{
	friend class TSingleton<CTeamClientManager>;
public:
	typedef map<uint32,CSyncSceneInfo*, less<uint32>, TFighterAllocator<pair<uint32,CSyncSceneInfo*> > > MapSyncInfo;
	static void SetHandler( ITeamClientManagerHandler *pHandler );
	static ITeamClientManagerHandler* GetHandler() { return GetInst()->m_pHandler; }
	static CTeamClientManager& CreateTeamManager()
	{
		return *GetInst();
	}
	static MapSyncInfo& GetSyncInfoMap()
	{
		return GetInst()->m_mapSyncInfo;
	}
	~CTeamClientManager();
	static uint32 GetHP(uint32 uEntityID);
	static uint32 GetMP(uint32 uEntityID);
	static uint32 GetEP(uint32 uEntityID);
	static uint32 GetRP(uint32 uEntityID);
	static uint32 GetBaseHP(uint32 uEntityID);
	static uint32 GetBaseMP(uint32 uEntityID);
	static uint32 GetBaseEP(uint32 uEntityID);
	static uint32 GetBaseRP(uint32 uEntityID);
	static uint32 GetLevel(uint32 uEntityID);
	static void GetPos(uint32 uEntityID,CFPos& Pos);
	static uint32 GetHairResID(uint32 uEntityID);
	static uint32 GetFaceResID(uint32 uEntityID);
	static uint32 GetHeadIndexID(uint32 uEntityID);
	static uint32 GetBodyIndexID(uint32 uEntityID);
	static bool GetInBattle(uint32 uEntityID);
private:
	CTeamClientManager(){}
	MapSyncInfo	m_mapSyncInfo;
	ITeamClientManagerHandler* m_pHandler;
};
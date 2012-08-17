#pragma once
#include "FightDef.h"
#include "TFighterAllocator.h"

class ITeamServerManagerHandler;
class CCharacterMediator;
class CTeamInfoServer;
class CGas2GacCPP_TeamSyncInfo;
struct TempVariantCache;

class CTeamServerManager
	:public CFighterMallocObject
{
public:
	typedef set<ITeamServerManagerHandler*, less<ITeamServerManagerHandler*>, TFighterAllocator<ITeamServerManagerHandler*> > SetHandler;
	typedef set<uint32,less<uint32>,TFighterAllocator<uint32> >		SetEntityID;	
	typedef map<pair<uint32,ETeamType>,CTeamInfoServer*, less<pair<uint32,ETeamType> >, TFighterAllocator<pair<pair<uint32,ETeamType>,CTeamInfoServer* > > > MapTeamMgr;
private:
	CTeamServerManager(){}
	MapTeamMgr	m_mapTeamManager;
	void AddTeamMember(uint32 uTeamID,ETeamType eTeamType,uint32 uEntityID);
	void RemoveTeamMember(uint32 uTeamID,ETeamType eTeamType,uint32 uEntityID);
	void CreateTeam(uint32 uTeamID,ETeamType eTeamType,uint32 uEntityID);
	void DestroyTeam(uint32 uTeamID,ETeamType eTeamType);
	void ChangeTeamMaster(uint32 uTeamID,ETeamType eTeamType,uint32 uEntityID);
	SetHandler	m_setHandler;
public:
	static void Init()
	{
		GetInst() = new CTeamServerManager();
	}
	static CTeamServerManager*& GetInst()
	{
		static CTeamServerManager *pTeamMgr = NULL;
		return pTeamMgr;
	}
	static void Unit()
	{
		delete CTeamServerManager::GetInst();
		CTeamServerManager::GetInst() = NULL;
	}
	static CTeamServerManager& CreateTeamManager()
	{
		return *GetInst();
	}
	static void CppAddTeamMember(uint32 uTeamID,ETeamType eTeamType,uint32 uEntityID)
	{
		return GetInst()->AddTeamMember(uTeamID,eTeamType,uEntityID);
	}
	static void CppRemoveTeamMember(uint32 uTeamID,ETeamType eTeamType,uint32 uEntityID)
	{
		GetInst()->RemoveTeamMember(uTeamID,eTeamType,uEntityID);
	}
	static void CppCreateTeam(uint32 uTeamID,ETeamType eTeamType,uint32 uEntityID)
	{
		return GetInst()->CreateTeam(uTeamID,eTeamType,uEntityID);
	}
	static void CppDestroyTeam(uint32 uTeamID,ETeamType eTeamType)
	{
		GetInst()->DestroyTeam(uTeamID,eTeamType);
	}
	static void CppChangeTeamMaster(uint32 uTeamID,ETeamType eTeamType,uint32 uEntityID)
	{
		return GetInst()->ChangeTeamMaster(uTeamID,eTeamType,uEntityID);
	}
	static const SetEntityID& CppGetTeamMember(uint32 uTeamID,ETeamType eTeamType)
	{
		return GetInst()->GetTeamMember(uTeamID,eTeamType);
	}

	static void CppDestroyTeamManager()
	{
		return GetInst()->DestroyTeamManager();
	}
	uint32 GetTeamMaster(uint32 uTeamID,ETeamType eTeamType)const;
	const SetEntityID& GetTeamMember(uint32 uTeamID,ETeamType eTeamType)const;
	~CTeamServerManager();
	void DestroyTeamManager(){delete this;}
	void AddHandler(ITeamServerManagerHandler* pHandler);
	void RemoveHander(ITeamServerManagerHandler* pHandler);
};

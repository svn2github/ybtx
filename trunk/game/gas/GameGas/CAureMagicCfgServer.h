#pragma once
#include "CMagicCfgServer.h"
#include "CMagicEffServer.h"
#include "TCfgSharedPtr.h"

class CEffDataByGlobalIDMgr;
class CCharacterDictator;

DefineCfgSharedPtr(CAureMagicCfgServer)

class CAureMagicCfgServer
	:public CMagicCfgServer
{
public:
	static		bool LoadAureMagicConfig(const string& szFileName);
	static		void UnloadAureMagicConfig();
	static		CAureMagicCfgServerSharedPtr& GetAureMagic(const string& szName);
	static		void UpdateCfg(const string& strName);

	CAureMagicCfgServer();
	CAureMagicCfgServer(const CAureMagicCfgServer& cfg);
	~CAureMagicCfgServer();

	EStanceType	GetStanceType()	{ return m_eStanceType; }
	EAgileType	GetAgileType()	{ return m_eAgileType; }
	bool		GetNeedSaveToDB()	{ return m_bNeedSaveToDB; }
	bool		GetPersistent() { return m_eStanceType <= eSS_MutexStance; }		//判断是否死亡不清除
	virtual const CMagicEffServerSharedPtr& GetMagicEff()const;

	static CAureMagicCfgServerSharedPtr ms_NULL;

private:
	typedef map<string, CAureMagicCfgServerSharedPtr*, less<string>, 
		TConfigAllocator<pair<string, CAureMagicCfgServerSharedPtr* > > >	MapAureMagicCfgServer;
	typedef map<string, EStanceType, less<string>, 
		TConfigAllocator<pair<string, EStanceType > > > MapStanceType;
	typedef map<string, EAgileType, less<string>, 
		TConfigAllocator<pair<string, EAgileType > > > MapAgileType;
	typedef TStringRefer<CAureMagicCfgServer, CMagicEffServer> MagicEffServerStringRefer;

	static MapAureMagicCfgServer				ms_mapAureMagicServer;
	static MapStanceType						ms_mapStanceType;
	static MapAgileType							ms_mapAgileType;
	inline static bool					InitMapStanceType()
	{
		ms_mapStanceType["姿态"]		= eSS_Stance;
		ms_mapStanceType["互斥姿态"]	= eSS_MutexStance;
		ms_mapStanceType["光环"]		= eSS_Aure;
		ms_mapStanceType["变身"]		= eSS_Form;
		ms_mapStanceType["防护"]		= eSS_Shield;
		ms_mapStanceType["叠加光环"]	= eSS_CascadeAure;
		ms_mapStanceType["隐形光环"]	= eSS_InvisibleAure;
		return true;
	}
	inline static bool					InitMapAgileType()
	{
		ms_mapAgileType["魔法值"]		= eAT_MP;
		ms_mapAgileType["能量值"]		= eAT_EP;
		ms_mapAgileType["怒气值"]		= eAT_RP;
		return true;
	}

	bool						m_bNeedSaveToDB;	////下线是否需要保存到数据库
	EStanceType					m_eStanceType;		//姿态类型
	EAgileType					m_eAgileType;		//易变值类型
	MagicEffServerStringRefer*	m_pMagicEff;

};


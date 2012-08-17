#pragma once
#include "CMagicCfgServer.h"
#include "CMagicMallocObject.h"
#include "CMagicEffServer.h"
#include "TCfgSharedPtr.h"

class CBattleArrayMagicServer;

DefineCfgSharedPtr(CBattleArrayMagicCfgServer)

class CBattleArrayMagicCfgServer
	:public CMagicCfgServer
{
public:
	static bool LoadBattleArrayMagicConfig(const string& szFileName);
	static void UnloadBattleArrayMagicConfig();
	static CBattleArrayMagicCfgServerSharedPtr& GetBattleArrayMagic(const string& szName);
	static		void UpdateCfg(const string& strName);

	CBattleArrayMagicCfgServer();
	CBattleArrayMagicCfgServer(const CBattleArrayMagicCfgServer& cfg);
	~CBattleArrayMagicCfgServer();

	string GetPosSkill(uint8 uNum)				{ return m_strPosSkill[uNum]; }
	uint32 GetMaxAddUpValue()					{ return m_uMaxAddUpValue; }
	virtual const CMagicEffServerSharedPtr& GetMagicEff()const;

	static CBattleArrayMagicCfgServerSharedPtr ms_NULL;
private:
	typedef map<string, CBattleArrayMagicCfgServerSharedPtr*, less<string>, 
		TConfigAllocator<pair<string, CBattleArrayMagicCfgServerSharedPtr* > > >	MapBattleArrayMagicCfgServer;
	typedef TStringRefer<CBattleArrayMagicCfgServer, CMagicEffServer> MagicEffServerStringRefer;

	static MapBattleArrayMagicCfgServer					ms_mapBattleArrayMagic;

	string m_strPosSkill[5];		//位置技能
	uint32 m_uMaxAddUpValue;		//聚气值
	MagicEffServerStringRefer*	m_pMagicEff;
};	


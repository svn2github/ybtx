#pragma once
#include "GameDef.h"
#include "CMagicEffServer.h"
#include "TCfgSharedPtr.h"
#include "TStringRef.h"

DefineCfgSharedPtr(CTalentEffCfgServer)

class CTalentEffCfgServer : public CConfigMallocObject
{
	friend class CTalentEffServer;
public:
	typedef vector<CTalentEffCfgServerSharedPtr*, TConfigAllocator<CTalentEffCfgServerSharedPtr*> > VecTalentEffCfgServer;
private:
	typedef TStringRefer<CTalentEffCfgServer, CMagicEffServer>	MagicEffServerStringRefer;
	typedef map<string, CTalentEffCfgServerSharedPtr* , less<string>, TConfigAllocator<pair<string, CTalentEffCfgServerSharedPtr*> > >			MapTalentEffCfgServer;
	typedef multimap<uint32, CTalentEffCfgServerSharedPtr*, less<uint32>, TConfigAllocator<pair<uint32, CTalentEffCfgServerSharedPtr*> > >		MultiMapTalentEffCfgServer;

public:
	static bool			LoadTalentEffCfgConfig(const string& szFileName);
	static void			UnloadTalentEffCfgConfig();
	static CTalentEffCfgServerSharedPtr&	GetTalentEff(const string& szName);
	static void			GetClassTalentEffVec(EClass eClass,VecTalentEffCfgServer& vecTalentEff);
	static void			UpdateCfg(const string& strName);
	ETalentType			GetTalentType()		{return m_eTalentType;}
	ETalentRuleType		GetRuleType()		{return m_eRuleType;}
	const string&		GetName()			{return m_sTalentName;}	
	bool				IsUpdateWithLevel() {return m_bUpdate;}
	const CMagicEffServerSharedPtr&			GetMagicEff()const;
	CTalentEffCfgServer();
	CTalentEffCfgServer(const CTalentEffCfgServer& cfg);
	~CTalentEffCfgServer();

private:
	static MapTalentEffCfgServer				ms_mapTalentEff;
	static MultiMapTalentEffCfgServer			ms_multimapClassTalentEff;
	static CTalentEffCfgServerSharedPtr			ms_NULL;
	string						m_sTalentName;		//天赋名称
	ETalentType					m_eTalentType;		//天赋类型
	ETalentRuleType				m_eRuleType;		//规则类型
	bool						m_bUpdate;
	MagicEffServerStringRefer*	m_pMagicEff;		//魔法效果
};


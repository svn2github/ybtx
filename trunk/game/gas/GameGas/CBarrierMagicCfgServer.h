#pragma once
#include "CMagicCfgServer.h"
#include "FindPathDefs.h"
#include "CPos.h"
#include "TMagicAllocator.h"
#include "CMagicEffServer.h"
#include "TCfgSharedPtr.h"

class CCfgCalc;
class CSkillInstServer;
class CBarrierMagicServer;

DefineCfgSharedPtr(CBarrierMagicCfgServer)

namespace sqr
{
	class CCoreSceneServer;
}

class CBarrierMagicCfgServer : public CMagicCfgServer
{
	friend class CBarrierMagicServer;
public:
	enum EBarrierMagicType {eBMT_Normal, eBMT_Reflect, eBMT_End};

	static		bool LoadBarrierMagicConfig(const string& szFileName);
	static		void UnloadBarrierMagicConfig();
	static		CBarrierMagicCfgServerSharedPtr& GetBarrierMagic(const string& szName);	
	static		void UpdateCfg(const string& strName);

	CBarrierMagicCfgServer();
	CBarrierMagicCfgServer(const CBarrierMagicCfgServer& cfg);
	~CBarrierMagicCfgServer();	

	EBarrierMagicType	GetBarrierMagicType() { return m_eBarrierMagicType;}
	EBarrierType		GetBarrierType() { return m_eBarrierType;}
	virtual const CMagicEffServerSharedPtr& GetMagicEff()const;

	static CBarrierMagicCfgServerSharedPtr ms_NULL;
private:
	typedef map<string, EBarrierMagicType, less<string>, 
		TConfigAllocator<pair<string, EBarrierMagicType > > > MapBarrierMagicType;
	typedef map<string, EBarrierType, less<string>, 
		TConfigAllocator<pair<string, EBarrierType > > > MapBarrierType;
	typedef map<string, CBarrierMagicCfgServerSharedPtr*, less<string>, 
		TConfigAllocator<pair<string, CBarrierMagicCfgServerSharedPtr* > > >	MapBarrierMagicCfgServer;
	typedef TStringRefer<CBarrierMagicCfgServer, CMagicEffServer> MagicEffServerStringRefer;
	typedef vector<CPos, TConfigAllocator<CPos> >	VecPos_t;

	static MapBarrierMagicCfgServer	ms_mapBarrierMagic;
	static MapBarrierType			ms_mapBarrierType;
	static MapBarrierMagicType		ms_mapBarrierMagicType;

	vector<VecPos_t, TConfigAllocator<VecPos_t> >			m_vecCoord;
	EBarrierType				m_eBarrierType;
	EBarrierMagicType			m_eBarrierMagicType;
	MagicEffServerStringRefer*	m_pMagicEff;
};



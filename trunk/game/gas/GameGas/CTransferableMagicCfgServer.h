#pragma once
#include "CMagicCfgServer.h"
#include "CMagicEffServer.h"
#include "TCfgSharedPtr.h"

class CCfgCalc;

// 传递方式类型
enum ETransMagicType
{
	eTMT_OneOff,			// 一次性
	eTMT_Iteration,			// 反复
};

DefineCfgSharedPtr(CTransferableMagicCfgServer)

class CTransferableMagicCfgServer
	:public CMagicCfgServer
{
public:
	CTransferableMagicCfgServer();
	CTransferableMagicCfgServer(const CTransferableMagicCfgServer& cfg);
	virtual ~CTransferableMagicCfgServer();

	static bool LoadTransferableMagicConfig(const string& szFileName);
	static void UnloadTransferableMagicConfig();
	static CTransferableMagicCfgServerSharedPtr& GetTransferableMagic(const string& szName);
	static		void UpdateCfg(const string& strName);

	ETransMagicType			GetTransType()					{return m_eTransType;}
	const CMagicEffServerSharedPtr&		GetMainMagicEff()const;
	const CMagicEffServerSharedPtr&		GetSecondMagicEff()const;

	static CTransferableMagicCfgServerSharedPtr ms_NULL;

private:
	typedef map<string, CTransferableMagicCfgServerSharedPtr*, less<string>, 
		TConfigAllocator<pair<string, CTransferableMagicCfgServerSharedPtr* > > >	MapTransferableMagicCfgServer;
	typedef map<string, ETransMagicType, less<string>, TConfigAllocator<pair<string, ETransMagicType > > > MapTransType;
	typedef TStringRefer<CTransferableMagicCfgServer, CMagicEffServer> MagicEffServerStringRefer;

	inline static bool								InitMapTransType()
	{
		ms_mapTransType["一次性"]	= eTMT_OneOff;
		ms_mapTransType["反复"]		= eTMT_Iteration;
		return true;
	}

	static MapTransferableMagicCfgServer	ms_mapTransferableMagic;	// 可传递魔法MAP
	static MapTransType						ms_mapTransType;			// 传递方式MAP

	ETransMagicType			m_eTransType;				// 传递方式
	MagicEffServerStringRefer*		m_pMainMagicEff;			// 主魔法效果
	MagicEffServerStringRefer*		m_pSecondMagicEff;			// 副魔法效果	
};


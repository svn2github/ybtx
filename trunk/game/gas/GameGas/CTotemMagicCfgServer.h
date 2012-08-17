#pragma once
#include "CMagicCfgServer.h"
#include "CMagicEffServer.h"
#include "TCfgSharedPtr.h"

DefineCfgSharedPtr(CTotemMagicCfgServer)

class CTotemMagicCfgServer
	:public CMagicCfgServer
{
public:
	static		bool LoadTotemMagicConfig(const string& szFileName);
	static		void UnloadTotemMagicConfig();
	static		CTotemMagicCfgServerSharedPtr& GetTotemMagic(const string& szName);
	static		void UpdateCfg(const string& strName);

	CTotemMagicCfgServer();
	CTotemMagicCfgServer(const CTotemMagicCfgServer& cfg);
	virtual ~CTotemMagicCfgServer();

	const CMagicEffServerSharedPtr&	GetTouchMagicEff()const;
	const CMagicEffServerSharedPtr&	GetDotMagicEff()const;
	const CMagicEffServerSharedPtr&	GetFinalMagicEff()const;
	EOperateObjectType	GetOperateObject()			{return m_eOperateObject;}

	static CTotemMagicCfgServerSharedPtr ms_NULL;
private:
	typedef map<string, CTotemMagicCfgServerSharedPtr*, less<string>, 
		TConfigAllocator<pair<string, CTotemMagicCfgServerSharedPtr* > > >	MapTotemMagicCfgServer;
	typedef TStringRefer<CTotemMagicCfgServer, CMagicEffServer> MagicEffServerStringRefer;

	static MapTotemMagicCfgServer				ms_mapTotemMagic;

	static bool			__init;

	EOperateObjectType			m_eOperateObject;		// 作用对象
	MagicEffServerStringRefer*	m_pTouchMagicEff;		// 触发魔法效果
	MagicEffServerStringRefer*	m_pDotMagicEff;			// 不可撤销间隔魔法效果
	MagicEffServerStringRefer*	m_pFinalMagicEff;		// 不可撤销最终魔法效果
};

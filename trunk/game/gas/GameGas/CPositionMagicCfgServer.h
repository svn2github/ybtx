#pragma once
#include "CMagicCfgServer.h"
#include "CMagicEffServer.h"
#include "TCfgSharedPtr.h"

DefineCfgSharedPtr(CPositionMagicCfgServer)

class CPositionMagicCfgServer
	:public CMagicCfgServer
{
public:
	static		bool LoadPositionMagicConfig(const string& szFileName);
	static		void UnloadPositionMagicConfig();
	static		CPositionMagicCfgServerSharedPtr& GetPositionMagic(const string& szName);
	static		void UpdateCfg(const string& strName);

	CPositionMagicCfgServer();
	CPositionMagicCfgServer(const CPositionMagicCfgServer& cfg);
	virtual ~CPositionMagicCfgServer();

	bool				GetMutexType()				{return m_bMutexType;}
	bool				GetSingleObject()			{return m_bSingleObject;}
	const CMagicEffServerSharedPtr&	GetTouchMagicEff()const;
	const CMagicEffServerSharedPtr&	GetDotMagicEff()const;
	int32				GetDotInterval()			{return m_iDotInterval;}
	const CMagicEffServerSharedPtr&	GetFinalMagicEff()const;

	static CPositionMagicCfgServerSharedPtr ms_NULL;

private:
	typedef map<string, CPositionMagicCfgServerSharedPtr*, less<string>, 
		TConfigAllocator<pair<string, CPositionMagicCfgServerSharedPtr* > > >	MapPositionMagicCfgServer;
	typedef TStringRefer<CPositionMagicCfgServer, CMagicEffServer> MagicEffServerStringRefer;

	static MapPositionMagicCfgServer				ms_mapPositionMagic;

	bool			m_bMutexType;			// 互斥类型
	bool			m_bSingleObject;				// 单体目标
	MagicEffServerStringRefer*	m_pTouchMagicEff;		// 触发魔法效果
	MagicEffServerStringRefer*	m_pDotMagicEff;			// 不可撤销间隔魔法效果
	int32						m_iDotInterval;
	MagicEffServerStringRefer*	m_pFinalMagicEff;		// 不可撤销最终魔法效果"
};


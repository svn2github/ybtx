#pragma once
#include "CMagicCfgServer.h"
#include "FindPathDefs.h"
#include "CMoveMagicDefs.h"
#include "CMagicEffServer.h"
#include "TCfgSharedPtr.h"

class CCfgCalc;

DefineCfgSharedPtr(CMoveMagicCfgServer)

class CMoveMagicCfgServer : public CMagicCfgServer
{
public:
	static bool LoadMoveMagicConfig(const string& szFileName);
	static void UnloadMoveMagicConfig();
	static CMoveMagicCfgServerSharedPtr& GetMoveMagic(const string& szName);
	static void UpdateCfg(const string& strName);

	CMoveMagicCfgServer();
	CMoveMagicCfgServer(const CMoveMagicCfgServer& cfg);
	virtual ~CMoveMagicCfgServer();

	EMoveMagicType		GetMoveType()			{return m_eMoveType;}
	EBarrierType		GetBarrierType()		{return m_eBarrierType;}
	EOperateObjectType	GetAreaOperateObject()	{return m_eAreaOperateObject;}
	EMoveMagicArg 		GetMoveArgType()		{return m_eMoveArgType;}
	EMoveMagicArgLimit 	GetMoveArgTypeLimit()		{return m_eMoveArgLimit;}
	CCfgCalc* 			GetMoveArgValue()			{return m_pMoveArgValue;}
	EMoveActionType		GetActionType()				{return m_eActionType;}
	const CMagicEffServerSharedPtr& GetCancelableMagicEff()const;
	virtual const CMagicEffServerSharedPtr& GetMagicEff()const;

	static CMoveMagicCfgServerSharedPtr ms_NULL;

private:
	typedef map<string, CMoveMagicCfgServerSharedPtr* ,less<string>, 
		TConfigAllocator<pair<string, CMoveMagicCfgServerSharedPtr* > > >		MapMoveMagicCfgServer;
	typedef TStringRefer<CMoveMagicCfgServer, CMagicEffServer> MagicEffServerStringRefer;

	static MapMoveMagicCfgServer		ms_mapMoveMagic;
	EMoveMagicType		m_eMoveType;			//移动方式
	EMoveMagicArg		m_eMoveArgType;			//移动类型
	EMoveMagicArgLimit	m_eMoveArgLimit;		//移动参数限制
	CCfgCalc*			m_pMoveArgValue;		//移动参数值
	EOperateObjectType	m_eAreaOperateObject;	//作用对象
	EBarrierType		m_eBarrierType;			//障碍类型
	EMoveActionType		m_eActionType;			//动作
	MagicEffServerStringRefer*		m_pMagicEff;
	MagicEffServerStringRefer*		m_pCancelableMagicEff;		//可撤销魔法效果
};


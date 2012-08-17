#pragma once
#include "FightDef.h"
#include "TCastingProcessAllocator.h"
#include "CMagicEffServer.h"
#include "TCfgSharedPtr.h"
#include "TStringRef.h"

class CCfgCalc;

DefineCfgSharedPtr(CCastingProcessCfgServer)

class CCastingProcessCfgServer
	:public CCastingProcessMallocObject
	,public TStringRefee<CCastingProcessCfgServer, CCastingProcessCfgServerSharedPtr>
{
public:
	static bool							LoadConfig(const TCHAR* cfgFile, bool bReload = false);		//读配置表
	static void							UnloadConfig();	
	static CCastingProcessCfgServerSharedPtr&	Get(const string& castingProcessName);	//获取施法流程配置行
	static void UpdateCfg(const string& strName);

	static CCastingProcessCfgServerSharedPtr ms_NULL;

	CCastingProcessCfgServer();
	CCastingProcessCfgServer(const CCastingProcessCfgServer& cfg);
	~CCastingProcessCfgServer();
	uint32								GetId()						{return m_uId;}
	const string&						GetName()					{return m_sName;}
	ECastingProcessType					GetCastingType()			{return m_eCastingType;}
	bool								GetMoveInterrupt();
	bool								GetTurnAroundInterrupt()	{return m_eMoveInterrupt == eCIT_TurnAround;}
	CCfgCalc*							GetCastingTime()			{return m_pCastingTime;}
	CCfgCalc*							GetInterruptPercent()		{return m_pInterruptPercent;}
	const CMagicEffServerSharedPtr&		GetMagicEff(uint32 uIndex = 1)const;
	float								GetChannelInterval()		{return m_fChannelInterval;}
	const CMagicEffServerSharedPtr&		GetSelfCancelableMagicEff()const;
	const CMagicEffServerSharedPtr&		GetObjCancelableMagicEff()const;
	const CMagicEffServerSharedPtr&		GetFinalMagicEff()const;

private:
	typedef TStringRefee<CCastingProcessCfgServer, CCastingProcessCfgServerSharedPtr>	ParentType;
	typedef TStringRefer<CCastingProcessCfgServer, CMagicEffServer>	MagicEffServerStringRefer;
	typedef ParentType::MapImpClass	MapCastingProcessCfg;
	typedef vector<MagicEffServerStringRefer*>		VecMagicEff;

	static MapCastingProcessCfg&					GetStringKeyMap();

	uint32					m_uId;						//编号
	string					m_sName;					//名称	
	ECastingProcessType		m_eCastingType;				//施法类型
	ECastingInterruptType	m_eMoveInterrupt;			//移动是否打断
	CCfgCalc*				m_pCastingTime;				//施法时间
	float					m_fChannelInterval;			//引导间隔时间
	CCfgCalc*				m_pInterruptPercent;		//中断几率
	MagicEffServerStringRefer*		m_pSelfCancelableMagicEff;		//可撤销魔法效果名称
	MagicEffServerStringRefer*		m_pObjCancelableMagicEff;		//可撤销魔法效果名称
	MagicEffServerStringRefer*		m_pFinalMagicEff;			//不可撤销最终魔法效果
	VecMagicEff				m_vecMagicEff;				//魔法效果
};


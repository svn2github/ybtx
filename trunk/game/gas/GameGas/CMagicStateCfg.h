#pragma once
#include "FightDef.h"
#include "CBaseStateCfgServer.h"
#include "CMagicEffServer.h"
#include "TCfgSharedPtr.h"

class CCfgCalc;

DefineCfgSharedPtr(CMagicStateCfgServer)

//魔法状态配置类
class CMagicStateCfgServer	
	: public CBaseStateCfgServer
{
	friend class CBaseStateCfgServer;
	friend class CMagicStateServer;
	typedef map<string, CMagicStateCfgServerSharedPtr*, less<string>, TConfigAllocator<pair<string, CMagicStateCfgServerSharedPtr*> > >	MapMagicStateCfg;
	typedef map<uint32, CMagicStateCfgServerSharedPtr*, less<uint32>, TConfigAllocator<pair<uint32, CMagicStateCfgServerSharedPtr*> > >	MapMagicStateCfgById;
	typedef map<string, ECascadeType, less<string>, TConfigAllocator<pair<string, ECascadeType > > > MapCascadeType;
	typedef map<string, EReplaceRuler, less<string>, TConfigAllocator<pair<string, EReplaceRuler > > > MapReplaceRuler;
	typedef TStringRefer<CMagicStateCfgServer, CMagicEffServer>	MagicEffServerStringRefer;

public:
	CMagicStateCfgServer();
	CMagicStateCfgServer(const CMagicStateCfgServer& cfg);
	virtual ~CMagicStateCfgServer();

	static bool									LoadConfig(const TCHAR* cfgFile);	//读配置表
	static void									UnloadConfig();
	static CMagicStateCfgServerSharedPtr&		Get(const string& magicStateName);	//获取魔法状态配置行
	static CMagicStateCfgServerSharedPtr&		GetById(uint32);					//用编号获取魔法状态配置行
	static EStateGlobalType						GetStaticType()				{return eSGT_MagicState;}
	static void UpdateCfg(const string& strName);
	static CMagicStateCfgServerSharedPtr ms_NULL;

private:
	static bool									InitMapCascadeType();				//建立群体叠加类型字符串到对应枚举类型的映射
	static MapMagicStateCfg						m_mapCfg;							//魔法状态配置映射表
	static MapMagicStateCfgById					m_mapCfgById;						//魔法状态配置映射表
	static MapCascadeType						m_mapCascadeType;					//群体叠加类型映射表
	static MapReplaceRuler						m_mapReplaceRuler;

public:
	const EReplaceRuler&	GetReplaceType()			{return m_eReplaceType;}
	const ECascadeType&		GetCascadeType()			{return m_eCascadeType;}
	const bool				GetCascadeGradation()		{return m_bCascadeGradation;}
	const CCfgCalc*			GetCascadeMax()				{return m_calcCascadeMax;}
	const CMagicEffServerSharedPtr&		GetCancelableMagicEff()const;
	const CMagicEffServerSharedPtr&		GetDotMagicEff()const;
	const CMagicEffServerSharedPtr&		GetFinalMagicEff()const;
	//const CCfgCalc*			GetFMAssociateDotM()		{return m_pFMAssociateDotM;}
	float					GetDotInterval()			{return m_fDotInterval;}//不可撤销间隔魔法效果的间隔时间
	bool					InstallerOughtToBeNull();

private:

	EReplaceRuler				m_eReplaceType;				//魔法状态替换规则
	ECascadeType				m_eCascadeType;				//群体叠加类型
	bool						m_bCascadeGradation;		//替换和叠加是否限制等级
	CCfgCalc*					m_calcCascadeMax;			//叠加上限
	MagicEffServerStringRefer*	m_pCancelableMagicEff;		//可撤销魔法效果
	MagicEffServerStringRefer*	m_pDotMagicEff;				//不可撤销间隔魔法效果
	MagicEffServerStringRefer*	m_pFinalMagicEff;			//不可撤销最终魔法效果
	float						m_fDotInterval;				//不可撤销间隔魔法效果的间隔时间

};


#pragma once
#include "FightDef.h"
#include "CTriggerableState.h"
#include "CMagicEffServer.h"
#include "TCfgSharedPtr.h"

DefineCfgSharedPtr(CDamageChangeStateCfgServer);

//伤害变更魔法状态配置类
class CDamageChangeStateCfgServer
	: public CTriggerableStateCfgServer
{
	friend class CBaseStateCfgServer;
	friend class CDamageChangeStateServer;
	friend class CDamageChangeStateMgrServer; 
	friend class TOtherStateMgrServer<CDamageChangeStateCfgServer, CDamageChangeStateServer>;

	typedef map<string, CDamageChangeStateCfgServerSharedPtr*, less<string>, TConfigAllocator<pair<string, CDamageChangeStateCfgServerSharedPtr*> > >	MapDamageChangeStateCfg;
	typedef map<uint32, CDamageChangeStateCfgServerSharedPtr*, less<uint32>, TConfigAllocator<pair<uint32, CDamageChangeStateCfgServerSharedPtr*> > >	MapDamageChangeStateCfgById;
	typedef TStringRefer<CDamageChangeStateCfgServer, CMagicEffServer>	MagicEffServerStringRefer;

public:
	typedef CDamageChangeStateCfgServerSharedPtr SharedPtrType;

	CDamageChangeStateCfgServer();
	CDamageChangeStateCfgServer(const CDamageChangeStateCfgServer& cfg);
	virtual ~CDamageChangeStateCfgServer();
	static bool									LoadConfig(const TCHAR* cfgFile);	//读配置表
	static void									UnloadConfig();
	static CDamageChangeStateCfgServerSharedPtr& Get(const string& damageChangeStateName);	//获取伤害改变魔法状态配置行
	static EStateGlobalType						GetStaticType()				{return eSGT_DamageChangeState;}
	static void UpdateCfg(const string& strName);

	const string&								GetTempVar() {return m_sTempVar;}			//获取需改变的临时变量
	const CCfgCalc*								GetTempValue() {return m_calcTempValue;}	//获取要该变的值的表达式
	const CMagicEffServerSharedPtr&				GetFinalMagicEff()const;							//获取不可撤销最终魔法效
	const CMagicEffServerSharedPtr&				GetCancelableMagicEff()const;

	static CDamageChangeStateCfgServerSharedPtr ms_NULL;
private:
	static MapDamageChangeStateCfg				m_mapCfg;							//魔法状态配置映射表
	static MapDamageChangeStateCfgById			m_mapCfgById;						//用编号查找的魔法状态配置映射表

	string										m_sTempVar;							//需要改变的临时变量
	CCfgCalc*									m_calcTempValue;					//改变成表达式的值
	bool										m_bApplyTempValue;					//是否更新临时变量的值
	MagicEffServerStringRefer*					m_pFinalMagicEff;			//不可撤销最终魔法效果
	MagicEffServerStringRefer*					m_pCancelableMagicEff;		//可撤销魔法效果

};


#pragma once
#include "FightDef.h"
#include "CTriggerableState.h"
#include "CMagicEffServer.h"
#include "TCfgSharedPtr.h"

DefineCfgSharedPtr(CCumuliTriggerStateCfgServer);

//伤害变更魔法状态配置类
class CCumuliTriggerStateCfgServer
	: public CTriggerableStateCfgServer
{
	friend class CBaseStateCfgServer;
	friend class CCumuliTriggerStateServer;
	friend class CCumuliTriggerStateMgrServer; 
	friend class TOtherStateMgrServer<CCumuliTriggerStateCfgServer, CCumuliTriggerStateServer>;

	typedef map<string, CCumuliTriggerStateCfgServerSharedPtr*, less<string>, TConfigAllocator<pair<string, CCumuliTriggerStateCfgServerSharedPtr*> > >	MapCumuliTriggerStateCfg;
	typedef map<uint32, CCumuliTriggerStateCfgServerSharedPtr*, less<uint32>, TConfigAllocator<pair<uint32, CCumuliTriggerStateCfgServerSharedPtr*> > >	MapCumuliTriggerStateCfgById;
	typedef TStringRefer<CCumuliTriggerStateCfgServer, CMagicEffServer>	MagicEffServerStringRefer;

public:
	typedef CCumuliTriggerStateCfgServerSharedPtr SharedPtrType;

	CCumuliTriggerStateCfgServer();
	CCumuliTriggerStateCfgServer(const CCumuliTriggerStateCfgServer& cfg);
	virtual ~CCumuliTriggerStateCfgServer();
	static bool									LoadConfig(const TCHAR* cfgFile);	//读配置表
	static void									UnloadConfig();
	static CCumuliTriggerStateCfgServerSharedPtr& Get(const string& damageChangeStateName);	//获取伤害改变魔法状态配置行
	static EStateGlobalType						GetStaticType()				{return eSGT_CumuliTriggerState;}
	static void UpdateCfg(const string& strName);

	const string&								GetTempVar() {return m_sTempVar;}			//获取需改变的临时变量
	const CMagicEffServerSharedPtr&				GetTriggerEff()const;
	ETargetChange								GetChangedTarget()			{return m_eTargetChange;}
	const CCfgCalc*								GetMaxNumInSingleEvent()	{return m_calcMaxNumInSingleEvent;}

	static CCumuliTriggerStateCfgServerSharedPtr ms_NULL;
private:
	static MapCumuliTriggerStateCfg				m_mapCfg;							//魔法状态配置映射表
	static MapCumuliTriggerStateCfgById			m_mapCfgById;						//用编号查找的魔法状态配置映射表

	string										m_sTempVar;							//需要改变的临时变量
	MagicEffServerStringRefer*					m_pTriggerEff;						//不可撤销触发器魔法效果
	ETargetChange								m_eTargetChange;					//是否更换目标为触发者
	CCfgCalc*									m_calcMaxNumInSingleEvent;			//单次触发最大次数

};


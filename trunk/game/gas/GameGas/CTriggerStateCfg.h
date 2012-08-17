#pragma once
#include "FightDef.h"
#include "CTriggerableState.h"
#include "CMagicEffServer.h"
#include "TCfgSharedPtr.h"


DefineCfgSharedPtr(CTriggerStateCfgServer)

class CTempVarMgrServer;

//触发器魔法状态配置类
class CTriggerStateCfgServer
	: public CTriggerableStateCfgServer
{
	friend class CBaseStateCfgServer;
	friend class CTriggerStateServer;
	friend class TOtherStateMgrServer<CTriggerStateCfgServer, CTriggerStateServer>;
	friend class CTriggerStateMgrServer;

	typedef map<string, CTriggerStateCfgServerSharedPtr*, less<string>, TConfigAllocator<pair<string, CTriggerStateCfgServerSharedPtr*> > >	MapTriggerStateCfg;
	typedef map<uint32, CTriggerStateCfgServerSharedPtr*, less<uint32>, TConfigAllocator<pair<uint32, CTriggerStateCfgServerSharedPtr*> > >	MapTriggerStateCfgById;
	typedef map<string, ECascadeType, less<string>, TConfigAllocator<pair<string, ECascadeType > > > MapCascadeType;
	typedef TStringRefer<CTriggerStateCfgServer, CMagicEffServer>	MagicEffServerStringRefer;

public:
	typedef CTriggerStateCfgServerSharedPtr SharedPtrType;

	CTriggerStateCfgServer();
	CTriggerStateCfgServer(const CTriggerStateCfgServer& cfg);
	virtual ~CTriggerStateCfgServer();
	static bool									LoadConfig(const TCHAR* cfgFile);	//读配置表
	static void									UnloadConfig();
	static CTriggerStateCfgServerSharedPtr&		Get(const string& triggerStateName);	//获取触发器魔法状态配置行
	static EStateGlobalType						GetStaticType()				{return eSGT_TriggerState;}
	static void UpdateCfg(const string& strName);
	//static bool									InitMapCascadeType();				//建立群体叠加类型字符串到对应枚举类型的映射

	const CMagicEffServerSharedPtr&				GetTriggerEff()const;
	const CMagicEffServerSharedPtr&				GetCancelableMagicEff()const;
	ETargetChange								GetChangedTarget()			{return m_eTargetChange;}

	static CTriggerStateCfgServerSharedPtr ms_NULL;

private:
	static MapTriggerStateCfg					m_mapCfg;							//魔法状态配置映射表
	static MapTriggerStateCfgById				m_mapCfgById;						//用编号查找的魔法状态配置映射表
	static MapCascadeType						m_mapCascadeType;					//群体叠加类型映射表

	MagicEffServerStringRefer*					m_pTriggerEff;				//不可撤销触发器魔法效果
	MagicEffServerStringRefer*					m_pCancelableMagicEff;		//可撤销魔法效果
	ETargetChange								m_eTargetChange;			//是否更换目标为触发者

};




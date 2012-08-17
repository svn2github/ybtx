#pragma once
#include "CBaseStateCfgServer.h"
#include "TCfgSharedPtr.h"

class CCfgCalc;

class CSpecialStateBase;
template<typename CfgType, typename StateType>
class TOtherStateMgrServer;

DefineCfgSharedPtr(CSpecialStateCfgServer)

class CSpecialStateCfgServer
	: public CBaseStateCfgServer
{
	friend class CBaseStateCfgServer;
	friend class CSpecialStateBase;
	friend class CSpecialStateMgrServer;
	friend class TOtherStateMgrServer<CSpecialStateCfgServer, CSpecialStateBase>;
	typedef map<string, CSpecialStateCfgServerSharedPtr*, less<string>, TConfigAllocator<pair<string, CSpecialStateCfgServerSharedPtr*> > >	MapSpecialStateCfg;
	typedef map<uint32, CSpecialStateCfgServerSharedPtr*, less<uint32>, TConfigAllocator<pair<uint32, CSpecialStateCfgServerSharedPtr*> > >	MapSpecialStateCfgById;
	typedef map<string, ESpecialStateType, less<string>, TConfigAllocator<pair<string, ESpecialStateType > > > MapSpecialStateType;

public:
	typedef CSpecialStateCfgServerSharedPtr SharedPtrType;

	CSpecialStateCfgServer();
	CSpecialStateCfgServer(const CSpecialStateCfgServer& cfg);
	~CSpecialStateCfgServer();
	static bool									LoadConfig(const TCHAR* cfgFile);	//读配置表
	static void									UnloadConfig();
	static CSpecialStateCfgServerSharedPtr&		Get(const string& name);			//获取魔法状态配置行
	static EStateGlobalType						GetStaticType()				{return eSGT_SpecialState;}

	static CSpecialStateCfgServerSharedPtr ms_NULL;

private:
	static bool									InitMapType();						//建立特殊状态类型字符串到对应枚举类型的映射
	static MapSpecialStateCfg					m_mapCfg;							//特殊状态配置映射表
	static MapSpecialStateCfgById				m_mapCfgById;						//用编号查找的魔法状态配置映射表
	static MapSpecialStateType					m_mapSpecailStateType;				//特殊状态类型映射表
	static bool									__init;

public:

	const ESpecialStateType&	GetSSType()					{return m_eSSType;}

private:

	ESpecialStateType	m_eSSType;					//魔法状态的类型
};


#pragma once
#include "CBaseState.h"
#include "boost/shared_ptr.hpp"
#include "TConfigAllocator.h"

class CCfgCalc;

class CBaseStateCfgClient
	:public CBaseStateCfg
{
public:
	CBaseStateCfgClient(EStateGlobalType type = eSGT_Undefined);

	static bool LoadConfig();
	static void UnloadConfig();

	const TCHAR*		GetName()					{return m_sName.c_str();}
	const bool		GetDecrease()				{return m_bDecrease;}
	float			GetDotInterval()			{return m_fDotInterval;}

protected:
	string			m_sName;						//名称
	bool			m_bDecrease;					//是否是负面效果
	float			m_fDotInterval;					//不可撤销间隔魔法效果的间隔时间
};

template <class StateType>
class CTplStateCfgClient
	: public CBaseStateCfgClient
{
	friend class CBaseStateCfgClient;
public:
	typedef boost::shared_ptr<CTplStateCfgClient<StateType> >	SharedPtrType;
	typedef map<uint32, SharedPtrType*, less<uint32>, TConfigAllocator<pair<uint32, SharedPtrType*> > >							MapTplStateCfgById;
	typedef map<string, SharedPtrType*, less<string>, TConfigAllocator<pair<string, SharedPtrType*> > >							MapTplStateCfgByName;
public:
	static bool					LoadConfig(const TCHAR* cfgFile);	//读配置表
	static void					UnloadConfig();						//卸载配置表
	static SharedPtrType&		GetById(uint32 uId);				//获取魔法状态配置行
	static SharedPtrType&		GetByName(const TCHAR* name);		//获取魔法状态配置行
	static EStateGlobalType		GetStateType();
	static uint32				GetStateTypeFloor();

private:
	static MapTplStateCfgById	m_mapCfgById;						//魔法状态配置映射表
	static MapTplStateCfgByName	m_mapCfgByName;						//魔法状态配置映射表

public:
	CTplStateCfgClient();
	~CTplStateCfgClient();
	uint32			GetId()						{return m_uId;}
	const TCHAR*		GetIcon()					{return m_sIcon.c_str();}
	const TCHAR*		GetModel()					{return m_sModel.c_str();}
	const TCHAR*		GetFX();
	const bool		IsMultiCascadeFX();
	const TCHAR*		GetFX(uint32 i);
	const string&	GetModelStr()				{return m_sModel;}
	const CCfgCalc*	GetTime()					{return m_calcTime;}
	//const bool	GetCancelable()				{return m_bCancelable;}
	const bool		GetCancelable()				{return m_eIconCancelCond == eICC_All;}
	const EIconCancelCond		GetIconCancelCond()			{return m_eIconCancelCond;}

private:

	uint32			m_uId;							//编号
	EDecreaseStateType		m_eDecreaseType;		//负面魔法状态类型
	string			m_sIcon;						//对应小图标
	string			m_sModel;						//模型
	CCfgCalc*		m_calcFX;						//对应特效
	CCfgCalc*		m_calcTime;						//作用时间
	//string		m_sTime;						//作用时间
	EIconCancelCond	m_eIconCancelCond;				//取消条件


	static bool		__init;
};
	
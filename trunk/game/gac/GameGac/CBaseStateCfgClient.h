#pragma once

template <class StateType>
class CBaseStateCfgClient
{

public:
	typedef map<uint32, CBaseStateCfgClient<StateType>*>		MapBaseStateCfgById;

public:
	static bool									LoadConfig(const char* cfgFile);	//读配置表
	static void									UnloadConfig();						//卸载配置表
	static CBaseStateCfgClient<StateType>*		GetById(uint32 uId);				//获取魔法状态配置行
	static uint32								GetStateTypeFloor();

private:
	static MapBaseStateCfgById					m_mapCfgById;						//魔法状态配置映射表
public:
	const uint32&	GetId()						{return m_uId;}
	const string&	GetIcon()					{return m_sIcon;}
	const string&	GetModel()					{return m_sModel;}
	const string&	GetFX()						{return m_sFX;}

private:
	uint32			m_uId;						//编号
	string			m_sIcon;					//对应小图标
	string			m_sModel;					//模型
	string			m_sFX;						//对应特效

};
	
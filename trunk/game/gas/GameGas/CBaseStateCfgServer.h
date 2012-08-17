#pragma once
#include "CBaseState.h"
#include "CMagicStateMallocObject.h"
#include "TConfigAllocator.h"

class CCfgCalc;
class CSkillInstServer;
class CForceNoSaveStateCfgServer;

class CBaseStateCfgServer
	: public CBaseStateCfg
{
	friend class CForceNoSaveStateCfgServer;
	friend class CPersistentStateCfgServer;
public:
	CBaseStateCfgServer(EStateGlobalType type);
	CBaseStateCfgServer(const CBaseStateCfgServer& cfg);
	virtual ~CBaseStateCfgServer();

	static bool					ExistDecreaseType(const string& sTypeName)
	{
		return ms_mapDecreaseType.find(sTypeName) != ms_mapDecreaseType.end();
	}
	static EDecreaseStateType	GetDecreaseType(const string& sTypeName)
	{
		if(!ExistDecreaseType(sTypeName)) return eDST_Null;
		return ms_mapDecreaseType.find(sTypeName)->second;
	}

	uint32						GetId()						{return m_uId;}
	const TCHAR*				GetName()					{return m_sName.c_str();}
	const bool&					GetDecrease()				{return m_bDecrease;}
	const bool					GetTouchBattleState();
	const EDecreaseStateType	GetDecreaseType()			{return m_eDecreaseType;}
	const bool					GetDispellable()			{return m_bDispellable;}
	//const bool				GetCancelable()				{return m_bCancelable;}
	const EIconCancelCond		GetIconCancelCond()			{return m_eIconCancelCond;}
	const CCfgCalc*				GetTime()					{return m_calcTime;}
	//const uint32&				GetIconID()					{return m_uIconID;}
	const string&				GetModelStr()				{return m_sModel;}
	//const uint32&				GetEffectID()				{return m_uEffectID;}
	bool						GetPersistent()				{return m_bPersistent;}
	bool						GetNeedSync()				{return m_bNeedSync;}
	void						SetNeedSync();
	void						SetNeedSaveToDB(double dTime);
	bool						GetNeedSaveToDB()			{return m_eNeedSaveToDB != eFSOFNS_ForceNoSave;}
	EForceSaveOrForceNoSave		GetNeedSaveToDBType()		{return m_eNeedSaveToDB;}


	static bool LoadConfig();
	static void UnloadConfig();
	static bool					Init();
	static uint32				GetNeedSyncCount()			{return m_uNeedSyncCount;}
	static void					SetNeedSyncByName(TCHAR* sName);

protected:
	uint32						m_uId;						//编号
	string						m_sName;					//魔法状态名称
	bool						m_bDecrease;				//是否为负面魔法状态
	EDecreaseStateType			m_eDecreaseType;			//负面魔法状态类型
	bool						m_bDispellable;				//是否可被驱散或净化
	EIconCancelCond				m_eIconCancelCond;			//是否可以由客户端自己（按图标）取消
	CCfgCalc*					m_calcTime;					//作用时间
	string						m_sModel;					//模型
	EForceSaveOrForceNoSave		m_eNeedSaveToDB;			//下线是否需要保存到数据库
	bool						m_bPersistent;				//死亡后仍然保留的状
	bool						m_bNeedSync;				//需要同步

private:
	static bool					__init;
	static uint32				m_uNeedSyncCount;
};

#pragma once
#include "CTriggerState.h"
#include "CTriggerableStateMgr.h"



class CTriggerStateMgrServer
	: public TTriggerableStateMgrServer<CTriggerStateCfgServer, CTriggerStateServer>
{
public:
	CTriggerStateMgrServer(CFighterDictator* pOwner)
		: TTriggerableStateMgrServer<CTriggerStateCfgServer, CTriggerStateServer>(pOwner)
	{

	}
};

//class CAllStateMgrServer;
//
//namespace sqr
//{
//	class ICharacterMediatorCallbackHandler;
//};
//
//class CStateCondBase;
//class CStateDBDataSet;

////触发器状态管理
//class CTriggerStateMgrServer :
//	public TTriggerableStateMgrServer<CTriggerStateCfgServer, CTriggerStateServer>,
//	public CPtCORHandler
//{
//	friend class CTriggerableStateServer;
//	friend class CTriggerStateServer;
//	friend class CDamageChangeStateServer;
//
//public:
//	CTriggerStateMgrServer():m_pOwner(NULL)
//	{
//	}
//	~CTriggerStateMgrServer()
//	{
//		ClearAll();
//	}
//	void						SetOwner(CFighterDictator* pOwner) {m_pOwner = pOwner;}			//初始化目标对象指针
//	bool						SetupTriggerState(CSkillInstServer* pSkillInst, const string& name, CFighterDictator* pInstaller);		//安装魔法状态
//	bool						RestoreTriggerStateFromDB(CSkillInstServer* pSkillInst, CTriggerStateCfgServer* pCfg,
//		CFighterDictator* pInstaller, int32 iRemainTime, uint32 uAccumulate, float fProbability);	//从数据库恢复某个状态
//	CTriggerStateServer*		FindTriggerState(const string& name);						//检测是否存在名字为name的触发器状态
//	bool						ExistState(const string& name);						//检测是否存在名字为name的触发器状态
//	bool						ExistState(const string& name, const CFighterDictator* pInstaller);						//检测是否存在pInstaller安装的名字为name的触发器状态
//	//bool						EraseTriggerState(uint32 uId);	//删除某个ID的状态
//	void						ClearTriggerState(const string& name);
//	bool						ClearTriggerStateByCond(CStateCondBase* pStateCond, uint32 uId);
//	bool						SetupDamageChangeState(CSkillInstServer* pSkillInst, const string& name, CFighterDictator* pInstaller);		//安装魔法状态
//	bool						RestoreDamageChangeStateFromDB(CSkillInstServer* pSkillInst, CDamageChangeStateCfgServer* pCfg,
//		CFighterDictator* pInstaller, int32 iRemainTime, uint32 uAccumulate, float fProbability);	//从数据库恢复某个状态
//	bool						ExistState(const string& name);						//检测是否存在名字为name的触发器状态
//	bool						ExistState(const string& name, const CFighterDictator* pInstaller);						//检测是否存在pInstaller安装的名字为name的触发器状态
//	//bool						EraseDamageChangeState(uint32 uId);
//	void						ClearDamageChangeState(const string& name);
//	bool						ClearDamageChangeStateByCond(CStateCondBase* pStateCond, uint32 uId);
//	bool						MessageEvent(uint32 uEventID, CFighterDictator* pTrigger, ETriggerStateType eType = eTST_All);	
//	bool						MessageEvent(uint32 uEventID, const CPos& pTrigger, ETriggerStateType eType = eTST_All);	
//	bool						MessageEvent(uint32 uEventID, CGenericTarget* pTrigger, ETriggerStateType eType = eTST_All);	
//	//被通知事件发生
//	void						ClearAll();						//用于析构时清除所有状态
//	void						ClearAllByCond(CStateCondBase* pStateCond);
//	bool						CountSub(const string& name);	//用于从外部手动触发事件时减少一次作用次数
//	void						SyncAllState(CFighterDictator* pObserver, uint32 uNow);					//用于同步所有触发器状态和伤害变更状态
//	CAllStateMgrServer*			GetAllMgr();					//获取总魔法状态管理器
//	bool						SerializeToDB(CStateDBDataSet* pRet,
//		ICharacterMediatorCallbackHandler* pHandler, uint32 uFighterGlobalID, uint32 uNow);						//往数据库写魔法状态
//	bool						LoadFromDB(ICharacterMediatorCallbackHandler* pHandler, uint32 uFighterGlobalID);					//从数据库读取魔法状态
//
//private:
//	MapTriggerState				m_mapTS;						//触发器状态种类到触发器状态对象的映射表
//	MapDamageChangeState		m_mapDS;						//伤害变更状态种类到伤害变更状态对象的映射表
//	MapTSEventBandle			m_mapBSEB;						//事件类型到状态对象指针束的映射表
//	CFighterDictator*			m_pOwner;						//目标所属对象，即本对象的拥有者
//
//};

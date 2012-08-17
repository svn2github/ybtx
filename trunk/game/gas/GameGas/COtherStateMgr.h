#pragma once
#include "TMagicStateAllocator.h"
#include "CSpecialStateCfg.h"
#include "CTriggerStateCfg.h"
#include "CDamageChangeStateCfg.h"
#include "CCumuliTriggerStateCfg.h"

class CFighterDictator;
class CSkillInstServer;
class CGenericTarget;
class CTriggerStateCfgServer;
class CDamageChangeStateCfgServer;
class CCumuliTriggerStateCfgServer;
class CSpecialStateCfgServer;
class CTriggerStateServer;
class CDamageChangeStateServer;
class CCumuliTriggerStateServer;
class CSpecialStateBase;
template<typename CfgType, typename StateType>
class TOtherStateMgrServer;
template<typename CfgType, typename StateType>
class TTriggerableStateMgrServer;


//触发器状态的事件类型束
template<typename StateType>
class TStateBundleByEvent
	: public CMagicStateMallocObject
{
	friend class CTriggerStateServer;
	friend class CDamageChangeStateServer;
	friend class CCumuliTriggerStateServer;
	friend class CSpecialStateBase;
	friend class CTriggerStateMgrServer;
	friend class TTriggerableStateMgrServer<CTriggerStateCfgServer, CTriggerStateServer>;
	friend class TTriggerableStateMgrServer<CDamageChangeStateCfgServer, CDamageChangeStateServer>;
	friend class TTriggerableStateMgrServer<CCumuliTriggerStateCfgServer, CCumuliTriggerStateServer>;
	friend class TOtherStateMgrServer<CTriggerStateCfgServer, CTriggerStateServer>;
	friend class TOtherStateMgrServer<CDamageChangeStateCfgServer, CDamageChangeStateServer>;
	friend class TOtherStateMgrServer<CCumuliTriggerStateCfgServer, CCumuliTriggerStateServer>;
	friend class TOtherStateMgrServer<CSpecialStateCfgServer, CSpecialStateBase>;

	typedef list<StateType*, TMagicStateAllocator<StateType*> >		ListOtherState;

private:
	TStateBundleByEvent() {}
	ListOtherState				m_listState;				//事件类型束里的
};

class CAllStateMgrServer;
class ICharacterDictatorCallbackHandler;
class CStateCondBase;
class CStateDBDataSet;
class CStateDBData;

template<typename CfgType, typename StateType>
class TOtherStateMgrServer
	: public CMagicStateMallocObject
{
	friend class CTriggerableStateServer;
	friend class CTriggerStateServer;
	friend class CDamageChangeStateServer;
	friend class CCumuliTriggerStateServer;
	friend class CSpecialStateBase;
	typedef typename CfgType::SharedPtrType SharedPtrType;

protected:
	typedef map<CfgType*, StateType*, less<CfgType*>, TMagicStateAllocator<pair<CfgType*, StateType*> > >
		MapState;
	typedef map<uint32, TStateBundleByEvent<StateType>, less<uint32>,
		TMagicStateAllocator<pair<uint32, TStateBundleByEvent<StateType> > > >	MapStateBundleByEvent;
	typedef typename TStateBundleByEvent<StateType>::ListOtherState				ListBundleByEventState;

public:

	TOtherStateMgrServer(CFighterDictator* pOwner):m_pOwner(pOwner)
	{

	}
	virtual ~TOtherStateMgrServer()
	{

	}

	CFighterDictator*			GetOwner() {return m_pOwner;}
	void						SetOwner(CFighterDictator* pOwner) {m_pOwner = pOwner;}			//初始化目标对象指针
	CAllStateMgrServer*			GetAllMgr();					//获取总魔法状态管理器
	StateType*					FindState(const string& name);						//检测是否存在名字为name的状态
	bool						ExistState(const string& name);						//检测是否存在名字为name的状态
	bool						ExistState(const SharedPtrType& pCfg);						//检测是否存在名字为name的状态
	bool						ExistState(const SharedPtrType& pCfg, const CFighterDictator* pInstaller);						//检测是否存在pInstaller安装的名字为name的状态
	int32						StateLeftTime(const string& name);					//获取名字为name的状态剩余时间
	void						ClearState(const SharedPtrType& pCfg);
	bool						ClearStateByCond(CStateCondBase* pStateCond, uint32 uId);
	virtual void				ClearAll() = 0;					//用于析构时清除所有状态
	bool						ExistStateByCond(CStateCondBase* pStateCond);
	void						ClearAllByCond(CStateCondBase* pStateCond);
	void						SyncAllState(CFighterDictator* pObserver, uint64 uNow);					//用于同步所有其它状态
	bool						SerializeToDB(CStateDBDataSet* pRet,
		ICharacterDictatorCallbackHandler* pHandler, uint32 uFighterGlobalID, uint64 uNow, uint32 uGrade);						//往数据库写魔法状态
	bool						LoadFromDB(ICharacterDictatorCallbackHandler* pHandler, uint32 uFighterGlobalID);					//从数据库读取魔法状态
	virtual CStateDBData*		CreateStateDBData(StateType* pState, int32 iRemainTime, CSkillInstServer* pInst) = 0;
	virtual bool				RestoreStateFromDB(CSkillInstServer* pSkillInst, const SharedPtrType&,
		CFighterDictator* pInstaller, int32 iTime, int32 iRemainTime, uint32 uAccumulate, float fProbability) = 0;	//从数据库恢复某个状态
	bool						MessageEvent(uint32 uEventID, CGenericTarget* pTrigger);
	void						RegisterEvent(StateType* pState, uint32 uTriggerEvent);


protected:
	MapState					m_mapState;
	CFighterDictator*			m_pOwner;						//目标所属对象，即本对象的拥有者
	MapStateBundleByEvent		m_mapStateBundleByEvent;
};


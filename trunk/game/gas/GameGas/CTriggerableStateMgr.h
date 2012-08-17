#pragma once
#include "CTriggerableState.h"
#include "COtherStateMgr.h"

template<typename CfgType, typename StateType>
class TTriggerableStateMgrServer
	: public TOtherStateMgrServer<CfgType, StateType>
{
	typedef typename CfgType::SharedPtrType SharedPtrType;
public:
	typedef typename TOtherStateMgrServer<CfgType, StateType>::MapState MapState;
	TTriggerableStateMgrServer(CFighterDictator* pOwner)
		: TOtherStateMgrServer<CfgType, StateType>(pOwner)
	{

	}
	virtual ~TTriggerableStateMgrServer()
	{
		ClearAll();
	}
	virtual void				ClearAll();						//用于析构时清除所有状态
	bool						SetupState(CSkillInstServer* pSkillInst, const SharedPtrType& pCfg, CFighterDictator* pInstaller);		//安装魔法状态
	virtual CStateDBData*		CreateStateDBData(StateType* pState, int32 iRemainTime, CSkillInstServer* pInst);
	virtual bool				RestoreStateFromDB(CSkillInstServer* pSkillInst, const SharedPtrType& pCfg,
		CFighterDictator* pInstaller, int32 iTime, int32 iRemainTime, uint32 uAccumulate, float fProbability);	//从数据库恢复某个状态
	uint32						TriggerCount(const string& name);					//获取名字为name的状态剩余时间
	bool						SetAccumulate(const string& name, uint32 uValue);
};

#pragma once
#include "CTriggerableState.h"
#include "TMagicStateAllocator.h"

class CTriggerStateMgrServer;
class CFighterDictator;
class CTriggerStateServer;
class CDamageChangeStateServer;
class CSkillInstServer;
class CGenericTarget;
class CTriggerStateCfgServerSharedPtr;

typedef list<CTriggerStateServer*, TMagicStateAllocator<CTriggerStateServer*> >									ListPTriggerState;
typedef map<CTriggerStateCfgServer*, CTriggerStateServer*, less<CTriggerStateCfgServer*>,
	TMagicStateAllocator<pair<CTriggerStateCfgServer*, CTriggerStateServer*> > >
	MapTriggerState;

template<typename CfgType, typename StateType>
class TTriggerableStateMgrServer;
template<typename StateType>
class TStateBundleByEvent;

//触发器状态
class CTriggerStateServer
	: public CTriggerableStateServer
{
friend class TOtherStateMgrServer<CTriggerStateCfgServer, CTriggerStateServer>;
friend class TTriggerableStateMgrServer<CTriggerStateCfgServer, CTriggerStateServer>;
friend class CTriggerStateMgrServer;

public:
	virtual CBaseStateCfgServer* GetCfg();
	CTriggerStateCfgServerSharedPtr& GetCfgSharedPtr()const;
	virtual CFighterDictator*	GetOwner();								//获取拥有者
	virtual CAllStateMgrServer*	GetAllMgr();
	virtual pair<bool, bool>	MessageEvent(uint32 uEventID, CGenericTarget * pTrigger);	//被通知事件发生
	~CTriggerStateServer();

private:
	CTriggerStateServer(CSkillInstServer *pSkillInst, CFighterDictator* pInstaller,
		TTriggerableStateMgrServer<CTriggerStateCfgServer, CTriggerStateServer>* pMgr, 
		const CTriggerStateCfgServerSharedPtr& pCfg, uint32 uAccumulate = 0, int32 iTime = 0, int32 iRemainTime = 0, float fProbability = 0.0f);

	virtual void				DeleteSelf();				//删除自己和管理器中自己的指针
	virtual void				DeleteSelfExceptItr();		//删除自己但不删除管理器中自己的指针
	void						PrepareDeleteSelf();		//删除自己前的所有工作
	void						DeleteItr();				//删除管理器中自己的指针
	virtual bool				DoCancelableMagicEff(bool bFromDB);		//执行可撤销魔法效果
	virtual void				CancelCancelableMagicEff();	//撤销可撤销魔法效果

	CTriggerStateMgrServer*		m_pMgr;				//所属的魔法状态管理类，即本对象的容器
	TStateBundleByEvent<CTriggerStateServer>*	m_pStateEventBundle;					//所在的事件类型束EventBandle指针
	MapTriggerState::iterator	m_mapStateItr;				//自己在状态管理TriggerState列表的位置
	ListPTriggerState::iterator	m_listStateItr;				//自己在事件类型束EventBandle映射中的位置
	CTriggerStateCfgServerSharedPtr* m_pCfg;
};




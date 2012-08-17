#pragma once
#include "CTriggerableState.h"
#include "TMagicStateAllocator.h"

class CCumuliTriggerStateServer;
class CCumuliTriggerStateMgrServer;
class CCumuliTriggerStateCfgServerSharedPtr;

typedef list<CCumuliTriggerStateServer*, TMagicStateAllocator<CCumuliTriggerStateServer*> >
ListPCumuliTriggerState;
typedef map<CCumuliTriggerStateCfgServer*, CCumuliTriggerStateServer*, less<CCumuliTriggerStateCfgServer*>,
TMagicStateAllocator<pair<CCumuliTriggerStateCfgServer*, CCumuliTriggerStateServer*> > >
MapCumuliTriggerState;





template<typename CfgType, typename StateType>
class TTriggerableStateMgrServer;
template<typename StateType>
class TStateBundleByEvent;

//伤害改变状态
class CCumuliTriggerStateServer
	: public CTriggerableStateServer
{
	friend class TOtherStateMgrServer<CCumuliTriggerStateCfgServer, CCumuliTriggerStateServer>;
	friend class TTriggerableStateMgrServer<CCumuliTriggerStateCfgServer, CCumuliTriggerStateServer>;
	friend class CCumuliTriggerStateMgrServer;

public:
	virtual CBaseStateCfgServer*	GetCfg();
	CCumuliTriggerStateCfgServerSharedPtr& GetCfgSharedPtr()const;
	virtual CFighterDictator*	GetOwner();								//获取拥有者
	virtual CAllStateMgrServer*	GetAllMgr();

	~CCumuliTriggerStateServer();

private:
	CCumuliTriggerStateServer(CSkillInstServer *pSkillInst, CFighterDictator* pInstaller,
		TTriggerableStateMgrServer<CCumuliTriggerStateCfgServer, CCumuliTriggerStateServer>* pMgr, 
		const CCumuliTriggerStateCfgServerSharedPtr& pCfg, uint32 uAccumulate = 0, int32 iTime = 0, int32 iRemainTime = 0, float fProbability = 0.0f);

	virtual void				DeleteSelf();				//删除自己和管理器中自己的指针
	virtual void				DeleteSelfExceptItr();		//删除自己但不删除管理器中自己的指针
	void						PrepareDeleteSelf();		//删除自己前的所有工作
	void						DeleteItr();				//删除管理器中自己的指针
	virtual pair<bool, bool>	MessageEvent(uint32 uEventID, CGenericTarget * pTrigger);	//被通知事件发生
	virtual void				OnCOREvent(CPtCORSubject* pSubject,uint32 uEvent,void* pArg);
	virtual void				ChangeInstaller(CFighterDictator* pInstaller);

	CCumuliTriggerStateMgrServer*		m_pMgr;				//所属的魔法状态管理类，即本对象的容器
	TStateBundleByEvent<CCumuliTriggerStateServer>*				m_pStateEventBundle;//所在的事件类型束EventBandle指针
	MapCumuliTriggerState::iterator		m_mapStateItr;		//自己在状态管理CumuliTriggerState列表的位置
	ListPCumuliTriggerState::iterator	m_listStateItr;		//自己在事件类型束EventBandle映射中的位置
	CCumuliTriggerStateCfgServerSharedPtr*	m_pCfg;			//魔法状态配置表对象指针
};


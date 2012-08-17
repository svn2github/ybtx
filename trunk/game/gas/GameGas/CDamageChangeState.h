#pragma once
#include "CTriggerableState.h"
#include "TMagicStateAllocator.h"

class CDamageChangeStateServer;
class CDamageChangeStateMgrServer;
class CDamageChangeStateCfgServerSharedPtr;

typedef list<CDamageChangeStateServer*, TMagicStateAllocator<CDamageChangeStateServer*> >
	ListPDamageChangeState;
typedef map<CDamageChangeStateCfgServer*, CDamageChangeStateServer*, less<CDamageChangeStateCfgServer*>,
	TMagicStateAllocator<pair<CDamageChangeStateCfgServer*, CDamageChangeStateServer*> > >
	MapDamageChangeState;





template<typename CfgType, typename StateType>
class TTriggerableStateMgrServer;
template<typename StateType>
class TStateBundleByEvent;

//伤害改变状态
class CDamageChangeStateServer
	: public CTriggerableStateServer
{
	friend class TOtherStateMgrServer<CDamageChangeStateCfgServer, CDamageChangeStateServer>;
	friend class TTriggerableStateMgrServer<CDamageChangeStateCfgServer, CDamageChangeStateServer>;
	friend class CDamageChangeStateMgrServer;

public:
	virtual CBaseStateCfgServer*	GetCfg();
	CDamageChangeStateCfgServerSharedPtr& GetCfgSharedPtr()const;
	virtual CFighterDictator*	GetOwner();								//获取拥有者
	virtual CAllStateMgrServer*	GetAllMgr();

	void						AddValue(const uint32& uAccumulate) {m_uAccumulate += uAccumulate;}
	const uint32				GetValue() {return m_uAccumulate;}
	~CDamageChangeStateServer();

private:
	CDamageChangeStateServer(CSkillInstServer *pSkillInst, CFighterDictator* pInstaller,
		TTriggerableStateMgrServer<CDamageChangeStateCfgServer, CDamageChangeStateServer>* pMgr, 
		const CDamageChangeStateCfgServerSharedPtr& pCfg, uint32 uAccumulate = 0, int32 iTime = 0, int32 iRemainTime = 0, float fProbability = 0.0f);

	virtual bool				DoCancelableMagicEff(bool bFromDB);		//执行可撤销魔法效果
	virtual bool				DoFinalMagicEff();			//执行最终魔法效果
	virtual void				CancelCancelableMagicEff();	//撤销可撤销魔法效果
	virtual void				DeleteSelf();				//删除自己和管理器中自己的指针
	virtual void				DeleteSelfExceptItr();		//删除自己但不删除管理器中自己的指针
	void						PrepareDeleteSelf();		//删除自己前的所有工作
	void						DeleteItr();				//删除管理器中自己的指针
	virtual pair<bool, bool>	MessageEvent(uint32 uEventID, CGenericTarget * pTrigger);	//被通知事件发生

	CDamageChangeStateMgrServer*		m_pMgr;				//所属的魔法状态管理类，即本对象的容器
	TStateBundleByEvent<CDamageChangeStateServer>*				m_pStateEventBundle;//所在的事件类型束EventBandle指针
	MapDamageChangeState::iterator		m_mapStateItr;		//自己在状态管理DamageChangeState列表的位置
	ListPDamageChangeState::iterator	m_listStateItr;		//自己在事件类型束EventBandle映射中的位置
	CDamageChangeStateCfgServerSharedPtr*	m_pCfg;			//魔法状态配置表对象指针
};


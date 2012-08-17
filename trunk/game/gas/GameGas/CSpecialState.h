#pragma once
#include "CBaseStateServer.h"
#include "TMagicStateAllocator.h"

class CSpecialStateBase;
class CSpecialStateCfgServer;
class CSpecialStateCfgServerSharedPtr;

typedef list<CSpecialStateBase*, TMagicStateAllocator<CSpecialStateBase*> >
	ListPSpecialState;
typedef map<CSpecialStateCfgServer*, CSpecialStateBase*, less<CSpecialStateCfgServer*>,
	TMagicStateAllocator<pair<CSpecialStateCfgServer*, CSpecialStateBase*> > >	MapSpecialState;

class CSpecialStateMgrServer;
class CFighterDictator;
class CSkillInstServer;
class CSpecialStateTypeBundle;
class CAllStateMgrServer;

template<typename CfgType, typename StateType>
class TOtherStateMgrServer;
template<typename StateType>
class TStateBundleByEvent;

class CSpecialStateBase
	: public CBaseStateServer
{
	friend class TOtherStateMgrServer<CSpecialStateCfgServer, CSpecialStateBase>;
	friend class CSpecialStateMgrServer;
	friend class CAllStateMgrServer;

public:
	void							OnTick();				//计时器消息响应函数
	virtual CFighterDictator*		GetOwner();				//获取拥有者
	virtual CBaseStateCfgServer*	GetCfg();
	CSpecialStateCfgServerSharedPtr& GetCfgSharedPtr()const;

	CAllStateMgrServer*				GetAllMgr();

	CSpecialStateBase(CSkillInstServer* pSkillInst, CFighterDictator* pInstaller, CSpecialStateMgrServer* pMgr, 
		const CSpecialStateCfgServerSharedPtr& pCfg, int32 iTime = 0, int32 iRemainTime = 0);
	virtual ~CSpecialStateBase();

protected:
	void						Start(bool bFromDB = false);	//一加入map就执行这个函数
	void						StartDo();					//用于每次魔法状态更新一开始就执行效果
	void						StartTime(bool bFromDB = false);//开始计时器计时
	void						CancelDo() {}
	virtual void				DeleteSelf();				//删除自己和管理器中自己的指针
	virtual void				DeleteSelfExceptItr();		//删除自己但不删除管理器中自己的指针
	void						PrepareDeleteSelf();		//删除自己前的所有工作
	void						DeleteItr();				//删除管理器中自己的指针
	virtual pair<bool, bool>	MessageEvent(uint32 uEventID, CGenericTarget * pTrigger) {return make_pair(false,false);}	//被通知事件发生
	virtual void				RegisterEvent();
	//uint32						GetDynamicId();

	MapSpecialState::iterator	m_mapSSItr;			//自己在状态管理SpecialState列表的位置
	CSpecialStateMgrServer*		m_pMgr;				//所属的特殊魔法状态管理类，即本对象的容器
	CSpecialStateCfgServerSharedPtr*		m_pCfg;				//特殊魔法状态配置表对象指针
	CSpecialStateTypeBundle*	m_pStateBundleByType;						//所在的类型束SpecialStateTypeBundle指针
	ListPSpecialState::iterator	m_listSSItr;		//自己在状态类型束TypeBandle映射中的位置
	TStateBundleByEvent<CSpecialStateBase>*			m_pStateEventBundle;	//所在的事件类型束EventBandle指针
	ListPSpecialState::iterator				m_listStateItr;			//自己在事件类型束EventBandle映射中的位置
};





//反射魔法状态
class CReflectStateServer
	: CSpecialStateBase
{
	CReflectStateServer(CSkillInstServer *pSkillInst, CFighterDictator* pInstaller, CSpecialStateMgrServer* pMgr, 
		const CSpecialStateCfgServerSharedPtr& pCfg, int32 iTime, int32 iRemainTime = 0);
	virtual ~CReflectStateServer()					{};
	friend class CSpecialStateMgrServer;
private:
};

//重放魔法状态
class CRepeatStateServer
	: CSpecialStateBase
{
	CRepeatStateServer(CSkillInstServer *pSkillInst, CFighterDictator* pInstaller, CSpecialStateMgrServer* pMgr, 
		const CSpecialStateCfgServerSharedPtr& pCfg, int32 iTime, int32 iRemainTime = 0);
	virtual ~CRepeatStateServer()					{};
	friend class CSpecialStateMgrServer;
private:
};


//死尸状态
class CDeadBodyStateServer
	: CSpecialStateBase
{
	CDeadBodyStateServer(CSkillInstServer *pSkillInst, CFighterDictator* pInstaller, CSpecialStateMgrServer* pMgr, 
		const CSpecialStateCfgServerSharedPtr& pCfg, int32 iTime, int32 iRemainTime = 0);
	virtual ~CDeadBodyStateServer()					{};
	friend class CSpecialStateMgrServer;
private:
};


#pragma once
#include "CBaseStateServer.h"
#include "CTriggerableStateCfg.h"
#include "PatternCOR.h"

class CGenericTarget;
class CAllStateMgrServer;
class CTriggerStateCfgServer;
class CTriggerStateServer;
class CDamageChangeStateCfgServer;
class CDamageChangeStateServer;
class CCumuliTriggerStateCfgServer;
class CCumuliTriggerStateServer;
template<typename CfgType, typename StateType>
class TOtherStateMgrServer;
template<typename CfgType, typename StateType>
class TTriggerableStateMgrServer;
class CMagicEffDataMgrServer;

class CTriggerableStateServer
	: public CBaseStateServer
	, public CPtCORHandler
{
	friend class CTriggerStateMgrServer;
	friend class TOtherStateMgrServer<CTriggerStateCfgServer, CTriggerStateServer>;
	friend class TOtherStateMgrServer<CDamageChangeStateCfgServer, CDamageChangeStateServer>;
	friend class TOtherStateMgrServer<CCumuliTriggerStateCfgServer, CCumuliTriggerStateServer>;
	friend class TTriggerableStateMgrServer<CTriggerStateCfgServer, CTriggerStateServer>;
	friend class TTriggerableStateMgrServer<CDamageChangeStateCfgServer, CDamageChangeStateServer>;
	friend class TTriggerableStateMgrServer<CCumuliTriggerStateCfgServer, CCumuliTriggerStateServer>;
	friend class CAllStateMgrServer;

public:
	void							OnTick();								//计时器消息响应函数
	virtual CFighterDictator*		GetOwner() = 0;							//获取拥有者
	virtual CAllStateMgrServer*		GetAllMgr() = 0;
	virtual CBaseStateCfgServer*	GetCfg() {return NULL;}				//获取配置表对象
	const uint32				GetValue() {return m_uAccumulate;}

protected:
	CTriggerableStateServer(CSkillInstServer *pSkillInst, CFighterDictator* pInstaller,
		uint32 uAccumulate, int32 iTime = 0, int32 iRemainTime = 0, float fProbability = 0.0f);
	virtual ~CTriggerableStateServer();

	//void					Start(bool bFromDB = false);			//一加入map就执行这个函数
	void					StartDo(bool bFromDB = false);			//用于每次魔法状态更新一开始就执行效果
	void					CancelDo();								//撤销时执行的效果
	void					StartTime(bool bFromDB = false);		//开始计时器计时
	void					RefreshTime();							//刷新时间
	virtual void			OnCOREvent(CPtCORSubject* pSubject,uint32 uEvent,void* pArg);
	void					SetInstaller(CFighterDictator* pInstaller);	//设置施法者
	virtual void			ChangeInstaller(CFighterDictator* pInstaller);//改变施法者
	void					ClearInstaller();							//清空施法者
	pair<bool, bool>		MessageEvent(uint32 uEventID, CFighterDictator * pTrigger);	//被通知事件发生

	virtual bool			Replace(CSkillInstServer *pSkillInst, CFighterDictator* pInstaller);			//替换操作
	virtual void			DeleteSelf() = 0;						//删除自己和管理器中自己的指针
	virtual void			DeleteSelfExceptItr() = 0;				//删除自己但不删除管理器中自己的指针
	virtual bool			DoCancelableMagicEff(bool bFromDB) {return false;}				//执行可撤销魔法效果
	virtual bool			DoFinalMagicEff() { return false; }					//执行最终魔法效果
	virtual void			CancelCancelableMagicEff() {}			//撤销可撤销魔法效果
	virtual pair<bool, bool>	MessageEvent(uint32 uEventID, CGenericTarget * pTrigger) = 0;	//被通知事件发生
	void					CreateInst(CSkillInstServer* pSkillInst);
	void					DeleteInst();
	//uint32					GetDynamicId();
	void					SetAccumulate(uint32 uValue);

	uint32					m_uGrade;			//魔法状态对应技能的等级

	uint32					m_uAccumulate;		//作用次数或伤害数值累计值
	int32					m_iInitialValue;	//作用次数或伤害数值初始值
	float					m_fProbability;		//作用几率
	bool					m_bEndByCount;		//表示由触发达到上限而结束
	bool					m_bEndByTime;		//表示由时间达到上限而结束
	bool					m_bCancellationDone;		//表示撤销已执行
	bool					m_bTriggering;				//同步触发中
	CMagicEffDataMgrServer* m_pCancelEffData;
	//CMagicStateCascadeDataMgr*	m_pCancelableInst;		//用于存储可撤销魔法效果中间对象的MagicStateInst

};


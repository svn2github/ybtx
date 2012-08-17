#pragma once
#include "PatternCOR.h"
#include "CBaseStateServer.h"
#include "CMagicStateCfg.h"
#include "TMagicStateAllocator.h"

class CSkillInstServer;
class CMagicStateMgrServer;
class CMagicStateCategoryServer;
class CFighterDictator;
class CMagicStateServer;
class CMagicStateCascadeDataMgr;
class CAllStateMgrServer;

typedef multimap<const uint32, CMagicStateServer*, less<uint32>,
	TMagicStateAllocator<pair<const uint32, CMagicStateServer*> > >		MultiMapMagicState;
typedef map<CMagicStateCfgServer*, CMagicStateCategoryServer*,
	less<CMagicStateCfgServer*>, TMagicStateAllocator<pair<CMagicStateCfgServer*, CMagicStateCategoryServer*> > >
	MapMagicStateCategory;
typedef map<uint32, CMagicStateServer*, less<uint32>, 
	TMagicStateAllocator<pair<uint32, CMagicStateServer> > >				MapMagicStateByDynamicId;

typedef pair<MultiMapMagicState::const_iterator, MultiMapMagicState::const_iterator> MtMapMagicStatePairItr;
//typedef set<CFighterDictator*>								SetStateTarget;

//魔法状态，代表叠加的一个魔法状态
class CMagicStateServer
	: public CBaseStateServer
{
friend class CMagicStateCategoryServer;
friend class CMagicStateMgrServer;
friend class CAllStateMgrServer;

public:
	~CMagicStateServer();
	void							OnTick();								//计时器消息响应函数
	virtual CFighterDictator*		GetOwner();								//获取拥有者
	virtual CBaseStateCfgServer*	GetCfg();
	CMagicStateCfgServerSharedPtr&	GetCfgSharedPtr();
	CAllStateMgrServer*				GetAllMgr();

private:

	CMagicStateServer(CSkillInstServer* pSkillInst, CFighterDictator* pInstaller, CMagicStateCategoryServer* pMSCategory);
							//正常安装的构造
	CMagicStateServer(CSkillInstServer* pSkillInst, CMagicStateCascadeDataMgr* pCancelableDataMgr, CMagicStateCascadeDataMgr* pDotDataMgr, CMagicStateCascadeDataMgr* pFinalDataMgr, 
		uint32 uCount, CFighterDictator* pInstaller, CMagicStateCategoryServer* pMSCategory, int32 iTime, int32 iRemainTime);
							//从数据库恢复的构造
	void					CreateInst(CSkillInstServer* pSkillInst, bool bFromDB = false);
	void					DeleteInst();

	pair<bool, bool>		Cascade(CSkillInstServer* pSkillInst, const uint32& grade, CFighterDictator* pInstaller);			//叠加操作
	pair<bool, bool>		Replace(CSkillInstServer* pSkillInst, const uint32& grade, CFighterDictator* pInstaller);			//替换操作
	bool					CascadeFull();							//叠加是否到顶
	//bool					Start(bool bFromDB = false);			//一加入map就执行这个函数
	bool					StartDo();								//用于每次魔法状态更新一开始就执行效果
	bool					StartDoFromDB();						//用于从数据库恢复状态是执行效果
	bool					EndDo();								//用于结束魔法状态时执行效果
	bool					CancelDo(bool bIgnoreSync = false);		//用于取消魔法状态（状态需要随后删除）
	bool					CancelDo(uint32 grade);					//用于按层数取消魔法状态（状态不删除）
	void					StartTime(bool bTimeSet = false);		//开始计时器计时
	void					RefreshTime();							//刷新时间
	bool					TickLengthStrategy();					//计时策略是长度方式，否则为间隔方式
	bool					TickEnd();								//计时是否结束
	bool					DeleteSelf();							//删除自己。true表示删了整个Category，false表示所在链还有别的结点
	void					PrepareDeleteSelf();					//删除自己前的所有工作
	void					SetInstaller(CFighterDictator* pInstaller);	//设置施法者，包含Attach所关心的事件的操作
	void					ChangeInstaller(CFighterDictator* pInstaller);
	void					DetachInstaller();
	void					CalculateMagicOpArg(CFighterDictator* pInstaller);	//在安装魔法状态时计算出该状态中的所有效果中的各个魔法操作中的参数
	pair<bool,bool>			CancelCascade(uint32& count, bool bInCancel);			//删除count层魔法状态
	bool					ResetTime();							//重置时间（不执行任何效果）
	bool					AddTime(int32 iTime);					//增加时间
	int32					GetCalcValue();
	//uint32					GetDynamicId()		{return m_uDynamicId;}

	//uint32					m_uDynamicId;		//魔法状态动态编号（区别于魔法状态配置表对象的静态编号）
	float					m_fTickCount;		//计数时间
	float					m_fTickStep;		//步进时间
	uint32					m_uCount;			//魔法效果叠加次数（作用值计数）
	uint32					m_uShareCount;		//共享叠加的安装总数
	uint32					m_uGrade;			//魔法状态对应技能的等级
	MultiMapMagicState::iterator	m_mtmapMSItr;	//自己在本魔法状态种类的map的位置
	CMagicStateCategoryServer*	m_pMSCategory;		//所属的魔法种类，即本魔法状态的直接容器，用于获取m_uGrade

	int32					m_iCascadeMax;		//叠加上限
	CSkillInstServer*				m_pCancelableInst;	//用于存储可撤销魔法效果中间对象的MagicStateInst
	CSkillInstServer*				m_pDotInst;			//用于存储间隔魔法效果中间对象的MagicStateInst
	CSkillInstServer*				m_pFinalInst;		//用于存储最终魔法效果中间对象的MagicStateInst
	CMagicStateCascadeDataMgr*		m_pCancelableDataMgr;
	CMagicStateCascadeDataMgr*		m_pDotDataMgr;
	CMagicStateCascadeDataMgr*		m_pFinalDataMgr;
	bool					m_bCancellationDone;		//表示撤销已执行
};



#pragma once
#include "CMagicState.h"

class CMagicEffServer;
class CMagicStateCfgServer;
class CMagicStateCfgServerSharedPtr;


//魔法状态种类，代表不同Fighter叠加的多个同种魔法状态
class CMagicStateCategoryServer
	: public CMagicStateMallocObject
{
	friend class CMagicStateServer;
	friend class CMagicStateMgrServer;
public:
	CMagicStateCategoryServer(const CMagicStateCfgServerSharedPtr& pCfg, CMagicStateMgrServer* pMgr);
	~CMagicStateCategoryServer();

	CMagicStateCfgServerSharedPtr& GetCfg()const;

private:
	pair<bool, MultiMapMagicState::iterator> AddMS(CSkillInstServer* pSkillInst, CFighterDictator* pInstaller);			//用于增加一个魔法状态元素到本魔法状态种类的map
	pair<bool, MultiMapMagicState::iterator> AddMSFromDB(CSkillInstServer* pSkillInst, CMagicStateCascadeDataMgr* pCancelableDataMgr,
		CMagicStateCascadeDataMgr* pDotDataMgr, CMagicStateCascadeDataMgr* pFinaDataMgr, CFighterDictator* pInstaller, 
		uint32 uCount, int32 iTime, int32 iRemainTime);	//用于从数据库恢复一个魔法状态元素到本魔法状态种类的map
	MultiMapMagicState		m_mtmapMS;	//不同人的魔法状态的映射表
	CMagicStateCfgServerSharedPtr*	m_pCfg;		//魔法状态配置表对象指针
	CMagicStateMgrServer*	m_pMgr;		//所属的魔法状态管理类，即本对象的容器
};

class CSkillInstServer;

class CAllStateMgrServer;
class ICharacterDictatorCallbackHandler;
class CStateCondBase;
class CStateDBDataSet;
class CRivalStateMgr;

//魔法状态管理
//每个CFighterDictator需要定义一个这个成员对象。在游戏开始，创建人物时初始化。
//需要参数为CFighterDictator的指针，即目标。
class CMagicStateMgrServer 
	: public CPtCORHandler
	, public CMagicStateMallocObject
{
	friend class CMagicStateServer;
	friend class CMagicStateCategoryServer;

public:
	CMagicStateMgrServer(CFighterDictator* pOwner);
	~CMagicStateMgrServer();
	//void						SetOwner(CFighterDictator* pOwner);				//初始化目标对象指针
	void						UnloadFighter();
	bool						IgnoreImmuneSetupState(CSkillInstServer* pSkillInst, const CMagicStateCfgServerSharedPtr& pCfg, CFighterDictator* pInstaller);					// 无视免疫安装魔法状态
	bool						SetupState(CSkillInstServer* pSkillInst, const CMagicStateCfgServerSharedPtr& pCfg, CFighterDictator* pInstaller);		//安装魔法状态
	bool						ExistState(const string& name);
	bool						ExistState(const CMagicStateCfgServerSharedPtr& pCfg);
	bool						ExistState(const CMagicStateCfgServerSharedPtr& pCfg, const CFighterDictator *pInstaller);
	//bool						ExistState(uint32 DynamicId);
	uint32						MagicStateCount(const string& name);
	uint32						MagicStateCountOfInstaller(const string& name, const CFighterDictator* pInstaller);
	int32						StateLeftTime(const string& name, const CFighterDictator* pInstaller);
	void						ClearState(const CMagicStateCfgServerSharedPtr& pCfg);				//删除一个魔法状态（强制）
	//void						ClearState(const string& name, CFighterDictator* pInstaller);					//删除一个魔法状态（强制）
	void						InstallerOffline(CFighterDictator * pInstaller);			//某个施法者离线时调用这个函数
	virtual void				OnCOREvent(CPtCORSubject* pSubject, uint32 uEvent,void* pArg);
	void						ClearAll();						//用于死亡时清除所有状态，只能在删除Fighter对象时使用（強制）
	bool						ExistStateByCond(CStateCondBase* pStateCond);
	void						ClearAllByCond(CStateCondBase * pStateCond);
	bool						ClearStateByCond(CStateCondBase* pStateCond, uint32 uId);
	void						ReflectAllByCond(CStateCondBase * pStateCond);
	//bool						ClearStateByCondAndDynamicId(CStateCondBase* pStateCond, uint32 uDynamicId);
	void						SyncAllState(CFighterDictator* pObserver, uint64 uNow);					//用于重置客户端所有状态
	CAllStateMgrServer*			GetAllMgr();					//获取总魔法状态管理器
	bool						CanDecStateCascade(const CMagicStateCfgServerSharedPtr& pCfg, uint32 uCascade);					//判断name魔法状态总层数是否大于或等于uCasecade
	bool						DecStateCascade(const CMagicStateCfgServerSharedPtr& pCfg, uint32 uCascade, bool bInCancel = false);						//name魔法状态撤销uCascade层
	bool						SerializeToDB(CStateDBDataSet* pRet,	
		ICharacterDictatorCallbackHandler* pHandler, uint32 uFighterGlobalID, uint64 uNow, uint32 uGrade);				//往数据库写状态
	bool						LoadFromDB(ICharacterDictatorCallbackHandler* pHandler, uint32 uFighterGlobalID);						//从数据库读取状态
	bool						CreateDataMgrFromDB(ICharacterDictatorCallbackHandler* pHandler, uint32 uFighterGlobalID,
		CMagicStateCascadeDataMgr*& pCascadeDataMgr, CSkillInstServer* pSkillInst, uint32 uInstID, const string& sEffTitle,
		uint32 uCascadeMax, CFighterDictator* pInstaller, const CMagicEffServerSharedPtr& pMagicEff, const CMagicStateCfgServerSharedPtr& pCfg);	//从数据库读取状态上下文
	bool						RestoreStateFromDB(CSkillInstServer* pSkillInst, const CMagicStateCfgServerSharedPtr& pCfg, CFighterDictator* pInstaller, 
		int32 iTime, int32 iRemainTime, uint32 uCount, CMagicStateCascadeDataMgr* pCancelableDataMgr, 
		CMagicStateCascadeDataMgr* pDotDataMgr, CMagicStateCascadeDataMgr* pFinalDataMgr);		//从数据库恢复一个状态
	bool						ResetTime(const CMagicStateCfgServerSharedPtr& pCfg, CFighterDictator* pInstaller = NULL);		//只刷新状态时间
	bool						AddTime(const CMagicStateCfgServerSharedPtr& pCfg, int32 iTime);
	CRivalStateMgr*				GetRivalStateMgr()	{return m_pRivalStateMgr;}
private:
	//void						AddStateTarget(CFighterDictator* pTarget);
	//void						DelStateTarget(CFighterDictator* pTarget);
	//bool						SetupMagicState(CSkillInstServer* pSkillInst,CMagicStateCfgServer* pCfg, const uint32& grade, CFighterDictator* pInstaller);

	//安装魔法状态

	MapMagicStateCategory		m_mapMSCategory;				//状态种类表
	CFighterDictator*			m_pOwner;						//所属对象，即本对象的拥有者
	//SetStateTarget			m_setTarget;					//给别人所安装状态的对象

	//uint32						m_uMaxDynamicId;				//当前最大的魔法状态ID
	//MapMagicStateByDynamicId	m_mapMSByDynamicId;				//按动态ID排的魔法状态索引

	CRivalStateMgr*				m_pRivalStateMgr;

};


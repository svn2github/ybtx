#pragma once
#include "CSpecialState.h"
#include "COtherStateMgr.h"

//特殊状态的类型束
class CSpecialStateTypeBundle
	: public CMagicStateMallocObject
{
public:
	CSpecialStateTypeBundle() {}
	ListPSpecialState		m_listState;
};

class CSkillInstServer;
class CAllStateMgrServer;
class ICharacterMediatorCallbackHandler;
class CStateCondBase;
class CStateDBDataSet;

typedef map<ESpecialStateType, CSpecialStateTypeBundle>		MapSSTypeBandle;

class CSpecialStateMgrServer
	: public TOtherStateMgrServer<CSpecialStateCfgServer, CSpecialStateBase>
{
	friend class CSpecialStateBase;
	friend class CReflectStateServer;
	friend class CRepeatStateServer;
public:
	CSpecialStateMgrServer(CFighterDictator* pOwner)
		: TOtherStateMgrServer<CSpecialStateCfgServer, CSpecialStateBase>(pOwner)
	{

	}
	virtual ~CSpecialStateMgrServer()
	{
		ClearAll();
	}
	bool						SetupState(CSkillInstServer* pSkillInst, const CSpecialStateCfgServerSharedPtr& pCfg, CFighterDictator* pInstaller);		//安装特殊魔法状态
	bool						RestoreStateFromDB(CSkillInstServer* pSkillInst, const CSpecialStateCfgServerSharedPtr& pCfg,
			CFighterDictator* pInstaller, int32 iTime, int32 iRemainTime);
	bool						ExistStateByType(ESpecialStateType type);				//有某类特殊状态
	void						ClearStateByType(ESpecialStateType type);
	virtual void				ClearAll();
	virtual CStateDBData*		CreateStateDBData(CSpecialStateBase* pState, int32 iRemainTime, CSkillInstServer* pInst);
	virtual bool				RestoreStateFromDB(CSkillInstServer* pSkillInst, const CSpecialStateCfgServerSharedPtr& pCfg,
		CFighterDictator* pInstaller, int32 iTime, int32 iRemainTime, uint32 uAccumulate, float fProbability);	//从数据库恢复某个状态
	
private:
	CSpecialStateBase*			CreateSpecialState(CSkillInstServer* pSkillInst, CFighterDictator* pInstaller,
		CSpecialStateMgrServer* pMgr, const CSpecialStateCfgServerSharedPtr& pCfg, int32 iTime = 0, int32 iRemainTime = 0);

	MapSSTypeBandle				m_mapStateTypeBundle;						//类型到对象指针束的映射表
};
//class CSpecialStateMgrServer
//{
//	friend class CSpecialStateBase;
//	friend class CReflectStateServer;
//	friend class CRepeatStateServer;
//public:
//	CSpecialStateMgrServer(CFighterDictator* pOwner=NULL);
//	~CSpecialStateMgrServer();
//	void						SetOwner(CFighterDictator* pOwner) {m_pOwner = pOwner;}			//初始化目标对象指针
//	bool						SetupSpecialState(CSkillInstServer* pSkillInst, const string& name, CFighterDictator* pInstaller);		//安装特殊魔法状态
//	bool						RestoreSpecialStateFromDB(CSkillInstServer* pSkillInst, CSpecialStateCfgServer* pCfg,
//	bool						ExistSpecialState(const string& name);					//有某类特殊状态
//	bool						ExistSpecialState(const string& name, const CFighterDictator* pInstaller);					//有某人安装的某类特殊状态
//	bool						ExistSpecialState(ESpecialStateType type);				//有某类特殊状态
//	void						ClearSpecialState(ESpecialStateType type);
//	void						ClearSpecialState(const string& name);
//	bool						ClearSpecialStateByCond(CStateCondBase* pStateCond, uint32 uId);
//	void						ClearAll();						//用于死亡时清除所有状态
//	void						ClearAllByCond(CStateCondBase* pStateCond);
//	void						SyncAllState(CFighterDictator* pObserver, uint32 uNow);					//用于同步所有特殊状态
//	CAllStateMgrServer*			GetAllMgr();					//获取总魔法状态管理器
//	bool						SerializeToDB(CStateDBDataSet* pRet,
//		ICharacterMediatorCallbackHandler* pHandler, uint32 uFighterGlobalID, uint32 uNow);						//往数据库写魔法状态
//	bool						LoadFromDB(ICharacterMediatorCallbackHandler* pHandler, uint32 uFighterGlobalID);					//从数据库读取魔法状态
//
//private:
//	CSpecialStateBase*			CreateSpecialState(CSkillInstServer* pSkillInst, CFighterDictator* pInstaller,
//		CSpecialStateMgrServer* pMgr, CSpecialStateCfgServer* pCfg, int32 iTime = 0);
//
//private:
//	MapSpecialState				m_mapSS;						//特殊魔法状态种类到特殊魔法状态对象的映射表
//	MapSSTypeBandle				m_mapSSTB;						//类型到对象指针束的映射表
//
//	CFighterDictator*			m_pOwner;						//目标所属对象，即本对象的容器
//};


#pragma once
#include "FightDef.h"
#include "CPos.h"
#include "TMagicStateAllocator.h"
#include "TIDPtrMap.h"

class CFighterDictator;
class CMagicStateMgrServer;
class CTriggerStateMgrServer;
class CDamageChangeStateMgrServer;
class CCumuliTriggerStateMgrServer;
class CSpecialStateMgrServer;
class CBaseStateCfgServer;
class CSkillInstServer;
class CStateCondBase;
class CStateDBDataSet;
class CStateDBData;
class CStoredObjDBData;
class CGenericTarget;
class CSizeChangeMgr;
class CBaseStateServer;
class CBaseStateCfg;

typedef TIDPtrMap<CBaseStateServer>	IDMapStateByDynamicId;

class CStateCtrlCount
	: public CMagicStateMallocObject
{
public:
	CStateCtrlCount() :m_iNonStateCount(0)		{}
	typedef map<uint32, int32, less<uint32>, TMagicStateAllocator<pair<uint32,uint32> > > MapStateIdCount;
	MapStateIdCount m_mapStateIdCount;
	int32 m_iNonStateCount;
};

namespace sqr
{
	class CError;
}

class CAllStateMgrServer
	: public CMagicStateMallocObject
{
public:
	CAllStateMgrServer(CFighterDictator* pOwner);
	~CAllStateMgrServer();

	CMagicStateMgrServer*	GetMagicStateMgrServer()const			{return m_pMagicStateMgrServer;}
	CTriggerStateMgrServer* GetTriggerStateMgrServer()const			{return m_pTriggerStateMgrServer;}
	CDamageChangeStateMgrServer* GetDamageChangeStateMgrServer()const			{return m_pDamageChangeStateMgrServer;}
	CCumuliTriggerStateMgrServer* GetCumuliTriggerStateMgrServer()const			{return m_pCumuliTriggerStateMgrServer;}
	CSpecialStateMgrServer* GetSpecialStateMgrServer()const			{return m_pSpecialStateMgrServer;}

	//魔法状态回调
	void					OnClearState( CFighterDictator* pObserver );
	void					OnDeleteState( uint32 uCategoryId, uint32 uDynamicId );
	void					OnSetState( uint32 uCategoryId, uint32 uDynamicId, uint32 uCount, int32 iTime,
		int32 iRemainTime, uint32 uSkillLevel, CFighterDictator* pObserver);

	void					OnSetState( uint32 uCategoryId, uint32 uDynamicId, uint32 uCount, int32 iTime,
		int32 iRemainTime, uint32 uSkillLevel,uint32 uInstallerID, int32 iCalcValue = 0);

	void					SetModelStateId(uint32 uId);
	void					ClearModelStateId();
	void					AddModelStateId(uint32 uId);
	void					SubModelStateId();
	uint32					GetModelStateId() const					{return m_uModelStateId;}
	bool					ReplaceModelRuler(CBaseStateCfgServer* pCfgNew,CBaseStateCfgServer* pCfgOld);
	bool					ReplaceModelState(CBaseStateCfgServer* pCfg);

	void					SyncClearState(CFighterDictator* pObserver);						//同步清除状态信息给客户端
	void					SyncAllState(CFighterDictator* pObserver);							//同步状态信息给客户端

	bool					ExistDecreaseState();		//处于负面状态
	bool					ExistStateByDecreaseType(EDecreaseStateType eDecreaseType);					//处于状态类型
	bool					ExistState(const string& strName);
	bool					ExistState(uint32 uDynamicId);
	bool					ExistStateByCond(CStateCondBase* pStateCond);	
	uint32					GetStateCascade(const string& strName);

	bool					CancelState(uint32 uId);
	bool					CancelStateByDynamicId(uint32 uDynamicId);
	void					ClearAll();
	void					ClearAllByCond(CStateCondBase* pCond);
	void					ClearAllNonpersistentState();
	void					EraseAllState();				// 清除所有状态（非强制）
	void					EraseDecreaseState(int32 iCount = -1);			// 清除所有负面状态（非强制）
	void					EraseIncreaseState(int32 iCount = -1);			// 清除所有正面状态（非强制）
	void					EraseState(EDecreaseStateType eDecreaseType, int32 iCount = -1, bool bDelEqual = true);			// 清除所有负面状态（非强制）
	void					EraseDispellableDecreaseState(int32 iCount = -1);			// 清除所有负面状态（非强制）
	void					EraseDispellableDecreaseStateRand(int32 iCount = -1);			// 清除所有负面状态（非强制）
	void					EraseDispellableIncreaseState(int32 iCount = -1);			// 清除所有正面状态（非强制）
	void					EraseDispellableIncreaseStateRand(int32 iCount = -1);			// 清除所有负面状态（非强制）
	void					EraseDispellableState(EDecreaseStateType eDecreaseType, int32 iCount = -1, bool bDelEqual = true);			// 清除所有负面状态（非强制）
	bool					EraseErasableState(const string& strName);
	void					EraseState(const string& strName);										// 清除状态（非强制）
	void					EraseState(CBaseStateCfg* pCfg);
	void					EraseSelfState();
	void					EraseState(CBaseStateCfg* pCfg, CFighterDictator* pInstaller);
	bool					ClearStateByCond(CStateCondBase* pStateCond, uint32 uId);
	bool					ClearStateByCondAndDynamicId(CStateCondBase* pStateCond, uint32 uDynamicId);

	bool					DecreaseStateResist(CBaseStateCfgServer* pCfg);							//按抗性计算抵抗安装状态
	bool					Immume(CBaseStateCfgServer* pCfg, CSkillInstServer* pSkillInst, CFighterDictator* pFrom);		//是否免疫安装状态
	void					ReflectState(EDecreaseStateType eDecreaseType);							// 反射某种负面状态
	void					ReflectStateByCond(CStateCondBase* pCond);

	CStateDBDataSet*		SerializeToDB(uint32 uGrade);														// 存到数据库
	void					SaveStateToDBEnd();														// 存数据库结束
	bool					LoadFromDB(CStateDBDataSet* pRet);										// 从数据库读
	CStateDBDataSet*		GetStateDataMgrRet()			{return m_pRet;}
	bool					ReadStateFromDB(CStateDBData*& pStateDBData, EStateGlobalType eType);
	bool					ReadStoredObjFromDB(CStoredObjDBData*& pStored, uint32 uInstID);

	bool					MessageEvent(uint32 uEventID, CFighterDictator* pTrigger, ETriggerStateType eType = eTST_All);	
	bool					MessageEvent(uint32 uEventID, const CFPos& pTrigger, ETriggerStateType eType = eTST_All);	
	bool					MessageEvent(uint32 uEventID, CGenericTarget* pTrigger, ETriggerStateType eType = eTST_All);	

	void					PrintFightInfo(bool bSetupOrDelete, uint32 uObjInstallerID, CBaseStateCfgServer* pCfg);

	void					ChangeSizeRate(CBaseStateCfgServer* pCfg, int32 iChangeCount);
	double					GetStateZoomRate();

	bool					GetStateByDId(const CBaseStateServer*& pValue,uint32 uID) const;
	uint32					AddStateByDId(CBaseStateServer* const & Value);
	bool					DelStateByDId(uint32 uID);

	void					CountCtrlState(EFighterCtrlState eState, bool bSet, uint32 uStateId, sqr::CError* pExp = NULL);
	void					ClearCtrlStateCount();



private:
	CMagicStateMgrServer*			m_pMagicStateMgrServer;			// 魔法状态管理器
	CTriggerStateMgrServer*			m_pTriggerStateMgrServer;		// 触发器状态管理器
	CDamageChangeStateMgrServer*	m_pDamageChangeStateMgrServer;	// 伤害变更状态管理器
	CCumuliTriggerStateMgrServer*	m_pCumuliTriggerStateMgrServer;		// 触发器状态管理器
	CSpecialStateMgrServer*			m_pSpecialStateMgrServer;		// 特殊状态管理器

	uint32							m_uModelStateId;		//变形状态ID
	uint32							m_uModelStateCascde;	//变形状态模型层数

	CFighterDictator*				m_pOwner;				//拥有者
	CStateDBDataSet*				m_pRet;					//存取数据库传递用的状态数据总集
	CSizeChangeMgr*					m_pSizeChangeMgr;		//AOISIZE和BARRIERSIZE渐变管理器

	IDMapStateByDynamicId			m_idmapStateByDId;
	
	typedef map<EFighterCtrlState, CStateCtrlCount, less<EFighterCtrlState>, TMagicStateAllocator<pair<EFighterCtrlState,CStateCtrlCount> > > MapStateCtrlCount;
	MapStateCtrlCount m_mapStateCtrlCount;
};


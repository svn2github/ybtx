#pragma once
#include "CTplStateCfgClient.h"
#include "CStaticObject.h"
#include "CAllStateMgrClient.h"
#include "TMagicStateAllocator.h"

class CFighterFollower;
template <class StateType>
class COtherStateMgrClient;

//其它状态（包括触发器状态、伤害变更状态、特殊状态）
template <class StateType>
class COtherStateClient
	: public CBaseStateClient
{
public:
	virtual CBaseStateCfg*	GetCfg() {return m_pCfg.get();}
	//virtual uint32			GetDynamicID() {return m_uDynamicId;}
	virtual uint32			GetID() {return GetCfg()->GetId();}
	virtual uint32			GetCount() {return 1;}
	virtual CFighterFollower* GetOwner() {return m_pMgr->GetOwner();}
	COtherStateClient(uint32 uDynamicId, int32 iTime, int32 iRemainTime, uint32 uSkillLevel,
		const typename CTplStateCfgClient<StateType>::SharedPtrType& pCfg, uint32 uInstallerID,
		COtherStateMgrClient<StateType>* pMgr, CAllStateMgrClient* pAllStateMgr)
		:CBaseStateClient(uDynamicId, iTime, iRemainTime, uSkillLevel, uInstallerID, pAllStateMgr)
		, m_pCfg(pCfg), m_pMgr(pMgr) {}
	typename CTplStateCfgClient<StateType>::SharedPtrType			m_pCfg;
	COtherStateMgrClient<StateType>*		m_pMgr;
private:
	//uint32						m_uDynamicId;					//动态ID
};

//其它状态管理器（包括触发器状态、伤害变更状态、特殊状态）
template <class StateType>
class COtherStateMgrClient
	: public virtual CStaticObject
	, public CMagicStateMallocObject
{
public:
	COtherStateMgrClient(CFighterFollower* pOwner)
	{
		m_pOwner = pOwner;
	}
	~COtherStateMgrClient()
	{
		ClearAll();
	}

	typedef map<uint32, COtherStateClient<StateType>*, less<uint32>, TMagicStateAllocator<pair<uint32, typename COtherStateClient<StateType>*> > >	MapTplStateClient;
	bool		SetupState(uint32 uCategoryId, uint32 uDynamicId, int32 iTime, int32 iRemainTime, uint32 uSkillLevel, uint32 uInstallerID);
	void		DeleteState(uint32 uCategoryI, uint32 uDynamicId);
	bool		ExistState(const string& name);
	int32		StateLeftTime(const string& name);
	void		SetTargetBuff();
	void		ClearAll();
	float		GetRemainTime(const TCHAR* sName, uint32 uDynamicId);
	CFighterFollower* GetOwner() {return m_pOwner;}
private:
	MapTplStateClient				m_mapState;
	CFighterFollower*				m_pOwner;
};

class CTriggerStateClient;
class CDamageChangeStateClient;
class CCumuliTriggerStateClient;
class CSpecialStateClient;

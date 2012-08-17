#pragma once
#include "CTplStateCfgClient.h"
#include "CStaticObject.h"
#include "CAllStateMgrClient.h"
#include "TMagicStateAllocator.h"

class CMagicStateMgrClient;
class CMagicStateCategoryClient;
class CMagicStateClient;
class CFighterFollower;

typedef map<uint32, CMagicStateClient*, less<uint32>, TMagicStateAllocator<pair<uint32, CMagicStateClient*> > >			MapMagicStateClient;
typedef map<uint32, CMagicStateCategoryClient*, less<uint32>, TMagicStateAllocator<pair<uint32, CMagicStateCategoryClient*> > >	MapMagicStateCategoryClient;

//魔法状态
class CMagicStateClient
	: public CBaseStateClient
{
	friend class CMagicStateCategoryClient;
	friend class CMagicStateMgrClient;
public:
	virtual CBaseStateCfg*		GetCfg();
	virtual CFighterFollower*	GetOwner();
	//virtual uint32				GetDynamicID() {return m_uDynamicId;}
	virtual uint32				GetID() {return GetCfg()->GetId();}
	virtual uint32				GetCount()	{return m_uCount;}
private:
	CMagicStateClient(uint32 uDynamicId, uint32 uCount, int32 iTime, int32 iRemainTime,
		uint32 uSkillLevel, uint32 uInstallerID, int32 iCalcValue, CMagicStateCategoryClient* pMSCategory, CAllStateMgrClient* pAllStateMgr);
	void						UpdateCount(uint32 uCount)		{m_uCount = uCount;}
	//uint32						m_uDynamicId;					
	uint32						m_uCount;						//叠加的次数
	int32							m_iCalcValue;
	CMagicStateCategoryClient*	m_pMSCategory;					//所属的魔法种类，即本魔法状态的直接容器，用于获取配置表指针
};

//魔法状态种类
class CMagicStateCategoryClient
	: public CMagicStateMallocObject
{
	friend class CMagicStateClient;
	friend class CMagicStateMgrClient;
public:
	CMagicStateCategoryClient(const CTplStateCfgClient<CMagicStateClient>::SharedPtrType& pCfg, CMagicStateMgrClient* pMgr)
		: m_pCfg(pCfg),
		m_pMgr(pMgr)
	{}
	~CMagicStateCategoryClient();

private:
	void AddMS(uint32 uCategoryId, uint32 uCount, int32 iTime, int32 iRemainTime, uint32 uSkillLevel,
		uint32 uInstallerID, int32 iCalcValue, CAllStateMgrClient* pAllStateMgr);	//添加一个魔法状态种类
	MapMagicStateClient		m_mapMS;							//不同魔法状态的映射表
	CTplStateCfgClient<CMagicStateClient>::SharedPtrType	m_pCfg;				//魔法状态配置表对象指针
	CMagicStateMgrClient*	m_pMgr;								//所属的魔法状态管理类，即本对象的容器
};

//魔法状态管理器
class CMagicStateMgrClient
	: public virtual CStaticObject
	, public CMagicStateMallocObject
{
	friend class CMagicStateClient;
	friend class CMagicStateCategoryClient;
public:
	CMagicStateMgrClient(CFighterFollower* pOwner)
	{
		m_pOwner = pOwner;
	}
	~CMagicStateMgrClient()
	{
		ClearAll();
	}
	bool						SetState(uint32 uCategoryId, uint32 uDynamicId, uint32 uCount, int32 iTime, int32 iRemainTime, uint32 uSkillLevel,uint32 uInstallerID,int32 iCalcValue);			//设置一个状态（用于不确定新加还是更新状态时）
	//void						AddNewState(uint32 uCategoryId, uint32 uDynamicId);			//新加一个状态
	//void						UpdateState(uint32 uCategoryId, uint32 uDynamicId, uint32 uCount);			//更新一个状态
	void						DeleteState(uint32 uCategoryId, uint32 uDynamicId);			//删除一个状态
	bool						ExistState(const string& name);
	uint32						StateCount(const string& name);
	int32						StateLeftTime(const string& name, CFighterFollower* pFrom);
	//void						SetupStateList({uCategoryId, uDynamicId, uCount}[]);				//设置一个状态序列
	void						ClearAll();
	void						SetTargetBuff();
	bool						CanDecStateCascade(const string& name, uint32 uCascade);		//name魔法状态叠加层数是否大于等于uCascade
	float						GetRemainTime(const TCHAR* sName, uint32 uDynamicId);
	
private:
	MapMagicStateCategoryClient	m_mapMSCategory;
	CFighterFollower*			m_pOwner;
};

